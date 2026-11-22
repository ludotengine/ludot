#ifdef TOOLS_ENABLED
#include "luau_analysis.h"

#include "Luau/AstQuery.h"
#include "Luau/Autocomplete.h"
#include "Luau/BuiltinDefinitions.h"
#include "Luau/Clone.h"
#include "Luau/Config.h"
#include "Luau/ConstraintSolver.h"
#include "Luau/Frontend.h"
#include "Luau/LudotClasses.h"
#include "Luau/LudotEnums.h"
#include "Luau/ToString.h"
#include "Luau/TypePack.h"
#include "luau_documentation.h"
#include "luau_script.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/core_constants.h"
#include "core/io/file_access.h"
#include "core/object/class_db.h"
#include "core/os/thread.h"
#include "editor/doc/editor_help.h"
#include "editor/editor_node.h"
#include "scene/main/node.h"

#include <map>
#include <memory>
#include <set>
#include <unordered_map>

namespace {
String library_description(const String &p_class, const String &p_member);
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
struct SceneNodeLookup : Luau::MagicFunction {
    std::function<std::optional<Luau::TypeId>(const Luau::MagicFunctionCallContext &)> resolve;
    explicit SceneNodeLookup(decltype(resolve) p_resolve) : resolve(std::move(p_resolve)) {}
    std::optional<Luau::WithPredicate<Luau::TypePackId>> handleOldSolver(Luau::TypeChecker &, const std::shared_ptr<Luau::Scope> &, const Luau::AstExprCall &, Luau::WithPredicate<Luau::TypePackId>) override { return std::nullopt; }
    bool infer(const Luau::MagicFunctionCallContext &context) override {
        const auto type = resolve(context);
        if (!type) return false;
        context.solver->bind(context.constraint, context.result, context.solver->arena->addTypePack({*type}));
        return true;
    }
};
struct Session {
    Files files;
    Config configs;
    Luau::Frontend frontend;
    std::unordered_map<std::string, Luau::TypeId> instances;
    std::unordered_map<std::string, Luau::TypeId> classes;
	Luau::EngineEnumTypes engine_enums;
	Luau::LudotSignalTypes signal_types;
	Luau::TypeId any_signal = nullptr;
	String scene_fingerprint;
    ObjectID scene_root;
    ObjectID analysis_owner;
    String primary_path;
    std::unordered_map<std::string, Luau::TypeId> script_instances;
    std::unordered_map<Luau::TypeId, std::pair<String, String>> function_docs;
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
            getMutable<ExternType>(cls)->props["new"] = Property::readonly(constructor);
            getMutable<ExternType>(cls)->relation = Obj{instance};
            getMutable<ExternType>(instance)->relation = Klass{cls};
            scope->bindings[frontend.globals.globalNames.names->getOrAdd(name.c_str())] = Binding{cls, Location()};
            scope->exportedTypeBindings[name] = TypeFun{{}, {}, instance};
        }
		const auto &catalog = ludot_engine_enum_catalog();
		engine_enums = addEngineEnumTypes(frontend.globals, catalog, LuauLanguage::get_singleton()->method_casing, LuauLanguage::get_singleton()->property_casing);
		// Ludot's Signal library replaces Godot's Signal value type, for engine signals too.
		signal_types = addLudotSignalTypes(frontend.globals, LuauLanguage::get_singleton()->method_casing, LuauLanguage::get_singleton()->property_casing);
		any_signal = signal_types.instantiate(arena, arena.addTypePack(VariadicTypePack{builtins->anyType}));
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
                if (renamed.is_valid_ascii_identifier()) object->props[utf8(renamed)] = Property::rw(type(LuauLanguage::enum_property(name, property)));
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
			for (const String &constant : constants) {
				const StringName declaration = ClassDB::get_integer_constant_enum(name, constant, true);
				auto found = catalog.byConstant.find(utf8(String(name) + "." + constant));
				TypeId value = builtins->numberType;
				// Non-bitfield class enum constants are their Enum items. Bitfield constants stay numbers.
				if (declaration != StringName() && !ClassDB::is_enum_bitfield(name, declaration, true) && found != catalog.byConstant.end()) {
					value = engine_enums.members[found->second.first][found->second.second];
				}
				auto legacy = Property::readonly(value);
				legacy.tags.push_back("LudotHiddenAlias");
				for (auto target : { getMutable<ExternType>(this->classes.at(utf8(name))), object }) {
					target->props[utf8(LuauLanguage::api_name(constant, true, true))] = legacy;
					target->props[utf8(constant)] = Property::readonly(value);
				}
			}
			List<MethodInfo> signals;
            ClassDB::get_signal_list(name, &signals, true);
            for (const auto &signal : signals) {
                std::vector<TypeId> arguments;
                for (const PropertyInfo &argument : signal.arguments) arguments.push_back(type(argument));
                object->props[utf8(LuauLanguage::api_name(signal.name, true))] = Property::readonly(signal_types.instantiate(arena, arena.addTypePack(arguments)));
            }
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
				TypeId member_type = type(PropertyInfo(value.get_type(), ""));
				auto target = getMutable<ExternType>(this->classes.at(it->first));
				// Builtin type enum constants, such as Vector3.AXIS_X, stay numbers. Enum.Axis holds the items.
				if (!::Variant::get_enum_for_enumeration(::Variant::Type(i), constant).is_empty()) {
					auto legacy = Property::readonly(member_type);
					legacy.tags.push_back("LudotHiddenAlias");
					for (auto destination : { target, object }) {
						destination->props[utf8(LuauLanguage::api_name(constant, true, true))] = legacy;
						destination->props[utf8(constant)] = Property::readonly(member_type);
					}
				} else {
					target->props[utf8(LuauLanguage::api_name(constant, true, true))] = Property::readonly(member_type);
				}
			}
            List<StringName> declarations;
            ::Variant::get_enums_for_type(::Variant::Type(i), &declarations);
            for (const StringName &declaration : declarations) {
                List<StringName> values;
                ::Variant::get_enumerations_for_enum(::Variant::Type(i), declaration, &values);
                for (const StringName &constant : values) {
                    auto legacy = Property::readonly(builtins->numberType);
                    legacy.tags.push_back("LudotHiddenAlias");
                    for (auto destination : { getMutable<ExternType>(this->classes.at(it->first)), object }) {
                        destination->props[utf8(LuauLanguage::api_name(constant, true, true))] = legacy;
                        destination->props[utf8(constant)] = Property::readonly(builtins->numberType);
                    }
                }
            }
            TableType::Props operations;
            const std::pair<const char *, ::Variant::Operator> arithmetic[] = {
                {"__add", ::Variant::OP_ADD}, {"__sub", ::Variant::OP_SUBTRACT},
                {"__mul", ::Variant::OP_MULTIPLY}, {"__div", ::Variant::OP_DIVIDE},
            };
            for (const auto &operation : arithmetic) {
                std::vector<TypeId> overloads;
                std::set<std::pair<TypeId, TypeId>> signatures;
                for (int right = ::Variant::BOOL; right < ::Variant::VARIANT_MAX; ++right) {
                    const auto result = ::Variant::get_operator_return_type(operation.second, ::Variant::Type(i), ::Variant::Type(right));
                    if (result == ::Variant::NIL) continue;
                    TypeId argument = type(PropertyInfo(::Variant::Type(right), ""));
                    TypeId returned = type(PropertyInfo(result, ""));
                    if (!signatures.emplace(argument, returned).second) continue;
                    overloads.push_back(arena.addType(FunctionType{arena.addTypePack({it->second, argument}), arena.addTypePack({returned})}));
                }
                if (overloads.size() == 1) operations[operation.first] = Property::readonly(overloads.front());
                else if (!overloads.empty()) operations[operation.first] = Property::readonly(arena.addType(IntersectionType{std::move(overloads)}));
            }
            object->metatable = arena.addType(TableType{operations, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
        }
        List<Engine::Singleton> singletons;
        Engine::get_singleton()->get_singletons(&singletons);
        for (const auto &entry : singletons) {
            const auto it = instances.find(utf8(entry.ptr->get_class_name()));
            if (it != instances.end()) scope->bindings[frontend.globals.globalNames.names->getOrAdd(utf8(entry.name).c_str())] = Binding{it->second, Location()};
        }
        for (const char *native_method : {"get_node", "get_node_or_null", "get_parent"}) {
            const std::string method = utf8(LuauLanguage::api_name(native_method));
            const auto node = getMutable<ExternType>(instances.at("Node"));
            const auto fn = getMutable<FunctionType>(*node->props.at(method).readTy);
            fn->magic = std::make_shared<SceneNodeLookup>([this](const MagicFunctionCallContext &context) { return scene_result(context); });
        }
		addLudotIsA(arena, *builtins, instances.at("Object"), LuauLanguage::get_singleton()->method_casing);
		addLudotTaskLibrary(frontend.globals);
		function_docs[*getMutable<ExternType>(instances.at("Object"))->props.at(utf8(LuauLanguage::api_name("is_a"))).readTy] = { "Object", "is_class" };
		freeze(arena);
    }
    Node *find_script_node(Node *node, const String &path) const {
        if (node == nullptr) return nullptr;
        const Ref<Script> script = node->get_script();
        if (script.is_valid() && script->get_path() == path) return node;
        for (int i = 0; i < node->get_child_count(); ++i) {
            if (auto found = find_script_node(node->get_child(i), path)) return found;
        }
        return nullptr;
    }
    Node *resolve_scene_expression(Luau::AstExpr *expr, const String &path) const {
        if (auto group = expr->as<Luau::AstExprGroup>()) return resolve_scene_expression(group->expr, path);
        if (auto local = expr->as<Luau::AstExprLocal>(); local && local->local->name == "self") {
            if (path == primary_path && analysis_owner.is_valid()) return Object::cast_to<Node>(ObjectDB::get_instance(analysis_owner));
            return find_script_node(Object::cast_to<Node>(ObjectDB::get_instance(scene_root)), path);
        }
        const auto call = expr->as<Luau::AstExprCall>();
        if (call == nullptr || !call->self) return nullptr;
        const auto index = call->func->as<Luau::AstExprIndexName>();
        if (index == nullptr) return nullptr;
        auto receiver = resolve_scene_expression(index->expr, path);
        if (receiver == nullptr) return nullptr;
        if (String::utf8(index->index.value) == LuauLanguage::api_name("get_parent") && call->args.size == 0) return receiver->get_parent();
        if ((String::utf8(index->index.value) == LuauLanguage::api_name("get_node") || String::utf8(index->index.value) == LuauLanguage::api_name("get_node_or_null")) && call->args.size == 1) {
            if (auto literal = call->args.data[0]->as<Luau::AstExprConstantString>())
                return receiver->get_node_or_null(NodePath(String::utf8(literal->value.data, literal->value.size)));
        }
        return nullptr;
    }
    std::optional<Luau::TypeId> scene_result(const Luau::MagicFunctionCallContext &context) const {
        if (!Thread::is_main_thread()) return std::nullopt;
        const String module_path = String::utf8(context.constraint->moduleName->c_str());
        auto target = resolve_scene_expression(const_cast<Luau::AstExprCall *>(context.callSite.get()), module_path);
        if (target == nullptr) return std::nullopt;
        auto type = instances.at(utf8(target->get_class_name()));
        const Ref<Script> script = target->get_script();
        if (script.is_valid()) {
            const auto found = script_instances.find(utf8(script->get_path()));
            if (found != script_instances.end()) type = found->second;
            if (script->get_path() == module_path) {
                if (auto own_type = context.constraint->scope->lookupType(utf8(script->get_global_name()))) type = own_type->type;
            }
        }
        const auto index = context.callSite->func->as<Luau::AstExprIndexName>();
        if (index && String::utf8(index->index.value) == LuauLanguage::api_name("get_node_or_null")) return context.solver->arena->addType(Luau::UnionType{{type, frontend.builtinTypes->nilType}});
        return type;
    }
    void prepare_scene(Node *root, Node *owner, const String &path) {
        scene_root = root != nullptr ? root->get_instance_id() : ObjectID();
        analysis_owner = owner != nullptr ? owner->get_instance_id() : ObjectID();
        primary_path = path;
        if (root == nullptr) return;
        Luau::FrontendOptions options;
        options.retainFullTypeGraphs = true;
        options.moduleTimeLimitSec = 2.0;
        Luau::unfreeze(frontend.globals.globalTypes);
        Luau::CloneState clones{frontend.builtinTypes};
        for (const auto &[name, type] : instances) clones.seenTypes[type] = type;
        for (const auto &[name, type] : classes) clones.seenTypes[type] = type;
        std::function<void(Node *)> visit = [&](Node *node) {
            const Ref<LuauScript> script = node->get_script();
            if (script.is_valid() && script->get_path() != path && !script_instances.count(utf8(script->get_path()))) {
                const auto name = update(script->get_source_code(), script->get_path());
                frontend.check(name, options);
                const auto module = frontend.moduleResolver.getModule(name);
                if (module) {
                    const auto type = module->getModuleScope()->lookupType(utf8(script->get_global_name()));
                    if (type) {
                        const auto copied = Luau::clone(type->type, frontend.globals.globalTypes, clones);
                        script_instances[name] = copied;
                        // Script classes can be named directly in instance annotations.
                        frontend.globals.globalScope->exportedTypeBindings[utf8(script->get_global_name())] = Luau::TypeFun{{}, {}, copied};
                    }
                }
            }
            for (int i = 0; i < node->get_child_count(); ++i) visit(node->get_child(i));
        };
        visit(root);
        Luau::freeze(frontend.globals.globalTypes);
    }
    Luau::TypeId type(const PropertyInfo &info) {
        const auto builtin = frontend.builtinTypes;
		if ((info.usage & PROPERTY_USAGE_CLASS_IS_ENUM) != 0 && (info.usage & PROPERTY_USAGE_CLASS_IS_BITFIELD) == 0) {
			const auto &catalog = ludot_engine_enum_catalog();
			auto found = catalog.bySource.find(utf8(info.class_name));
			if (found != catalog.bySource.end()) {
				return engine_enums.roots[found->second];
			}
		}
		switch (info.type) {
            case ::Variant::SIGNAL: return any_signal;
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
        auto fn = getMutable<FunctionType>(result);
        fn->hasSelf = !is_static;
        if (auto owner_type = get<ExternType>(self)) function_docs[result] = {String::utf8(owner_type->name.c_str()), method.name};
        if (!is_static) fn->argNames.push_back(FunctionArgument{"self", Location()});
        for (const PropertyInfo &arg : method.arguments) fn->argNames.push_back(FunctionArgument{Luau::engineApiName(utf8(arg.name), Luau::ApiCase::Camel), Location()});
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
Session &get_session(const String &path, Object *owner = nullptr) {
    Node *root = Thread::is_main_thread() && EditorNode::get_singleton() != nullptr ? EditorNode::get_singleton()->get_edited_scene() : nullptr;
    String fingerprint = root != nullptr ? path : String();
    std::function<void(Node *)> visit = [&](Node *node) {
        fingerprint += String::num_uint64(node->get_instance_id()) + String(node->get_name()) + String(node->get_class_name());
        const Ref<Script> script = node->get_script();
        if (script.is_valid()) {
            fingerprint += script->get_path();
            if (script->get_path() != path) fingerprint += script->get_source_code();
        }
        for (int i = 0; i < node->get_child_count(); ++i) visit(node->get_child(i));
    };
    if (root != nullptr) visit(root);
    if (session == nullptr || session->scene_fingerprint != fingerprint) {
        session = std::make_unique<Session>();
        session->scene_fingerprint = fingerprint;
        session->prepare_scene(root, Object::cast_to<Node>(owner), path);
    } else if (owner != nullptr) session->analysis_owner = owner->get_instance_id();
    return *session;
}
}
bool ludot_validate(const String &source, const String &path, List<ScriptLanguage::ScriptError> *errors, List<ScriptLanguage::Warning> *warnings) {
	MutexLock lock(analysis_mutex);
    auto &analysis = get_session(path);
    const auto name = analysis.update(source, path);
    Luau::FrontendOptions options;
    options.retainFullTypeGraphs = true;
    options.moduleTimeLimitSec = 2.0;
	options.runLintChecks = warnings != nullptr;
	Luau::LintOptions lint;
	lint.enableWarning(Luau::LintWarning::Code_MatchExhaustiveness);
	lint.enableWarning(Luau::LintWarning::Code_SignalDeclaration);
	options.enabledLintWarnings = lint;
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
	if (warnings != nullptr) {
		for (const auto &warning : checked.lintResult.warnings) {
			const bool signal = warning.code == Luau::LintWarning::Code_SignalDeclaration;
			if (warning.code != Luau::LintWarning::Code_MatchExhaustiveness && !signal) {
				continue;
			}
			ScriptLanguage::Warning diagnostic;
			diagnostic.start_line = warning.location.begin.line + 1;
			diagnostic.end_line = warning.location.end.line + 1;
			diagnostic.code = signal ? 2002 : 2001;
			diagnostic.string_code = signal ? "signal_declaration" : "match_exhaustiveness";
			diagnostic.message = String::utf8(warning.text.c_str());
			warnings->push_back(diagnostic);
		}
	}
	return checked.errors.empty();
}
Error ludot_complete(const String &source, const String &path, List<ScriptLanguage::CodeCompletionOption> *options, String &call_hint, Object *owner) {
    MutexLock lock(analysis_mutex);
    auto &analysis = get_session(path, owner);
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
    const auto function_type = [](Luau::TypeId type) -> const Luau::FunctionType * {
        type = Luau::follow(type);
        if (auto fn = Luau::get<Luau::FunctionType>(type)) return fn;
        if (auto overloads = Luau::get<Luau::IntersectionType>(type)) {
            for (auto part : overloads->parts) if (auto fn = Luau::get<Luau::FunctionType>(Luau::follow(part))) return fn;
        }
        return nullptr;
    };
    for (const auto &[label, entry] : result.entryMap) {
        auto kind = ScriptLanguage::CODE_COMPLETION_KIND_MEMBER;
        if (entry.kind == Luau::AutocompleteEntryKind::Type) kind = ScriptLanguage::CODE_COMPLETION_KIND_CLASS;
        else if (entry.kind == Luau::AutocompleteEntryKind::Keyword) kind = ScriptLanguage::CODE_COMPLETION_KIND_KEYWORD;
        else if (entry.kind == Luau::AutocompleteEntryKind::Binding) kind = ScriptLanguage::CODE_COMPLETION_KIND_VARIABLE;
        else if (entry.kind == Luau::AutocompleteEntryKind::RequirePath || entry.kind == Luau::AutocompleteEntryKind::Module) kind = ScriptLanguage::CODE_COMPLETION_KIND_FILE_PATH;
        if (entry.type) {
            auto type = Luau::follow(*entry.type);
            if (function_type(type)) kind = ScriptLanguage::CODE_COMPLETION_KIND_FUNCTION;
            else if (auto external = Luau::get<Luau::ExternType>(type)) {
                if (external->relation && external->relation->get_if<Luau::Obj>() != nullptr) kind = ScriptLanguage::CODE_COMPLETION_KIND_CLASS;
            } else if (auto table = Luau::get<Luau::TableType>(type); table && table->name == "Signal") {
                kind = ScriptLanguage::CODE_COMPLETION_KIND_SIGNAL;
            }
        }
        ScriptLanguage::CodeCompletionOption option(String::utf8(label.c_str()), kind);
        if (entry.insertText) option.insert_text = String::utf8(entry.insertText->c_str());
        else if (entry.parens == Luau::ParenthesesRecommendation::CursorInside) option.insert_text += "(";
        else if (entry.parens == Luau::ParenthesesRecommendation::CursorAfter) option.insert_text += "()";
        options->push_back(option);
    }
    call_hint = "";
    const auto module = analysis.frontend.moduleResolver.getModule(name);
    const auto source_module = analysis.frontend.getSourceModule(name);
    if (module && source_module) {
        const Luau::Position position{line, column};
        const auto ancestry = Luau::findAstAncestryOfPosition(*source_module, position, true);
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            const auto call = (*it)->as<Luau::AstExprCall>();
            if (call == nullptr || call->func->location.contains(position)) continue;
            const auto type = module->astTypes.find(call->func);
            const auto fn = type != nullptr ? function_type(*type) : nullptr;
            if (fn == nullptr) continue;
            auto [args, tail] = Luau::flatten(fn->argTypes);
            const size_t skip = call->self && fn->hasSelf ? 1 : 0;
            unsigned active = 0;
            for (auto arg : call->args) if (arg->location.end < position) ++active;
            call_hint = "(";
            for (size_t i = skip; i < args.size(); ++i) {
                if (i != skip) call_hint += ", ";
                if (i - skip == active) call_hint += String::chr(0xffff);
                const std::string parameter = i < fn->argNames.size() && fn->argNames[i] ? fn->argNames[i]->name : "arg" + std::to_string(i - skip + 1);
                call_hint += String::utf8(parameter.c_str()) + ": " + String::utf8(Luau::toString(args[i]).c_str());
                if (i - skip == active) call_hint += String::chr(0xffff);
            }
            if (tail) {
                const auto variadic = Luau::get<Luau::VariadicTypePack>(Luau::follow(*tail));
                if (!variadic || !variadic->hidden) {
                    if (args.size() > skip) call_hint += ", ";
                    call_hint += "...: " + String::utf8(Luau::toString(*tail).c_str());
                }
            }
            call_hint += ") -> " + String::utf8(Luau::toString(fn->retTypes).c_str());
            break;
        }
    }
    return OK;
}
Dictionary ludot_symbol_help(const String &source, const String &path, const String &symbol, Object *owner) {
    MutexLock lock(analysis_mutex);
    const int cursor = source.find(String::chr(0xffff));
    if (cursor < 0) return {};
    auto &analysis = get_session(path, owner);
    const String clean_source = source.replace(String::chr(0xffff), "");
    // Mouse hit testing can select the trailing boundary of an identifier.
    const int position_cursor = cursor >= symbol.length() && !symbol.is_empty() && clean_source.substr(cursor - symbol.length(), symbol.length()) == symbol ? cursor - 1 : cursor;
    const PackedStringArray before = clean_source.substr(0, position_cursor).split("\n");
    const Luau::Position position{unsigned(before.size() - 1), unsigned(before[before.size() - 1].utf8().length())};
    const auto name = analysis.update(clean_source, path);
    Luau::FrontendOptions options;
    options.retainFullTypeGraphs = true;
    options.moduleTimeLimitSec = 2.0;
    try { analysis.frontend.check(name, options); }
    catch (const Luau::InternalCompilerError &error) { ERR_PRINT(String::utf8(error.what())); return {}; }
    const auto module = analysis.frontend.moduleResolver.getModule(name);
    const auto parsed = analysis.frontend.getSourceModule(name);
    if (!module || !parsed) return {};
    auto type = Luau::findTypeAtPosition(*module, *parsed, position);
    if (!type) {
        if (auto binding = Luau::findBindingAtPosition(*module, *parsed, position)) type = binding->typeId;
    }
    if (!type) {
        const auto ancestry = Luau::findAstAncestryOfPosition(*parsed, position, true);
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            if (auto annotation = (*it)->asType()) {
                if (auto found = module->astResolvedTypes.find(annotation)) { type = *found; break; }
            }
        }
    }
    if (!type) {
        const auto ancestry = Luau::findAstAncestryOfPosition(*parsed, position, true);
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            if (auto expr = (*it)->as<Luau::AstExprFunction>()) {
                if (auto found = module->astTypes.find(expr)) { type = *found; break; }
            }
        }
    }
    if (!type) return {};
    auto resolved = Luau::follow(*type);
    if (Luau::get<Luau::IntersectionType>(resolved)) {
        const auto ancestry = Luau::findAstAncestryOfPosition(*parsed, position, true);
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            if (auto call = (*it)->as<Luau::AstExprCall>(); call && call->func->location.contains(position)) {
                if (auto selected = module->astOverloadResolvedTypes.find(call); selected && Luau::get<Luau::FunctionType>(Luau::follow(*selected))) {
                    resolved = Luau::follow(*selected);
                    break;
                }
            }
        }
    }
    const Luau::FunctionType *fn = Luau::get<Luau::FunctionType>(resolved);
    if (!fn) {
        if (auto intersection = Luau::get<Luau::IntersectionType>(resolved)) {
            for (auto part : intersection->parts) {
                resolved = Luau::follow(part);
                fn = Luau::get<Luau::FunctionType>(resolved);
                if (fn) break;
            }
        }
    }
    if (!fn) {
        Dictionary value;
        value["kind"] = "value";
        value["display_name"] = symbol;
        value["value_type"] = String::utf8(Luau::toString(resolved).c_str());
        value["symbol_type"] = "Variable";
		if (const auto table = Luau::get<Luau::TableType>(resolved); table && table->name && (*table->name == "Signal" || *table->name == "task")) {
			value["description"] = library_description(String::utf8(table->name->c_str()), "");
		}
		if (const auto native = analysis.classes.find(utf8(symbol)); native != analysis.classes.end() && Luau::follow(native->second) == resolved) {
			value["symbol_type"] = "Type";
			value["declaring_class"] = symbol;
            value["value_type"] = symbol;
		}
		const auto ancestry = Luau::findAstAncestryOfPosition(*parsed, position, true);
		for (auto *node : ancestry) {
            if (!node->asType()) continue;
            value["symbol_type"] = "Type";
            if (const auto external = Luau::get<Luau::ExternType>(resolved)) {
                if (analysis.instances.count(external->name)) value["declaring_class"] = String::utf8(external->name.c_str());
            }
        }
        const std::pair<const char *, const char *> primitive_docs[] = {
            {"number", "A double-precision numeric value used for arithmetic."},
            {"string", "A sequence of bytes used for text and binary data."},
            {"boolean", "The logical values true and false."},
            {"nil", "The absence of a value."},
            {"any", "A value whose type is not checked. Operations on it are permitted without type validation."},
            {"unknown", "A value of an unknown type. Refine its type before using it."},
            {"never", "A type with no possible values, used for unreachable code."},
            {"thread", "A Luau coroutine."},
            {"buffer", "A mutable sequence of bytes with typed read and write operations."},
        };
        if (String(value["symbol_type"]) == "Type") {
            for (const auto &primitive : primitive_docs) {
                if (symbol == primitive.first) value["description"] = primitive.second;
            }
        }
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            auto index = (*it)->as<Luau::AstExprIndexName>();
            if (!index || !index->indexLocation.contains(position)) continue;
            auto receiver = module->astTypes.find(index->expr);
            if (!receiver) break;
            auto external = Luau::get<Luau::ExternType>(Luau::follow(*receiver));
            while (external) {
                String class_name = String::utf8(external->name.c_str());
                List<PropertyInfo> properties;
				for (const auto stat : parsed->root->body) {
					const auto declaration = stat->as<Luau::AstStatClass>();
					if (!declaration || String::utf8(declaration->name->name.value) != class_name) {
						continue;
					}
					for (const auto &member : declaration->members) {
						const auto property = Luau::get_if<Luau::AstClassProperty>(&member);
						if (!property || String::utf8(property->name.value) != symbol) {
							continue;
						}
						const auto table = Luau::get<Luau::TableType>(resolved);
						if (!table || !table->name || *table->name != "Signal") {
							continue;
						}
						value.erase("description");
						value["symbol_type"] = "Signal";
						if (!property->isPrivate) {
							value["native_kind"] = "signal";
							value["declaring_class"] = class_name;
							value["native_member"] = symbol;
						}
						const PackedStringArray lines = clean_source.split("\n");
						String description;
						for (int line = int(property->qualifierLocation.begin.line) - 1; line >= 0; --line) {
							const String comment = lines[line].strip_edges();
							if (!comment.begins_with("---")) {
								break;
							}
							description = comment.substr(3).strip_edges() + (description.is_empty() ? String() : "\n" + description);
						}
						if (!description.is_empty()) {
							value["description"] = description.replace("[", "[lb]");
						}
						return value;
					}
				}
				const auto script_doc = EditorHelp::get_doc_data() ? EditorHelp::get_doc(class_name) : nullptr;
				if (script_doc && script_doc->is_script_doc) {
					for (const auto &signal : script_doc->signals) {
						if (signal.name != symbol) {
							continue;
						}
						value.erase("description");
						value["symbol_type"] = "Signal";
						value["native_kind"] = "signal";
						value["declaring_class"] = class_name;
						value["native_member"] = signal.name;
						return value;
					}
				}
				if (ClassDB::class_exists(class_name)) {
					List<MethodInfo> signals;
					ClassDB::get_signal_list(class_name, &signals, true);
					for (const MethodInfo &signal : signals) {
						if (LuauLanguage::api_name(signal.name, true) != symbol) {
							continue;
						}
						value.erase("description");
						value["symbol_type"] = "Signal";
						value["native_kind"] = "signal";
						value["declaring_class"] = class_name;
						value["native_member"] = signal.name;
						return value;
					}
					ClassDB::get_property_list(class_name, &properties, true);
				} else {
					for (int builtin = ::Variant::VECTOR2; builtin < ::Variant::VARIANT_MAX; ++builtin) {
						if (::Variant::get_type_name(::Variant::Type(builtin)) != class_name) {
							continue;
						}
						::Variant instance;
                        Callable::CallError error;
                        ::Variant::construct(::Variant::Type(builtin), instance, nullptr, 0, error);
                        if (error.error == Callable::CallError::CALL_OK) instance.get_property_list(&properties);
                        break;
					}
				}
				for (const PropertyInfo &property : properties) {
					if (LuauLanguage::api_name(property.name, true) != symbol) {
						continue;
					}
					value.erase("description");
					value["symbol_type"] = "Property";
                    value["declaring_class"] = class_name;
                    value["native_member"] = property.name;
                    return value;
				}
				external = external->parent ? Luau::get<Luau::ExternType>(Luau::follow(*external->parent)) : nullptr;
			}
            break;
        }
        return value;
    }
    Dictionary info;
    info["display_name"] = symbol;
    info["return_type"] = String::utf8(Luau::toString(fn->retTypes).c_str());
    Array parameters;
    const auto [args, tail] = Luau::flatten(fn->argTypes);
    const size_t skip = fn->hasSelf ? 1 : 0;
    info["is_method"] = fn->hasSelf && !args.empty() && Luau::get<Luau::ExternType>(Luau::follow(args.front())) != nullptr;
    String signature = symbol + "(";
    for (size_t i = skip; i < args.size(); ++i) {
        Dictionary parameter;
        const auto arg_name = i < fn->argNames.size() && fn->argNames[i] ? fn->argNames[i]->name : "arg" + std::to_string(i - skip + 1);
        parameter["name"] = String::utf8(arg_name.c_str());
        parameter["type"] = String::utf8(Luau::toString(args[i]).c_str());
        parameters.push_back(parameter);
        if (i != skip) signature += ", ";
        signature += String(parameter["name"]) + ": " + String(parameter["type"]);
    }
    if (tail) {
        const auto variadic = Luau::get<Luau::VariadicTypePack>(Luau::follow(*tail));
        if (!variadic || !variadic->hidden) {
            Dictionary parameter;
            parameter["name"] = "...";
            parameter["type"] = String::utf8((variadic ? Luau::toString(variadic->ty) : Luau::toString(*tail)).c_str());
            parameters.push_back(parameter);
            if (args.size() > skip) signature += ", ";
            signature += "...: " + String(parameter["type"]);
        }
    }
    info["parameters"] = parameters;
    info["signature"] = signature + ") -> " + String(info["return_type"]);
    if (const auto doc = analysis.function_docs.find(resolved); doc != analysis.function_docs.end()) {
        info["declaring_class"] = doc->second.first;
        info["native_member"] = doc->second.second;
    } else if (fn->definition && fn->definition->definitionModuleName) {
        const auto file = analysis.files.readSource(*fn->definition->definitionModuleName);
        if (file) {
            const auto lines = String::utf8(file->source.c_str()).split("\n");
            const auto location = fn->definition->originalNameLocation == Luau::Location() ? fn->definition->definitionLocation : fn->definition->originalNameLocation;
            String description;
            for (int line = int(location.begin.line) - 1; line >= 0 && line < lines.size(); --line) {
                const String comment = lines[line].strip_edges();
                if (!comment.begins_with("---")) break;
                description = comment.substr(3).strip_edges() + (description.is_empty() ? String() : "\n" + description);
            }
            info["description"] = description.replace("[", "[lb]");
        }
    }
	if (fn->hasSelf && !args.empty()) {
		if (const auto receiver = Luau::get<Luau::ExternType>(Luau::follow(args.front()))) {
			const String description = library_description(String::utf8(receiver->name.c_str()), symbol);
			if (!description.is_empty()) {
				info["description"] = description;
			}
		}
		if (const auto table = Luau::get<Luau::TableType>(Luau::follow(args.front())); table && table->name) {
			const String description = library_description(String::utf8(table->name->c_str()), symbol);
			if (!description.is_empty()) {
				info["description"] = description;
			}
		}
	}
	if (!info.has("declaring_class") && fn->hasSelf && !args.empty()) {
		auto receiver = Luau::get<Luau::ExternType>(Luau::follow(args.front()));
		while (receiver && !info.has("declaring_class")) {
            String class_name = String::utf8(receiver->name.c_str());
            if (ClassDB::class_exists(class_name)) {
                List<MethodInfo> virtuals;
                ClassDB::get_virtual_methods(class_name, &virtuals, true);
                for (const auto &method : virtuals) {
                    if (LuauLanguage::api_name(method.name) == symbol) {
                        info["declaring_class"] = class_name;
                        info["native_member"] = method.name;
                        break;
                    }
                }
            }
            receiver = receiver->parent ? Luau::get<Luau::ExternType>(Luau::follow(*receiver->parent)) : nullptr;
        }
	}
	return info;
}
void ludot_clear_analysis() { MutexLock lock(analysis_mutex); session.reset(); }

namespace {
// Godot documentation BBCode as plain text, with member references in Luau spelling.
// Callers select Luau examples before converting markup to Markdown.
String clean_doc(const String &p_text) {
	String text = p_text;
	String out;
	int i = 0;
	while (i < text.length()) {
		if (text[i] != '[') {
			out += text[i++];
			continue;
		}
		const int close = text.find("]", i);
		if (close < 0) {
			out += text.substr(i);
			break;
		}
		const String tag = text.substr(i + 1, close - i - 1);
		i = close + 1;
		const String kind = tag.get_slice(" ", 0);
		if (tag == "codeblock lang=luau") {
			const int end = text.find("[/codeblock]", i);
			if (end < 0) {
				break;
			}
			out += "\n```luau\n" + text.substr(i, end - i).strip_edges() + "\n```\n";
			i = end + String("[/codeblock]").length();
		} else if (tag == "code" || tag == "/code") {
			out += "`";
		} else if (tag == "br") {
			out += "\n";
		} else if (tag == "lb") {
			out += "[";
		} else if (tag == "rb")
			out += "]";
		else if (kind == "method" || kind == "member" || kind == "signal" || kind == "constant" || kind == "param" || kind == "enum" || kind == "theme_item" || kind == "annotation") {
			const String name = tag.substr(kind.length() + 1);
			const int dot = name.rfind(".");
			const String owner = dot >= 0 ? name.substr(0, dot + 1) : String();
			String member = dot >= 0 ? name.substr(dot + 1) : name;
			if (kind == "method") member = LuauLanguage::api_name(member);
			else if (kind == "member" || kind == "signal") member = LuauLanguage::api_name(member, true);
			out += "`" + owner + member + "`";
		} else if (tag.begins_with("url") || tag == "/url" || tag.begins_with("color") || tag == "/color" || tag.begins_with("font") || tag == "/font" || tag == "b" || tag == "/b" || tag == "i" || tag == "/i" || tag == "u" || tag == "/u" || tag == "s" || tag == "/s" || tag == "kbd" || tag == "/kbd" || tag == "center" || tag == "/center") {
			continue;
		} else if (tag.is_valid_ascii_identifier()) {
			out += tag;
		} else {
			out += "[" + tag + "]";
		}
	}
	// The class reference indents its text with tabs, which mean nothing here.
	PackedStringArray lines = out.split("\n");
	bool in_code = false;
	for (int line = 0; line < lines.size(); ++line) {
		if (lines[line].begins_with("```")) {
			in_code = !in_code;
		}
		if (!in_code) {
			lines.set(line, lines[line].strip_edges());
		}
	}
	return String("\n").join(lines).strip_edges();
}

// Godot's class reference, or nullptr before the editor has loaded it.
const DocData::ClassDoc *class_doc(const String &p_class) {
	return EditorHelp::get_doc_data() != nullptr ? EditorHelp::get_doc(p_class) : nullptr;
}

String first_sentence(const String &p_text) {
	String text = p_text.get_slice("\n", 0);
	const int end = text.find(". ");
	if (end >= 0) text = text.substr(0, end + 1);
	return text.length() > 240 ? text.substr(0, 237) + "..." : text;
}

struct DocMember {
	String luau;
	String godot;
	String kind;
	bool is_static = false;
	PackedStringArray arguments;
};

// The members a class declares itself, in Luau spelling.
Vector<DocMember> class_members(const String &p_class) {
	Vector<DocMember> members;
	if (ClassDB::class_exists(p_class)) {
		List<MethodInfo> methods;
		ClassDB::get_method_list(p_class, &methods, true);
		for (const MethodInfo &method : methods) {
			if (String(method.name).begins_with("_") && (method.flags & METHOD_FLAG_VIRTUAL) == 0) continue;
			DocMember member{ (method.flags & METHOD_FLAG_VIRTUAL) ? "_" + String(method.name).trim_prefix("_").to_camel_case() : LuauLanguage::api_name(method.name), method.name, (method.flags & METHOD_FLAG_VIRTUAL) ? "callback" : "method", (method.flags & METHOD_FLAG_STATIC) != 0 };
			for (const PropertyInfo &argument : method.arguments) member.arguments.push_back(argument.name);
			members.push_back(member);
		}
		List<PropertyInfo> properties;
		ClassDB::get_property_list(p_class, &properties, true);
		for (const PropertyInfo &property : properties) {
			if (property.usage & (PROPERTY_USAGE_GROUP | PROPERTY_USAGE_SUBGROUP | PROPERTY_USAGE_CATEGORY) || String(property.name).contains("/")) continue;
			members.push_back({ LuauLanguage::api_name(property.name, true), property.name, "property" });
		}
		List<MethodInfo> signals;
		ClassDB::get_signal_list(p_class, &signals, true);
		for (const MethodInfo &signal : signals) {
			DocMember member{ LuauLanguage::api_name(signal.name, true), signal.name, "signal" };
			for (const PropertyInfo &argument : signal.arguments) member.arguments.push_back(argument.name);
			members.push_back(member);
		}
		List<String> constants;
		ClassDB::get_integer_constant_list(p_class, &constants, true);
		for (const String &constant : constants) members.push_back({ constant, constant, "constant", true });
		return members;
	}
	for (int i = ::Variant::VECTOR2; i < ::Variant::VARIANT_MAX; ++i) {
		const auto type = ::Variant::Type(i);
		if (::Variant::get_type_name(type) != p_class) continue;
		List<StringName> methods;
		::Variant::get_builtin_method_list(type, &methods);
		for (const StringName &method : methods) {
			DocMember member{ LuauLanguage::api_name(method), method, "method", ::Variant::is_builtin_method_static(type, method) };
			for (int argument = 0; argument < ::Variant::get_builtin_method_argument_count(type, method); ++argument) member.arguments.push_back(::Variant::get_builtin_method_argument_name(type, method, argument));
			members.push_back(member);
		}
		List<StringName> properties;
		::Variant::get_member_list(type, &properties);
		for (const StringName &property : properties) members.push_back({ LuauLanguage::api_name(property, true), property, "property" });
		List<StringName> constants;
		::Variant::get_constants_for_type(type, &constants);
		for (const StringName &constant : constants) members.push_back({ LuauLanguage::api_name(constant, true, true), constant, "constant", true });
	}
	return members;
}

String member_description(const DocData::ClassDoc *p_doc, const DocMember &p_member) {
	if (p_doc == nullptr) return String();
	auto find = [&](const auto &list) -> String {
		for (const auto &entry : list) {
			if (entry.name == p_member.godot) return entry.description;
		}
		return String();
	};
	if (p_member.kind == "method" || p_member.kind == "callback") {
		return clean_doc(ludot_luau_doc(find(p_doc->methods), p_doc->name, p_member.godot));
	}
	if (p_member.kind == "property") {
		return clean_doc(ludot_luau_doc(find(p_doc->properties), p_doc->name, p_member.godot));
	}
	if (p_member.kind == "signal") {
		return clean_doc(ludot_luau_doc(find(p_doc->signals), p_doc->name, p_member.godot));
	}
	return clean_doc(find(p_doc->constants));
}

// A library entry: the Ludot globals that have no ClassDB documentation.
struct LibraryEntry {
	const char *owner;
	const char *name;
	const char *kind;
	const char *signature;
	const char *description;
};

Vector<LibraryEntry> library_entries() {
	return {
		{ "task", "", "library", "task", "Schedules threads, modeled on Roblox's task library. Waits use game time and follow pause and Engine.time_scale. Engine callbacks start in resumable threads, so task.wait works in _ready, _process, and signal handlers." },
		{ "task", "spawn", "function", "task.spawn(fnOrThread: ((...any) -> ...any) | thread, ...any): thread", "Runs the function now in a new thread, or resumes the given thread, and returns the thread." },
		{ "task", "defer", "function", "task.defer(fnOrThread: ((...any) -> ...any) | thread, ...any): thread", "Runs it after the current engine callback returns, later in the same frame." },
		{ "task", "delay", "function", "task.delay(seconds: number?, fnOrThread: ((...any) -> ...any) | thread, ...any): thread", "Resumes it after the delay and returns the thread." },
		{ "task", "wait", "function", "task.wait(seconds: number?): number", "Yields the current thread and returns the time that passed. With no argument, waits one frame. Errors outside a yieldable thread, such as module top-level code." },
		{ "task", "cancel", "function", "task.cancel(thread: thread)", "Stops a scheduled or suspended thread. It cannot cancel a running thread." },
		{ "Signal", "", "library", "Signal<T...>", "A Roblox-style signal type, created with `signal with (param: Type, ...)` (or `signal with ()`). Its type arguments list the fired arguments, as in Signal<number, number>, Signal<>, or Signal<...any>. Engine signals such as node.TreeExiting use the same type. On a class that extends an engine class, a public signal field is also a script signal: the Node dock lists it with its parameter names, scenes save its connections, and GDScript can connect and emit it. Private signal fields stay Luau-only. There is no emit method." },
		{ "Signal", "fire", "method", "signal:fire(...: T...)", "Calls every connected handler with the arguments, in connection order, from a snapshot. Each handler runs in its own resumable thread; a failing handler is reported with its stack trace and the rest still run. On an engine signal it emits the engine signal, so GDScript handlers run too." },
		{ "Signal", "connect", "method", "signal:connect(handler: (T...) -> ()): Connection", "Runs the handler on every fire and returns its Connection. The same function connected twice runs twice. The connection is dropped when the object whose code connected it is freed." },
		{ "Signal", "once", "method", "signal:once(handler: (T...) -> ()): Connection", "Runs the handler on the next fire only and returns its Connection." },
		{ "Signal", "wait", "method", "signal:wait(): T...", "Yields the current thread until the next fire and returns the fired arguments. Errors outside a yieldable thread." },
		{ "Signal", "disconnect_all", "method", "signal:disconnectAll()", "Disconnects every handler connected through this API. On an engine signal, editor and GDScript connections stay." },
		{ "Connection", "", "library", "Connection", "Returned by Signal connect and once." },
		{ "Connection", "disconnect", "method", "connection:disconnect()", "Disconnects the handler. Disconnecting twice does nothing." },
		{ "Connection", "connected", "property", "connection.Connected: boolean", "Whether the handler is still connected." },
		{ "Enum", "", "library", "Enum", "Every exposed engine enum, modeled on Roblox's Enum. Items have Name, Value, and EnumType, and engine methods and properties return and accept them, as in Input:setMouseMode(Enum.MouseMode.Captured). Look up an enum's items with its name, such as Enum.MouseMode. User enums declared with `enum` are not part of Enum." },
		{ "Enum", "get_enums", "method", "Enum:getEnums(): { Enum }", "Lists every engine enum." },
		{ "Enum", "from_name", "method", "Enum.MouseMode:fromName(name: string): EnumItem?", "The item with this name." },
		{ "Enum", "from_value", "method", "Enum.MouseMode:fromValue(value: number): EnumItem?", "The item with this value." },
		{ "Enum", "get_enum_items", "method", "Enum.MouseMode:getEnumItems(): { EnumItem }", "Every item of the enum." },
	};
}

String library_description(const String &p_class, const String &p_member) {
	for (const auto &entry : library_entries()) {
		if (p_class != entry.owner) {
			continue;
		}
		const String name = String(entry.kind) == "property" ? LuauLanguage::api_name(entry.name, true) : LuauLanguage::api_name(entry.name);
		if (p_member == name) {
			return ludot_luau_doc(entry.description, p_class, entry.name);
		}
	}
	return String();
}

Dictionary enum_entry(const Luau::EngineEnum &p_enum, bool p_items) {
	Dictionary entry;
	entry["name"] = "Enum." + String::utf8(p_enum.name.c_str());
	entry["bitfield"] = p_enum.bitfield;
	PackedStringArray sources;
	for (const std::string &source : p_enum.sources) sources.push_back(String::utf8(source.c_str()));
	entry["godot_sources"] = sources;
	entry["item_count"] = int(p_enum.items.size());
	if (p_items) {
		Array items;
		for (const Luau::EngineEnumItem &item : p_enum.items) {
			Dictionary value;
			value["name"] = String::utf8(item.name.c_str());
			value["value"] = int64_t(item.value);
			items.push_back(value);
		}
		entry["items"] = items;
	}
	return entry;
}
} // namespace

Array ludot_find_enums(const String &p_query) {
	const auto &catalog = ludot_engine_enum_catalog();
	const String query = p_query.trim_prefix("Enum.");
	Array result;
	for (const Luau::EngineEnum &enumeration : catalog.enums) {
		if (String::utf8(enumeration.name.c_str()) == query) {
			result.push_back(enum_entry(enumeration, true));
			return result;
		}
	}
	const String lower = query.to_lower();
	for (const Luau::EngineEnum &enumeration : catalog.enums) {
		bool matches = String::utf8(enumeration.name.c_str()).to_lower().contains(lower);
		for (const std::string &source : enumeration.sources) matches = matches || String::utf8(source.c_str()).to_lower().contains(lower);
		if (!matches) continue;
		result.push_back(enum_entry(enumeration, enumeration.items.size() <= 40));
		if (result.size() >= 30) break;
	}
	return result;
}

Dictionary ludot_api_docs(const String &p_symbol) {
	using namespace Luau;
	MutexLock lock(analysis_mutex);
	Dictionary result;
	const String symbol = p_symbol.strip_edges();
	result["query"] = symbol;
	Dictionary casing;
	casing["method"] = GLOBAL_GET("luau/api/method_casing");
	casing["property"] = GLOBAL_GET("luau/api/property_casing");
	result["casing"] = casing;
	const String head = symbol.get_slice(".", 0);
	const String rest = symbol.contains(".") ? symbol.substr(head.length() + 1) : String();

	if (head == "Enum" && !rest.is_empty()) {
		const Array found = ludot_find_enums(rest.get_slice(".", 0));
		if (found.size() == 1 && String(Dictionary(found[0])["name"]) == "Enum." + rest.get_slice(".", 0)) {
			Dictionary entry = found[0];
			result["found"] = true;
			result["kind"] = "enum";
			result.merge(entry);
			if (rest.contains(".")) {
				const String item = rest.get_slice(".", 1);
				for (const ::Variant &value : Array(entry["items"])) {
					if (String(Dictionary(value)["name"]) == item) {
						result["kind"] = "enum_item";
						result["name"] = String(entry["name"]) + "." + item;
						result["value"] = Dictionary(value)["value"];
						result.erase("items");
					}
				}
			}
			return result;
		}
		result["found"] = false;
		Array names;
		for (const ::Variant &value : found) names.push_back(Dictionary(value)["name"]);
		result["suggestions"] = names;
		return result;
	}
	bool library = false;
	Array library_members;
	for (const LibraryEntry &entry : library_entries()) {
		if (head != entry.owner) continue;
		library = true;
		const String name = String(entry.name).is_empty() ? String() : (String(entry.kind) == "property" ? LuauLanguage::api_name(entry.name, true) : String(entry.kind) == "function" ? String(entry.name) : LuauLanguage::api_name(entry.name));
		const String signature = String(entry.signature).replace("disconnectAll", LuauLanguage::api_name("disconnect_all")).replace(":fire(", ":" + LuauLanguage::api_name("fire") + "(").replace(":connect(", ":" + LuauLanguage::api_name("connect") + "(").replace(":once(", ":" + LuauLanguage::api_name("once") + "(").replace(":wait(", ":" + LuauLanguage::api_name("wait") + "(").replace(":disconnect(", ":" + LuauLanguage::api_name("disconnect") + "(").replace("connection.Connected", "connection." + LuauLanguage::api_name("connected", true)).replace(":getEnums(", ":" + LuauLanguage::api_name("get_enums") + "(").replace(":fromName(", ":" + LuauLanguage::api_name("from_name") + "(").replace(":fromValue(", ":" + LuauLanguage::api_name("from_value") + "(").replace(":getEnumItems(", ":" + LuauLanguage::api_name("get_enum_items") + "(");
		if (name.is_empty()) {
			if (rest.is_empty()) {
				result["found"] = true;
				result["kind"] = "library";
				result["name"] = head;
				result["signature"] = signature;
				result["description"] = clean_doc(ludot_luau_doc(entry.description, head));
			}
			continue;
		}
		Dictionary member;
		member["name"] = name;
		member["kind"] = entry.kind;
		member["signature"] = signature;
		member["summary"] = first_sentence(entry.description);
		library_members.push_back(member);
		if (!rest.is_empty() && (rest == name || rest == String(entry.name))) {
			result["found"] = true;
			result["kind"] = entry.kind;
			result["name"] = head + "." + name;
			result["signature"] = signature;
			result["description"] = clean_doc(ludot_luau_doc(entry.description, head, entry.name));
		}
	}
	if (library) {
		if (rest.is_empty()) {
			result["members"] = library_members;
			if (head == "Enum") result["enum_count"] = int(ludot_engine_enum_catalog().enums.size());
		}
		if (!result.has("found")) {
			result["found"] = false;
			result["suggestions"] = library_members;
		}
		return result;
	}

	auto &analysis = get_session("res://__ludot_api_docs__.luau");
	const bool known = ClassDB::class_exists(head) || analysis.instances.count(utf8(head));
	if (!known) {
		result["found"] = false;
		Array suggestions;
		const String lower = head.to_lower();
		LocalVector<StringName> classes;
		ClassDB::get_class_list(classes);
		for (const StringName &name : classes) {
			if (String(name).to_lower().contains(lower) && suggestions.size() < 15) suggestions.push_back(name);
		}
		result["suggestions"] = suggestions;
		result["hint"] = "Query an engine class (Node), a member (Node.addChild), a builtin type (Vector3), task, Signal, Connection, Enum, or Enum.<Name>. Your own Luau classes are documented by their source.";
		return result;
	}
	// The member's Luau type from the analyzer, which already applies the casing settings.
	auto type_of = [&](const String &p_class, const DocMember &p_member) -> String {
		for (auto *table : { &analysis.instances, &analysis.classes }) {
			const auto found = table->find(utf8(p_class));
			if (found == table->end()) continue;
			const auto external = get<ExternType>(follow(found->second));
			if (external == nullptr) continue;
			const auto prop = external->props.find(utf8(p_member.luau));
			if (prop == external->props.end() || !prop->second.readTy) continue;
			const TypeId type = follow(*prop->second.readTy);
			if (const auto fn = get<FunctionType>(type)) {
				const auto [args, tail] = flatten(fn->argTypes);
				const size_t skip = fn->hasSelf ? 1 : 0;
				String signature = (p_member.is_static ? p_class + "." : String("object:")) + p_member.luau + "(";
				for (size_t i = skip; i < args.size(); ++i) {
					const int index = int(i - skip);
					const String name = index < p_member.arguments.size() ? p_member.arguments[index] : "arg" + itos(index + 1);
					signature += (i > skip ? ", " : "") + name + ": " + String::utf8(toString(args[i]).c_str());
				}
				if (tail) signature += String(args.size() > skip ? ", " : "") + "...";
				return signature + "): " + String::utf8(toString(fn->retTypes).c_str());
			}
			return String::utf8(toString(type).c_str());
		}
		return String();
	};
	PackedStringArray chain;
	for (StringName current = head; current != StringName(); current = ClassDB::get_parent_class(current)) chain.push_back(current);
	if (rest.is_empty()) {
		const DocData::ClassDoc *doc = class_doc(head);
		result["found"] = true;
		result["kind"] = "class";
		result["name"] = head;
		result["inherits"] = chain.slice(1);
		if (doc != nullptr) {
			result["brief"] = clean_doc(doc->brief_description);
			const String description = clean_doc(ludot_luau_doc(doc->description, head));
			result["description"] = description.length() > 4000 ? description.substr(0, 4000) + "..." : description;
		} else {
			result["docs_loaded"] = false;
		}
		Array members;
		for (const DocMember &member : class_members(head)) {
			Dictionary entry;
			entry["name"] = member.luau;
			entry["godot_name"] = member.godot;
			entry["kind"] = member.kind;
			entry["type"] = type_of(head, member);
			if (member.is_static) entry["static"] = true;
			const String description = member_description(doc, member);
			if (!description.is_empty()) entry["summary"] = first_sentence(description);
			members.push_back(entry);
		}
		result["members"] = members;
		result["note"] = "Members declared by this class only. Query a parent in inherits for its members, or Class.member for one member in full.";
		return result;
	}
	const String wanted = rest.get_slice(".", 0);
	for (const String &owner : chain) {
		for (const DocMember &member : class_members(owner)) {
			if (member.luau != wanted && member.godot != wanted) continue;
			const DocData::ClassDoc *doc = class_doc(owner);
			result["found"] = true;
			result["kind"] = member.kind;
			result["name"] = owner + "." + member.luau;
			result["godot_name"] = member.godot;
			result["declaring_class"] = owner;
			result["type"] = type_of(owner, member);
			if (member.is_static) result["static"] = true;
			result["description"] = member_description(doc, member);
			return result;
		}
	}
	result["found"] = false;
	Array suggestions;
	const String lower = wanted.to_lower();
	for (const String &owner : chain) {
		for (const DocMember &member : class_members(owner)) {
			if ((member.luau.to_lower().contains(lower) || member.godot.contains(lower)) && suggestions.size() < 15) suggestions.push_back(owner + "." + member.luau);
		}
	}
	result["suggestions"] = suggestions;
	return result;
}
#endif
