#ifdef TOOLS_ENABLED
#include "luau_analysis.h"
#include "luau_script.h"
#include "Luau/Autocomplete.h"
#include "Luau/BuiltinDefinitions.h"
#include "Luau/Config.h"
#include "Luau/Frontend.h"
#include "Luau/ToString.h"
#include "core/config/engine.h"
#include "core/io/file_access.h"
#include "core/object/class_db.h"
#include <memory>
#include <unordered_map>

namespace {
std::string utf8(const String &value) { const CharString text = value.utf8(); return {text.get_data(), size_t(text.length())}; }
struct Files : Luau::FileResolver {
    std::unordered_map<std::string, std::string> overlays;
    std::optional<Luau::SourceCode> readSource(const Luau::ModuleName &name) override {
        if (auto it = overlays.find(name); it != overlays.end()) return Luau::SourceCode{it->second, Luau::SourceCode::Module};
        const String path = String::utf8(name.c_str());
        if (!FileAccess::exists(path)) return std::nullopt;
        return Luau::SourceCode{utf8(FileAccess::get_file_as_string(path)), Luau::SourceCode::Module};
    }
    std::optional<Luau::ModuleInfo> resolveModule(const Luau::ModuleInfo *context, Luau::AstExpr *expr, const Luau::TypeCheckLimits &limits) override {
        auto path = expr->as<Luau::AstExprConstantString>();
        if (path == nullptr || context == nullptr) return std::nullopt;
        const String resolved = LuauLanguage::get_singleton()->resolve_module(String::utf8(path->value.data, path->value.size), String::utf8(context->name.c_str()));
        return resolved.is_empty() ? std::nullopt : std::optional<Luau::ModuleInfo>({utf8(resolved)});
    }
};
struct Config : Luau::ConfigResolver {
    Luau::Config config;
    Config() { config.mode = Luau::Mode::Strict; }
    const Luau::Config &getConfig(const Luau::ModuleName &name, const Luau::TypeCheckLimits &limits) const override { return config; }
};
struct Session {
    Files files;
    Config configs;
    Luau::Frontend frontend;
    std::unordered_map<std::string, Luau::TypeId> instances;
    std::unordered_map<std::string, Luau::TypeId> classes;
    explicit Session() : frontend(Luau::SolverMode::New, &files, &configs) {
        using namespace Luau;
        registerBuiltinGlobals(frontend, frontend.globals);
        auto &arena = frontend.globals.globalTypes;
        auto scope = frontend.globals.globalScope;
        auto builtins = frontend.builtinTypes;
        LocalVector<StringName> classes;
        ClassDB::get_class_list(classes);
        // Allocate every nominal type first, so recursive engine signatures can refer to it.
        for (const StringName &name : classes) instances[utf8(name)] = arena.addType(ExternType{utf8(name), {}, builtins->objectType, std::nullopt, {}, nullptr, "ludot", std::nullopt});
        for (int i = ::Variant::VECTOR2; i < ::Variant::VARIANT_MAX; ++i) {
            const std::string name = utf8(::Variant::get_type_name(::Variant::Type(i)));
            if (instances.count(name) == 0) instances[name] = arena.addType(ExternType{name, {}, std::nullopt, std::nullopt, {}, nullptr, "ludot", std::nullopt});
        }
        for (const auto &[name, instance] : instances) {
            TypeId constructor = arena.addType(FunctionType{arena.addTypePack({}, arena.addTypePack(VariadicTypePack{builtins->anyType})), arena.addTypePack({instance})});
            TypeId metatable = arena.addType(TableType{{{"__call", Property::readonly(constructor)}}, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
            TypeId cls = arena.addType(ExternType{name, {}, builtins->classType, metatable, {}, nullptr, "ludot", std::nullopt, true});
            this->classes[name] = cls;
            getMutable<ExternType>(cls)->relation = Obj{instance};
            getMutable<ExternType>(instance)->relation = Klass{cls};
            scope->bindings[frontend.globals.globalNames.names->getOrAdd(name.c_str())] = Binding{cls, Location()};
            scope->exportedTypeBindings[name] = TypeFun{{}, {}, instance};
        }
        for (const StringName &name : classes) {
            auto object = getMutable<ExternType>(instances.at(utf8(name)));
            const StringName parent = ClassDB::get_parent_class(name);
            if (instances.count(utf8(parent))) {
                object->parent = instances.at(utf8(parent));
                getMutable<ExternType>(this->classes.at(utf8(name)))->parent = this->classes.at(utf8(parent));
            }
            List<PropertyInfo> properties;
            ClassDB::get_property_list(name, &properties, true);
            for (const PropertyInfo &property : properties) {
                const String renamed = LuauLanguage::api_name(property.name, true);
                if (renamed.is_valid_ascii_identifier()) object->props[utf8(renamed)] = Property::rw(type(property));
            }
            List<MethodInfo> methods;
            ClassDB::get_method_list(name, &methods, true);
            for (const MethodInfo &method : methods) {
                const String renamed = LuauLanguage::api_name(method.name);
                auto target = (method.flags & METHOD_FLAG_STATIC) != 0 ? getMutable<ExternType>(this->classes.at(utf8(name))) : object;
                if (renamed.is_valid_ascii_identifier()) target->props[utf8(renamed)] = Property::readonly(function(method, instances.at(utf8(name))));
            }
            List<String> constants;
            ClassDB::get_integer_constant_list(name, &constants, true);
            for (const String &constant : constants) getMutable<ExternType>(this->classes.at(utf8(name)))->props[utf8(LuauLanguage::api_name(constant, true))] = Property::readonly(builtins->numberType);
            List<MethodInfo> signals;
            ClassDB::get_signal_list(name, &signals, true);
            for (const auto &signal : signals) object->props[utf8(LuauLanguage::api_name(signal.name, true))] = Property::readonly(instances.at("Signal"));
        }
        for (int i = ::Variant::VECTOR2; i < ::Variant::VARIANT_MAX; ++i) {
            if (i == ::Variant::OBJECT) continue;
            auto it = instances.find(utf8(::Variant::get_type_name(::Variant::Type(i))));
            if (it == instances.end()) continue;
            auto object = getMutable<ExternType>(it->second);
            ::Variant value;
            Callable::CallError error;
            ::Variant::construct(::Variant::Type(i), value, nullptr, 0, error);
            if (error.error != Callable::CallError::CALL_OK) continue;
            List<PropertyInfo> properties;
            value.get_property_list(&properties);
            for (const PropertyInfo &property : properties) object->props[utf8(LuauLanguage::api_name(property.name, true))] = Property::rw(type(property));
            List<MethodInfo> methods;
            value.get_method_list(&methods);
            for (const MethodInfo &method : methods) {
                auto target = (method.flags & METHOD_FLAG_STATIC) != 0 ? getMutable<ExternType>(this->classes.at(it->first)) : object;
                target->props[utf8(LuauLanguage::api_name(method.name))] = Property::readonly(function(method, it->second));
            }
            List<StringName> constants;
            ::Variant::get_constants_for_type(::Variant::Type(i), &constants);
            for (const auto &constant : constants) {
                const ::Variant value = ::Variant::get_constant_value(::Variant::Type(i), constant);
                getMutable<ExternType>(this->classes.at(it->first))->props[utf8(LuauLanguage::api_name(constant, true))] = Property::readonly(type(PropertyInfo(value.get_type(), "")));
            }
            TableType::Props operations;
            for (const char *operation : {"__add", "__sub", "__mul", "__div"}) operations[operation] = Property::readonly(arena.addType(FunctionType{arena.addTypePack({it->second, builtins->anyType}), arena.addTypePack({it->second})}));
            object->metatable = arena.addType(TableType{operations, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
        }
        List<Engine::Singleton> singletons;
        Engine::get_singleton()->get_singletons(&singletons);
        for (const auto &entry : singletons) {
            const auto it = instances.find(utf8(entry.ptr->get_class_name()));
            if (it != instances.end()) scope->bindings[frontend.globals.globalNames.names->getOrAdd(utf8(entry.name).c_str())] = Binding{it->second, Location()};
        }
        freeze(arena);
    }
    Luau::TypeId type(const PropertyInfo &info) {
        const auto builtin = frontend.builtinTypes;
        switch (info.type) {
            case ::Variant::NIL: return (info.usage & PROPERTY_USAGE_NIL_IS_VARIANT) != 0 ? builtin->anyType : builtin->nilType;
            case ::Variant::BOOL: return builtin->booleanType;
            case ::Variant::INT: case ::Variant::FLOAT: return builtin->numberType;
            case ::Variant::STRING: case ::Variant::STRING_NAME: case ::Variant::NODE_PATH: return builtin->stringType;
            case ::Variant::OBJECT: {
                const auto it = instances.find(utf8(info.class_name));
                return it != instances.end() ? it->second : builtin->anyType;
            }
            default: {
                const auto it = instances.find(utf8(::Variant::get_type_name(info.type)));
                return it != instances.end() ? it->second : builtin->anyType;
            }
        }
    }
    Luau::TypeId function(const MethodInfo &method, Luau::TypeId self) {
        using namespace Luau;
        auto &arena = frontend.globals.globalTypes;
        const bool is_static = (method.flags & METHOD_FLAG_STATIC) != 0;
        std::vector<TypeId> arguments;
        if (!is_static) arguments.push_back(self);
        int index = 0;
        for (const PropertyInfo &arg : method.arguments) {
            TypeId arg_type = type(arg);
            if (arg.type == ::Variant::CALLABLE) arg_type = arena.addType(UnionType{{arg_type, frontend.builtinTypes->functionType}});
            if (index >= method.arguments.size() - method.default_arguments.size()) arg_type = arena.addType(UnionType{{arg_type, frontend.builtinTypes->nilType}});
            arguments.push_back(arg_type);
            ++index;
        }
        std::optional<TypePackId> tail;
        if ((method.flags & METHOD_FLAG_VARARG) != 0) tail = arena.addTypePack(VariadicTypePack{frontend.builtinTypes->anyType});
        TypePackId returns = method.return_val.type == ::Variant::NIL && (method.return_val.usage & PROPERTY_USAGE_NIL_IS_VARIANT) == 0 ? frontend.builtinTypes->emptyTypePack : arena.addTypePack({type(method.return_val)});
        auto result = arena.addType(FunctionType{arena.addTypePack(arguments, tail), returns});
        getMutable<FunctionType>(result)->hasSelf = !is_static;
        return result;
    }
    std::string update(const String &source, const String &path) {
        const std::string name = utf8(path.is_empty() ? String("res://__editor_buffer.luau") : path);
        files.overlays[name] = utf8(source);
        frontend.markDirty(name);
        return name;
    }
};
std::unique_ptr<Session> session;
Mutex analysis_mutex;
Session &get_session() { if (session == nullptr) session = std::make_unique<Session>(); return *session; }
}
bool ludot_validate(const String &source, const String &path, List<ScriptLanguage::ScriptError> *errors) {
    MutexLock lock(analysis_mutex);
    auto &analysis = get_session();
    const auto name = analysis.update(source, path);
    Luau::FrontendOptions options;
    options.retainFullTypeGraphs = true;
    options.moduleTimeLimitSec = 2.0;
    Luau::CheckResult checked;
    try {
        checked = analysis.frontend.check(name, options);
    } catch (const Luau::InternalCompilerError &error) {
        if (errors != nullptr) {
            ScriptLanguage::ScriptError diagnostic;
            diagnostic.path = path;
            diagnostic.line = error.location ? error.location->begin.line + 1 : 1;
            diagnostic.column = error.location ? error.location->begin.column + 1 : 1;
            diagnostic.message = String::utf8(error.what());
            errors->push_back(diagnostic);
        }
        return false;
    }
    for (const auto &error : checked.errors) {
        if (error.moduleName != name) continue;
        if (errors != nullptr) {
            ScriptLanguage::ScriptError diagnostic;
            diagnostic.path = path;
            diagnostic.line = error.location.begin.line + 1;
            diagnostic.column = error.location.begin.column + 1;
            diagnostic.message = String::utf8(Luau::toString(error).c_str());
            errors->push_back(diagnostic);
        }
    }
    return checked.errors.empty();
}
Error ludot_complete(const String &source, const String &path, List<ScriptLanguage::CodeCompletionOption> *options) {
    MutexLock lock(analysis_mutex);
    auto &analysis = get_session();
    const int cursor = source.find(String::chr(0xffff));
    if (cursor < 0) return ERR_INVALID_PARAMETER;
    const String prefix = source.substr(0, cursor);
    const PackedStringArray lines = prefix.split("\n");
    const unsigned line = lines.size() - 1;
    const unsigned column = lines[lines.size() - 1].utf8().length();
    const auto name = analysis.update(source.replace(String::chr(0xffff), ""), path);
    Luau::FrontendOptions config;
    config.retainFullTypeGraphs = true;
    config.forAutocomplete = true;
    config.moduleTimeLimitSec = 2.0;
    try {
        analysis.frontend.check(name, config);
    } catch (const Luau::InternalCompilerError &error) {
        ERR_PRINT(String::utf8(error.what()));
        return ERR_PARSE_ERROR;
    }
    const auto result = Luau::autocomplete(analysis.frontend, name, {line, column}, {});
    for (const auto &[label, entry] : result.entryMap) options->push_back(ScriptLanguage::CodeCompletionOption(String::utf8(label.c_str()), entry.kind == Luau::AutocompleteEntryKind::Type ? ScriptLanguage::CODE_COMPLETION_KIND_CLASS : ScriptLanguage::CODE_COMPLETION_KIND_MEMBER));
    return OK;
}
void ludot_clear_analysis() { MutexLock lock(analysis_mutex); session.reset(); }
#endif
