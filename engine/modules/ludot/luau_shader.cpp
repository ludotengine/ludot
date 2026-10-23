#include "luau_shader.h"
#include "core/object/class_db.h"
#include "core/io/file_access.h"
#include "ludot/shader.h"
#include "luau_script.h"
#include "core/io/resource_loader.h"
#include "servers/rendering/shader_language.h"
#include "servers/rendering/shader_types.h"

HashSet<LuauShader *> LuauShader::live_shaders;

Error LuauShader::compile_source(const String &p_source) {
    source = p_source;
    diagnostics = "";
    const String path = get_path().is_empty() ? load_path : get_path();
    // Imports use the same aliases as Luau modules. A loaded library supplies its current source,
    // including unsaved edits made in the editor.
    const Ludot::ShaderIncludeResolver resolver = [&](const std::string &p_requested, const std::string &p_from, std::string &r_error) -> std::optional<Ludot::ShaderInclude> {
        const String resolved = LuauLanguage::get_singleton()->resolve_module(String::utf8(p_requested.c_str()), String::utf8(p_from.c_str()), "lushader");
        if (resolved.is_empty()) {
            r_error = "Cannot find shader include '" + p_requested + "'";
            return std::nullopt;
        }
        String text;
        const Ref<LuauShader> cached = ResourceCache::get_ref(resolved);
        if (cached.is_valid()) text = cached->get_source();
        else {
            Error error;
            const Ref<FileAccess> file = FileAccess::open(resolved, FileAccess::READ, &error);
            if (file.is_null()) {
                r_error = "Cannot open shader include '" + std::string(resolved.utf8().get_data()) + "'";
                return std::nullopt;
            }
            text = file->get_as_text();
        }
        const CharString resolved_utf8 = resolved.utf8();
        const CharString text_utf8 = text.utf8();
        return Ludot::ShaderInclude{ std::string(resolved_utf8.get_data(), resolved_utf8.length()), std::string(text_utf8.get_data(), text_utf8.length()) };
    };
    const CharString utf8 = source.utf8();
    const CharString path_utf8 = path.utf8();
    const auto result = Ludot::compileShader(std::string(utf8.get_data(), utf8.length()), std::string(path_utf8.get_data(), path_utf8.length()), resolver);
    for (const auto &error : result.diagnostics) {
        const String file = error.path.empty() ? path : String::utf8(error.path.c_str());
        diagnostics += vformat("%s:%d:%d: %s\n", file, error.line + 1, error.column + 1, String::utf8(error.message.c_str()));
    }
    if (!result.diagnostics.empty()) {
        emit_changed();
        return ERR_PARSE_ERROR;
    }
    // Loaded only after a successful compile, which has already rejected cyclic imports.
    Vector<Ref<LuauShader>> loaded;
    for (const std::string &include : result.includes) {
        const Ref<LuauShader> library_resource = ResourceLoader::load(String::utf8(include.c_str()), "LuauShader");
        if (library_resource.is_valid()) loaded.push_back(library_resource);
    }
    includes = loaded;
    library = ShaderLanguage::get_shader_type(String::utf8(result.code.c_str())).is_empty();
    if (library) {
        // Recompile shaders that include this library, directly or through other libraries.
        Vector<LuauShader *> dependents;
        for (LuauShader *shader : live_shaders) {
            for (const Ref<LuauShader> &include : shader->includes) {
                if (include.ptr() == this) dependents.push_back(shader);
            }
        }
        for (LuauShader *dependent : dependents) dependent->compile_source(dependent->source);
        emit_changed();
        return OK;
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
    ClassDB::bind_method(D_METHOD("is_library"), &LuauShader::is_library);
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
    shader->set_load_path(p_path);
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
