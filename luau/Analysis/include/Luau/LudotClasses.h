// Ludot engine class identity and refinement. Licensed under MIT.
#pragma once
#include "Luau/LudotNaming.h"
#include "Luau/Type.h"
#include <string>
#include <vector>
namespace Luau
{
TypeId findLudotClass(const Scope& scope, const std::string& name);
std::vector<std::string> ludotClassNames(const Scope& scope);
void addLudotIsA(TypeArena& arena, const BuiltinTypes& builtins, TypeId object, ApiCase methodCase);
} // namespace Luau
