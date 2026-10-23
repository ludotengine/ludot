#include "ludot/shader.h"
#include "Luau/Parser.h"
#include "Luau/Lexer.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace Ludot {
namespace {
struct Generator {
    ShaderResult result;
    std::string kind;
    std::set<std::string> uniforms;
    const std::set<std::string> types = {"bool", "int", "uint", "float", "number", "boolean", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4", "bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4", "sampler2D", "sampler2DArray", "sampler3D", "samplerCube", "samplerExternalOES"};
    const std::set<std::string> functions = {"mix", "clamp", "normalize", "texture", "textureLod", "textureSize", "texelFetch", "dot", "cross", "length", "distance", "reflect", "refract", "faceforward", "min", "max", "abs", "sign", "floor", "ceil", "round", "trunc", "fract", "mod", "step", "smoothstep", "sqrt", "inversesqrt", "pow", "exp", "exp2", "log", "log2", "sin", "cos", "tan", "asin", "acos", "atan", "radians", "degrees", "dFdx", "dFdy", "fwidth", "transpose", "determinant", "inverse", "outerProduct", "matrixCompMult", "lessThan", "greaterThan", "equal", "any", "all", "not"};
    const std::map<std::string, std::string> builtins = {{"Color","COLOR"},{"Texture","TEXTURE"},{"Vertex","VERTEX"},{"Normal","NORMAL"},{"Position","POSITION"},{"Time","TIME"},{"Albedo","ALBEDO"},{"Alpha","ALPHA"},{"Roughness","ROUGHNESS"},{"Metallic","METALLIC"},{"Emission","EMISSION"},{"Light","LIGHT"},{"LightColor","LIGHT_COLOR"},{"LightEnergy","LIGHT_ENERGY"},{"PointSize","POINT_SIZE"},{"UV","UV"},{"UV2","UV2"},{"ScreenUV","SCREEN_UV"},{"TexturePixelSize","TEXTURE_PIXEL_SIZE"},{"ModelMatrix","MODEL_MATRIX"},{"ViewMatrix","VIEW_MATRIX"},{"ProjectionMatrix","PROJECTION_MATRIX"},{"Velocity","VELOCITY"},{"Transform","TRANSFORM"},{"Active","ACTIVE"},{"Custom","CUSTOM"},{"Lifetime","LIFETIME"},{"Delta","DELTA"},{"Index","INDEX"},{"Number","NUMBER"},{"RandomSeed","RANDOM_SEED"}};
    [[noreturn]] void fail(const Luau::AstNode* node, const std::string& message) {
        result.diagnostics.push_back({node->location.begin.line, node->location.begin.column, message});
        throw std::runtime_error(message);
    }
    std::string type(Luau::AstType* annotation, const Luau::AstNode* node) {
        auto ref = annotation ? annotation->as<Luau::AstTypeReference>() : nullptr;
        if (!ref || ref->prefix || ref->parameters.size != 0 || types.count(ref->name.value) == 0)
            fail(node, "A shader declaration requires a scalar, vector, matrix, or sampler type");
        std::string name = ref->name.value;
        if (name == "number") return "float";
        if (name == "boolean") return "bool";
        return name;
    }
    std::string expression(Luau::AstExpr* expr) {
        using namespace Luau;
        if (auto n = expr->as<AstExprConstantNumber>()) {
            std::ostringstream value; value.precision(17); value << n->value;
            auto text = value.str();
            if (text.find_first_of(".eE") == std::string::npos) text += ".0";
            return text;
        }
        if (auto n = expr->as<AstExprConstantBool>()) return n->value ? "true" : "false";
        if (auto n = expr->as<AstExprLocal>()) return n->local->name.value;
        if (auto n = expr->as<AstExprGlobal>()) {
            auto it = builtins.find(n->name.value);
            return it == builtins.end() ? n->name.value : it->second;
        }
        if (auto n = expr->as<AstExprGroup>()) return "(" + expression(n->expr) + ")";
        if (auto n = expr->as<AstExprIndexName>()) return expression(n->expr) + "." + n->index.value;
        if (auto n = expr->as<AstExprIndexExpr>()) return expression(n->expr) + "[int(" + expression(n->index) + ")]";
        if (auto n = expr->as<AstExprCall>()) {
            auto name = n->func->as<AstExprGlobal>();
            if (!name || n->self || (functions.count(name->name.value) == 0 && types.count(name->name.value) == 0))
                fail(expr, "Only shader standard functions and type constructors may be called");
            std::string call = name->name.value;
            if (call == "number") call = "float";
            if (call == "boolean") call = "bool";
            call += "(";
            for (size_t i = 0; i < n->args.size; ++i) { if (i) call += ", "; call += expression(n->args.data[i]); }
            return call + ")";
        }
        if (auto n = expr->as<AstExprBinary>()) {
            const char* op = nullptr;
            switch (n->op) {
                case AstExprBinary::Add: op = "+"; break; case AstExprBinary::Sub: op = "-"; break;
                case AstExprBinary::Mul: op = "*"; break; case AstExprBinary::Div: op = "/"; break;
                case AstExprBinary::And: op = "&&"; break; case AstExprBinary::Or: op = "||"; break;
                case AstExprBinary::CompareEq: op = "=="; break; case AstExprBinary::CompareNe: op = "!="; break;
                case AstExprBinary::CompareLt: op = "<"; break; case AstExprBinary::CompareLe: op = "<="; break;
                case AstExprBinary::CompareGt: op = ">"; break; case AstExprBinary::CompareGe: op = ">="; break;
                default: fail(expr, "Unsupported shader operator; use a shader standard function");
            }
            return "(" + expression(n->left) + " " + op + " " + expression(n->right) + ")";
        }
        if (auto n = expr->as<AstExprUnary>()) {
            if (n->op == AstExprUnary::Op::Minus) return "(-" + expression(n->expr) + ")";
            if (n->op == AstExprUnary::Op::Not) return "(!" + expression(n->expr) + ")";
        }
        fail(expr, "This Luau expression is not supported in shaders");
    }
    std::string block(Luau::AstStatBlock* body) {
        using namespace Luau;
        std::string code;
        for (auto stat : body->body) {
            code += "// ludot:" + std::to_string(stat->location.begin.line) + "\n";
            if (auto n = stat->as<AstStatAssign>()) {
                if (n->vars.size != n->values.size) fail(stat, "Shader assignments require one value per target");
                for (size_t i = 0; i < n->vars.size; ++i) code += expression(n->vars.data[i]) + " = " + expression(n->values.data[i]) + ";\n";
            } else if (auto n = stat->as<AstStatLocal>()) {
                if (uniforms.count(std::to_string(stat->location.begin.line) + ":" + std::to_string(stat->location.begin.column))) fail(stat, "Uniforms must be exported at top level");
                if (n->vars.size != n->values.size) fail(stat, "Shader locals need typed initializers");
                for (size_t i = 0; i < n->vars.size; ++i) code += type(n->vars.data[i]->annotation, stat) + " " + n->vars.data[i]->name.value + " = " + expression(n->values.data[i]) + ";\n";
            } else if (auto n = stat->as<AstStatIf>()) {
                code += "if (" + expression(n->condition) + ") {\n" + block(n->thenbody) + "}\n";
                if (n->elsebody) {
                    if (auto b = n->elsebody->as<AstStatBlock>()) code += "else {\n" + block(b) + "}\n";
                    else fail(stat, "Use a nested if block instead of elseif in shaders");
                }
            } else if (auto n = stat->as<AstStatReturn>()) {
                if (n->list.size != 0) fail(stat, "Shader entry points cannot return values");
                code += "return;\n";
            } else fail(stat, "This Luau statement is not supported in shaders");
        }
        return code;
    }
};
}
ShaderResult compileShader(const std::string& source) {
    using namespace Luau;
    Generator gen;
    Allocator allocator;
    AstNameTable names(allocator);
    Lexer lexer(source.data(), source.size(), names);
    lexer.next();
    if (lexer.current().type != Lexeme::Name || std::string(lexer.current().name) != "shader")
        return {"", {{0, 0, "Expected 'shader canvas_item', 'shader spatial', or 'shader particles'"}}};
    lexer.next();
    if (lexer.current().type != Lexeme::Name)
        return {"", {{0, 0, "Expected shader type"}}};
    gen.kind = lexer.current().name;
    if (gen.kind != "canvas_item" && gen.kind != "spatial" && gen.kind != "particles")
        return {"", {{0, 0, "Unsupported shader type '" + gen.kind + "'"}}};
    auto headerEnd = lexer.current().location.end;
    std::vector<size_t> lines{0};
    for (size_t i = 0; i < source.size(); ++i) if (source[i] == '\n') lines.push_back(i + 1);
    std::string lowered = source;
    for (size_t i = 0; i < lines[headerEnd.line] + headerEnd.column; ++i)
        if (lowered[i] != '\n' && lowered[i] != '\r') lowered[i] = ' ';
    // Replace only lexical export tokens, leaving comments and strings untouched.
    lexer.next();
    while (lexer.current().type != Lexeme::Eof) {
        if (lexer.current().type == Lexeme::Name && std::string(lexer.current().name) == "export") {
            size_t offset = lines[lexer.current().location.begin.line] + lexer.current().location.begin.column;
            lowered.replace(offset, 6, "local ");
            gen.uniforms.insert(std::to_string(lexer.current().location.begin.line) + ":" + std::to_string(lexer.current().location.begin.column));
        }
        lexer.next();
    }
    auto parsed = Parser::parse(lowered.data(), lowered.size(), names, allocator);
    for (const auto& error : parsed.errors) gen.result.diagnostics.push_back({error.getLocation().begin.line, error.getLocation().begin.column, error.what()});
    if (!parsed.errors.empty()) return gen.result;
    try {
        gen.result.code = "shader_type " + gen.kind + ";\n";
        std::set<std::string> entries;
        for (auto stat : parsed.root->body) {
            gen.result.code += "// ludot:" + std::to_string(stat->location.begin.line) + "\n";
            if (auto decl = stat->as<AstStatLocal>()) {
                auto key = std::to_string(stat->location.begin.line) + ":" + std::to_string(stat->location.begin.column);
                if (gen.uniforms.count(key) == 0 || decl->vars.size != 1 || decl->values.size > 1) gen.fail(stat, "Top-level shader variables must use 'export Name: type'");
                const std::string uniformType = gen.type(decl->vars.data[0]->annotation, stat);
                gen.result.code += "uniform " + uniformType + " " + decl->vars.data[0]->name.value;
                if (decl->values.size) {
                    auto number = decl->values.data[0]->as<AstExprConstantNumber>();
                    if (number && (uniformType == "int" || uniformType == "uint")) {
                        if (std::floor(number->value) != number->value || number->value < (uniformType == "uint" ? 0.0 : -2147483648.0) || number->value > (uniformType == "uint" ? 4294967295.0 : 2147483647.0)) gen.fail(stat, "Integer uniform initializer is out of range");
                        std::ostringstream literal;
                        literal << std::fixed << std::setprecision(0) << number->value;
                        gen.result.code += " = " + literal.str() + (uniformType == "uint" ? "u" : "");
                    } else gen.result.code += " = " + gen.expression(decl->values.data[0]);
                }
                gen.result.code += ";\n";
            } else if (auto fn = stat->as<AstStatFunction>()) {
                auto name = fn->name->as<AstExprGlobal>();
                if (!name) gen.fail(stat, "Shader entry points need simple names");
                std::string entry = name->name.value;
                bool allowed = gen.kind == "particles" ? (entry == "start" || entry == "process") : (entry == "vertex" || entry == "fragment" || entry == "light");
                if (!allowed) gen.fail(stat, "Entry point '" + entry + "' is not supported for " + gen.kind);
                if (!entries.insert(entry).second) gen.fail(stat, "Duplicate shader entry point '" + entry + "'");
                if (fn->func->attributes.size || fn->func->args.size || fn->func->vararg || fn->func->generics.size || fn->func->returnAnnotation) gen.fail(stat, "Shader entry points take no arguments or return annotations");
                gen.result.code += "void " + entry + "() {\n" + gen.block(fn->func->body) + "}\n";
            } else gen.fail(stat, "Only exported uniforms and shader entry points are allowed at top level");
        }
    } catch (const std::runtime_error&) { gen.result.code.clear(); }
    std::istringstream generated(gen.result.code);
    std::string line;
    unsigned sourceLine = 0;
    while (std::getline(generated, line)) {
        if (line.rfind("// ludot:", 0) == 0) sourceLine = unsigned(std::stoul(line.substr(9)));
        gen.result.sourceLines.push_back(sourceLine);
    }
    return gen.result;
}
std::vector<std::string> shaderCompletions() {
    Generator gen;
    std::set<std::string> words = gen.types;
    words.insert(gen.functions.begin(), gen.functions.end());
    for (const auto& item : gen.builtins) words.insert(item.first);
    for (const char* word : {"shader", "canvas_item", "spatial", "particles", "export", "function", "vertex", "fragment", "light", "start", "process", "local", "const", "if", "then", "else", "end", "return"}) words.insert(word);
    return {words.begin(), words.end()};
}
}
