// Ludot match validation and value refinements. Licensed under MIT.
#include "Luau/LudotMatches.h"
#include "Luau/LudotEnums.h"
#include "Luau/Module.h"
#include "Luau/Scope.h"
#include "Luau/StringUtils.h"
#include "Luau/TypeArena.h"
#include "Luau/TypePack.h"
#include "Luau/TypeUtils.h"
#include "Luau/TypeInfer.h"
#include <map>
#include <set>
#include <unordered_set>

namespace Luau
{
namespace
{
std::string numberKey(double value)
{
    return "n:" + format("%.17g", value == 0 ? 0 : value);
}
std::string enumKey(const EnumTypeInfo& info, size_t index)
{
    return info.isString ? "s:" + info.members[index].string : numberKey(info.members[index].number);
}
std::string literalKey(AstExpr* expression)
{
    if (auto group = expression->as<AstExprGroup>())
        return literalKey(group->expr);
    if (expression->is<AstExprConstantNil>())
        return "nil";
    if (auto boolean = expression->as<AstExprConstantBool>())
        return boolean->value ? "true" : "false";
    if (auto number = expression->as<AstExprConstantNumber>())
        return numberKey(number->value);
    if (auto integer = expression->as<AstExprConstantInteger>())
        return numberKey(double(integer->value));
    if (auto string = expression->as<AstExprConstantString>())
        return "s:" + std::string(string->value.data, string->value.size);
    if (auto unary = expression->as<AstExprUnary>(); unary && unary->op == AstExprUnary::Op::Minus)
    {
        if (auto number = unary->expr->as<AstExprConstantNumber>())
            return numberKey(-number->value);
        if (auto integer = unary->expr->as<AstExprConstantInteger>())
            return numberKey(-double(integer->value));
    }
    return "";
}
TypeId expressionType(const Module& module, AstExpr* expression)
{
    if (auto type = module.astTypes.find(expression))
        return follow(*type);
    return nullptr;
}
struct Domain
{
    std::map<std::string, std::string> values;
    std::set<const EnumTypeInfo*> enums;
    std::unordered_set<TypeId> seen;
    bool collect(TypeId type)
    {
        type = follow(type);
        if (!seen.insert(type).second)
            return is<ExternType>(type) || is<PrimitiveType>(type) || is<SingletonType>(type);
        if (auto enumeration = get<ExternType>(type); enumeration && enumeration->enumInfo)
        {
            enums.insert(enumeration->enumInfo.get());
            const auto& info = *enumeration->enumInfo;
            for (size_t i = 0; i < info.members.size(); ++i)
            {
                if (enumeration->enumMember && enumKey(info, i) != enumKey(info, *enumeration->enumMember))
                    continue;
                values.emplace(enumKey(info, i), info.name + "." + info.members[i].name);
            }
            return true;
        }
        if (auto unionType = get<UnionType>(type))
        {
            bool finite = true;
            for (TypeId part : unionType->options)
                finite = collect(part) && finite;
            return finite;
        }
        if (auto primitive = get<PrimitiveType>(type))
        {
            if (primitive->type == PrimitiveType::Boolean)
            {
                values["true"] = "true";
                values["false"] = "false";
                return true;
            }
            if (primitive->type == PrimitiveType::NilType)
            {
                values["nil"] = "nil";
                return true;
            }
        }
        if (auto singleton = get<SingletonType>(type))
        {
            if (auto boolean = get<BooleanSingleton>(singleton))
            {
                values[boolean->value ? "true" : "false"] = boolean->value ? "true" : "false";
                return true;
            }
            if (auto string = get<StringSingleton>(singleton))
            {
                values["s:" + string->value] = '"' + string->value + '"';
                return true;
            }
        }
        return is<NeverType>(type);
    }
};
TypeId filterEnum(TypeArena& arena, const BuiltinTypes& builtins, TypeId type, const std::string& key)
{
    type = follow(type);
    if (auto enumeration = get<ExternType>(type); enumeration && enumeration->enumInfo)
    {
        const auto& info = *enumeration->enumInfo;
        std::vector<TypeId> members;
        for (size_t i = 0; i < info.members.size(); ++i)
        {
            if (enumKey(info, i) != key || (enumeration->enumMember && enumKey(info, *enumeration->enumMember) != key))
                continue;
            TypeId member = arena.addType(
                ExternType{
                    info.name + "." + info.members[i].name,
                    {},
                    type,
                    enumeration->metatable,
                    {},
                    nullptr,
                    enumeration->definitionModuleName,
                    info.members[i].location
                }
            );
            getMutable<ExternType>(member)->enumInfo = enumeration->enumInfo;
            getMutable<ExternType>(member)->enumMember = i;
            members.push_back(member);
            // Aliases have the same runtime value and nominal member identity.
            break;
        }
        return members.empty() ? builtins.neverType : members.front();
    }
    if (auto unionType = get<UnionType>(type))
    {
        std::vector<TypeId> options;
        for (TypeId part : unionType->options)
        {
            TypeId filtered = filterEnum(arena, builtins, part, key);
            if (filtered != builtins.neverType)
                options.push_back(filtered);
        }
        return options.empty() ? builtins.neverType : options.size() == 1 ? options.front() : arena.addType(UnionType{options});
    }
    if (auto primitive = get<PrimitiveType>(type))
    {
        bool compatible = (primitive->type == PrimitiveType::NilType && key == "nil") ||
                          (primitive->type == PrimitiveType::Boolean && (key == "true" || key == "false")) ||
                          ((primitive->type == PrimitiveType::Number || primitive->type == PrimitiveType::Integer) && key.rfind("n:", 0) == 0) ||
                          (primitive->type == PrimitiveType::String && key.rfind("s:", 0) == 0);
        if (!compatible)
            return builtins.neverType;
    }
    if (auto singleton = get<SingletonType>(type))
    {
        if (auto boolean = get<BooleanSingleton>(singleton); boolean && key != (boolean->value ? "true" : "false"))
            return builtins.neverType;
        if (auto string = get<StringSingleton>(singleton); string && key != "s:" + string->value)
            return builtins.neverType;
    }
    if (key == "nil")
        return builtins.nilType;
    if (key == "true" || key == "false")
        return arena.addType(SingletonType{BooleanSingleton{key == "true"}});
    if (key.rfind("s:", 0) == 0)
        return arena.addType(SingletonType{StringSingleton{key.substr(2)}});
    return builtins.numberType;
}
struct MatchEquality : MagicFunction
{
    std::optional<WithPredicate<TypePackId>> handleOldSolver(
        TypeChecker& checker,
        const std::shared_ptr<Scope>& scope,
        const AstExprCall& call,
        WithPredicate<TypePackId> result
    ) override
    {
        if (call.args.size < 2)
            return std::nullopt;
        auto target = checker.currentModule->astTypes.find(call.args.data[0]);
        if (!target)
            return std::nullopt;
        std::string key = literalKey(call.args.data[1]);
        if (key.empty())
            if (auto pattern = checker.currentModule->astTypes.find(call.args.data[1]))
                key = ludotEnumValueKey(*pattern);
        if (key.empty())
            return std::nullopt;
        TypeId discriminant = filterEnum(*checker.currentModule->internalTypes, *checker.builtinTypes, *target, key);
        for (size_t index : {size_t(0), size_t(2)})
            if (index < call.args.size)
                if (auto lvalue = tryGetLValue(*call.args.data[index]))
                    result.predicates.emplace_back(EqPredicate{*lvalue, discriminant, call.location});
        return result;
    }
    bool infer(const MagicFunctionCallContext&) override
    {
        return false;
    }
    void refine(const MagicRefinementContext& context) override
    {
        if (context.callSite->args.size < 2 || context.discriminantTypes.empty())
            return;
        std::string key = literalKey(context.callSite->args.data[1]);
        auto [arguments, tail] = flatten(context.arguments);
        if (key.empty() && arguments.size() >= 2)
        {
            if (auto member = get<ExternType>(follow(arguments[1])); member && member->enumInfo && member->enumMember)
                key = enumKey(*member->enumInfo, *member->enumMember);
        }
        if (key.empty() || arguments.empty())
            return;
        TypeId discriminant = filterEnum(*context.arena, *context.builtins, arguments.front(), key);
        for (size_t index : {size_t(0), size_t(2)})
        {
            if (index >= context.discriminantTypes.size() || !context.discriminantTypes[index])
                continue;
            TypeId type = follow(*context.discriminantTypes[index]);
            if (is<BlockedType>(type))
                emplaceType<BoundType>(asMutable(type), discriminant);
        }
    }
};
} // namespace
std::string ludotEnumValueKey(TypeId type)
{
    type = follow(type);
    if (auto member = get<ExternType>(type); member && member->enumInfo && member->enumMember)
        return enumKey(*member->enumInfo, *member->enumMember);
    if (auto unionType = get<UnionType>(type))
    {
        std::string key;
        for (TypeId part : unionType->options)
        {
            std::string value = ludotEnumValueKey(part);
            if (value.empty() || (!key.empty() && key != value))
                return "";
            key = value;
        }
        return key;
    }
    return "";
}
std::string ludotCaseKey(AstExpr* expression)
{
    return literalKey(expression);
}
TypeId ludotCaseType(TypeArena& arena, const BuiltinTypes& builtins, TypeId type, const std::string& key)
{
    return filterEnum(arena, builtins, type, key);
}
std::shared_ptr<MagicFunction> makeMatchEqualityMagic()
{
    return std::make_shared<MatchEquality>();
}
std::vector<MatchDiagnostic> checkLudotMatch(const AstMatchInfo& match, const Module& module)
{
    std::vector<MatchDiagnostic> diagnostics;
    Domain domain;
    TypeId subject = expressionType(module, match.subject);
    bool finite = subject && domain.collect(subject);
    if (!finite)
    {
        std::string literal = literalKey(match.subject);
        if (!literal.empty())
        {
            domain.values[literal] = literal;
            finite = true;
        }
    }
    bool otherwise = false, unconditional = false;
    std::set<std::string> covered;
    for (const auto& arm : match.arms)
    {
        if (otherwise || unconditional)
            diagnostics.push_back({arm.location, "Unreachable match arm after an unconditional arm"});
        if (arm.otherwise)
        {
            otherwise = true;
            continue;
        }
        if (arm.guard)
        {
            if (auto value = arm.pattern->as<AstExprConstantBool>())
            {
                if (value->value)
                    unconditional = true;
                else
                    diagnostics.push_back({arm.pattern->location, "Unreachable match arm: guard is always false"});
            }
            continue;
        }
        std::string key = literalKey(arm.pattern);
        TypeId pattern = expressionType(module, arm.pattern);
        if (auto enumeration = pattern ? get<ExternType>(pattern) : nullptr; enumeration && enumeration->enumInfo && enumeration->enumMember)
        {
            key = enumKey(*enumeration->enumInfo, *enumeration->enumMember);
            if (!domain.enums.empty() && !domain.enums.count(enumeration->enumInfo.get()))
            {
                diagnostics.push_back(
                    {arm.pattern->location, "Case belongs to the distinct enum '" + enumeration->enumInfo->name + "', not the subject enum"}
                );
                continue;
            }
        }
        if (key.empty())
        {
            diagnostics.push_back({arm.pattern->location, "Case must be a literal or enum member; use 'case if' for guards"});
            continue;
        }
        if (!covered.insert(key).second)
            diagnostics.push_back({arm.pattern->location, "Duplicate or unreachable match case"});
        if (finite && !domain.values.count(key))
            diagnostics.push_back({arm.pattern->location, "Unreachable case: this value is outside the subject's domain"});
    }
    if (!otherwise)
    {
        std::vector<std::string> missing;
        for (const auto& [key, label] : domain.values)
            if (!covered.count(key))
                missing.push_back(label);
        if (!finite || !missing.empty())
        {
            std::string message = "Non-exhaustive match";
            if (!missing.empty())
            {
                message += ": missing ";
                for (size_t i = 0; i < missing.size(); ++i)
                {
                    if (i)
                        message += ", ";
                    message += missing[i];
                }
                message += ". Add the missing case(s) or otherwise";
            }
            else
                message += ": add otherwise; guards do not establish exhaustive coverage";
            diagnostics.push_back({match.location, message, !match.expression});
        }
    }
    return diagnostics;
}
} // namespace Luau
