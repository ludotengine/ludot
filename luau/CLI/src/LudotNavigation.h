// Ludot language navigation. Licensed under the MIT license.
#pragma once
#include "Luau/AstQuery.h"
#include "Luau/Frontend.h"
#include "Luau/JsonEmitter.h"
#include "Luau/ToString.h"
#include "Luau/TypePack.h"
#include <set>

namespace LudotNavigation {
using namespace Luau;
struct Site { std::string path; Location location; };
inline void writeSite(Json::JsonEmitter& json, const Site& site) {
    auto object = json.writeObject();
    object.writePair("path", site.path);
    object.writePair("line", site.location.begin.line);
    object.writePair("column", site.location.begin.column);
    object.writePair("endLine", site.location.end.line);
    object.writePair("endColumn", site.location.end.column);
}
struct LocalReferences : AstVisitor {
    AstLocal* target;
    std::vector<Location> locations;
    explicit LocalReferences(AstLocal* target) : target(target) { locations.push_back(target->location); }
    bool visit(AstTypeReference* reference) override {
        if (reference->prefixLocal == target && reference->prefixLocation) locations.push_back(*reference->prefixLocation);
        return true;
    }
    bool visit(AstExprLocal* expr) override {
        if (expr->local == target && expr->location != target->location) locations.push_back(expr->location);
        return false;
    }
};
inline const FunctionType* function(TypeId type) {
    type = follow(type);
    if (auto fn = get<FunctionType>(type)) return fn;
    if (auto overloads = get<IntersectionType>(type))
        for (TypeId part : overloads->parts) if (auto fn = get<FunctionType>(follow(part))) return fn;
    return nullptr;
}
inline void write(Json::JsonEmitter& json, Frontend& frontend, const std::string& path, Position position) {
    auto source = frontend.getSourceModule(path);
    auto module = frontend.moduleResolver.getModule(path);
    std::optional<Site> definition;
    std::optional<Site> rename;
    std::vector<Site> references;
    std::set<std::string> conflicts;
    std::string importName;
    const AstExprCall* call = nullptr;
    const FunctionType* signature = nullptr;
    if (source && module) {
        auto selected = findExprOrLocalAtPosition(*source, position);
        AstLocal* local = selected.getLocal();
        if (auto expr = selected.getExpr())
            if (auto reference = expr->as<AstExprLocal>()) local = reference->local;
        if (!local) {
            for (AstNode* node : findAstAncestryOfPosition(*source, position, true)) {
                if (auto cls = node->as<AstStatClass>(); cls && cls->name->location.contains(position)) local = cls->name;
                if (auto ref = node->as<AstTypeReference>(); ref && ref->prefixLocation && ref->prefixLocation->contains(position)) local = ref->prefixLocal;
            }
        }
        if (local) {
            definition = Site{path, local->location};
            if (local->name != "self" && std::string(local->name.value).rfind("__ludot_", 0) != 0) {
                rename = Site{path, local->location};
                LocalReferences finder(local);
                source->root->visit(&finder);
                std::set<std::tuple<unsigned, unsigned, unsigned, unsigned>> seen;
                for (const Location& location : finder.locations) {
                    if (!seen.emplace(location.begin.line, location.begin.column, location.end.line, location.end.column).second) continue;
                    references.push_back({path, location});
                    for (auto scope = findScopeAtPosition(*module, location.begin); scope; scope = scope->parent)
                        for (const auto& [symbol, binding] : scope->bindings)
                            if (symbol.local != local) conflicts.insert(symbol.c_str());
                }
            }
        }
        if (local) {
            for (AstStat* stat : source->root->body) {
                auto declaration = stat->as<AstStatLocal>();
                if (!declaration) continue;
                for (size_t i = 0; i < declaration->vars.size && i < declaration->values.size; ++i) {
                    if (declaration->vars.data[i] != local) continue;
                    auto index = declaration->values.data[i]->as<AstExprIndexName>();
                    auto ns = index ? index->expr->as<AstExprLocal>() : nullptr;
                    // Import items lower to `local name = ns.name` with the value placed on the binding itself.
                    if (ns && index->location == local->location) importName = index->index.value;
                }
            }
        }
        if (auto type = findTypeAtPosition(*module, *source, position)) {
            if (auto fn = function(*type); fn && fn->definition && fn->definition->definitionModuleName) {
                const auto& def = *fn->definition;
                if (!local || *def.definitionModuleName != path) {
                    const Location location = def.originalNameLocation != Location() ? def.originalNameLocation : def.definitionLocation;
                    definition = Site{*def.definitionModuleName, location};
                }
            }
        }
        // Clicking an import/require path opens the resolved module.
        if (auto trace = frontend.requireTrace.find(path); trace != frontend.requireTrace.end()) {
            for (const auto& [node, info] : trace->second.exprs)
                if (node->is<AstExprConstantString>() && node->location.contains(position)) definition = Site{info.name, Location()};
        }
        auto ancestry = findAncestryAtPositionForAutocomplete(*source, position);
        for (auto it = ancestry.rbegin(); it != ancestry.rend(); ++it) {
            auto candidate = (*it)->as<AstExprCall>();
            if (!candidate || position < candidate->argLocation.begin) continue;
            if (auto type = module->astTypes.find(candidate->func)) signature = function(*type);
            if (signature) { call = candidate; break; }
        }
    }
    json.writeRaw(",\"definition\":");
    if (definition) writeSite(json, *definition); else json.writeRaw("null");
    json.writeRaw(",\"rename\":");
    if (rename) writeSite(json, *rename); else json.writeRaw("null");
    json.writeRaw(",\"importName\":"); Json::write(json, importName);
    json.writeRaw(",\"references\":[");
    for (size_t i = 0; i < references.size(); ++i) { if (i) json.writeRaw(','); writeSite(json, references[i]); }
    json.writeRaw("],\"renameConflicts\":[");
    bool first = true;
    for (const auto& conflict : conflicts) { if (!first) json.writeRaw(','); first = false; Json::write(json, conflict); }
    json.writeRaw("],\"signatureHelp\":");
    if (!signature || !call) { json.writeRaw("null"); return; }
    auto [args, tail] = flatten(signature->argTypes);
    const size_t skip = call->self && signature->hasSelf ? 1 : 0;
    std::vector<std::string> parameters;
    std::string label = "(";
    for (size_t i = skip; i < args.size(); ++i) {
        std::string parameter = i < signature->argNames.size() && signature->argNames[i] ? signature->argNames[i]->name : "arg" + std::to_string(i - skip + 1);
        parameter += ": " + toString(args[i]);
        if (!parameters.empty()) label += ", ";
        label += parameter;
        parameters.push_back(parameter);
    }
    const auto variadic = tail ? get<VariadicTypePack>(follow(*tail)) : nullptr;
    if (tail && (!variadic || !variadic->hidden)) {
        std::string parameter = "...: " + (variadic ? toString(variadic->ty) : toString(*tail));
        if (!parameters.empty()) label += ", ";
        label += parameter;
        parameters.push_back(parameter);
    }
    label += ") -> " + toString(signature->retTypes);
    unsigned active = 0;
    for (AstExpr* arg : call->args) if (arg->location.end < position) ++active;
    if (!parameters.empty()) active = std::min(active, unsigned(parameters.size() - 1));
    json.writeRaw("{\"signatures\":[{\"label\":"); Json::write(json, label);
    json.writeRaw(",\"parameters\":[");
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i) json.writeRaw(',');
        auto object = json.writeObject(); object.writePair("label", parameters[i]);
    }
    json.writeRaw(" ]}],\"activeSignature\":0,\"activeParameter\":");
    Json::write(json, active);
    json.writeRaw('}');
}
}
