#pragma once
#include <string>
#include <vector>

namespace Ludot {
struct Diagnostic {
    unsigned line;
    unsigned column;
    std::string message;
};
struct ShaderResult {
    std::string code;
    std::vector<Diagnostic> diagnostics;
    std::vector<unsigned> sourceLines;
};
// Produces Godot shader source. The engine shader compiler performs GPU type checking.
std::vector<std::string> shaderCompletions();
ShaderResult compileShader(const std::string& source);
}
