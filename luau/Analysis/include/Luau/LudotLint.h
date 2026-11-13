// Ludot source-level lint suppression. Licensed under MIT.
#pragma once
#include "Luau/Location.h"
#include <string_view>
#include <vector>
namespace Luau
{
class AstStatBlock;
struct Comment;
bool ludotLintSuppressed(std::string_view source, AstStatBlock* root, const std::vector<Comment>& comments, Location warning, std::string_view rule);
} // namespace Luau
