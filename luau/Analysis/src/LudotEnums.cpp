// Ludot nominal enum metadata. Licensed under the MIT license.
#include "Luau/LudotEnums.h"
#include "Luau/Ast.h"
#include "Luau/Scope.h"
#include "Luau/GlobalTypes.h"
#include <algorithm>

namespace Luau
{
std::optional<PrimitiveType::Type> enumUnderlyingType(TypeId type)
{
    type = follow(type);
    if (auto enumeration = get<ExternType>(type); enumeration && enumeration->enumInfo && enumeration->enumInfo->isObject)
        return std::nullopt;
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
EngineEnumTypes addEngineEnumTypes(GlobalTypes& globals, const EngineEnumCatalog& catalog, ApiCase methodCase, ApiCase propertyCase)
{
    auto& arena = globals.globalTypes;
    auto scope = globals.globalScope;
    const BuiltinTypes& builtins = *globals.builtinTypes;
    auto external = [&](const std::string& name, std::optional<TypeId> parent)
    {
        return arena.addType(ExternType{name, {}, parent, std::nullopt, {}, nullptr, "ludot", std::nullopt});
    };
    auto array = [&](TypeId element)
    {
        return arena.addType(TableType{TableType::Props{}, TableIndexer{builtins.numberType, element}, TypeLevel{}, scope.get(), TableState::Sealed});
    };
    auto method = [&](TypeId self, std::vector<TypeId> arguments, TypeId result)
    {
        arguments.insert(arguments.begin(), self);
        TypeId fn = arena.addType(FunctionType{arena.addTypePack(arguments), arena.addTypePack({result})});
        getMutable<FunctionType>(fn)->hasSelf = true;
        return Property::readonly(fn);
    };
    auto optional = [&](TypeId type)
    {
        return arena.addType(UnionType{{type, builtins.nilType}});
    };
    const std::string name = engineApiName("name", propertyCase);
    const std::string value = engineApiName("value", propertyCase);
    const std::string enumType = engineApiName("enum_type", propertyCase);
    const std::string getEnumItems = engineApiName("get_enum_items", methodCase);
    const std::string fromName = engineApiName("from_name", methodCase);
    const std::string fromValue = engineApiName("from_value", methodCase);

    EngineEnumTypes result;
    result.item = external("EnumItem", std::nullopt);
    result.enumeration = external("Enum", std::nullopt);
    auto item = getMutable<ExternType>(result.item);
    item->props[name] = Property::readonly(builtins.stringType);
    item->props[value] = Property::readonly(builtins.numberType);
    item->props[enumType] = Property::readonly(result.enumeration);
    auto base = getMutable<ExternType>(result.enumeration);
    base->props[getEnumItems] = method(result.enumeration, {}, array(result.item));
    base->props[fromName] = method(result.enumeration, {builtins.stringType}, optional(result.item));
    base->props[fromValue] = method(result.enumeration, {builtins.numberType}, optional(result.item));

    TypeId global = external("Enums", std::nullopt);
    getMutable<ExternType>(global)->props[engineApiName("get_enums", methodCase)] = method(global, {}, array(result.enumeration));
    for (const EngineEnum& enumeration : catalog.enums)
    {
        auto info = std::make_shared<EnumTypeInfo>();
        info->name = "Enum." + enumeration.name;
        info->isObject = true;
        for (const EngineEnumItem& member : enumeration.items)
            info->members.push_back({member.name, member.value, {}, {}});
        TypeId root = external(info->name, result.item);
        getMutable<ExternType>(root)->enumInfo = info;
        TypeId namespaceType = external(enumeration.name, result.enumeration);
        getMutable<ExternType>(root)->props[enumType] = Property::readonly(namespaceType);
        std::vector<TypeId> members;
        auto space = getMutable<ExternType>(namespaceType);
        for (size_t i = 0; i < enumeration.items.size(); ++i)
        {
            TypeId member = nullptr;
            for (size_t j = 0; j < i; ++j)
                if (enumeration.items[j].value == enumeration.items[i].value)
                    member = members[j];
            if (!member)
            {
                member = external(info->name + "." + enumeration.items[i].name, root);
                getMutable<ExternType>(member)->enumInfo = info;
                getMutable<ExternType>(member)->enumMember = i;
            }
            members.push_back(member);
            space->props[enumeration.items[i].name] = Property::readonly(member);
        }
        space->props[getEnumItems] = method(namespaceType, {}, array(root));
        space->props[fromName] = method(namespaceType, {builtins.stringType}, optional(root));
        space->props[fromValue] = method(namespaceType, {builtins.numberType}, optional(root));
        TypeId iterator = arena.addType(
            FunctionType{arena.addTypePack({builtins.anyType, builtins.anyType}), arena.addTypePack({builtins.optionalStringType, root})}
        );
        TypeId iter = arena.addType(FunctionType{arena.addTypePack({namespaceType}), arena.addTypePack({iterator, builtins.nilType, builtins.nilType})});
        space->metatable = arena.addType(TableType{{{"__iter", Property::readonly(iter)}}, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
        getMutable<ExternType>(global)->props[enumeration.name] = Property::readonly(namespaceType);
        scope->importedTypeBindings["Enum"][enumeration.name] = TypeFun{{}, {}, root};
        result.roots.push_back(root);
        result.members.push_back(std::move(members));
    }
    scope->exportedTypeBindings["EnumItem"] = TypeFun{{}, {}, result.item};
    scope->exportedTypeBindings["Enum"] = TypeFun{{}, {}, result.enumeration};
    scope->bindings[globals.globalNames.names->getOrAdd("Enum")] = Binding{global, Location()};
    return result;
}
} // namespace Luau
