#include "luau_shader.h"
#include "core/object/class_db.h"
#include "core/io/file_access.h"
#include "ludot/shader.h"
#include "servers/rendering/shader_language.h"
#include "servers/rendering/shader_types.h"

Error LuauShader::compile_source(const String &p_source) {
    source = p_source;
    diagnostics = "";
    const CharString utf8 = source.utf8();
    const auto result = Ludot::compileShader(std::string(utf8.get_data(), utf8.length()));
    for (const auto &error : result.diagnostics) {
        diagnostics += vformat("%s:%d:%d: %s\n", get_path(), error.line + 1, error.column + 1, String::utf8(error.message.c_str()));
    }
    if (!result.diagnostics.empty()) {
        emit_changed();
        return ERR_PARSE_ERROR;
    }
    const String generated = String::utf8(result.code.c_str());
    const String type = ShaderLanguage::get_shader_type(generated);
    const RSE::ShaderMode mode = type == "canvas_item" ? RSE::SHADER_CANVAS_ITEM : type == "particles" ? RSE::SHADER_PARTICLES : RSE::SHADER_SPATIAL;
    ShaderLanguage validator;
    ShaderLanguage::ShaderCompileInfo info;
    info.functions = ShaderTypes::get_singleton()->get_functions(mode);
    info.render_modes = ShaderTypes::get_singleton()->get_modes(mode);
    info.stencil_modes = ShaderTypes::get_singleton()->get_stencil_modes(mode);
    info.shader_types = ShaderTypes::get_singleton()->get_types();
    const Error error = validator.compile(generated, info);
    if (error != OK) {
        const int generated_line = validator.get_error_line() - 1;
        const unsigned original_line = generated_line >= 0 && size_t(generated_line) < result.sourceLines.size() ? result.sourceLines[generated_line] : 0;
        diagnostics = vformat("%s:%d: %s", get_path(), original_line + 1, validator.get_error_text());
        emit_changed();
        return error;
    }
    // Keep the existing RID and material uniform values. Invalid edits never replace it.
    set_code(generated);
    return OK;
}
void LuauShader::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_source", "source"), &LuauShader::set_source);
    ClassDB::bind_method(D_METHOD("get_source"), &LuauShader::get_source);
    ClassDB::bind_method(D_METHOD("get_diagnostics"), &LuauShader::get_diagnostics);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "source", PROPERTY_HINT_MULTILINE_TEXT), "set_source", "get_source");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "diagnostics", PROPERTY_HINT_MULTILINE_TEXT, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_diagnostics");
}
void LuauShader::reload_from_file() {
    Error error;
    const Ref<FileAccess> file = FileAccess::open(get_path(), FileAccess::READ, &error);
    if (error == OK) compile_source(file->get_as_text());
}
Ref<Resource> ResourceFormatLoaderLuauShader::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
    Error error;
    const Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ, &error);
    if (file.is_null()) { if (r_error != nullptr) *r_error = error; return {}; }
    Ref<LuauShader> shader;
    shader.instantiate();
    error = shader->compile_source(file->get_as_text());
    if (r_error != nullptr) *r_error = error;
    if (error != OK) { ERR_PRINT(shader->get_diagnostics()); return {}; }
    if (r_progress != nullptr) *r_progress = 1.0;
    return shader;
}
Error ResourceFormatSaverLuauShader::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
    const Ref<LuauShader> shader = p_resource;
    Error error;
    const Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE, &error);
    if (file.is_null()) return error;
    file->store_string(shader->get_source());
    return file->get_error();
}
