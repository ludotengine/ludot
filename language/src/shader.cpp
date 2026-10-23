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
struct Helper {
    std::string returnType;
    std::vector<std::string> parameters;
};
struct Generator {
    ShaderResult result;
    std::string kind;
    std::set<std::string> uniforms;
    std::map<std::string, std::string> uniformHints;
    std::map<const Luau::AstLocal*, std::string> localTypes;
    std::map<std::string, Helper> helpers;
    std::string returnType = "void";
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
    static std::string scalar(const std::string& type) {
        if (type == "int" || type.rfind("ivec", 0) == 0) return "int";
        if (type == "uint" || type.rfind("uvec", 0) == 0) return "uint";
        return "";
    }
    static const Luau::AstExprConstantNumber* number(Luau::AstExpr* expr) {
        if (auto group = expr->as<Luau::AstExprGroup>()) return number(group->expr);
        return expr->as<Luau::AstExprConstantNumber>();
    }
    // Integral literals adopt int or uint when the surrounding expression has that scalar type.
    std::string literal(const Luau::AstNode* node, double value, const std::string& expected) {
        const std::string kind = scalar(expected);
        if (!kind.empty() && std::floor(value) == value) {
            if (kind == "uint" && value < 0) fail(node, "A negative literal cannot be an unsigned shader integer");
            std::ostringstream text; text << std::fixed << std::setprecision(0) << value;
            return text.str() + (kind == "uint" ? "u" : "");
        }
        std::ostringstream text; text.precision(17); text << value;
        auto out = text.str();
        if (out.find_first_of(".eE") == std::string::npos) out += ".0";
        return out;
    }
    // Helpers may be global functions or local functions; everything else must be a global name.
    std::string callee(Luau::AstExprCall* call) {
        if (auto global = call->func->as<Luau::AstExprGlobal>()) return global->name.value;
        if (auto local = call->func->as<Luau::AstExprLocal>(); local && helpers.count(local->local->name.value)) return local->local->name.value;
        return "";
    }
    // Returns the statically known result type, or an empty string when the engine must infer it.
    std::string typeOf(Luau::AstExpr* expr) {
        using namespace Luau;
        if (expr->is<AstExprConstantNumber>()) return "float";
        if (expr->is<AstExprConstantBool>()) return "bool";
        if (auto n = expr->as<AstExprLocal>()) { auto it = localTypes.find(n->local); return it == localTypes.end() ? "" : it->second; }
        if (auto n = expr->as<AstExprGroup>()) return typeOf(n->expr);
        if (auto n = expr->as<AstExprUnary>()) return n->op == AstExprUnary::Op::Not ? "bool" : typeOf(n->expr);
        if (auto n = expr->as<AstExprIfElse>()) { auto t = typeOf(n->trueExpr); return t.empty() || number(n->trueExpr) ? typeOf(n->falseExpr) : t; }
        if (auto n = expr->as<AstExprBinary>()) {
            if (n->op >= AstExprBinary::CompareNe) return "bool";
            if (n->op == AstExprBinary::Mul && typeOf(n->left).rfind("mat", 0) == 0) return typeOf(n->right);
            auto t = number(n->left) ? typeOf(n->right) : typeOf(n->left);
            return t.empty() ? typeOf(n->right) : t;
        }
        if (auto n = expr->as<AstExprCall>()) {
            std::string call = callee(n);
            if (call == "number") return "float";
            if (call == "boolean") return "bool";
            if (types.count(call)) return call;
            if (auto helper = helpers.find(call); helper != helpers.end()) return helper->second.returnType;
        }
        return "";
    }
    std::string expression(Luau::AstExpr* expr, const std::string& expected = "") {
        using namespace Luau;
        if (auto n = expr->as<AstExprConstantNumber>()) return literal(expr, n->value, expected);
        if (auto n = expr->as<AstExprConstantBool>()) return n->value ? "true" : "false";
        if (auto n = expr->as<AstExprLocal>()) return n->local->name.value;
        if (auto n = expr->as<AstExprGlobal>()) {
            auto it = builtins.find(n->name.value);
            return it == builtins.end() ? n->name.value : it->second;
        }
        if (auto n = expr->as<AstExprGroup>()) return "(" + expression(n->expr, expected) + ")";
        if (auto n = expr->as<AstExprIndexName>()) return expression(n->expr) + "." + n->index.value;
        if (auto n = expr->as<AstExprIndexExpr>()) return expression(n->expr) + "[int(" + expression(n->index, "int") + ")]";
        if (auto n = expr->as<AstExprIfElse>()) {
            if (n->conditionLocal) fail(expr, "Shader conditional expressions cannot bind locals");
            auto context = typeOf(expr);
            if (context.empty()) context = expected;
            return "(" + expression(n->condition) + " ? " + expression(n->trueExpr, context) + " : " + expression(n->falseExpr, context) + ")";
        }
        if (auto n = expr->as<AstExprCall>()) {
            std::string call = callee(n);
            if (call.empty() || n->self) fail(expr, "Only shader standard functions, helpers, and type constructors may be called");
            auto helper = helpers.find(call);
            if (helper == helpers.end() && functions.count(call) == 0 && types.count(call) == 0)
                fail(expr, call == "discard" ? "discard() is a statement" : "Unknown shader function '" + call + "'; helpers must be declared before use and cannot recurse");
            if (helper != helpers.end() && helper->second.parameters.size() != n->args.size)
                fail(expr, "Shader helper '" + call + "' expects " + std::to_string(helper->second.parameters.size()) + " arguments");
            // Standard functions are generic over their first argument, so literals follow its type.
            std::string context;
            if (helper == helpers.end() && types.count(call) == 0)
                for (size_t i = 0; i < n->args.size && context.empty(); ++i) if (!number(n->args.data[i])) context = typeOf(n->args.data[i]);
            if (call == "number") call = "float";
            if (call == "boolean") call = "bool";
            call += "(";
            for (size_t i = 0; i < n->args.size; ++i) {
                if (i) call += ", ";
                call += expression(n->args.data[i], helper != helpers.end() ? helper->second.parameters[i] : context);
            }
            return call + ")";
        }
        if (auto n = expr->as<AstExprBinary>()) {
            std::string context = number(n->left) ? typeOf(n->right) : typeOf(n->left);
            if (context.empty()) context = number(n->right) ? typeOf(n->left) : typeOf(n->right);
            if (context.empty() && n->op < AstExprBinary::CompareNe) context = expected;
            if (n->op == AstExprBinary::And || n->op == AstExprBinary::Or) context.clear();
            const std::string left = expression(n->left, context), right = expression(n->right, context);
            const char* op = nullptr;
            switch (n->op) {
                case AstExprBinary::Add: op = "+"; break; case AstExprBinary::Sub: op = "-"; break;
                case AstExprBinary::Mul: op = "*"; break; case AstExprBinary::Div: op = "/"; break;
                case AstExprBinary::Mod: if (scalar(context).empty()) return "mod(" + left + ", " + right + ")"; op = "%"; break;
                case AstExprBinary::Pow: return "pow(" + left + ", " + right + ")";
                case AstExprBinary::FloorDiv: if (scalar(context).empty()) return "floor(" + left + " / " + right + ")"; op = "/"; break;
                case AstExprBinary::And: op = "&&"; break; case AstExprBinary::Or: op = "||"; break;
                case AstExprBinary::CompareEq: op = "=="; break; case AstExprBinary::CompareNe: op = "!="; break;
                case AstExprBinary::CompareLt: op = "<"; break; case AstExprBinary::CompareLe: op = "<="; break;
                case AstExprBinary::CompareGt: op = ">"; break; case AstExprBinary::CompareGe: op = ">="; break;
                default: fail(expr, "Unsupported shader operator; use a shader standard function");
            }
            return "(" + left + " " + op + " " + right + ")";
        }
        if (auto n = expr->as<AstExprUnary>()) {
            if (n->op == AstExprUnary::Op::Minus) return "(-" + expression(n->expr, expected) + ")";
            if (n->op == AstExprUnary::Op::Not) return "(!" + expression(n->expr) + ")";
        }
        fail(expr, "This Luau expression is not supported in shaders");
    }
    std::string ifChain(Luau::AstStatIf* n) {
        using namespace Luau;
        std::string code = "if (" + expression(n->condition) + ") {\n" + block(n->thenbody) + "}\n";
        if (auto elseif = n->elsebody ? n->elsebody->as<AstStatIf>() : nullptr) code += "else " + ifChain(elseif);
        else if (auto b = n->elsebody ? n->elsebody->as<AstStatBlock>() : nullptr) code += "else {\n" + block(b) + "}\n";
        return code;
    }
    std::string block(Luau::AstStatBlock* body) {
        using namespace Luau;
        std::string code;
        for (auto stat : body->body) {
            code += "// ludot:" + std::to_string(stat->location.begin.line) + "\n";
            if (auto n = stat->as<AstStatAssign>()) {
                if (n->vars.size != n->values.size) fail(stat, "Shader assignments require one value per target");
                for (size_t i = 0; i < n->vars.size; ++i) code += expression(n->vars.data[i]) + " = " + expression(n->values.data[i], typeOf(n->vars.data[i])) + ";\n";
            } else if (auto n = stat->as<AstStatCompoundAssign>()) {
                const std::string target = expression(n->var), context = typeOf(n->var), value = expression(n->value, context);
                switch (n->op) {
                    case AstExprBinary::Add: code += target + " += " + value + ";\n"; break;
                    case AstExprBinary::Sub: code += target + " -= " + value + ";\n"; break;
                    case AstExprBinary::Mul: code += target + " *= " + value + ";\n"; break;
                    case AstExprBinary::Div: code += target + " /= " + value + ";\n"; break;
                    case AstExprBinary::Mod: code += scalar(context).empty() ? target + " = mod(" + target + ", " + value + ");\n" : target + " %= " + value + ";\n"; break;
                    case AstExprBinary::Pow: code += target + " = pow(" + target + ", " + value + ");\n"; break;
                    default: fail(stat, "Unsupported shader compound assignment");
                }
            } else if (auto n = stat->as<AstStatLocal>()) {
                if (uniforms.count(std::to_string(stat->location.begin.line) + ":" + std::to_string(stat->location.begin.column))) fail(stat, "Uniforms must be exported at top level");
                if (n->values.size > n->vars.size) fail(stat, "A shader local has more values than names");
                for (size_t i = 0; i < n->vars.size; ++i) {
                    const std::string localType = type(n->vars.data[i]->annotation, stat);
                    localTypes[n->vars.data[i]] = localType;
                    code += (n->isConst ? "const " : "") + localType + " " + n->vars.data[i]->name.value;
                    if (i < n->values.size) code += " = " + expression(n->values.data[i], localType);
                    else if (n->isConst) fail(stat, "Shader constants need initializers");
                    code += ";\n";
                }
            } else if (auto n = stat->as<AstStatIf>()) {
                code += ifChain(n);
            } else if (auto n = stat->as<AstStatWhile>()) {
                code += "while (" + expression(n->condition) + ") {\n" + block(n->body) + "}\n";
            } else if (auto n = stat->as<AstStatFor>()) {
                double step = 1;
                if (n->step) {
                    auto unary = n->step->as<AstExprUnary>();
                    auto constant = number(unary && unary->op == AstExprUnary::Op::Minus ? unary->expr : n->step);
                    if (!constant) fail(n->step, "Shader loop steps must be numeric literals");
                    step = unary ? -constant->value : constant->value;
                    if (step == 0) fail(n->step, "A shader loop step cannot be zero");
                }
                const bool integral = number(n->from) && std::floor(number(n->from)->value) == number(n->from)->value && std::floor(step) == step;
                const std::string loopType = n->var->annotation ? type(n->var->annotation, stat) : (integral ? "int" : "float");
                if (loopType != "int" && loopType != "uint" && loopType != "float") fail(stat, "Shader loop variables must be int, uint, or number");
                localTypes[n->var] = loopType;
                const std::string name = n->var->name.value;
                code += "for (" + loopType + " " + name + " = " + expression(n->from, loopType) + "; " + name + (step < 0 ? " >= " : " <= ") + expression(n->to, loopType) + "; " + name + " += " + (n->step ? expression(n->step, loopType) : literal(stat, 1, loopType)) + ") {\n" + block(n->body) + "}\n";
            } else if (stat->is<AstStatBreak>()) {
                code += "break;\n";
            } else if (stat->is<AstStatContinue>()) {
                code += "continue;\n";
            } else if (auto n = stat->as<AstStatExpr>()) {
                auto call = n->expr->as<AstExprCall>();
                const std::string name = call ? callee(call) : "";
                if (name == "discard" && call->args.size == 0) {
                    if (kind == "particles") fail(stat, "discard() is only available in canvas_item and spatial shaders");
                    code += "discard;\n";
                } else if (helpers.count(name)) code += expression(n->expr) + ";\n";
                else fail(stat, "Only discard() and shader helper calls may be used as statements");
            } else if (auto n = stat->as<AstStatReturn>()) {
                if (returnType == "void") {
                    if (n->list.size != 0) fail(stat, "This shader function cannot return a value");
                    code += "return;\n";
                } else {
                    if (n->list.size != 1) fail(stat, "Shader helpers return exactly one value");
                    code += "return " + expression(n->list.data[0], returnType) + ";\n";
                }
            } else fail(stat, "This Luau statement is not supported in shaders");
        }
        return code;
    }
    void function(const Luau::AstStat* stat, const std::string& name, Luau::AstExprFunction* fn, std::set<std::string>& entries) {
        using namespace Luau;
        if (fn->attributes.size || fn->vararg || fn->generics.size || fn->genericPacks.size) fail(stat, "Shader functions cannot use attributes, varargs, or generics");
        const bool entry = kind == "particles" ? (name == "start" || name == "process") : (name == "vertex" || name == "fragment" || name == "light");
        const bool reserved = name == "vertex" || name == "fragment" || name == "light" || name == "start" || name == "process";
        if (reserved && !entry) fail(stat, "Entry point '" + name + "' is not supported for " + kind);
        if (functions.count(name) || types.count(name) || builtins.count(name) || helpers.count(name)) fail(stat, "Shader function '" + name + "' is already defined");
        if (entry) {
            if (!entries.insert(name).second) fail(stat, "Duplicate shader entry point '" + name + "'");
            if (fn->args.size || fn->returnAnnotation) fail(stat, "Shader entry points take no arguments or return annotations");
            returnType = "void";
            result.code += "void " + name + "() {\n" + block(fn->body) + "}\n";
            return;
        }
        Helper helper{"void", {}};
        if (fn->returnAnnotation) {
            auto pack = fn->returnAnnotation->as<AstTypePackExplicit>();
            if (!pack || pack->typeList.tailType || pack->typeList.types.size > 1) fail(stat, "Shader helpers return at most one typed value");
            if (pack->typeList.types.size == 1) helper.returnType = type(pack->typeList.types.data[0], stat);
        }
        std::string signature = helper.returnType + " " + name + "(";
        for (size_t i = 0; i < fn->args.size; ++i) {
            const std::string parameter = type(fn->args.data[i]->annotation, stat);
            localTypes[fn->args.data[i]] = parameter;
            helper.parameters.push_back(parameter);
            signature += (i ? ", " : "") + parameter + " " + fn->args.data[i]->name.value;
        }
        returnType = helper.returnType;
        const std::string body = block(fn->body);
        if (returnType != "void" && (fn->body->body.size == 0 || !fn->body->body.data[fn->body->body.size - 1]->is<AstStatReturn>()))
            fail(stat, "Shader helper '" + name + "' must end with a return statement");
        // Registered after the body so helpers cannot recurse, which shaders forbid.
        helpers[name] = helper;
        result.code += signature + ") {\n" + body + "}\n";
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
    auto offset = [&](const Position& position) { return lines[position.line] + position.column; };
    auto blank = [&](size_t from, size_t to) { for (size_t i = from; i < to; ++i) if (lowered[i] != '\n' && lowered[i] != '\r') lowered[i] = ' '; };
    blank(0, offset(headerEnd));
    // Replace only lexical export tokens, leaving comments and strings untouched. Editor range
    // hints after an export ("in 0 to 1 step 0.1") become hint_range and are removed from the Luau source.
    lexer.next();
    std::string exportKey;
    unsigned exportLine = 0;
    while (lexer.current().type != Lexeme::Eof) {
        const Lexeme token = lexer.current();
        if (token.type == Lexeme::Name && std::string(token.name) == "export") {
            lowered.replace(offset(token.location.begin), 6, "local ");
            exportKey = std::to_string(token.location.begin.line) + ":" + std::to_string(token.location.begin.column);
            exportLine = token.location.begin.line;
            gen.uniforms.insert(exportKey);
        } else if (token.type == Lexeme::ReservedIn && !exportKey.empty() && token.location.begin.line == exportLine) {
            std::vector<std::string> values;
            size_t end = offset(token.location.end);
            auto readNumber = [&]() {
                lexer.next();
                std::string text;
                if (lexer.current().type == '-') { text = "-"; lexer.next(); }
                if (lexer.current().type != Lexeme::Number) return false;
                values.push_back(text + std::string(lexer.current().data, lexer.current().getLength()));
                end = offset(lexer.current().location.end);
                lexer.next();
                return true;
            };
            auto word = [&](const char* expected) { return lexer.current().type == Lexeme::Name && std::string(lexer.current().name) == expected; };
            bool valid = readNumber() && word("to") && readNumber();
            if (valid && word("step")) valid = readNumber();
            if (!valid)
                return {"", {{token.location.begin.line, token.location.begin.column, "Expected 'in <min> to <max>' with an optional 'step <value>' using numeric literals"}}};
            blank(offset(token.location.begin), end);
            gen.uniformHints[exportKey] = " : hint_range(" + values[0] + ", " + values[1] + (values.size() > 2 ? ", " + values[2] : "") + ")";
            exportKey.clear();
            continue;
        }
        lexer.next();
    }
    auto parsed = Parser::parse(lowered.data(), lowered.size(), names, allocator);
    for (const auto& error : parsed.errors) gen.result.diagnostics.push_back({error.getLocation().begin.line, error.getLocation().begin.column, error.what()});
    if (!parsed.errors.empty()) return gen.result;
    try {
        std::string header = "shader_type " + gen.kind + ";\n";
        std::vector<std::string> renderModes;
        std::set<std::string> entries;
        for (auto stat : parsed.root->body) {
            gen.result.code += "// ludot:" + std::to_string(stat->location.begin.line) + "\n";
            if (auto decl = stat->as<AstStatLocal>()) {
                auto key = std::to_string(stat->location.begin.line) + ":" + std::to_string(stat->location.begin.column);
                if (gen.uniforms.count(key) == 0) {
                    // Top-level locals with values are constants; without values they are varyings.
                    for (size_t i = 0; i < decl->vars.size; ++i) {
                        const std::string declared = gen.type(decl->vars.data[i]->annotation, stat);
                        gen.localTypes[decl->vars.data[i]] = declared;
                        if (decl->values.size == 0) {
                            if (gen.kind == "particles") gen.fail(stat, "Varyings are only available in canvas_item and spatial shaders");
                            gen.result.code += "varying " + declared + " " + decl->vars.data[i]->name.value + ";\n";
                        } else if (i < decl->values.size) {
                            gen.result.code += "const " + declared + " " + decl->vars.data[i]->name.value + " = " + gen.expression(decl->values.data[i], declared) + ";\n";
                        } else gen.fail(stat, "Each shader constant needs a value");
                    }
                    continue;
                }
                if (decl->vars.size != 1 || decl->values.size > 1) gen.fail(stat, "Export one uniform per declaration");
                const std::string uniformType = gen.type(decl->vars.data[0]->annotation, stat);
                gen.localTypes[decl->vars.data[0]] = uniformType;
                gen.result.code += "uniform " + uniformType + " " + decl->vars.data[0]->name.value;
                if (auto hint = gen.uniformHints.find(key); hint != gen.uniformHints.end()) {
                    if (uniformType != "float" && uniformType != "int") gen.fail(stat, "Range hints are only available on number and int uniforms");
                    gen.result.code += hint->second;
                }
                if (decl->values.size) {
                    auto number = decl->values.data[0]->as<AstExprConstantNumber>();
                    if (number && (uniformType == "int" || uniformType == "uint") && (std::floor(number->value) != number->value || number->value < (uniformType == "uint" ? 0.0 : -2147483648.0) || number->value > (uniformType == "uint" ? 4294967295.0 : 2147483647.0)))
                        gen.fail(stat, "Integer uniform initializer is out of range");
                    gen.result.code += " = " + gen.expression(decl->values.data[0], uniformType);
                }
                gen.result.code += ";\n";
            } else if (auto fn = stat->as<AstStatFunction>()) {
                auto name = fn->name->as<AstExprGlobal>();
                if (!name) gen.fail(stat, "Shader functions need simple names");
                gen.function(stat, name->name.value, fn->func, entries);
            } else if (auto fn = stat->as<AstStatLocalFunction>()) {
                gen.function(stat, fn->name->name.value, fn->func, entries);
            } else if (auto expr = stat->as<AstStatExpr>(); expr && expr->expr->is<AstExprCall>() && expr->expr->as<AstExprCall>()->func->is<AstExprGlobal>() && std::string(expr->expr->as<AstExprCall>()->func->as<AstExprGlobal>()->name.value) == "renderMode") {
                auto call = expr->expr->as<AstExprCall>();
                if (call->args.size == 0) gen.fail(stat, "renderMode() needs at least one mode");
                for (auto arg : call->args) {
                    auto mode = arg->as<AstExprConstantString>();
                    std::string text = mode ? std::string(mode->value.data, mode->value.size) : "";
                    if (text.empty() || text.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789_") != std::string::npos) gen.fail(arg, "Render modes are lowercase string literals such as \"unshaded\"");
                    renderModes.push_back(text);
                }
            } else gen.fail(stat, "Only uniforms, varyings, constants, functions, and renderMode() are allowed at top level");
        }
        if (!renderModes.empty()) {
            header += "render_mode ";
            for (size_t i = 0; i < renderModes.size(); ++i) header += (i ? ", " : "") + renderModes[i];
            header += ";\n";
        }
        gen.result.code = header + gen.result.code;
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
    for (const char* word : {"shader", "canvas_item", "spatial", "particles", "export", "function", "vertex", "fragment", "light", "start", "process", "local", "const", "if", "then", "else", "elseif", "end", "return", "for", "while", "do", "break", "continue", "in", "to", "step", "discard", "renderMode"}) words.insert(word);
    return {words.begin(), words.end()};
}
}
