// Ludot match validation and value refinements. Licensed under MIT.
#pragma once
#include "Luau/Ast.h"
#include "Luau/Type.h"
#include <string>
#include <vector>
namespace Luau
{
struct Module;
struct MatchDiagnostic
{
    Location location;
    std::string message;
    bool warning = false;
};
std::vector<MatchDiagnostic> checkLudotMatch(const AstMatchInfo& match, const Module& module);
std::shared_ptr<MagicFunction> makeMatchEqualityMagic();
std::string ludotCaseKey(AstExpr* expression);
std::string ludotEnumValueKey(TypeId type);
TypeId ludotCaseType(TypeArena& arena, const BuiltinTypes& builtins, TypeId type, const std::string& key);
} // namespace Luau
