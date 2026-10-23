#include "luau_script.h"
#ifdef TOOLS_ENABLED
#include "luau_analysis.h"
#endif
#include "Luau/Compiler.h"
#include "Luau/ExperimentalFlags.h"
#include "Luau/Parser.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/object/class_db.h"
#include "core/debugger/script_debugger.h"
#include "core/debugger/engine_debugger.h"
#include "lualib.h"
#include <new>
#include <set>

LuauLanguage *LuauLanguage::singleton = nullptr;
LuauLanguage::LuauLanguage() {
    singleton = this;
    Luau::assertHandler() = [](const char *expression, const char *file, int line, const char *function) {
        fprintf(stderr, "Luau assertion: %s at %s:%d (%s)\n", expression, file, line, function);
        return 1;
    };
    for (auto flag = Luau::FValue<bool>::list; flag != nullptr; flag = flag->next)
        if (strncmp(flag->name, "Luau", 4) == 0 && !Luau::isAnalysisFlagExperimental(flag->name)) flag->value = true;
}
namespace {
constexpr const char *VARIANT_META = "Ludot.Variant";
Variant *variant_at(lua_State *L, int index) { return static_cast<Variant *>(luaL_checkudata(L, index, VARIANT_META)); }
void destroy_variant(lua_State *L, void *pointer) { static_cast<Variant *>(pointer)->~Variant(); }
int native_call(lua_State *L) {
    Variant *self = variant_at(L, 1);
    const StringName method = String::utf8(lua_tostring(L, lua_upvalueindex(1)));
    const int count = lua_gettop(L) - 1;
    Vector<Variant> values;
    Vector<const Variant *> pointers;
    values.resize(count);
    pointers.resize(count);
    for (int i = 0; i < count; ++i) { values.write[i] = LuauLanguage::to_variant(L, i + 2); pointers.write[i] = &values[i]; }
    Variant result;
    Callable::CallError error;
    self->callp(method, pointers.ptrw(), count, result, error);
    if (error.error != Callable::CallError::CALL_OK) luaL_error(L, "%s", Variant::get_call_error_text(method, pointers.ptrw(), count, error).utf8().get_data());
    LuauLanguage::push_variant(L, result);
    return 1;
}
int native_index(lua_State *L) {
    Variant *self = variant_at(L, 1);
    if (lua_type(L, 2) == LUA_TNUMBER) {
        bool valid;
        const Variant result = self->get(LuauLanguage::to_variant(L, 2), &valid);
        if (!valid) luaL_error(L, "Invalid native index");
        LuauLanguage::push_variant(L, result);
        return 1;
    }
    const String requested = String::utf8(luaL_checkstring(L, 2));
    List<PropertyInfo> properties;
    self->get_property_list(&properties);
    for (const PropertyInfo &property : properties) {
        if (LuauLanguage::api_name(property.name, true) == requested) {
            bool valid;
            LuauLanguage::push_variant(L, self->get_named(property.name, valid));
            return 1;
        }
    }
    List<MethodInfo> methods;
    self->get_method_list(&methods);
    for (const MethodInfo &method : methods) {
        if (LuauLanguage::api_name(method.name) == requested) {
            lua_pushstring(L, String(method.name).utf8().get_data());
            lua_pushcclosure(L, native_call, "native method", 1);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}
int native_newindex(lua_State *L) {
    Variant *self = variant_at(L, 1);
    const String requested = String::utf8(luaL_checkstring(L, 2));
    List<PropertyInfo> properties;
    self->get_property_list(&properties);
    for (const PropertyInfo &property : properties) {
        if (LuauLanguage::api_name(property.name, true) == requested) {
            bool valid;
            self->set_named(property.name, LuauLanguage::to_variant(L, 3), valid);
            if (!valid) luaL_error(L, "Invalid native property assignment");
            return 0;
        }
    }
    luaL_error(L, "Unknown native property '%s'", requested.utf8().get_data());
}
int native_construct(lua_State *L) {
    const Variant::Type type = Variant::Type(lua_tointeger(L, lua_upvalueindex(1)));
    const int count = lua_gettop(L);
    Vector<Variant> values;
    Vector<const Variant *> pointers;
    values.resize(count);
    pointers.resize(count);
    for (int i = 0; i < count; ++i) { values.write[i] = LuauLanguage::to_variant(L, i + 1); pointers.write[i] = &values[i]; }
    Variant value;
    Callable::CallError error;
    Variant::construct(type, value, pointers.ptrw(), count, error);
    if (error.error != Callable::CallError::CALL_OK) luaL_error(L, "Invalid native constructor arguments");
    LuauLanguage::push_variant(L, value);
    return 1;
}
int native_operator(lua_State *L) {
    const auto op = Variant::Operator(lua_tointeger(L, lua_upvalueindex(1)));
    Variant result;
    bool valid;
    Variant::evaluate(op, LuauLanguage::to_variant(L, 1), lua_gettop(L) > 1 ? LuauLanguage::to_variant(L, 2) : Variant(), result, valid);
    if (!valid) luaL_error(L, "Unsupported native operator");
    LuauLanguage::push_variant(L, result);
    return 1;
}
int native_new(lua_State *L) {
    const StringName name = String::utf8(luaL_checkstring(L, 1));
    Object *object = ClassDB::instantiate(name);
    if (object == nullptr) luaL_error(L, "Cannot instantiate native class '%s'", String(name).utf8().get_data());
    LuauLanguage::push_variant(L, object);
    return 1;
}
int native_global(lua_State *L) {
    const StringName name = String::utf8(luaL_checkstring(L, 2));
    if (!ClassDB::class_exists(name)) { lua_pushnil(L); return 1; }
    auto language = LuauLanguage::get_singleton();
    if (language->ensure_native_class(name) != OK) luaL_error(L, "%s", language->last_error.utf8().get_data());
    lua_getglobal(L, String(name).utf8().get_data());
    return 1;
}
int native_tostring(lua_State *L) { lua_pushstring(L, String(*variant_at(L, 1)).utf8().get_data()); return 1; }
int ludot_print(lua_State *L) {
    String message;
    const int count = lua_gettop(L);
    for (int i = 1; i <= count; ++i) {
        if (i > 1) message += "\t";
        size_t size;
        const char *text = luaL_tolstring(L, i, &size);
        message += String::utf8(text, size);
        lua_pop(L, 1);
    }
    print_line(message);
    return 0;
}
int ludot_require(lua_State *L) {
    auto language = LuauLanguage::get_singleton();
    const String requested = String::utf8(luaL_checkstring(L, 1));
    lua_Debug debug = {};
    lua_getinfo(L, 1, "s", &debug);
    String from = debug.source != nullptr ? String::utf8(debug.source).trim_prefix("@") : "res://";
    const String path = language->resolve_module(requested, from);
    if (path.is_empty()) luaL_error(L, "Cannot resolve module '%s'", requested.utf8().get_data());
    language->module_dependents[path].insert(from);
    if (const int *cached = language->module_cache.getptr(path)) { lua_getref(L, *cached); return 1; }
    if (language->loading.has(path)) luaL_error(L, "Cyclic import of '%s'", path.utf8().get_data());
    language->loading.insert(path);
    Error error;
    const Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ, &error);
    String module_source;
    if (error == OK) module_source = file->get_as_text();
    for (LuauScript *script : language->scripts) if (script->get_path() == path) module_source = script->get_source_code();
    if (error == OK) error = language->load_chunk(module_source, path, L);
    if (error == OK && lua_pcall(L, 0, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1));
        lua_pop(L, 1);
        error = ERR_SCRIPT_FAILED;
    }
    language->loading.erase(path);
    if (error != OK) luaL_error(L, "%s", language->last_error.utf8().get_data());
    const int ref = lua_ref(L, -1);
    language->module_cache.insert(path, ref);
    return 1;
}
Variant::Type annotation_type(Luau::AstType *type) {
    const auto ref = type != nullptr ? type->as<Luau::AstTypeReference>() : nullptr;
    if (ref == nullptr) return Variant::NIL;
    const String name = String::utf8(ref->name.value);
    if (name == "number") return Variant::FLOAT;
    if (name == "boolean") return Variant::BOOL;
    if (name == "string") return Variant::STRING;
    for (int i = Variant::VECTOR2; i < Variant::VARIANT_MAX; ++i) if (Variant::get_type_name(Variant::Type(i)) == name) return Variant::Type(i);
    return Variant::OBJECT;
}
Variant constant_default(Luau::AstExpr *expr) {
    if (expr == nullptr) return {};
    if (const auto number = expr->as<Luau::AstExprConstantNumber>()) return number->value;
    if (const auto boolean = expr->as<Luau::AstExprConstantBool>()) return boolean->value;
    if (const auto string = expr->as<Luau::AstExprConstantString>()) return String::utf8(string->value.data, string->value.size);
    return {};
}
}

String LuauLanguage::api_name(const String &p_name, bool p_property) {
    const bool internal = p_name.begins_with("_");
    const String body = internal ? p_name.substr(1) : p_name;
    const String renamed = p_property ? body.to_pascal_case() : body.to_camel_case();
    return internal ? "_" + renamed : renamed;
}
void LuauLanguage::push_variant(lua_State *L, const Variant &p_value) {
    if (p_value.get_type() == Variant::OBJECT && static_cast<Object *>(p_value) == nullptr) { lua_pushnil(L); return; }
    switch (p_value.get_type()) {
        case Variant::NIL: lua_pushnil(L); return;
        case Variant::BOOL: lua_pushboolean(L, p_value); return;
        case Variant::INT: lua_pushnumber(L, int64_t(p_value)); return;
        case Variant::FLOAT: lua_pushnumber(L, double(p_value)); return;
        case Variant::STRING: case Variant::STRING_NAME: case Variant::NODE_PATH: {
            const CharString text = String(p_value).utf8(); lua_pushlstring(L, text.get_data(), text.length()); return;
        }
        default: break;
    }
    new (lua_newuserdatadtor(L, sizeof(Variant), destroy_variant)) Variant(p_value);
    luaL_getmetatable(L, VARIANT_META);
    lua_setmetatable(L, -2);
}
Variant LuauLanguage::to_variant(lua_State *L, int p_index, int p_depth) {
    if (p_depth > 64) luaL_error(L, "A cyclic or deeply nested table cannot be converted to an engine value");
    switch (lua_type(L, p_index)) {
        case LUA_TNIL: return {};
        case LUA_TBOOLEAN: return bool(lua_toboolean(L, p_index));
        case LUA_TNUMBER: return lua_tonumber(L, p_index);
        case LUA_TSTRING: { size_t size; const char *text = lua_tolstring(L, p_index, &size); return String::utf8(text, size); }
        case LUA_TUSERDATA: return *variant_at(L, p_index);
        case LUA_TOBJECT: {
            lua_getfield(L, p_index, "_native");
            Variant result = lua_isuserdata(L, -1) ? *variant_at(L, -1) : Variant();
            lua_pop(L, 1);
            return result;
        }
        case LUA_TTABLE: {
            Dictionary dictionary;
            int absolute = lua_absindex(L, p_index);
            lua_pushnil(L);
            while (lua_next(L, absolute) != 0) {
                dictionary[to_variant(L, -2, p_depth + 1)] = to_variant(L, -1, p_depth + 1);
                lua_pop(L, 1);
            }
            return dictionary;
        }
        default: return {};
    }
}
namespace {
void debug_step(lua_State *L, lua_Debug *unused) {
    auto debugger = EngineDebugger::get_script_debugger();
    if (debugger == nullptr || debugger->is_skipping_breakpoints()) return;
    lua_Debug info = {};
    if (lua_getinfo(L, 0, "sl", &info) == 0 || info.source == nullptr) return;
    const String source = String::utf8(info.source).trim_prefix("@");
    static thread_local String previous_source;
    static thread_local int previous_line = -1;
    static thread_local int previous_depth = 0;
    const int depth = lua_stackdepth(L);
    if (debugger->get_depth() >= 0 && debugger->get_lines_left() > 0) debugger->set_depth(debugger->get_depth() + depth - previous_depth);
    previous_depth = depth;
    if (source == previous_source && info.currentline == previous_line) return;
    previous_source = source;
    previous_line = info.currentline;
    bool stepping = false;
    if (debugger->get_lines_left() > 0 && debugger->get_depth() <= 0) {
        debugger->set_lines_left(debugger->get_lines_left() - 1);
        stepping = debugger->get_lines_left() == 0;
    }
    if (stepping || debugger->is_breakpoint(info.currentline, source)) {
        auto language = LuauLanguage::get_singleton();
        language->debug_state = L;
        debugger->debug(language, true);
        language->debug_state = nullptr;
    }
}
void debug_error(lua_State *L) {
    auto debugger = EngineDebugger::get_script_debugger();
    if (debugger == nullptr || debugger->is_ignoring_error_breaks()) return;
    auto language = LuauLanguage::get_singleton();
    language->last_error = lua_isstring(L, -1) ? String::utf8(lua_tostring(L, -1)) : String("Luau runtime error");
    language->debug_state = L;
    debugger->debug(language, false, true);
    language->debug_state = nullptr;
}
}
void LuauLanguage::init() {
    state = luaL_newstate();
    luaL_openlibs(state);
    if (EngineDebugger::is_active()) {
        lua_callbacks(state)->debugstep = debug_step;
        lua_callbacks(state)->debugprotectederror = debug_error;
        lua_singlestep(state, true);
    }
    luaL_newmetatable(state, VARIANT_META);
    lua_pushcfunction(state, native_index, "native index"); lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, native_newindex, "native assignment"); lua_setfield(state, -2, "__newindex");
    lua_pushcfunction(state, native_tostring, "native string"); lua_setfield(state, -2, "__tostring");
    const char *operators[] = {"__add", "__sub", "__mul", "__div", "__unm", "__eq", "__lt", "__le"};
    const Variant::Operator operation[] = {Variant::OP_ADD, Variant::OP_SUBTRACT, Variant::OP_MULTIPLY, Variant::OP_DIVIDE, Variant::OP_NEGATE, Variant::OP_EQUAL, Variant::OP_LESS, Variant::OP_LESS_EQUAL};
    for (int i = 0; i < 8; ++i) { lua_pushinteger(state, operation[i]); lua_pushcclosure(state, native_operator, operators[i], 1); lua_setfield(state, -2, operators[i]); }
    lua_pop(state, 1);
    lua_newtable(state);
    lua_pushcfunction(state, native_global, "native class lookup"); lua_setfield(state, -2, "__index");
    lua_setmetatable(state, LUA_GLOBALSINDEX);
    lua_pushcfunction(state, native_new, "native constructor"); lua_setglobal(state, "__ludotConstructNative");
    lua_pushcfunction(state, ludot_print, "print"); lua_setglobal(state, "print");
    lua_pushcfunction(state, ludot_require, "require"); lua_setglobal(state, "require");
    for (int i = Variant::VECTOR2; i < Variant::VARIANT_MAX; ++i) {
        lua_pushinteger(state, i);
        lua_pushcclosure(state, native_construct, "native constructor", 1);
        lua_setglobal(state, Variant::get_type_name(Variant::Type(i)).utf8().get_data());
    }
    List<Engine::Singleton> singletons;
    Engine::get_singleton()->get_singletons(&singletons);
    for (const auto &entry : singletons) { push_variant(state, entry.ptr); lua_setglobal(state, String(entry.name).utf8().get_data()); }
    GLOBAL_DEF("luau/modules/aliases", Dictionary());
}
void LuauLanguage::finish() {
#ifdef TOOLS_ENABLED
    ludot_clear_analysis();
#endif
    module_cache.clear();
    module_dependents.clear();
    native_classes.clear();
    if (state != nullptr) { lua_State *closing = state; state = nullptr; lua_close(closing); }
}
Error LuauLanguage::load_chunk(const String &p_source, const String &p_path, lua_State *p_state) {
    lua_State *L = p_state != nullptr ? p_state : state;
    const CharString source = p_source.utf8();
    Luau::CompileOptions options;
    options.debugLevel = 2;
    options.optimizationLevel = 1;
    const std::string bytecode = Luau::compile(std::string(source.get_data(), source.length()), options);
    if (luau_load(L, ("@" + p_path).utf8().get_data(), bytecode.data(), bytecode.size(), 0) != 0) {
        last_error = String::utf8(lua_tostring(L, -1));
        lua_pop(L, 1);
        return ERR_PARSE_ERROR;
    }
    return OK;
}
HashSet<String> LuauLanguage::invalidate_modules(const String &p_path) {
    HashSet<String> affected;
    Vector<String> pending;
    pending.push_back(p_path);
    while (!pending.is_empty()) {
        const String path = pending[pending.size() - 1];
        pending.resize(pending.size() - 1);
        if (affected.has(path)) continue;
        affected.insert(path);
        if (const auto *dependents = module_dependents.getptr(path)) for (const String &dependent : *dependents) pending.push_back(dependent);
        if (const int *ref = module_cache.getptr(path)) { lua_unref(state, *ref); module_cache.erase(path); }
    }
    return affected;
}
String LuauLanguage::resolve_module(const String &p_requested, const String &p_from) const {
    String path;
    if (p_requested.begins_with("@")) {
        const int slash = p_requested.find("/");
        const String alias = p_requested.substr(1, slash == -1 ? p_requested.length() - 1 : slash - 1);
        const Dictionary project_aliases = GLOBAL_GET("luau/modules/aliases");
        Dictionary aliases = project_aliases.duplicate();
        if (FileAccess::exists("res://.luaurc")) {
            const Variant parsed = JSON::parse_string(FileAccess::get_file_as_string("res://.luaurc"));
            if (parsed.get_type() == Variant::DICTIONARY) {
                const Dictionary config = parsed;
                if (config.has("aliases")) { const Dictionary user_aliases = config["aliases"]; aliases.merge(user_aliases, true); }
            }
        }
        if (!aliases.has(alias)) return "";
        path = String(aliases[alias]);
        if (!path.begins_with("res://")) path = String("res://").path_join(path);
        if (slash != -1) path = path.path_join(p_requested.substr(slash + 1));
    } else if (p_requested.begins_with("./") || p_requested.begins_with("../")) {
        path = p_from.get_base_dir().path_join(p_requested);
    } else if (p_requested.begins_with("res://")) path = p_requested;
    else return "";
    path = path.simplify_path();
    if (!path.begins_with("res://")) return "";
    if (FileAccess::exists(path) && path.get_extension() == "luau") return path;
    if (FileAccess::exists(path + ".luau")) return path + ".luau";
    if (FileAccess::exists(path.path_join("init.luau"))) return path.path_join("init.luau");
    return "";
}
Error LuauLanguage::ensure_native_class(const StringName &p_name) {
    if (native_classes.has(p_name)) return OK;
    if (!ClassDB::class_exists(p_name)) { last_error = "Unknown native base class '" + String(p_name) + "'"; return ERR_INVALID_DATA; }
    String source = "open class " + String(p_name) + "\n public _native\n function __init(self) self._native = __ludotConstructNative(\"" + String(p_name) + "\") end\n";
    List<PropertyInfo> properties;
    ClassDB::get_property_list(p_name, &properties);
    std::set<String> names{"_native"};
    for (const auto &property : properties) {
        const String name = api_name(property.name, true);
        if (!String(property.name).contains("/") && name.is_valid_ascii_identifier() && names.insert(name).second) source += " public " + name + "\n";
    }
    List<MethodInfo> methods;
    ClassDB::get_method_list(p_name, &methods);
    for (const auto &method : methods) {
        const String name = api_name(method.name);
        if (!name.is_valid_ascii_identifier() || !names.insert(name).second || name.begins_with("__")) continue;
        source += " public function " + name + "(self, ...) return self._native:" + name + "(...) end\n";
    }
    source += "end\nreturn " + String(p_name);
    const Error error = load_chunk(source, "native/" + String(p_name));
    if (error != OK) return error;
    if (lua_pcall(state, 0, 1, 0) != LUA_OK) { last_error = String::utf8(lua_tostring(state, -1)); lua_pop(state, 1); return ERR_SCRIPT_FAILED; }
    lua_setglobal(state, String(p_name).utf8().get_data());
    native_classes.insert(p_name);
    return OK;
}

LuauScript::LuauScript() { LuauLanguage::get_singleton()->scripts.insert(this); }
LuauScript::~LuauScript() {
    auto language = LuauLanguage::get_singleton();
    if (language != nullptr) {
        MutexLock lock(language->mutex);
        if (language->state != nullptr && class_ref != LUA_NOREF) lua_unref(language->state, class_ref);
        language->scripts.erase(this);
    }
}
void LuauScript::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_diagnostics"), &LuauScript::get_diagnostics);
    ClassDB::bind_method(D_METHOD("get_completions", "code"), &LuauScript::get_completions);
}
PackedStringArray LuauScript::get_completions(const String &p_code) const {
    List<ScriptLanguage::CodeCompletionOption> options;
    bool force = false;
    String hint;
    LuauLanguage::get_singleton()->complete_code(p_code, get_path(), nullptr, &options, force, hint);
    PackedStringArray labels;
    for (const auto &option : options) labels.push_back(option.display);
    labels.sort();
    return labels;
}
Array LuauScript::get_diagnostics() const {
    List<ScriptLanguage::ScriptError> errors;
    LuauLanguage::get_singleton()->validate(source, get_path(), nullptr, &errors, nullptr, nullptr);
    Array result;
    for (const auto &error : errors) {
        Dictionary diagnostic;
        diagnostic["line"] = error.line;
        diagnostic["column"] = error.column;
        diagnostic["message"] = error.message;
        result.push_back(diagnostic);
    }
    return result;
}
Error LuauScript::reload(bool p_keep_state) {
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    const CharString text = source.utf8();
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);
    const auto parsed = Luau::Parser::parse(text.get_data(), text.length(), names, allocator);
    if (!parsed.errors.empty()) {
        language->last_error = String::utf8(parsed.errors.front().what());
        ERR_PRINT(language->last_error);
        return ERR_PARSE_ERROR;
    }
    Luau::AstStatClass *declaration = nullptr;
    for (auto stat : parsed.root->body) if (auto cls = stat->as<Luau::AstStatClass>()) declaration = cls;
    if (declaration == nullptr || declaration->super == nullptr ||
        (declaration->super->is<Luau::AstExprGlobal>() && !ClassDB::class_exists(String::utf8(declaration->super->as<Luau::AstExprGlobal>()->name.value)))) {
        const Error error = language->load_chunk(source, get_path());
        if (error != OK) return error;
        lua_pop(language->state, 1);
        const auto affected = language->invalidate_modules(get_path());
        valid = true;
        for (LuauScript *dependent : language->scripts) {
            if (dependent != this && dependent->can_instantiate() && affected.has(dependent->get_path())) dependent->reload(true);
        }
        return OK;
    }
    if (declaration->super == nullptr || declaration->super->is<Luau::AstExprGlobal>() == false) {
        language->last_error = "An attached class must extend a native engine class";
        return ERR_INVALID_DATA;
    }
    const StringName new_name = String::utf8(declaration->name->name.value);
    const auto super = declaration->super != nullptr ? declaration->super->as<Luau::AstExprGlobal>() : nullptr;
    const StringName new_base = super != nullptr ? StringName(String::utf8(super->name.value)) : StringName("RefCounted");
    Error error = language->ensure_native_class(new_base);
    if (error != OK) { ERR_PRINT(language->last_error); return error; }
    error = language->load_chunk(source + "\nreturn " + String(new_name), get_path());
    if (error != OK) { ERR_PRINT(language->last_error); return error; }
    lua_State *L = language->state;
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
        ERR_PRINT(language->last_error);
        return ERR_SCRIPT_FAILED;
    }
    const int next_class = lua_ref(L, -1);
    lua_pop(L, 1);
    // Stage all replacement objects before changing any live instance.
    HashMap<LuauScriptInstance *, int> replacements;
    for (LuauScriptInstance *instance : instances) {
        lua_getref(L, next_class);
        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
            language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
            for (const auto &replacement : replacements) lua_unref(L, replacement.value);
            lua_unref(L, next_class);
            return ERR_SCRIPT_FAILED;
        }
        language->push_variant(L, instance->owner);
        lua_setfield(L, -2, "_native");
        const int next_object = lua_ref(L, -1);
        lua_pop(L, 1);
        replacements.insert(instance, next_object);
        if (p_keep_state) {
            for (const PropertyInfo &property : properties) {
                // A removed property is intentionally dropped during reload.
                bool remains = false;
                for (const auto &member : declaration->members) if (const auto prop = Luau::get_if<Luau::AstClassProperty>(&member)) if (String::utf8(prop->name.value) == String(property.name)) remains = true;
                if (!remains) continue;
                lua_getref(L, next_object);
                lua_getref(L, instance->object_ref);
                lua_getfield(L, -1, String(property.name).utf8().get_data());
                lua_remove(L, -2);
                lua_setfield(L, -2, String(property.name).utf8().get_data());
                lua_pop(L, 1);
            }
        }
    }
    if (class_ref != LUA_NOREF) lua_unref(L, class_ref);
    class_ref = next_class;
    class_name = new_name;
    base_type = new_base;
    methods.clear(); properties.clear(); defaults.clear();
    for (const auto &member : declaration->members) {
        if (const auto property = Luau::get_if<Luau::AstClassProperty>(&member)) {
            PropertyInfo info(annotation_type(property->ty), String::utf8(property->name.value));
            info.usage = property->isExported ? PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE : PROPERTY_USAGE_SCRIPT_VARIABLE;
            if (property->rangeMin != nullptr && property->rangeMax != nullptr) {
                info.hint = PROPERTY_HINT_RANGE;
                info.hint_string = String(constant_default(property->rangeMin)) + "," + String(constant_default(property->rangeMax));
                if (property->rangeStep != nullptr) info.hint_string += "," + String(constant_default(property->rangeStep));
            }
            properties.push_back(info);
            defaults.insert(info.name, constant_default(property->defaultValue));
        } else if (const auto method = Luau::get_if<Luau::AstClassMethod>(&member)) {
            MethodInfo info(String::utf8(method->functionName.value));
            for (size_t i = 1; i < method->function->args.size; ++i) {
                auto arg = method->function->args.data[i];
                info.arguments.push_back(PropertyInfo(annotation_type(arg->annotation), String::utf8(arg->name.value)));
            }
            methods.insert(info.name, info);
        }
    }
    for (const auto &replacement : replacements) { lua_unref(L, replacement.key->object_ref); replacement.key->object_ref = replacement.value; }
    valid = true;
    emit_changed();
    return OK;
}
bool LuauScript::has_method(const StringName &p_method) const { return methods.has(p_method) || methods.has(LuauLanguage::api_name(p_method)); }
MethodInfo LuauScript::get_method_info(const StringName &p_method) const {
    const MethodInfo *info = methods.getptr(p_method);
    if (info == nullptr) info = methods.getptr(LuauLanguage::api_name(p_method));
    return info != nullptr ? *info : MethodInfo();
}
ScriptLanguage *LuauScript::get_language() const { return LuauLanguage::get_singleton(); }
bool LuauScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
    const Variant *value = defaults.getptr(p_property);
    if (value == nullptr) return false;
    r_value = *value; return true;
}
void LuauScript::get_script_method_list(List<MethodInfo> *r_methods) const { for (const auto &method : methods) r_methods->push_back(method.value); }
ScriptInstance *LuauScript::instance_create(Object *p_this) {
    ERR_FAIL_COND_V(!valid, nullptr);
    auto instance = memnew(LuauScriptInstance(p_this, Ref<LuauScript>(this)));
    if (instance->object_ref == LUA_NOREF) { memdelete(instance); return nullptr; }
    return instance;
}
PlaceHolderScriptInstance *LuauScript::placeholder_instance_create(Object *p_this) {
    auto placeholder = memnew(PlaceHolderScriptInstance(LuauLanguage::get_singleton(), Ref<Script>(this), p_this));
    placeholder->update(properties, defaults);
    return placeholder;
}
LuauScriptInstance::LuauScriptInstance(Object *p_owner, const Ref<LuauScript> &p_script) : owner(p_owner), script(p_script) {
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    lua_State *L = language->state;
    lua_getref(L, script->class_ref);
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
        ERR_PRINT(language->last_error);
        return;
    }
    // Each object keeps its native owner; engine calls still use the original API.
    language->push_variant(L, owner);
    lua_setfield(L, -2, "_native");
    object_ref = lua_ref(L, -1); lua_pop(L, 1);
    script->instances.insert(this);
}
LuauScriptInstance::~LuauScriptInstance() {
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    script->instances.erase(this);
    if (language->state != nullptr && object_ref != LUA_NOREF) lua_unref(language->state, object_ref);
}
bool LuauScriptInstance::set(const StringName &p_name, const Variant &p_value) {
    bool found = false;
    for (const PropertyInfo &property : script->properties) if (property.name == p_name) found = true;
    if (!found || object_ref == LUA_NOREF) return false;
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    lua_getref(language->state, object_ref);
    language->push_variant(language->state, p_value);
    lua_setfield(language->state, -2, String(p_name).utf8().get_data());
    lua_pop(language->state, 1);
    return true;
}
bool LuauScriptInstance::get(const StringName &p_name, Variant &r_value) const {
    bool found = false;
    for (const PropertyInfo &property : script->properties) if (property.name == p_name) found = true;
    if (!found || object_ref == LUA_NOREF) return false;
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    lua_getref(language->state, object_ref);
    lua_getfield(language->state, -1, String(p_name).utf8().get_data());
    r_value = language->to_variant(language->state, -1);
    lua_pop(language->state, 2);
    return true;
}
Variant::Type LuauScriptInstance::get_property_type(const StringName &p_name, bool *r_valid) const {
    for (const PropertyInfo &property : script->properties) if (property.name == p_name) { if (r_valid != nullptr) *r_valid = true; return property.type; }
    if (r_valid != nullptr) *r_valid = false;
    return Variant::NIL;
}
Variant LuauScriptInstance::callp(const StringName &p_method, const Variant **p_args, int p_count, Callable::CallError &r_error) {
    r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
    if (!has_method(p_method) || object_ref == LUA_NOREF) return {};
    auto language = LuauLanguage::get_singleton();
    MutexLock lock(language->mutex);
    lua_State *L = language->state;
    const int top = lua_gettop(L);
    const String method = script->methods.has(p_method) ? String(p_method) : LuauLanguage::api_name(p_method);
    List<PropertyInfo> native_properties;
    ClassDB::get_property_list(owner->get_class_name(), &native_properties);
    HashMap<StringName, Variant> before;
    lua_getref(L, object_ref);
    for (const PropertyInfo &property : native_properties) {
        const String name = LuauLanguage::api_name(property.name, true);
        if (!name.is_valid_ascii_identifier() || String(property.name).contains("/")) continue;
        const Variant value = owner->get(property.name);
        before.insert(property.name, value);
        language->push_variant(L, value);
        lua_setfield(L, -2, name.utf8().get_data());
    }
    lua_getfield(L, -1, method.utf8().get_data());
    lua_pushvalue(L, -2);
    for (int i = 0; i < p_count; ++i) language->push_variant(L, *p_args[i]);
    if (lua_pcall(L, p_count + 1, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1));
        ERR_PRINT(get_script()->get_path() + ": " + language->last_error);
        lua_settop(L, top);
        return {};
    }
    const Variant result = language->to_variant(L, -1);
    lua_pop(L, 1);
    for (const auto &property : before) {
        lua_getfield(L, -1, LuauLanguage::api_name(property.key, true).utf8().get_data());
        const Variant after = language->to_variant(L, -1);
        lua_pop(L, 1);
        if (after != property.value) owner->set(property.key, after);
    }
    lua_settop(L, top);
    r_error.error = Callable::CallError::CALL_OK;
    return result;
}
void LuauScriptInstance::notification(int p_notification, bool p_reversed) {
    if (has_method("_notification")) { const Variant arg = p_notification; const Variant *args[] = {&arg}; Callable::CallError error; callp("_notification", args, 1, error); }
}
ScriptLanguage *LuauScriptInstance::get_language() { return LuauLanguage::get_singleton(); }

Vector<String> LuauLanguage::get_reserved_words() const { return {"and", "break", "continue", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "const", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while", "class", "extends", "export", "private", "public", "import", "type", "from", "as", "to", "step"}; }
bool LuauLanguage::validate(const String &p_script, const String &p_path, List<String> *r_functions, List<ScriptError> *r_errors, List<Warning> *r_warnings, HashSet<int> *r_safe_lines) const {
    const CharString text = p_script.utf8();
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);
    const auto parsed = Luau::Parser::parse(text.get_data(), text.length(), names, allocator);
    for (const auto &error : parsed.errors) {
        if (r_errors != nullptr) { ScriptError info; info.path = p_path; info.line = error.getLocation().begin.line + 1; info.column = error.getLocation().begin.column + 1; info.message = String::utf8(error.what()); r_errors->push_back(info); }
    }
#ifdef TOOLS_ENABLED
    if (parsed.errors.empty()) return ludot_validate(p_script, p_path, r_errors);
#endif
    return parsed.errors.empty();
}
Error LuauLanguage::complete_code(const String &p_code, const String &p_path, Object *p_owner, List<CodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) {
#ifdef TOOLS_ENABLED
    r_force = true;
    return ludot_complete(p_code, p_path, r_options);
#else
    return ERR_UNAVAILABLE;
#endif
}
Ref<Script> LuauLanguage::make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const {
    Ref<LuauScript> script;
    script.instantiate();
    script->set_source_code("class " + p_class_name + " extends " + p_base_class_name + " = {\n    function _ready()\n        print(\"Ready!\")\n    end\n}\n");
    return script;
}
int LuauLanguage::find_function(const String &p_function, const String &p_code) const {
    const PackedStringArray lines = p_code.split("\n");
    for (int i = 0; i < lines.size(); ++i) if (lines[i].contains("function " + p_function + "(")) return i + 1;
    return -1;
}
String LuauLanguage::make_function(const String &p_class, const String &p_name, const PackedStringArray &p_args) const {
    return "    function " + api_name(p_name) + "(" + String(", ").join(p_args) + ")\n    end\n";
}
void LuauLanguage::add_global_constant(const StringName &p_variable, const Variant &p_value) { if (state != nullptr) { MutexLock lock(mutex); push_variant(state, p_value); lua_setglobal(state, String(p_variable).utf8().get_data()); } }
void LuauLanguage::reload_all_scripts() { for (LuauScript *script : scripts) script->reload(true); }
void LuauLanguage::reload_scripts(const Array &p_scripts, bool p_soft_reload) { for (int i = 0; i < p_scripts.size(); ++i) { const Ref<LuauScript> script = p_scripts[i]; if (script.is_valid()) script->reload(p_soft_reload); } }
int LuauLanguage::debug_get_stack_level_count() const { return state != nullptr ? lua_stackdepth(debug_state != nullptr ? debug_state : state) : 0; }
int LuauLanguage::debug_get_stack_level_line(int p_level) const { lua_Debug info = {}; return lua_getinfo(debug_state != nullptr ? debug_state : state, p_level, "l", &info) != 0 ? info.currentline : -1; }
String LuauLanguage::debug_get_stack_level_function(int p_level) const { lua_Debug info = {}; lua_getinfo(debug_state != nullptr ? debug_state : state, p_level, "n", &info); return info.name != nullptr ? String::utf8(info.name) : ""; }
String LuauLanguage::debug_get_stack_level_source(int p_level) const { lua_Debug info = {}; lua_getinfo(debug_state != nullptr ? debug_state : state, p_level, "s", &info); return info.source != nullptr ? String::utf8(info.source).trim_prefix("@") : ""; }
void LuauLanguage::debug_get_stack_level_locals(int p_level, List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) {
    for (int i = 1;; ++i) { const char *name = lua_getlocal(debug_state != nullptr ? debug_state : state, p_level, i); if (name == nullptr) break; r_names->push_back(String::utf8(name)); r_values->push_back(to_variant(debug_state != nullptr ? debug_state : state, -1)); lua_pop(debug_state != nullptr ? debug_state : state, 1); }
}
Ref<Resource> ResourceFormatLoaderLuau::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
    Error error;
    const Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ, &error);
    if (file.is_null()) { if (r_error != nullptr) *r_error = error; return {}; }
    Ref<LuauScript> script;
    script.instantiate();
    script->set_path(p_original_path.is_empty() ? p_path : p_original_path);
    script->set_source_code(file->get_as_text());
    error = script->reload();
    if (r_error != nullptr) *r_error = error;
    if (r_progress != nullptr) *r_progress = 1.0;
    return script;
}
Error ResourceFormatSaverLuau::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
    const Ref<LuauScript> script = p_resource;
    Error error;
    const Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE, &error);
    if (file.is_null()) return error;
    file->store_string(script->get_source_code());
    return file->get_error();
}
