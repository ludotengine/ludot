// Ludot nominal engine declarations. Licensed under the MIT license.
#pragma once
#include "Luau/Frontend.h"
#include "Luau/Type.h"
#include "Luau/TypeArena.h"
#include "Luau/LudotNaming.h"
#include "Luau/LudotClasses.h"
#include "Luau/LudotEnums.h"
#include <sstream>
#include <unordered_map>

inline bool loadLudotApi(Luau::Frontend& frontend, const std::string& source, Luau::ApiCase methodCase = Luau::ApiCase::Camel, Luau::ApiCase propertyCase = Luau::ApiCase::Pascal)
{
    using namespace Luau;
    std::istringstream stream(source);
    std::string line;
    if (!std::getline(stream, line)) return false;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line != "LUDOT_API_1" && line != "LUDOT_API_2" && line != "LUDOT_API_3")
        return false;
    std::vector<std::vector<std::string>> rows;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::vector<std::string> fields;
        size_t start = 0;
        do
        {
            size_t end = line.find('\t', start);
            fields.push_back(line.substr(start, end == std::string::npos ? end : end - start));
            if (end == std::string::npos) break;
            start = end + 1;
        } while (true);
        rows.push_back(std::move(fields));
    }
    auto& arena = frontend.globals.globalTypes;
    auto scope = frontend.globals.globalScope;
    auto builtins = frontend.builtinTypes;
    std::unordered_map<std::string, TypeId> instances, classes;
    auto bind = [&](const std::string& name, TypeId type) {
        scope->bindings[frontend.globals.globalNames.names->getOrAdd(name.c_str())] = Binding{type, Location()};
    };
    for (const auto& row : rows)
    {
        if (row[0] != "C" || row.size() != 4) continue;
        TypeId instance = arena.addType(ExternType{row[1], {}, std::nullopt, std::nullopt, {}, nullptr, "ludot", std::nullopt});
        TypeId constructor = arena.addType(FunctionType{arena.addTypePack({}, arena.addTypePack(VariadicTypePack{builtins->anyType})), arena.addTypePack({instance})});
        TypeId metatable = arena.addType(TableType{{{"__call", Property::readonly(constructor)}}, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
        TypeId cls = arena.addType(ExternType{row[1], {}, builtins->classType, metatable, {}, nullptr, "ludot", std::nullopt, true});
        getMutable<ExternType>(cls)->relation = Obj{instance};
        getMutable<ExternType>(cls)->props["new"] = Property::readonly(constructor);
        getMutable<ExternType>(instance)->relation = Klass{cls};
        instances[row[1]] = instance;
        classes[row[1]] = cls;
        scope->exportedTypeBindings[row[1]] = TypeFun{{}, {}, instance};
        bind(row[1], cls);
    }
    std::unordered_map<std::string, EnumTypes> enums;
    for (const auto& row : rows)
    {
        if (row[0] != "G" || row.size() < 3 || (row.size() - 3) % 2)
            continue;
        auto info = std::make_shared<EnumTypeInfo>();
        info->name = row[1];
        for (size_t i = 3; i + 1 < row.size(); i += 2)
            info->members.push_back({row[i], std::stod(row[i + 1]), {}, {}});
        auto enumeration = makeEnumTypes(arena, *builtins, info, "ludot", {});
        enums[info->name] = enumeration;
        const auto dot = row[1].find('.');
        if (dot == std::string::npos)
        {
            scope->exportedTypeBindings[row[1]] = TypeFun{{}, {}, enumeration.value};
            bind(row[1], enumeration.names);
        }
        else
        {
            std::string owner = row[1].substr(0, dot), name = row[1].substr(dot + 1);
            scope->importedTypeBindings[owner][name] = TypeFun{{}, {}, enumeration.value};
            auto symbol = frontend.globals.globalNames.names->getOrAdd(owner.c_str());
            auto found = scope->bindings.find(symbol);
            if (found == scope->bindings.end())
                bind(owner, arena.addType(TableType{{}, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed}));
            getMutable<TableType>(scope->bindings.at(symbol).typeId)->props[name] = Property::readonly(enumeration.names);
        }
    }
    for (const auto& row : rows)
    {
        if (row[0] != "E" || row.size() < 4 || (row.size() - 4) % 2 || !classes.count(row[1]))
            continue;
        auto info = std::make_shared<EnumTypeInfo>();
        info->name = row[1] + "." + row[2];
        for (size_t i = 4; i + 1 < row.size(); i += 2)
            info->members.push_back({row[i], std::stod(row[i + 1]), {}, {}});
        auto enumeration = makeEnumTypes(arena, *builtins, info, "ludot", {});
        enums[info->name] = enumeration;
        scope->importedTypeBindings[row[1]][row[2]] = TypeFun{{}, {}, enumeration.value};
        auto table = get<TableType>(get<MetatableType>(enumeration.names)->table);
        for (auto target : {getMutable<ExternType>(classes.at(row[1])), getMutable<ExternType>(instances.at(row[1]))})
        {
            if (!target->props.count("Enums"))
                target->props["Enums"] = Property::readonly(arena.addType(TableType{{}, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed}));
            getMutable<TableType>(*target->props.at("Enums").readTy)->props[row[2]] = Property::readonly(enumeration.names);
            for (const auto& member : info->members)
            {
                auto legacy = table->props.at(member.name);
                legacy.tags.push_back("LudotHiddenAlias");
                target->props[engineApiName(member.name, ApiCase::Pascal)] = legacy;
                target->props[member.name] = table->props.at(member.name);
            }
        }
    }
    auto type = [&](auto&& resolve, std::string name) -> TypeId {
        if (!name.empty() && name.back() == '?')
        {
            name.pop_back();
            return arena.addType(UnionType{{resolve(resolve, name), builtins->nilType}});
        }
        if (name == "int" || name == "float") return builtins->numberType;
        if (name == "bool") return builtins->booleanType;
        if (name == "String" || name == "StringName" || name == "NodePath") return builtins->stringType;
        if (name == "Nil") return builtins->nilType;
        if (name.rfind("enum::", 0) == 0)
        {
            auto found = enums.find(name.substr(6));
            return found == enums.end() ? builtins->numberType : found->second.value;
        }
        if (name.rfind("bitfield::", 0) == 0)
            return builtins->numberType;
        if (name.rfind("typedarray::", 0) == 0) name = "Array";
        auto it = instances.find(name);
        return it == instances.end() ? builtins->anyType : it->second;
    };
    for (const auto& row : rows)
    {
        if (row.size() < 3) continue;
        auto it = instances.find(row[1]);
        if (row[0] == "S")
        {
            if (instances.count(row[2])) bind(row[1], instances.at(row[2]));
            continue;
        }
        if (it == instances.end()) continue;
        auto instance = getMutable<ExternType>(it->second);
        auto cls = getMutable<ExternType>(classes.at(row[1]));
        if (row[0] == "C" && row.size() == 4)
        {
            if (instances.count(row[2])) instance->parent = instances.at(row[2]);
            if (classes.count(row[2])) cls->parent = classes.at(row[2]);
            if (row[3] == "1")
            {
                TableType::Props operations;
                instance->metatable = arena.addType(TableType{operations, std::nullopt, TypeLevel{}, scope.get(), TableState::Sealed});
            }
        }
        else if (row[0] == "O" && row.size() == 5 && instance->metatable)
        {
            auto operations = getMutable<TableType>(*instance->metatable);
            TypeId overload = arena.addType(FunctionType{arena.addTypePack({it->second, type(type, row[3])}), arena.addTypePack({type(type, row[4])})});
            auto found = operations->props.find(row[2]);
            if (found == operations->props.end()) operations->props[row[2]] = Property::readonly(overload);
            else
            {
                TypeId previous = *found->second.readTy;
                std::vector<TypeId> parts;
                if (auto intersection = get<IntersectionType>(previous)) parts = intersection->parts;
                else parts.push_back(previous);
                parts.push_back(overload);
                found->second = Property::readonly(arena.addType(IntersectionType{std::move(parts)}));
            }
        }
        else if (row[0] == "P" && row.size() == 4) instance->props[engineApiName(row[2], propertyCase)] = Property::rw(type(type, row[3]));
        else if (row[0] == "K" && row.size() == 4) cls->props[row[2]] = Property::readonly(type(type, row[3]));
        else if (row[0] == "F" && row.size() >= 6)
        {
            bool isStatic = row[4] == "1";
            std::vector<TypeId> args;
            if (!isStatic) args.push_back(it->second);
            for (size_t i = 6; i < row.size(); ++i) {
                TypeId argument = type(type, row[i]);
                if (row[i] == "Callable" || row[i] == "Callable?") argument = arena.addType(UnionType{{argument, builtins->functionType}});
                args.push_back(argument);
            }
            std::optional<TypePackId> tail;
            if (row[5] == "1") tail = arena.addTypePack(VariadicTypePack{builtins->anyType});
            TypePackId returns = row[3] == "void" ? builtins->emptyTypePack : arena.addTypePack({type(type, row[3])});
            TypeId fn = arena.addType(FunctionType{arena.addTypePack(args, tail), returns});
            getMutable<FunctionType>(fn)->hasSelf = !isStatic;
            (isStatic ? cls : instance)->props[engineApiName(row[2], methodCase)] = Property::readonly(fn);
        }
    }
    if (instances.count("Object"))
        addLudotIsA(arena, *builtins, instances.at("Object"), methodCase);
    return true;
}
