// Ludot source-level lint suppression. Licensed under MIT.
#include "Luau/LudotLint.h"
#include "Luau/Ast.h"
#include "Luau/ParseResult.h"
#include <limits>

namespace Luau
{
bool ludotLintSuppressed(std::string_view source, AstStatBlock* root, const std::vector<Comment>& comments, Location warning, std::string_view rule)
{
    std::vector<size_t> lines{0};
    for (size_t i = 0; i < source.size(); ++i)
        if (source[i] == '\n')
            lines.push_back(i + 1);
    auto offset = [&](Position position)
    {
        return lines.at(position.line) + position.column;
    };
    struct Statements : AstVisitor
    {
        std::vector<Location> locations;
        bool visit(AstStat* node) override
        {
            locations.push_back(node->location);
            return true;
        }
    } statements;
    root->visit(&statements);
    for (const auto& comment : comments)
    {
        std::string_view text = source.substr(offset(comment.location.begin), offset(comment.location.end) - offset(comment.location.begin));
        const bool global = text.substr(0, 18) == "--# selene: allow(";
        if (!global && text.substr(0, 17) != "-- selene: allow(")
            continue;
        size_t begin = text.find('('), end = text.find(')', begin);
        if (end == std::string_view::npos)
            continue;
        unsigned first = 0, last = std::numeric_limits<unsigned>::max();
        if (global)
        {
            if (root->body.size && !(comment.location.end <= root->body.data[0]->location.begin))
                continue;
        }
        else
        {
            Position next(std::numeric_limits<unsigned>::max(), std::numeric_limits<unsigned>::max());
            for (const auto& location : statements.locations)
                if (location.begin >= comment.location.end && location.begin < next)
                {
                    next = location.begin;
                    first = location.begin.line;
                    last = location.end.line;
                }
            if (next.line == std::numeric_limits<unsigned>::max())
                continue;
        }
        if (warning.begin.line < first || warning.begin.line > last)
            continue;
        std::string_view rules = text.substr(begin + 1, end - begin - 1);
        while (!rules.empty())
        {
            size_t comma = rules.find(',');
            std::string_view name = rules.substr(0, comma);
            size_t start = name.find_first_not_of(" \t\r\n"), finish = name.find_last_not_of(" \t\r\n");
            if (start != std::string_view::npos)
                name = name.substr(start, finish - start + 1);
            if (name == rule || name == "all")
                return true;
            if (comma == std::string_view::npos)
                break;
            rules.remove_prefix(comma + 1);
        }
    }
    return false;
}
} // namespace Luau
