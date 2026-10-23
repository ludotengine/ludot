#include "ludot/shader.h"
#include "Luau/Config.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
namespace fs = std::filesystem;

std::optional<std::string> readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return std::nullopt;
    return std::string(std::istreambuf_iterator<char>(file), {});
}

// Finds an alias in the nearest .luaurc that defines it, searching upward from a directory.
std::optional<fs::path> resolveAlias(const std::string& alias, fs::path directory) {
    for (; !directory.empty(); directory = directory.parent_path()) {
        if (auto contents = readFile(directory / ".luaurc")) {
            Luau::Config config;
            Luau::ConfigOptions options;
            options.aliasOptions = Luau::ConfigOptions::AliasOptions{std::nullopt, true};
            if (!Luau::parseConfig(*contents, config, options) && config.aliases.find(alias) != nullptr)
                return directory / config.aliases.find(alias)->value;
        }
        if (directory == directory.parent_path()) break;
    }
    return std::nullopt;
}

std::optional<Ludot::ShaderInclude> resolve(const std::string& requested, const std::string& from, std::string& error) {
    const fs::path base = fs::absolute(fs::path(from)).parent_path();
    fs::path target;
    if (requested.rfind("./", 0) == 0 || requested.rfind("../", 0) == 0) {
        target = base / requested;
    } else if (requested.rfind("@", 0) == 0) {
        const size_t slash = requested.find('/');
        std::string alias = requested.substr(1, slash == std::string::npos ? std::string::npos : slash - 1);
        std::transform(alias.begin(), alias.end(), alias.begin(), [](unsigned char c) { return char(std::tolower(c)); });
        const auto root = resolveAlias(alias, base);
        if (!root) {
            error = "Unknown alias '@" + alias + "' in shader import";
            return std::nullopt;
        }
        target = slash == std::string::npos ? *root : *root / requested.substr(slash + 1);
    } else {
        error = "Shader imports use ./, ../, or @alias paths";
        return std::nullopt;
    }
    for (const fs::path& candidate : {target, fs::path(target.string() + ".lushader")}) {
        if (candidate.extension() != ".lushader" || !fs::is_regular_file(candidate)) continue;
        if (auto source = readFile(candidate)) return Ludot::ShaderInclude{fs::weakly_canonical(candidate).string(), *source};
    }
    error = "Cannot find shader include '" + requested + "'";
    return std::nullopt;
}
}

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--completion-list") {
        for (const auto& word : Ludot::shaderCompletions()) std::cout << word << '\n';
        return 0;
    }
    // --origin names the file that imports resolve against, for compiling an unsaved copy.
    std::string origin;
    int fileArgument = 1;
    if (argc == 4 && std::string(argv[1]) == "--origin") {
        origin = argv[2];
        fileArgument = 3;
    } else if (argc != 2) {
        std::cerr << "usage: ludot-shader [--origin path] file.lushader\n";
        return 2;
    }
    const std::string path = argv[fileArgument];
    if (origin.empty()) origin = path;
    const auto source = readFile(path);
    if (!source) { std::cerr << "Cannot open shader\n"; return 2; }
    auto result = Ludot::compileShader(*source, origin, resolve);
    for (const auto& error : result.diagnostics)
        std::cerr << (error.path.empty() ? path : error.path) << ':' << error.line + 1 << ':' << error.column + 1 << ": " << error.message << '\n';
    if (!result.diagnostics.empty()) return 1;
    std::cout << result.code;
}
