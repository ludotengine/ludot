// Ludot engine class identity and refinement. Licensed under MIT.
#include "Luau/LudotClasses.h"
#include "Luau/Ast.h"
#include "Luau/Scope.h"
#include "Luau/TypeArena.h"
#include "Luau/TypeInfer.h"
#include "Luau/TypeChecker2.h"
#include "Luau/GlobalTypes.h"
#include <set>

namespace Luau
{
namespace
{
bool isEngineClass(TypeId type, const std::string& name)
{
    const auto instance = get<ExternType>(follow(type));
    if (!instance || instance->enumInfo || instance->name != name || (instance->relation && instance->relation->get_if<Obj>()))
        return false;
    for (auto ancestor = instance; ancestor; ancestor = ancestor->parent ? get<ExternType>(follow(*ancestor->parent)) : nullptr)
        if (ancestor->name == "Object" && ancestor->definitionModuleName == "ludot")
            return true;
    return false;
}
AstExprConstantString* classArgument(const AstExprCall& call)
{
    if (!call.self || call.args.size != 1)
        return nullptr;
    AstExpr* value = call.args.data[0];
    while (auto group = value->as<AstExprGroup>())
        value = group->expr;
    return value->as<AstExprConstantString>();
}
std::string stringValue(const AstExprConstantString& value)
{
    return {value.value.data, value.value.size};
}
struct ClassIdentity : MagicFunction
{
    std::optional<WithPredicate<TypePackId>> handleOldSolver(
        TypeChecker& checker,
        const ScopePtr& scope,
        const AstExprCall& call,
        WithPredicate<TypePackId> result
    ) override
    {
        auto literal = classArgument(call);
        if (!literal)
            return std::nullopt;
        auto target = findLudotClass(*scope, stringValue(*literal));
        if (!target)
            checker.reportError(literal->location, GenericError{"Unknown Godot class '" + stringValue(*literal) + "'"});
        else if (auto receiver = call.func->as<AstExprIndexName>())
            if (auto lvalue = tryGetLValue(*receiver->expr))
                result.predicates.emplace_back(IsAPredicate{*lvalue, call.location, target});
        return result;
    }
    bool infer(const MagicFunctionCallContext&) override
    {
        return false;
    }
    void refine(const MagicRefinementContext& context) override
    {
        auto literal = classArgument(*context.callSite);
        if (!literal || context.discriminantTypes.empty() || !context.discriminantTypes[0])
            return;
        auto target = findLudotClass(*context.scope, stringValue(*literal));
        if (!target)
            return;
        TypeId discriminant = follow(*context.discriminantTypes[0]);
        if (is<BlockedType>(discriminant))
            emplaceType<BoundType>(asMutable(discriminant), target);
    }
    bool typeCheck(const MagicFunctionTypeCheckContext& context) override
    {
        if (auto literal = classArgument(*context.callSite); literal && !findLudotClass(*context.checkScope, stringValue(*literal)))
            context.typechecker->reportError(GenericError{"Unknown Godot class '" + stringValue(*literal) + "'"}, literal->location);
        return false;
    }
};
} // namespace
TypeId findLudotClass(const Scope& scope, const std::string& name)
{
    for (const Scope* current = &scope; current; current = current->parent.get())
    {
        for (const auto* types : {&current->privateTypeBindings, &current->exportedTypeBindings})
            if (auto type = types->find(name); type != types->end() && isEngineClass(type->second.type, name))
                return type->second.type;
        for (const auto& [alias, types] : current->importedTypeBindings)
            for (const auto& [typeName, type] : types)
                if (isEngineClass(type.type, name))
                    return type.type;
    }
    return nullptr;
}
std::vector<std::string> ludotClassNames(const Scope& scope)
{
    std::set<std::string> names;
    auto collect = [&](const auto& types)
    {
        for (const auto& [name, type] : types)
            if (auto instance = get<ExternType>(follow(type.type)); instance && isEngineClass(type.type, instance->name))
                names.insert(instance->name);
    };
    for (const Scope* current = &scope; current; current = current->parent.get())
    {
        collect(current->privateTypeBindings);
        collect(current->exportedTypeBindings);
        for (const auto& [alias, types] : current->importedTypeBindings)
            collect(types);
    }
    return {names.begin(), names.end()};
}
void addLudotIsA(TypeArena& arena, const BuiltinTypes& builtins, TypeId object, ApiCase methodCase)
{
    auto function =
        arena.addType(FunctionType{arena.addTypePack({object, builtins.stringType}), arena.addTypePack({builtins.booleanType}), {}, true});
    auto type = getMutable<FunctionType>(function);
    type->magic = std::make_shared<ClassIdentity>();
    type->argNames = {FunctionArgument{"self", {}}, FunctionArgument{"name", {}}};
    type->tags.push_back("LudotClassName");
    const std::string canonical = engineApiName("is_a", methodCase);
    auto instance = getMutable<ExternType>(object);
    for (const std::string& name : {canonical, std::string("isA"), std::string("IsA")})
    {
        auto property = Property::readonly(function);
        if (name != canonical)
            property.tags.push_back("LudotHiddenAlias");
        instance->props[name] = property;
    }
}
void addLudotTaskLibrary(GlobalTypes& globals)
{
    auto& arena = globals.globalTypes;
    const BuiltinTypes& builtins = *globals.builtinTypes;
    TypePackId anyPack = arena.addTypePack(VariadicTypePack{builtins.anyType});
    TypeId callback = arena.addType(FunctionType{anyPack, anyPack});
    TypeId target = arena.addType(UnionType{{callback, builtins.threadType}});
    auto function = [&](std::vector<TypeId> arguments, bool variadic, TypePackId results)
    {
        return Property::readonly(arena.addType(FunctionType{arena.addTypePack(arguments, variadic ? std::optional<TypePackId>{anyPack} : std::nullopt), results}));
    };
    TypePackId thread = arena.addTypePack({builtins.threadType});
    TableType::Props props;
    props["spawn"] = function({target}, true, thread);
    props["defer"] = function({target}, true, thread);
    props["delay"] = function({builtins.optionalNumberType, target}, true, thread);
    props["wait"] = function({builtins.optionalNumberType}, false, arena.addTypePack({builtins.numberType}));
    props["cancel"] = function({builtins.threadType}, false, builtins.emptyTypePack);
    TypeId library = arena.addType(TableType{props, std::nullopt, TypeLevel{}, globals.globalScope.get(), TableState::Sealed});
    getMutable<TableType>(library)->name = "task";
    globals.globalScope->bindings[globals.globalNames.names->getOrAdd("task")] = Binding{library, Location()};
}

namespace
{
// The Signal table type for one argument pack, which may be the alias's generic pack.
TypeId buildSignalType(TypeArena& arena, const LudotSignalTypes& types, TypePackId params)
{
    const BuiltinTypes& builtins = *types.builtins;
    TypePackId anyPack = arena.addTypePack(VariadicTypePack{builtins.anyType});
    TypeId signal = arena.addType(TableType{TableState::Sealed, TypeLevel{}});
    TypeId handler = arena.addType(FunctionType{params, anyPack});
    auto method = [&](TypePackId arguments, TypePackId results)
    {
        TypeId function = arena.addType(FunctionType{arguments, results});
        getMutable<FunctionType>(function)->hasSelf = true;
        return Property::readonly(function);
    };
    TypePackId connection = arena.addTypePack({types.connection});
    TableType* table = getMutable<TableType>(signal);
    table->props[types.fire] = method(arena.addTypePack({signal}, params), builtins.emptyTypePack);
    table->props[types.connect] = method(arena.addTypePack({signal, handler}), connection);
    table->props[types.once] = method(arena.addTypePack({signal, handler}), connection);
    table->props[types.wait] = method(arena.addTypePack({signal}), params);
    table->props[types.disconnectAll] = method(arena.addTypePack({signal}), builtins.emptyTypePack);
    return signal;
}
} // namespace

TypeId LudotSignalTypes::instantiate(TypeArena& arena, TypePackId params) const
{
    TypeId signal = buildSignalType(arena, *this, params);
    TableType* table = getMutable<TableType>(signal);
    table->name = "Signal";
    table->instantiatedTypePackParams = {params};
    return signal;
}

LudotSignalTypes addLudotSignalTypes(GlobalTypes& globals, ApiCase methodCase, ApiCase propertyCase)
{
    auto& arena = globals.globalTypes;
    const BuiltinTypes& builtins = *globals.builtinTypes;
    LudotSignalTypes types;
    types.builtins = &builtins;
    types.fire = engineApiName("fire", methodCase);
    types.connect = engineApiName("connect", methodCase);
    types.once = engineApiName("once", methodCase);
    types.wait = engineApiName("wait", methodCase);
    types.disconnectAll = engineApiName("disconnect_all", methodCase);

    types.connection = arena.addType(ExternType{"Connection", {}, std::nullopt, std::nullopt, {}, nullptr, "ludot", std::nullopt});
    TypeId disconnect = arena.addType(FunctionType{arena.addTypePack({types.connection}), builtins.emptyTypePack});
    getMutable<FunctionType>(disconnect)->hasSelf = true;
    ExternType* connection = getMutable<ExternType>(types.connection);
    connection->props[engineApiName("disconnect", methodCase)] = Property::readonly(disconnect);
    connection->props[engineApiName("connected", propertyCase)] = Property::readonly(builtins.booleanType);
    globals.globalScope->exportedTypeBindings["Connection"] = TypeFun{{}, types.connection};

    // The type: Signal<T...>, whose pack is the fired arguments. Signal<> fires nothing.
    TypePackId generic = arena.addTypePack(GenericTypePack{"T"});
    TypeFun signal{{}, buildSignalType(arena, types, generic)};
    getMutable<TableType>(signal.type)->name = "Signal";
    signal.typePackParams.push_back(GenericTypePackDefinition{generic, std::nullopt});
    globals.globalScope->exportedTypeBindings["Signal"] = signal;

    // The value: Signal.new<T...>(...names), which takes the pack from its annotation.
    TypePackId fresh = arena.addTypePack(GenericTypePack{"T"});
    TypeId constructor = arena.addType(FunctionType{
        {}, {fresh}, arena.addTypePack(VariadicTypePack{builtins.stringType}), arena.addTypePack({types.instantiate(arena, fresh)})
    });
    TypeId library = arena.addType(TableType{
        {{"new", Property::readonly(constructor)}}, std::nullopt, TypeLevel{}, globals.globalScope.get(), TableState::Sealed
    });
    getMutable<TableType>(library)->name = "Signal";
    globals.globalScope->bindings[globals.globalNames.names->getOrAdd("Signal")] = Binding{library, Location()};
    return types;
}
} // namespace Luau
