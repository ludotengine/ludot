// Ludot nominal enum metadata. Licensed under the MIT license.
#include "Luau/LudotEnums.h"
#include "Luau/Ast.h"
#include "Luau/Scope.h"
#include <algorithm>

namespace Luau
{
std::optional<PrimitiveType::Type> enumUnderlyingType(TypeId type)
{
    type = follow(type);
    if (auto enumeration = get<ExternType>(type); enumeration && enumeration->enumInfo)
        return enumeration->enumInfo->isString ? PrimitiveType::String : PrimitiveType::Number;
    if (auto unionType = get<UnionType>(type))
    {
        std::optional<PrimitiveType::Type> result;
        for (TypeId part : unionType->options)
        {
            auto underlying = enumUnderlyingType(part);
            if (!underlying || (result && underlying != result))
                return std::nullopt;
            result = underlying;
        }
        return result;
    }
    return std::nullopt;
}
bool enumPrimitiveEquality(TypeId left, TypeId right)
{
    auto compatible = [](auto&& self, TypeId enumeration, TypeId other) -> bool
    {
        enumeration = follow(enumeration);
        other = follow(other);
        if (auto unionType = get<UnionType>(enumeration))
        {
            bool hasValue = false;
            for (TypeId part : unionType->options)
            {
                if (isNil(part))
                    continue;
                if (!self(self, part, other))
                    return false;
                hasValue = true;
            }
            return hasValue;
        }
        if (auto unionType = get<UnionType>(other))
        {
            bool hasValue = false;
            for (TypeId part : unionType->options)
            {
                if (isNil(part))
                    continue;
                if (!self(self, enumeration, part))
                    return false;
                hasValue = true;
            }
            return hasValue;
        }
        auto underlying = enumUnderlyingType(enumeration);
        if (!underlying)
            return false;
        if (auto primitive = get<PrimitiveType>(other))
            return primitive->type == *underlying;
        return *underlying == PrimitiveType::String && is<SingletonType>(other) && get<StringSingleton>(get<SingletonType>(other));
    };
    return compatible(compatible, left, right) || compatible(compatible, right, left);
}

std::shared_ptr<EnumTypeInfo> enumInfoFromAst(const AstEnumInfo& declaration)
{
    auto info = std::make_shared<EnumTypeInfo>();
    info->name = declaration.name.value;
    for (const auto& member : declaration.members)
    {
        EnumMemberInfo value;
        value.name = member.name.value;
        value.location = member.location;
        if (auto string = member.value->as<AstExprConstantString>())
        {
            info->isString = true;
            value.string.assign(string->value.data, string->value.size);
        }
        else if (auto number = member.value->as<AstExprConstantNumber>())
            value.number = number->value;
        info->members.push_back(std::move(value));
    }
    return info;
}
EnumTypes makeEnumTypes(
    TypeArena& arena,
    const BuiltinTypes& builtins,
    std::shared_ptr<EnumTypeInfo> info,
    const ModuleName& module,
    Location location
)
{
    TypeId root = arena.addType(ExternType{info->name, {}, std::nullopt, std::nullopt, {}, nullptr, module, location});
    getMutable<ExternType>(root)->enumInfo = info;
    TableType::Props properties;
    std::vector<TypeId> members;
    for (size_t i = 0; i < info->members.size(); ++i)
    {
        const auto& member = info->members[i];
        TypeId value = nullptr;
        for (size_t j = 0; j < i; ++j)
            if (info->isString ? info->members[j].string == member.string : info->members[j].number == member.number)
            {
                value = members[j];
                break;
            }
        if (!value)
        {
            value = arena.addType(ExternType{info->name + "." + member.name, {}, root, std::nullopt, {}, nullptr, module, member.location});
            auto type = getMutable<ExternType>(value);
            type->enumInfo = info;
            type->enumMember = i;
        }
        members.push_back(value);
        auto property = Property::readonly(value);
        property.location = member.location;
        properties[member.name] = property;
    }
    std::vector<TypeId> unique;
    for (TypeId member : members)
        if (std::find(unique.begin(), unique.end(), member) == unique.end())
            unique.push_back(member);
    TypeId domain = unique.empty() ? builtins.neverType : unique.size() == 1 ? unique.front() : arena.addType(UnionType{unique});
    TypeId optional = arena.addType(UnionType{{domain, builtins.nilType}});
    properties["from"] = Property::readonly(arena.addType(FunctionType{arena.addTypePack({builtins.unknownType}), arena.addTypePack({optional})}));
    TypeId names = arena.addType(TableType{properties, std::nullopt, TypeLevel{}, nullptr, TableState::Sealed});
    getMutable<TableType>(names)->name = info->name;
    TypeId iterator = arena.addType(
        FunctionType{arena.addTypePack({builtins.anyType, builtins.anyType}), arena.addTypePack({builtins.optionalStringType, domain})}
    );
    TypeId iter = arena.addType(FunctionType{arena.addTypePack({names}), arena.addTypePack({iterator, builtins.nilType, builtins.nilType})});
    TypeId metatable = arena.addType(TableType{{{"__iter", Property::readonly(iter)}}, std::nullopt, TypeLevel{}, nullptr, TableState::Sealed});
    TypeId namespaceType = arena.addType(MetatableType{names, metatable});
    TypeId primitive = info->isString ? builtins.stringType : builtins.numberType;
    TableType::Props operators;
    TypeId concatOperand = arena.addType(UnionType{{builtins.stringType, builtins.numberType}});
    operators["__concat"] =
        Property::readonly(arena.addType(FunctionType{arena.addTypePack({root, concatOperand}), arena.addTypePack({builtins.stringType})}));
    for (const char* operation : {"__eq", "__lt", "__le"})
        operators[operation] =
            Property::readonly(arena.addType(FunctionType{arena.addTypePack({root, primitive}), arena.addTypePack({builtins.booleanType})}));
    if (!info->isString)
    {
        for (const char* operation : {"__add", "__sub", "__mul", "__div", "__mod", "__pow"})
            operators[operation] =
                Property::readonly(arena.addType(FunctionType{arena.addTypePack({root, primitive}), arena.addTypePack({primitive})}));
    }
    TypeId operations = arena.addType(TableType{operators, std::nullopt, TypeLevel{}, nullptr, TableState::Sealed});
    getMutable<ExternType>(root)->metatable = operations;
    for (TypeId member : members)
        getMutable<ExternType>(member)->metatable = operations;
    return {domain, namespaceType};
}
} // namespace Luau
