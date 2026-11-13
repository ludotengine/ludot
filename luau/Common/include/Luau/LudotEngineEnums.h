// Ludot engine enum catalog. Licensed under the MIT license.
#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Luau
{
// One engine enum as reflected by Godot. Global enums have no owner, except
// dotted globals such as Variant.Type, which keep their owner and set global.
struct EngineEnumSource
{
    std::string owner;
    std::string name;
    bool global = false;
    bool bitfield = false;
    std::vector<std::pair<std::string, double>> constants;
};

struct EngineEnumItem
{
    std::string name;
    double value = 0;
};

struct EngineEnum
{
    std::string name;
    bool bitfield = false;
    std::vector<EngineEnumItem> items;
    // Godot spellings that map to this enum, such as Input.MouseMode or Key.
    std::vector<std::string> sources;
};

struct EngineEnumCatalog
{
    std::vector<EngineEnum> enums;
    std::unordered_map<std::string, size_t> bySource;
    // Owner.CONSTANT, or CONSTANT for undotted globals, to its enum and item index.
    std::unordered_map<std::string, std::pair<size_t, size_t>> byConstant;
};

namespace detail
{
inline std::vector<std::string> splitEngineConstant(const std::string& constant)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    while (true)
    {
        size_t end = constant.find('_', start);
        tokens.push_back(constant.substr(start, end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos)
            return tokens;
        start = end + 1;
    }
}

inline std::string pascalEngineWord(const std::string& token)
{
    std::string result = token;
    for (size_t i = 0; i < result.size(); ++i)
    {
        char c = result[i];
        bool first = i == 0 || (result[i - 1] >= '0' && result[i - 1] <= '9');
        if (first && c >= 'a' && c <= 'z')
            result[i] = char(c - 'a' + 'A');
        else if (!first && c >= 'A' && c <= 'Z')
            result[i] = char(c - 'A' + 'a');
    }
    return result;
}

// Removes the words every constant shares, so MOUSE_MODE_CAPTURED becomes Captured.
// Each constant keeps at least one word. A name that would start with a digit keeps
// the last shared word, so KEY_0 becomes Key0.
inline std::vector<std::string> engineItemNames(const std::vector<std::pair<std::string, double>>& constants)
{
    std::vector<std::vector<std::string>> tokens;
    for (const auto& constant : constants)
        tokens.push_back(splitEngineConstant(constant.first));
    size_t shared = 0;
    while (!tokens.empty())
    {
        bool same = true;
        for (const auto& words : tokens)
            same = same && words.size() > shared + 1 && words[shared] == tokens.front()[shared];
        if (!same)
            break;
        ++shared;
    }
    std::vector<std::string> names;
    for (const auto& words : tokens)
    {
        std::string name;
        for (size_t i = shared; i < words.size(); ++i)
            name += pascalEngineWord(words[i]);
        if (name.empty() || (name[0] >= '0' && name[0] <= '9'))
            name = (shared > 0 ? pascalEngineWord(words[shared - 1]) : std::string("Value")) + name;
        names.push_back(name);
    }
    return names;
}

struct EngineEnumDraft
{
    std::string name;
    std::vector<const EngineEnumSource*> sources;
};

struct EngineEnumNamed
{
    const EngineEnumSource* source;
    std::vector<EngineEnumItem> items;
};

inline std::vector<std::vector<const EngineEnumSource*>> clusterEngineEnums(
    std::vector<const EngineEnumSource*> sources,
    const std::unordered_map<const EngineEnumSource*, std::vector<EngineEnumItem>>& items
)
{
    std::stable_sort(
        sources.begin(),
        sources.end(),
        [&](const EngineEnumSource* left, const EngineEnumSource* right)
        {
            return items.at(left).size() > items.at(right).size();
        }
    );
    std::vector<std::vector<const EngineEnumSource*>> clusters;
    std::vector<std::map<std::string, double>> tables;
    for (const EngineEnumSource* source : sources)
    {
        bool placed = false;
        for (size_t i = 0; i < clusters.size() && !placed; ++i)
        {
            bool fits = true;
            for (const auto& item : items.at(source))
            {
                auto found = tables[i].find(item.name);
                fits = fits && found != tables[i].end() && found->second == item.value;
            }
            if (fits)
            {
                clusters[i].push_back(source);
                placed = true;
            }
        }
        if (!placed)
        {
            clusters.push_back({source});
            tables.emplace_back();
            for (const auto& item : items.at(source))
                tables.back()[item.name] = item.value;
        }
    }
    return clusters;
}

inline std::string qualifiedEngineEnumName(const std::vector<const EngineEnumSource*>& sources, const std::string& name)
{
    std::string owner = sources.front()->owner;
    for (const EngineEnumSource* source : sources)
        owner = std::min(owner, source->owner);
    return owner + name;
}
} // namespace detail

// Builds the single Enum namespace. An enum keeps its Godot name when the name is
// unique. Same-named enums whose items agree, including when one is a subset of the
// other, become one enum. Other same-named class enums use Owner + Name, such as
// Light3DParam, while an undotted global enum keeps the plain name.
inline EngineEnumCatalog buildEngineEnumCatalog(std::vector<EngineEnumSource> input)
{
    using namespace detail;
    auto qualified = [](const EngineEnumSource& source)
    {
        return source.owner.empty() ? source.name : source.owner + "." + source.name;
    };
    std::sort(
        input.begin(),
        input.end(),
        [&](const EngineEnumSource& left, const EngineEnumSource& right)
        {
            return qualified(left) < qualified(right);
        }
    );
    std::unordered_map<const EngineEnumSource*, std::vector<EngineEnumItem>> items;
    std::vector<std::string> order;
    std::map<std::string, std::vector<const EngineEnumSource*>> groups;
    for (const EngineEnumSource& source : input)
    {
        std::vector<std::string> names = engineItemNames(source.constants);
        auto& list = items[&source];
        for (size_t i = 0; i < names.size(); ++i)
            list.push_back({names[i], source.constants[i].second});
        if (!groups.count(source.name))
            order.push_back(source.name);
        groups[source.name].push_back(&source);
    }
    std::vector<EngineEnumDraft> drafts;
    for (const std::string& name : order)
    {
        auto clusters = clusterEngineEnums(groups[name], items);
        for (auto& cluster : clusters)
        {
            bool global = std::any_of(cluster.begin(), cluster.end(), [](const EngineEnumSource* source) { return source->global && source->owner.empty(); });
            drafts.push_back({clusters.size() == 1 || global ? name : qualifiedEngineEnumName(cluster, name), cluster});
        }
    }
    // A plain name can equal another enum's qualified name, such as CanvasItem + TextureFilter.
    for (int pass = 0; pass < 16; ++pass)
    {
        std::map<std::string, std::vector<size_t>> byName;
        for (size_t i = 0; i < drafts.size(); ++i)
            byName[drafts[i].name].push_back(i);
        bool changed = false;
        std::vector<bool> removed(drafts.size(), false);
        std::vector<EngineEnumDraft> added;
        for (const auto& [name, indices] : byName)
        {
            if (indices.size() < 2)
                continue;
            changed = true;
            std::vector<const EngineEnumSource*> sources;
            for (size_t index : indices)
                sources.insert(sources.end(), drafts[index].sources.begin(), drafts[index].sources.end());
            auto merged = clusterEngineEnums(sources, items);
            if (merged.size() == 1)
            {
                for (size_t index : indices)
                    removed[index] = true;
                added.push_back({name, merged.front()});
                continue;
            }
            for (size_t index : indices)
            {
                auto& draft = drafts[index];
                bool plain = std::all_of(draft.sources.begin(), draft.sources.end(), [&](const EngineEnumSource* source) { return source->name == name && !(source->global && source->owner.empty()); });
                if (plain)
                    draft.name = qualifiedEngineEnumName(draft.sources, name);
            }
            std::map<std::string, int> counts;
            for (size_t index : indices)
                ++counts[drafts[index].name];
            for (size_t index : indices)
                if (counts[drafts[index].name] > 1)
                    drafts[index].name = qualifiedEngineEnumName(drafts[index].sources, "_" + drafts[index].sources.front()->name);
        }
        if (!changed)
            break;
        std::vector<EngineEnumDraft> next;
        for (size_t i = 0; i < drafts.size(); ++i)
            if (!removed[i])
                next.push_back(std::move(drafts[i]));
        for (auto& draft : added)
            next.push_back(std::move(draft));
        drafts = std::move(next);
    }
    std::sort(
        drafts.begin(),
        drafts.end(),
        [](const EngineEnumDraft& left, const EngineEnumDraft& right)
        {
            return left.name < right.name;
        }
    );
    EngineEnumCatalog catalog;
    for (const auto& draft : drafts)
    {
        EngineEnum enumeration;
        enumeration.name = draft.name;
        std::unordered_map<std::string, size_t> indices;
        for (const EngineEnumSource* source : draft.sources)
        {
            enumeration.bitfield = enumeration.bitfield || source->bitfield;
            enumeration.sources.push_back(qualified(*source));
            for (const auto& item : items.at(source))
                if (indices.emplace(item.name, enumeration.items.size()).second)
                    enumeration.items.push_back(item);
        }
        const size_t index = catalog.enums.size();
        for (const EngineEnumSource* source : draft.sources)
        {
            catalog.bySource[qualified(*source)] = index;
            const auto& list = items.at(source);
            for (size_t i = 0; i < list.size(); ++i)
            {
                const std::string key = source->owner.empty() ? source->constants[i].first : source->owner + "." + source->constants[i].first;
                catalog.byConstant[key] = {index, indices.at(list[i].name)};
            }
        }
        std::sort(enumeration.sources.begin(), enumeration.sources.end());
        catalog.enums.push_back(std::move(enumeration));
    }
    return catalog;
}
} // namespace Luau
