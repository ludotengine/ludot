// Ludot nominal enum metadata. Licensed under the MIT license.
#pragma once
#include "Luau/Type.h"
#include "Luau/TypeArena.h"
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
} // namespace Luau
