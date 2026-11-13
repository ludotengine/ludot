// Ludot engine API naming. Licensed under the MIT license.
#pragma once

#include <string>
#include <vector>

namespace Luau
{
enum class ApiCase { Camel, Pascal, Snake };

inline bool parseApiCase(const std::string& name, ApiCase& result)
{
    if (name == "camelCase") result = ApiCase::Camel;
    else if (name == "PascalCase") result = ApiCase::Pascal;
    else if (name == "snake_case") result = ApiCase::Snake;
    else return false;
    return true;
}

// ASCII engine identifiers: preserve leading underscores, split snake words,
// lowercase-to-uppercase transitions, and the last capital of an acronym before
// a lowercase suffix. Digits stay in their word. Never apply this to user names.
inline std::string engineApiName(const std::string& name, ApiCase style)
{
    auto upper = [](char c) { return c >= 'A' && c <= 'Z'; };
    auto lower = [](char c) { return c >= 'a' && c <= 'z'; };
    size_t leading = 0;
    while (leading < name.size() && name[leading] == '_') ++leading;
    std::vector<std::string> words;
    std::string word;
    for (size_t i = leading; i < name.size(); ++i)
    {
        char c = name[i];
        bool boundary = c == '_' || (upper(c) && i > leading &&
            (lower(name[i - 1]) || (upper(name[i - 1]) && i + 1 < name.size() && lower(name[i + 1]))));
        if (boundary && !word.empty()) { words.push_back(word); word.clear(); }
        if (c != '_') word += upper(c) ? char(c + 'a' - 'A') : c;
    }
    if (!word.empty()) words.push_back(word);
    std::string result(leading, '_');
    for (size_t i = 0; i < words.size(); ++i)
    {
        if (style == ApiCase::Snake && i) result += '_';
        if (style == ApiCase::Pascal || (style == ApiCase::Camel && i))
            if (lower(words[i][0])) words[i][0] += 'A' - 'a';
        result += words[i];
    }
    return result;
}
}
