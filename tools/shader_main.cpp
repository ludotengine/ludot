#include "ludot/shader.h"
#include <fstream>
#include <iostream>
#include <iterator>
int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "usage: ludot-shader file.lushader\n"; return 2; }
    std::ifstream file(argv[1]);
    if (!file) { std::cerr << "Cannot open shader\n"; return 2; }
    auto result = Ludot::compileShader(std::string(std::istreambuf_iterator<char>(file), {}));
    for (const auto& error : result.diagnostics) std::cerr << argv[1] << ':' << error.line + 1 << ':' << error.column + 1 << ": " << error.message << '\n';
    if (!result.diagnostics.empty()) return 1;
    std::cout << result.code;
}
