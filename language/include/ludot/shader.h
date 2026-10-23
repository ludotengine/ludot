#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace Ludot {
struct Diagnostic {
    unsigned line;
    unsigned column;
    std::string message;
    // Empty for the compiled file, otherwise the resolved path of the include that failed.
    std::string path;
};
struct ShaderResult {
    std::string code;
    std::vector<Diagnostic> diagnostics;
    // Source line of the compiled file for each generated line. Included code maps to its import.
    std::vector<unsigned> sourceLines;
    // Resolved paths of every included library, in inclusion order.
    std::vector<std::string> includes;
};
struct ShaderInclude {
    std::string path;
    std::string source;
};
// Resolves an import path written in p_from. Returns nothing and sets r_error when it cannot.
using ShaderIncludeResolver = std::function<std::optional<ShaderInclude>(const std::string& p_requested, const std::string& p_from, std::string& r_error)>;
// Produces Godot shader source. The engine shader compiler performs GPU type checking.
// A file with the header "shader include" compiles as a library and produces no shader_type line.
std::vector<std::string> shaderCompletions();
ShaderResult compileShader(const std::string& source, const std::string& path = "", const ShaderIncludeResolver& resolver = {});
}
