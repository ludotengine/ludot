#include "luau_script.h"
#include "luau_callable.h"
#include "core/object/method_bind.h"
#ifdef TOOLS_ENABLED
#include "luau_analysis.h"
#endif
#include "Luau/Compiler.h"
#include "Luau/ExperimentalFlags.h"
#include "Luau/Parser.h"
#include "Luau/PrettyPrinter.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/io/file_access.h"
#include "core/os/os.h"
#include "core/io/dir_access.h"
#include "core/io/json.h"
#include "core/object/class_db.h"
#include "core/debugger/script_debugger.h"
#include "core/debugger/engine_debugger.h"
#include "lualib.h"
#include "ludot.h"
#include "lobject.h"
#include "lstate.h"
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
struct ModuleEvaluation {
    LuauLanguage *language;
    String path;
    HashSet<String> previous;
    bool committed = false;
    ModuleEvaluation(LuauLanguage *p_language, const String &p_path) : language(p_language), path(p_path) {
        if (const auto *dependencies = language->module_dependencies.getptr(path)) previous = *dependencies;
        remove_edges();
    }
    void remove_edges() {
        if (const auto *dependencies = language->module_dependencies.getptr(path)) {
            for (const String &dependency : *dependencies) {
                auto *importers = language->module_dependents.getptr(dependency);
                if (importers != nullptr) {
                    importers->erase(path);
                    if (importers->is_empty()) language->module_dependents.erase(dependency);
                }
            }
        }
        language->module_dependencies.erase(path);
    }
    ~ModuleEvaluation() {
        if (committed) return;
        remove_edges();
        language->module_dependencies.insert(path, previous);
        for (const String &dependency : previous) language->module_dependents[dependency].insert(path);
    }
};
void record_dependency(LuauLanguage *language, const String &from, const String &path) {
    language->module_dependents[path].insert(from);
    language->module_dependencies[from].insert(path);
}
String annotation_key(Luau::AstType *type) { return type != nullptr ? String::utf8(Luau::toString(type).c_str()) : String(); }
constexpr const char *VARIANT_META = "Ludot.Variant";
struct NativeValue {
    Variant value;
    ObjectID owner;
};
Variant variant_at(lua_State *L, int index) {
    const auto *native = static_cast<NativeValue *>(luaL_checkudata(L, index, VARIANT_META));
    if (native->owner.is_valid()) return ObjectDB::get_instance(native->owner);
    return native->value;
}
void update_variant(lua_State *L, int index, const Variant &value) {
    auto *native = static_cast<NativeValue *>(luaL_checkudata(L, index, VARIANT_META));
    if (native->owner.is_null()) native->value = value;
}
void destroy_variant(lua_State *L, void *pointer) { static_cast<NativeValue *>(pointer)->~NativeValue(); }
void push_owner(lua_State *L, Object *owner) {
    new (lua_newuserdatadtor(L, sizeof(NativeValue), destroy_variant)) NativeValue{Variant(), owner->get_instance_id()};
    luaL_getmetatable(L, VARIANT_META);
    lua_setmetatable(L, -2);
}
Variant native_argument(lua_State *L, int index, Variant::Type expected) {
    if (lua_istable(L, index) && expected >= Variant::ARRAY) {
        const int absolute = lua_absindex(L, index);
        const int size = lua_objlen(L, absolute);
        Array array;
        array.resize(size);
        lua_pushnil(L);
        int entries = 0;
        while (lua_next(L, absolute) != 0) {
            if (!lua_isnumber(L, -2)) luaL_error(L, "An engine array requires consecutive integer keys starting at 1");
            const double key = lua_tonumber(L, -2);
            if (key < 1 || key > size || key != int(key)) luaL_error(L, "An engine array requires consecutive integer keys starting at 1");
            array[int(key) - 1] = LuauLanguage::to_variant(L, -1);
            ++entries;
            lua_pop(L, 1);
        }
        if (entries != size) luaL_error(L, "An engine array cannot contain holes");
        if (expected == Variant::ARRAY) return array;
        const Variant value = array;
        const Variant *args[] = {&value};
        Variant converted;
        Callable::CallError error;
        Variant::construct(expected, converted, args, 1, error);
        if (error.error != Callable::CallError::CALL_OK) luaL_error(L, "Invalid packed array element type");
        return converted;
    }
    return LuauLanguage::to_variant(L, index);
}
int native_static_call(lua_State *L) {
    const StringName method = String::utf8(lua_tostring(L, lua_upvalueindex(2)));
    const bool builtin = lua_isnumber(L, lua_upvalueindex(1));
    const auto type = builtin ? Variant::Type(lua_tointeger(L, lua_upvalueindex(1))) : Variant::NIL;
    MethodBind *bind = builtin ? nullptr : ClassDB::get_method(String::utf8(lua_tostring(L, lua_upvalueindex(1))), method);
    const int count = lua_gettop(L);
    Vector<Variant> values;
    Vector<const Variant *> pointers;
    values.resize(count); pointers.resize(count);
    for (int i = 0; i < count; ++i) {
        Variant::Type expected = Variant::NIL;
        if (builtin && i < Variant::get_builtin_method_argument_count(type, method)) expected = Variant::get_builtin_method_argument_type(type, method, i);
        else if (bind != nullptr && i < bind->get_argument_count()) expected = bind->get_argument_info(i).type;
        values.write[i] = native_argument(L, i + 1, expected); pointers.write[i] = &values[i];
    }
    Variant result;
    Callable::CallError error;
    if (builtin) Variant::call_static(type, method, pointers.ptrw(), count, result, error);
    else if (bind != nullptr) result = bind->call(nullptr, pointers.ptrw(), count, error);
    else luaL_error(L, "Unknown static native method");
    if (error.error != Callable::CallError::CALL_OK) luaL_error(L, "%s", Variant::get_call_error_text(method, pointers.ptrw(), count, error).utf8().get_data());
    LuauLanguage::push_variant(L, result);
    return 1;
}
int native_length(lua_State *L) {
    Variant self = variant_at(L, 1);
    if (!self.is_array() && self.get_type() != Variant::DICTIONARY) luaL_error(L, "Only native collections have a length");
    Variant result;
    Callable::CallError error;
    self.callp("size", nullptr, 0, result, error);
    lua_pushnumber(L, int64_t(result));
    return 1;
}
int native_iterator_next(lua_State *L) {
    Variant self = variant_at(L, lua_upvalueindex(1));
    const int index = lua_tointeger(L, lua_upvalueindex(3));
    Variant key;
    if (self.get_type() == Variant::DICTIONARY) {
        const Array keys = variant_at(L, lua_upvalueindex(2));
        if (index >= keys.size()) return 0;
        key = keys[index];
    } else {
        Variant size;
        Callable::CallError error;
        self.callp("size", nullptr, 0, size, error);
        if (index >= int64_t(size)) return 0;
        key = index;
    }
    bool valid;
    const Variant value = self.get(key, &valid);
    LuauLanguage::push_variant(L, self.is_array() ? Variant(index + 1) : key);
    LuauLanguage::push_variant(L, value);
    lua_pushinteger(L, index + 1);
    lua_replace(L, lua_upvalueindex(3));
    return 2;
}
int native_iterate(lua_State *L) {
    Variant self = variant_at(L, 1);
    if (!self.is_array() && self.get_type() != Variant::DICTIONARY) luaL_error(L, "Only native collections can be iterated");
    lua_pushvalue(L, 1);
    if (self.get_type() == Variant::DICTIONARY) LuauLanguage::push_variant(L, Dictionary(self).keys());
    else lua_pushnil(L);
    lua_pushinteger(L, 0);
    lua_pushcclosure(L, native_iterator_next, "native collection iterator", 3);
    return 1;
}
int native_call(lua_State *L) {
    Variant self = variant_at(L, 1);
    const StringName method = String::utf8(lua_tostring(L, lua_upvalueindex(1)));
    const int count = lua_gettop(L) - 1;
    Vector<Variant> values;
    Vector<const Variant *> pointers;
    values.resize(count);
    pointers.resize(count);
    MethodBind *bind = nullptr;
    if (self.get_type() == Variant::OBJECT) {
        Object *object = self;
        if (object == nullptr) luaL_error(L, "The native object has been freed");
        bind = ClassDB::get_method(object->get_class_name(), method);
    }
    for (int i = 0; i < count; ++i) {
        Variant::Type expected = Variant::NIL;
        if (bind != nullptr && i < bind->get_argument_count()) expected = bind->get_argument_info(i).type;
        else if (self.get_type() != Variant::OBJECT && i < Variant::get_builtin_method_argument_count(self.get_type(), method)) expected = Variant::get_builtin_method_argument_type(self.get_type(), method, i);
        values.write[i] = native_argument(L, i + 2, expected); pointers.write[i] = &values[i];
    }
    Variant result;
    Callable::CallError error;
    auto language = LuauLanguage::get_singleton();
    lua_State *previous = language->active_state;
    language->active_state = L;
    const bool profiled = language->profiling && language->profile_native_calls;
    if (profiled) language->profile_enter();
    self.callp(method, pointers.ptrw(), count, result, error);
    if (profiled) {
        // Attribute the call to the first script frame above the generated native bindings.
        String source;
        lua_Debug info = {};
        for (int level = 1; lua_getinfo(L, level, "s", &info); ++level) {
            source = info.source != nullptr ? String::utf8(info.source).trim_prefix("@") : String();
            if (!source.begins_with("native/")) break;
        }
        const String type = self.get_type() == Variant::OBJECT ? String(self.operator Object *()->get_class_name()) : Variant::get_type_name(self.get_type());
        language->profile_exit(source + "::0::" + type + "." + LuauLanguage::api_name(method), true);
    }
    language->active_state = previous;
    update_variant(L, 1, self);
    if (error.error != Callable::CallError::CALL_OK) luaL_error(L, "%s", Variant::get_call_error_text(method, pointers.ptrw(), count, error).utf8().get_data());
    LuauLanguage::push_variant(L, result);
    return 1;
}
LuauLanguage::NativeMember *find_native_member(const Variant &p_self, const String &p_requested) {
    auto language = LuauLanguage::get_singleton();
    const Variant::Type type = p_self.get_type();
    const StringName key = type == Variant::OBJECT ? static_cast<Object *>(p_self)->get_class_name() : StringName(Variant::get_type_name(type));
    HashMap<String, LuauLanguage::NativeMember> *members = language->native_members.getptr(key);
    if (members == nullptr) {
        members = &language->native_members.insert(key, {})->value;
        // Later kinds win, matching the property, signal, method lookup order of the dynamic scan.
        if (type == Variant::OBJECT) {
            List<MethodInfo> methods;
            ClassDB::get_method_list(key, &methods);
            for (const MethodInfo &method : methods) (*members)[LuauLanguage::api_name(method.name)] = { LuauLanguage::NativeMember::METHOD, method.name };
            List<MethodInfo> signals;
            ClassDB::get_signal_list(key, &signals);
            for (const MethodInfo &signal : signals) (*members)[LuauLanguage::api_name(signal.name, true)] = { LuauLanguage::NativeMember::SIGNAL, signal.name };
            List<PropertyInfo> properties;
            ClassDB::get_property_list(key, &properties);
            for (const PropertyInfo &property : properties) {
                if (property.usage & (PROPERTY_USAGE_GROUP | PROPERTY_USAGE_SUBGROUP | PROPERTY_USAGE_CATEGORY)) continue;
                (*members)[LuauLanguage::api_name(property.name, true)] = { LuauLanguage::NativeMember::PROPERTY, property.name, property.type };
            }
        } else {
            List<StringName> methods;
            Variant::get_builtin_method_list(type, &methods);
            for (const StringName &method : methods) (*members)[LuauLanguage::api_name(method)] = { LuauLanguage::NativeMember::METHOD, method };
            List<StringName> properties;
            Variant::get_member_list(type, &properties);
            for (const StringName &property : properties) (*members)[LuauLanguage::api_name(property, true)] = { LuauLanguage::NativeMember::PROPERTY, property, Variant::get_member_type(type, property) };
        }
    }
    return members->getptr(p_requested);
}
int native_index(lua_State *L) {
    Variant self = variant_at(L, 1);
    if (lua_type(L, 2) == LUA_TNUMBER) {
        bool valid;
        const Variant key = self.is_array() ? Variant(lua_tointeger(L, 2) - 1) : LuauLanguage::to_variant(L, 2);
        const Variant result = self.get(key, &valid);
        if (!valid) luaL_error(L, "Invalid native index");
        LuauLanguage::push_variant(L, result);
        return 1;
    }
    const String requested = String::utf8(luaL_checkstring(L, 2));
    if (self.get_type() == Variant::DICTIONARY && Dictionary(self).has(requested)) {
        LuauLanguage::push_variant(L, Dictionary(self)[requested]);
        return 1;
    }
    if (self.get_type() == Variant::OBJECT && static_cast<Object *>(self) == nullptr) luaL_error(L, "The native object has been freed");
    if (LuauLanguage::NativeMember *member = find_native_member(self, requested)) {
        switch (member->kind) {
            case LuauLanguage::NativeMember::PROPERTY: {
                bool valid;
                LuauLanguage::push_variant(L, self.get_named(member->name, valid));
                return 1;
            }
            case LuauLanguage::NativeMember::SIGNAL:
                LuauLanguage::push_variant(L, Signal(static_cast<Object *>(self), member->name));
                return 1;
            case LuauLanguage::NativeMember::METHOD:
                if (member->method_ref != LUA_NOREF) {
                    lua_getref(L, member->method_ref);
                    return 1;
                }
                lua_pushstring(L, String(member->name).utf8().get_data());
                lua_pushcclosure(L, native_call, "native method", 1);
                member->method_ref = lua_ref(L, -1);
                return 1;
        }
    }
    List<PropertyInfo> properties;
    self.get_property_list(&properties);
    for (const PropertyInfo &property : properties) {
        if (LuauLanguage::api_name(property.name, true) == requested) {
            bool valid;
            LuauLanguage::push_variant(L, self.get_named(property.name, valid));
            return 1;
        }
    }
    if (self.get_type() == Variant::OBJECT) {
        Object *object = self;
        if (object == nullptr) luaL_error(L, "The native object has been freed");
        List<MethodInfo> signals;
        object->get_signal_list(&signals);
        for (const auto &signal : signals) {
            if (LuauLanguage::api_name(signal.name, true) == requested) {
                LuauLanguage::push_variant(L, Signal(object, signal.name));
                return 1;
            }
        }
    }
    List<MethodInfo> methods;
    self.get_method_list(&methods);
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
    Variant self = variant_at(L, 1);
    if (lua_type(L, 2) == LUA_TNUMBER) {
        bool valid;
        const Variant key = self.is_array() ? Variant(lua_tointeger(L, 2) - 1) : LuauLanguage::to_variant(L, 2);
        self.set(key, LuauLanguage::to_variant(L, 3), &valid);
        if (!valid) luaL_error(L, "Invalid native index assignment");
        update_variant(L, 1, self);
        return 0;
    }
    const String requested = String::utf8(luaL_checkstring(L, 2));
    if (self.get_type() == Variant::OBJECT && static_cast<Object *>(self) == nullptr) luaL_error(L, "The native object has been freed");
    if (const LuauLanguage::NativeMember *member = find_native_member(self, requested); member != nullptr && member->kind == LuauLanguage::NativeMember::PROPERTY) {
        bool valid;
        self.set_named(member->name, native_argument(L, 3, member->type), valid);
        if (!valid) luaL_error(L, "Invalid native property assignment");
        update_variant(L, 1, self);
        return 0;
    }
    List<PropertyInfo> properties;
    self.get_property_list(&properties);
    for (const PropertyInfo &property : properties) {
        if (LuauLanguage::api_name(property.name, true) == requested) {
            bool valid;
            self.set_named(property.name, native_argument(L, 3, property.type), valid);
            if (!valid) luaL_error(L, "Invalid native property assignment");
            update_variant(L, 1, self);
            return 0;
        }
    }
    if (self.get_type() == Variant::DICTIONARY) {
        Dictionary dictionary = self;
        dictionary[requested] = LuauLanguage::to_variant(L, 3);
        update_variant(L, 1, dictionary);
        return 0;
    }
    luaL_error(L, "Unknown native property '%s'", requested.utf8().get_data());
}
int native_property_get(lua_State *L) {
    lua_getfield(L, 1, "_native");
    Variant owner = variant_at(L, -1);
    bool valid;
    LuauLanguage::push_variant(L, owner.get_named(String::utf8(lua_tostring(L, lua_upvalueindex(1))), valid));
    return 1;
}
int native_signal_get(lua_State *L) {
    lua_getfield(L, 1, "_native");
    const Variant owner = variant_at(L, -1);
    Object *object = owner;
    if (object == nullptr) luaL_error(L, "The native object has been freed");
    LuauLanguage::push_variant(L, Signal(object, String::utf8(lua_tostring(L, lua_upvalueindex(1)))));
    return 1;
}
int native_property_set(lua_State *L) {
    const Variant value = LuauLanguage::to_variant(L, 3);
    lua_getfield(L, 1, "_native");
    Variant owner = variant_at(L, -1);
    bool valid;
    owner.set_named(String::utf8(lua_tostring(L, lua_upvalueindex(1))), value, valid);
    if (!valid) luaL_error(L, "Invalid native property assignment");
    return 0;
}
int native_construct(lua_State *L) {
    const Variant::Type type = Variant::Type(lua_tointeger(L, lua_upvalueindex(1)));
    const int count = lua_gettop(L) - 1;
    Vector<Variant> values;
    Vector<const Variant *> pointers;
    values.resize(count);
    pointers.resize(count);
    for (int i = 0; i < count; ++i) { values.write[i] = native_argument(L, i + 2, type >= Variant::ARRAY ? type : Variant::NIL); pointers.write[i] = &values[i]; }
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
    auto language = LuauLanguage::get_singleton();
    Object *object = language->construction_owner != nullptr ? language->construction_owner : ClassDB::instantiate(name);
    if (object == nullptr) luaL_error(L, "Cannot instantiate native class '%s'", String(name).utf8().get_data());
    if (language->construction_owner != nullptr) push_owner(L, object);
    else LuauLanguage::push_variant(L, object);
    return 1;
}
int native_global(lua_State *L) {
    const StringName name = String::utf8(luaL_checkstring(L, 2));
    auto language = LuauLanguage::get_singleton();
    if (!ClassDB::class_exists(name)) {
        if (ScriptServer::is_global_class(name)) {
            const String path = ScriptServer::get_global_class_path(name);
            const Ref<LuauScript> script = ResourceLoader::load(path, "LuauScript");
            if (script.is_valid() && script->get_class_reference() != LUA_NOREF) {
                language->module_resources.insert(path, script);
                lua_getref(L, script->get_class_reference());
                lua_Debug debug = {};
                lua_getinfo(L, 1, "s", &debug);
                if (debug.source != nullptr) record_dependency(language, String::utf8(debug.source).trim_prefix("@"), path);
                return 1;
            }
        }
        lua_pushnil(L); return 1;
    }
    if (language->ensure_native_class(name) != OK) luaL_error(L, "%s", language->last_error.utf8().get_data());
    lua_getglobal(L, String(name).utf8().get_data());
    return 1;
}
int native_tostring(lua_State *L) { lua_pushstring(L, String(variant_at(L, 1)).utf8().get_data()); return 1; }
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
    record_dependency(language, from, path);
    if (const int *cached = language->module_cache.getptr(path)) { lua_getref(L, *cached); return 1; }
    if (language->loading.has(path)) luaL_error(L, "Cyclic import of '%s'", path.utf8().get_data());
    language->loading.insert(path);
    Error error;
    Ref<LuauScript> module = ResourceLoader::load(path, "LuauScript", ResourceFormatLoader::CACHE_MODE_REUSE, &error);
    if (error == OK && module.is_valid() && module->get_class_reference() != LUA_NOREF) {
        lua_getref(L, module->get_class_reference());
        language->module_resources.insert(path, module);
        language->loading.erase(path);
        language->module_cache.insert(path, lua_ref(L, -1));
        return 1;
    }
    if (error == OK && module.is_valid()) error = language->load_chunk(module->get_source_code(), path, L);
    else { language->last_error = "Cannot load module '" + path + "'"; error = ERR_CANT_OPEN; }
    ModuleEvaluation evaluation(language, path);
    if (error == OK && lua_pcall(L, 0, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1));
        lua_pop(L, 1);
        error = ERR_SCRIPT_FAILED;
    }
    language->loading.erase(path);
    if (error != OK) luaL_error(L, "%s", language->last_error.utf8().get_data());
    evaluation.committed = true;
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
    const String renamed = p_property && !internal ? body.to_pascal_case() : body.to_camel_case();
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
    new (lua_newuserdatadtor(L, sizeof(NativeValue), destroy_variant)) NativeValue{p_value, ObjectID()};
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
        case LUA_TFUNCTION: return ludot_callable(L, p_index);
        case LUA_TUSERDATA: return variant_at(L, p_index);
        case LUA_TOBJECT: {
            lua_getfield(L, p_index, "_native");
            Variant result = lua_isuserdata(L, -1) ? variant_at(L, -1) : Variant();
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
void debug_step(lua_State *L, lua_Debug *) {
    auto debugger = EngineDebugger::get_script_debugger();
    if (debugger == nullptr) return;
    lua_Debug info = {};
    if (!lua_getinfo(L, 0, "slp", &info) || info.currentline <= 0) return;
    const String source = String::utf8(info.source != nullptr ? info.source : "").trim_prefix("@");
    if (source.begins_with("native/")) return;
    auto language = LuauLanguage::get_singleton();
    const int depth = lua_stackdepth(L);
    auto &cursor = language->debug_cursors[L];
    const void *pc = L->ci->savedpc;
    const bool same_line = source == cursor.source && info.currentline == cursor.line && depth == cursor.depth && info.protoid == cursor.prototype && uintptr_t(pc) > uintptr_t(cursor.pc);
    const int previous_depth = cursor.pc != nullptr ? cursor.depth : depth;
    if (debugger->get_depth() >= 0 && debugger->get_lines_left() > 0 && (language->stepping_state == nullptr || language->stepping_state == L))
        debugger->set_depth(debugger->get_depth() + depth - previous_depth);
    cursor = {source, info.currentline, depth, info.protoid, pc};
    if (same_line) return;
    bool stepping = false;
    if (debugger->get_lines_left() > 0 && debugger->get_depth() <= 0 && (debugger->get_depth() < 0 || language->stepping_state == nullptr || language->stepping_state == L)) {
        debugger->set_lines_left(debugger->get_lines_left() - 1);
        stepping = debugger->get_lines_left() == 0;
    }
    if (stepping || debugger->is_breakpoint(info.currentline, source)) {
        language->debug_state = L;
        language->stepping_state = L;
        debugger->debug(language, true);
        if (debugger->get_lines_left() < 0) language->stepping_state = nullptr;
        language->debug_state = nullptr;
    }
}
void debug_thread(lua_State *parent, lua_State *thread) {
    if (parent != nullptr) return;
    auto language = LuauLanguage::get_singleton();
    language->debug_cursors.erase(thread);
    if (language->stepping_state == thread) language->stepping_state = nullptr;
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
namespace {
struct VariantConversion { int index; Variant value; };
void convert_value(lua_State *L, void *context) {
    auto conversion = static_cast<VariantConversion *>(context);
    conversion->value = LuauLanguage::to_variant(L, conversion->index);
}
}
bool LuauLanguage::try_variant(lua_State *L, int index, Variant &value) {
    const int top = lua_gettop(L);
    VariantConversion conversion{lua_absindex(L, index), Variant()};
    const int status = lua_rawrunprotected(L, convert_value, &conversion);
    if (status == LUA_OK) value = conversion.value;
    else LuauLanguage::get_singleton()->last_error = String::utf8(lua_tostring(L, -1));
    lua_settop(L, top);
    return status == LUA_OK;
}
void LuauLanguage::profile_enter() {
    ProfileCall call;
    call.start = OS::get_singleton()->get_ticks_usec();
    profile_calls.push_back(call);
}
void LuauLanguage::profile_exit(const String &p_signature, bool p_native) {
    const ProfileCall call = profile_calls[profile_calls.size() - 1];
    profile_calls.remove_at(profile_calls.size() - 1);
    const uint64_t elapsed = OS::get_singleton()->get_ticks_usec() - call.start;
    if (!profile_calls.is_empty()) {
        profile_calls[profile_calls.size() - 1].children += elapsed;
        if (p_native) profile_calls[profile_calls.size() - 1].native += elapsed;
    }
    for (HashMap<String, Profile> *table : { &profile_accumulated, &profile_frame }) {
        Profile &entry = (*table)[p_signature];
        entry.call_count++;
        entry.total_time += elapsed;
        entry.self_time += elapsed - call.children;
        entry.internal_time += p_native ? elapsed : call.native;
    }
}
void LuauLanguage::profiling_start() {
    MutexLock lock(mutex);
    profile_accumulated.clear();
    profile_frame.clear();
    profile_last_frame.clear();
    profiling = true;
}
void LuauLanguage::profiling_stop() {
    MutexLock lock(mutex);
    profiling = false;
}
void LuauLanguage::profiling_set_save_native_calls(bool p_enable) {
    MutexLock lock(mutex);
    profile_native_calls = p_enable;
}
namespace {
int write_profile(const HashMap<String, LuauLanguage::Profile> &p_table, ScriptLanguage::ProfilingInfo *p_info, int p_max) {
    int count = 0;
    for (const KeyValue<String, LuauLanguage::Profile> &entry : p_table) {
        if (count >= p_max) break;
        p_info[count].signature = entry.key;
        p_info[count].call_count = entry.value.call_count;
        p_info[count].total_time = entry.value.total_time;
        p_info[count].self_time = entry.value.self_time;
        p_info[count].internal_time = entry.value.internal_time;
        count++;
    }
    return count;
}
}
int LuauLanguage::profiling_get_accumulated_data(ProfilingInfo *p_info, int p_max) {
    MutexLock lock(mutex);
    return write_profile(profile_accumulated, p_info, p_max);
}
int LuauLanguage::profiling_get_frame_data(ProfilingInfo *p_info, int p_max) {
    MutexLock lock(mutex);
    return write_profile(profile_last_frame, p_info, p_max);
}
void LuauLanguage::frame() {
    if (!profiling) return;
    MutexLock lock(mutex);
    profile_last_frame = profile_frame;
    profile_frame.clear();
}
void LuauLanguage::init() {
    static uint64_t next_generation = 0;
    generation = ++next_generation;
    state = luaL_newstate();
    luaL_openlibs(state);
    if (EngineDebugger::is_active()) {
        lua_callbacks(state)->debugstep = debug_step;
        lua_callbacks(state)->userthread = debug_thread;
        lua_callbacks(state)->debugprotectederror = debug_error;
        lua_singlestep(state, true);
    }
    luaL_newmetatable(state, VARIANT_META);
    lua_pushcfunction(state, native_index, "native index"); lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, native_newindex, "native assignment"); lua_setfield(state, -2, "__newindex");
    lua_pushcfunction(state, native_length, "native collection length"); lua_setfield(state, -2, "__len");
    lua_pushcfunction(state, native_iterate, "native collection iteration"); lua_setfield(state, -2, "__iter");
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
        if (i == Variant::OBJECT) continue;
        const auto type = Variant::Type(i);
        lua_newtable(state);
        List<StringName> constants;
        Variant::get_constants_for_type(type, &constants);
        for (const StringName &constant : constants) {
            push_variant(state, Variant::get_constant_value(type, constant));
            lua_setfield(state, -2, api_name(constant, true).utf8().get_data());
        }
        List<StringName> methods;
        Variant::get_builtin_method_list(type, &methods);
        for (const StringName &method : methods) {
            if (!Variant::is_builtin_method_static(type, method)) continue;
            lua_pushinteger(state, i);
            lua_pushstring(state, String(method).utf8().get_data());
            lua_pushcclosure(state, native_static_call, "native static method", 2);
            lua_setfield(state, -2, api_name(method).utf8().get_data());
        }
        lua_newtable(state);
        lua_pushinteger(state, i);
        lua_pushcclosure(state, native_construct, "native constructor", 1);
        lua_setfield(state, -2, "__call");
        lua_setmetatable(state, -2);
        lua_setreadonly(state, -1, true);
        lua_setglobal(state, Variant::get_type_name(type).utf8().get_data());
    }
    List<Engine::Singleton> singletons;
    Engine::get_singleton()->get_singletons(&singletons);
    for (const auto &entry : singletons) { push_variant(state, entry.ptr); lua_setglobal(state, String(entry.name).utf8().get_data()); }
    GLOBAL_DEF("luau/modules/aliases", Dictionary());
    GLOBAL_DEF("luau/editor/require_autocomplete", true);
}
void LuauLanguage::finish() {
#ifdef TOOLS_ENABLED
    ludot_clear_analysis();
#endif
    module_cache.clear();
    module_resources.clear();
    module_dependents.clear();
    module_dependencies.clear();
    native_classes.clear();
    native_members.clear();
    debug_cursors.clear();
    stepping_state = nullptr;
    if (state != nullptr) { lua_State *closing = state; state = nullptr; lua_close(closing); }
}
Error LuauLanguage::load_chunk(const String &p_source, const String &p_path, lua_State *p_state) {
    lua_State *L = p_state != nullptr ? p_state : state;
    const CharString source = p_source.utf8();
    Luau::CompileOptions options;
    options.debugLevel = 2;
    // Optimization folds constant locals out of registers, hiding them from the debugger.
    options.optimizationLevel = EngineDebugger::is_active() ? 0 : 1;
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
        module_resources.erase(path);
    }
    return affected;
}
Dictionary LuauLanguage::module_aliases(const String &p_from) const {
    Dictionary aliases;
    const Dictionary project_aliases = GLOBAL_GET("luau/modules/aliases");
    for (const Variant &key : project_aliases.keys()) {
        const String target = project_aliases[key];
        aliases[String(key).to_lower()] = target.begins_with("res://") ? target : String("res://").path_join(target).simplify_path();
    }
    Vector<String> directories;
    String directory = p_from.get_base_dir();
    while (directory.begins_with("res://")) {
        directories.push_back(directory);
        if (directory == "res://") break;
        directory = directory.get_base_dir();
    }
    for (int i = directories.size() - 1; i >= 0; --i) {
        const String config_path = directories[i].path_join(".luaurc");
        if (!FileAccess::exists(config_path)) continue;
        const Variant parsed = JSON::parse_string(FileAccess::get_file_as_string(config_path));
        if (parsed.get_type() != Variant::DICTIONARY) continue;
        const Dictionary config = parsed;
        if (!config.has("aliases")) continue;
        const Dictionary configured = config["aliases"];
        for (const Variant &key : configured.keys()) {
            const String target = configured[key];
            aliases[String(key).to_lower()] = target.begins_with("res://") ? target : directories[i].path_join(target).simplify_path();
        }
    }
    return aliases;
}
String LuauLanguage::resolve_module(const String &p_requested, const String &p_from) const {
    String path;
    if (p_requested.begins_with("@")) {
        const int slash = p_requested.find("/");
        const String alias = p_requested.substr(1, slash == -1 ? p_requested.length() - 1 : slash - 1).to_lower();
        const Dictionary aliases = module_aliases(p_from);
        if (!aliases.has(alias)) return "";
        path = String(aliases[alias]);
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
    List<MethodInfo> signals;
    ClassDB::get_signal_list(p_name, &signals);
    for (const auto &signal : signals) {
        const String name = api_name(signal.name, true);
        if (name.is_valid_ascii_identifier() && names.insert(name).second) source += " public " + name + "\n";
    }
    List<String> constants;
    ClassDB::get_integer_constant_list(p_name, &constants);
    HashMap<String, int64_t> constant_values;
    for (const String &constant : constants) {
        const String name = api_name(constant, true);
        if (!name.is_valid_ascii_identifier() || !names.insert(name).second) continue;
        source += " public function " + name + "(self) end\n";
        constant_values.insert(name, ClassDB::get_integer_constant(p_name, constant));
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
    for (const auto &constant : constant_values) {
        lua_pushnumber(state, constant.value);
        lua_setclassmember(state, -2, constant.key.utf8().get_data());
    }
    for (const auto &method : methods) {
        if ((method.flags & METHOD_FLAG_STATIC) == 0) continue;
        const String name = api_name(method.name);
        if (!name.is_valid_ascii_identifier() || name.begins_with("__")) continue;
        lua_pushstring(state, String(p_name).utf8().get_data());
        lua_pushstring(state, String(method.name).utf8().get_data());
        lua_pushcclosure(state, native_static_call, "native static method", 2);
        lua_setclassmember(state, -2, name.utf8().get_data());
    }
    lua_newtable(state);
    lua_newtable(state);
    for (const auto &signal : signals) {
        const String name = api_name(signal.name, true);
        if (!name.is_valid_ascii_identifier()) continue;
        lua_pushstring(state, String(signal.name).utf8().get_data());
        lua_pushcclosure(state, native_signal_get, "native signal", 1);
        lua_setfield(state, -3, name.utf8().get_data());
    }
    for (const auto &property : properties) {
        const String name = api_name(property.name, true);
        if (String(property.name).contains("/") || !name.is_valid_ascii_identifier()) continue;
        lua_pushstring(state, String(property.name).utf8().get_data());
        lua_pushcclosure(state, native_property_get, "native property getter", 1);
        lua_setfield(state, -3, name.utf8().get_data());
        lua_pushstring(state, String(property.name).utf8().get_data());
        lua_pushcclosure(state, native_property_set, "native property setter", 1);
        lua_setfield(state, -2, name.utf8().get_data());
    }
    lua_setclassaccessors(state, -3);
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
    const bool was_valid = valid;
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
    if (declaration == nullptr) {
        const Error error = language->load_chunk(source, get_path());
        if (error != OK) return error;
        lua_pop(language->state, 1);
        const auto affected = was_valid ? language->invalidate_modules(get_path()) : HashSet<String>();
        valid = true;
        for (LuauScript *dependent : language->scripts) {
            if (dependent != this && dependent->can_instantiate() && affected.has(dependent->get_path())) dependent->reload(true);
        }
        return OK;
    }
    const StringName new_name = String::utf8(declaration->name->name.value);
    const bool has_return = parsed.root->body.size > 0 && parsed.root->body.data[parsed.root->body.size - 1]->is<Luau::AstStatReturn>();
    Error error = language->load_chunk(source + (has_return ? String() : "\nreturn " + String(new_name)), get_path());
    if (error != OK) { ERR_PRINT(language->last_error); return error; }
    lua_State *L = language->state;
    ModuleEvaluation evaluation(language, get_path());
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
        ERR_PRINT(language->last_error);
        return ERR_SCRIPT_FAILED;
    }
    if (!lua_isclass(L, -1)) {
        lua_pop(L, 1);
        evaluation.committed = true;
        valid = true;
        return OK;
    }
    StringName new_base;
    Ref<LuauScript> new_base_script;
    const auto *next_type = static_cast<const LuauClass *>(lua_topointer(L, -1));
    for (const LuauClass *parent = next_type->super; parent != nullptr; parent = parent->super) {
        const StringName name = String::utf8(getstr(parent->name));
        if (language->native_classes.has(name)) { new_base = name; break; }
        if (new_base_script.is_null()) {
            for (LuauScript *candidate : language->scripts) {
                if (candidate == this || candidate->class_ref == LUA_NOREF) continue;
                lua_getref(L, candidate->class_ref);
                const bool matches = lua_topointer(L, -1) == parent;
                lua_pop(L, 1);
                if (matches) { new_base_script = Ref<LuauScript>(candidate); break; }
            }
        }
    }
    for (LuauScriptInstance *instance : instances) {
        if (new_base.is_empty() || !instance->owner->is_class(new_base)) {
            lua_pop(L, 1);
            language->last_error = "The new base class is incompatible with an existing native owner";
            return ERR_INVALID_DATA;
        }
    }
    const int next_class = lua_ref(L, -1);
    lua_pop(L, 1);
    // Stage all replacement objects before changing any live instance.
    HashMap<LuauScriptInstance *, int> replacements;
    for (LuauScriptInstance *instance : instances) {
        lua_getref(L, next_class);
        Object *previous_owner = language->construction_owner;
        language->construction_owner = instance->owner;
        const int construction_status = lua_pcall(L, 0, 1, 0);
        language->construction_owner = previous_owner;
        if (construction_status != LUA_OK) {
            language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
            for (const auto &replacement : replacements) lua_unref(L, replacement.value);
            lua_unref(L, next_class);
            return ERR_SCRIPT_FAILED;
        }
        push_owner(L, instance->owner);
        lua_setfield(L, -2, "_native");
        const int next_object = lua_ref(L, -1);
        lua_pop(L, 1);
        replacements.insert(instance, next_object);
        if (p_keep_state) {
            for (const PropertyInfo &property : properties) {
                // A removed property is intentionally dropped during reload.
                bool remains = false;
                for (const auto &member : declaration->members) if (const auto prop = Luau::get_if<Luau::AstClassProperty>(&member)) if (String::utf8(prop->name.value) == String(property.name) && property_types.has(property.name) && annotation_key(prop->ty) == property_types[property.name]) remains = true;
                if (!remains && new_base_script.is_valid()) {
                    for (const PropertyInfo &inherited : new_base_script->properties) if (inherited.name == property.name && new_base_script->property_types.has(property.name) && property_types.has(property.name) && new_base_script->property_types[property.name] == property_types[property.name]) remains = true;
                }
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
    base_script = new_base_script;
    methods.clear(); properties.clear(); defaults.clear(); property_types.clear();
    if (base_script.is_valid()) {
        methods = base_script->methods;
        properties = base_script->properties;
        defaults = base_script->defaults;
        property_types = base_script->property_types;
    }
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
            property_types.insert(info.name, annotation_key(property->ty));
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
    for (const auto &replacement : replacements) {
        lua_getref(L, replacement.key->object_ref);
        lua_getref(L, replacement.value);
        lua_migrateobject(L, -2, -1);
        lua_pop(L, 2);
        lua_unref(L, replacement.value);
    }
    evaluation.committed = true;
    valid = true;
    emit_changed();
    if (was_valid) {
        const auto affected = language->invalidate_modules(get_path());
        Vector<Ref<LuauScript>> dependents;
        for (LuauScript *dependent : language->scripts)
            if (dependent != this && dependent->can_instantiate() && affected.has(dependent->get_path())) dependents.push_back(Ref<LuauScript>(dependent));
        for (const auto &dependent : dependents) dependent->reload(true);
    }
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
    Object *previous_owner = language->construction_owner;
    language->construction_owner = owner;
    const int construction_status = lua_pcall(L, 0, 1, 0);
    language->construction_owner = previous_owner;
    if (construction_status != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1)); lua_pop(L, 1);
        ERR_PRINT(language->last_error);
        return;
    }
    // Each object keeps its native owner; engine calls still use the original API.
    push_owner(L, owner);
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
    const bool converted = LuauLanguage::try_variant(language->state, -1, r_value);
    lua_pop(language->state, 2);
    return converted;
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
    lua_getref(L, object_ref);
    lua_getfield(L, -1, method.utf8().get_data());
    const bool profiled = language->profiling;
    String signature;
    if (profiled) {
        lua_Debug info = {};
        lua_getinfo(L, -1, "s", &info);
        signature = get_script()->get_path() + "::" + itos(info.linedefined) + "::" + String(script->class_name) + "." + method;
        language->profile_enter();
    }
    lua_pushvalue(L, -2);
    for (int i = 0; i < p_count; ++i) language->push_variant(L, *p_args[i]);
    const int status = lua_pcall(L, p_count + 1, 1, 0);
    if (profiled) language->profile_exit(signature, false);
    if (status != LUA_OK) {
        language->last_error = String::utf8(lua_tostring(L, -1));
        ERR_PRINT(get_script()->get_path() + ": " + language->last_error);
        lua_settop(L, top);
        return {};
    }
    Variant result;
    if (!LuauLanguage::try_variant(L, -1, result)) {
        ERR_PRINT(get_script()->get_path() + ": " + language->last_error);
        lua_settop(L, top);
        r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
        return {};
    }
    lua_settop(L, top);
    r_error.error = Callable::CallError::CALL_OK;
    return result;
}
void LuauScriptInstance::notification(int p_notification, bool p_reversed) {
    if (has_method("_notification")) { const Variant arg = p_notification; const Variant *args[] = {&arg}; Callable::CallError error; callp("_notification", args, 1, error); }
}
ScriptLanguage *LuauScriptInstance::get_language() { return LuauLanguage::get_singleton(); }

String LuauLanguage::get_global_class_name(const String &p_path, String *r_base_type, String *r_icon_path, bool *r_is_abstract, bool *r_is_tool) const {
    const CharString source = FileAccess::get_file_as_string(p_path).utf8();
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);
    const auto parsed = Luau::Parser::parse(source.get_data(), source.length(), names, allocator);
    if (!parsed.errors.empty()) return "";
    String name;
    for (auto stat : parsed.root->body) {
        if (auto declaration = stat->as<Luau::AstStatClass>()) {
            name = String::utf8(declaration->name->name.value);
            if (r_base_type != nullptr) {
                if (auto base = declaration->super != nullptr ? declaration->super->as<Luau::AstExprGlobal>() : nullptr) *r_base_type = String::utf8(base->name.value);
                else if (auto base = declaration->super != nullptr ? declaration->super->as<Luau::AstExprLocal>() : nullptr) *r_base_type = String::utf8(base->local->name.value);
                else *r_base_type = "RefCounted";
            }
        }
    }
    if (r_icon_path != nullptr) *r_icon_path = "";
    if (r_is_abstract != nullptr) *r_is_abstract = false;
    if (r_is_tool != nullptr) *r_is_tool = false;
    return name;
}

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
    if (bool(GLOBAL_GET("luau/editor/require_autocomplete"))) {
        const int cursor = p_code.find(String::chr(0xffff));
        if (cursor >= 0) {
            const String line = p_code.substr(0, cursor).get_slice("\n", p_code.substr(0, cursor).get_slice_count("\n") - 1);
            const int quote = MAX(line.rfind("\""), line.rfind("'"));
            // An even quote count means the cursor follows a closed string rather than sitting inside one.
            if (quote >= 0 && line.count(line.substr(quote, 1)) % 2 == 1) {
                const String before = line.substr(0, quote).strip_edges();
                if (before.begins_with("import ") || before.ends_with("from") || before.ends_with("require(")) {
                    const String requested = line.substr(quote + 1);
                    const Dictionary aliases = module_aliases(p_path);
                    const int slash = requested.rfind("/");
                    if (requested.begins_with("@") && slash == -1) {
                        for (const Variant &key : aliases.keys()) {
                            const String name = "@" + String(key);
                            if (name.begins_with(requested.to_lower())) r_options->push_back(CodeCompletionOption(name, CODE_COMPLETION_KIND_FILE_PATH));
                        }
                        return OK;
                    }
                    const String base = requested.substr(0, slash + 1);
                    const String tail = requested.substr(slash + 1);
                    String directory;
                    if (requested.begins_with("@")) {
                        const int first = requested.find("/");
                        const String alias = requested.substr(1, first - 1).to_lower();
                        if (aliases.has(alias)) directory = String(aliases[alias]).path_join(base.substr(first + 1));
                    } else directory = p_path.get_base_dir().path_join(base);
                    const Ref<DirAccess> dir = DirAccess::open(directory);
                    if (dir.is_valid()) {
                        dir->list_dir_begin();
                        for (String entry = dir->get_next(); !entry.is_empty(); entry = dir->get_next()) {
                            if (entry.begins_with(".")) continue;
                            const bool folder = dir->current_is_dir();
                            if (!folder && entry.get_extension() != "luau") continue;
                            const String name = folder ? entry + "/" : entry.get_basename();
                            if (name.begins_with(tail)) r_options->push_back(CodeCompletionOption(base + name, CODE_COMPLETION_KIND_FILE_PATH));
                        }
                        dir->list_dir_end();
                    }
                    return OK;
                }
            }
        }
    }
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
