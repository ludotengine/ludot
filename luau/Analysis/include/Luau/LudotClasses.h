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
struct GlobalTypes;
// Declares the engine's task library: spawn, defer, delay, wait, and cancel.
void addLudotTaskLibrary(GlobalTypes& globals);

// The Roblox-style Signal library: the `Signal` global with `Signal.new<T...>(...names)`,
// the `Signal<T...>` type whose pack is the fired arguments, and `Connection`. Engine
// signals use the same type.
struct LudotSignalTypes
{
    TypeId connection = nullptr;
    // Builds `Signal<params...>` directly, for engine signals reflected without an annotation.
    TypeId instantiate(TypeArena& arena, TypePackId params) const;

    const BuiltinTypes* builtins = nullptr;
    std::string fire, connect, once, wait, disconnectAll;
};
LudotSignalTypes addLudotSignalTypes(GlobalTypes& globals, ApiCase methodCase, ApiCase propertyCase);
} // namespace Luau
