// Ludot nominal enum metadata. Licensed under the MIT license.
#pragma once
#include "Luau/Type.h"
#include "Luau/TypeArena.h"
#include "Luau/LudotEngineEnums.h"
#include "Luau/LudotNaming.h"
#include <string>
#include <vector>

namespace Luau
{
struct AstEnumInfo;
struct EnumMemberInfo
{
    std::string name;
    double number = 0;
    std::string string;
    Location location;
};
struct EnumTypeInfo
{
    std::string name;
    bool isString = false;
    // Engine EnumItems are objects, so they never compare or convert as numbers.
    bool isObject = false;
    std::vector<EnumMemberInfo> members;
};
struct EnumTypes
{
    TypeId value;
    TypeId names;
};
std::optional<PrimitiveType::Type> enumUnderlyingType(TypeId type);
bool enumPrimitiveEquality(TypeId left, TypeId right);
EnumTypes makeEnumTypes(
    TypeArena& arena,
    const BuiltinTypes& builtins,
    std::shared_ptr<EnumTypeInfo> info,
    const ModuleName& module,
    Location location
);
std::shared_ptr<EnumTypeInfo> enumInfoFromAst(const AstEnumInfo& declaration);
struct GlobalTypes;
struct EngineEnumTypes
{
    TypeId item = nullptr;
    TypeId enumeration = nullptr;
    // EnumItem type for each catalog enum, such as Enum.MouseMode.
    std::vector<TypeId> roots;
    // Nominal member types for each catalog item, used for match exhaustiveness.
    std::vector<std::vector<TypeId>> members;
};
// Declares the Enum global, the EnumItem and Enum types, and Enum.<Name> annotations.
EngineEnumTypes addEngineEnumTypes(GlobalTypes& globals, const EngineEnumCatalog& catalog, ApiCase methodCase, ApiCase propertyCase);
} // namespace Luau
