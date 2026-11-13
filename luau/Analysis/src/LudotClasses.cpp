// Ludot engine class identity and refinement. Licensed under MIT.
#include "Luau/LudotClasses.h"
#include "Luau/Ast.h"
#include "Luau/Scope.h"
#include "Luau/TypeArena.h"
#include "Luau/TypeInfer.h"
#include "Luau/TypeChecker2.h"
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
} // namespace Luau
