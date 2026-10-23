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
};
// Produces Godot shader source. The engine shader compiler performs GPU type checking.
ShaderResult compileShader(const std::string& source);
}
