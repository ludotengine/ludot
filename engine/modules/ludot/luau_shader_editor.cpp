#include "luau_shader_editor.h"
#ifdef TOOLS_ENABLED
#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "ludot/shader.h"
#include "scene/gui/box_container.h"
#include "scene/resources/syntax_highlighter.h"
#include "editor/settings/editor_settings.h"

LuauShaderEditor::LuauShaderEditor() {
    auto box = memnew(VBoxContainer);
    add_child(box);
    code = memnew(CodeEdit);
    code->set_v_size_flags(Control::SIZE_EXPAND_FILL);
    code->set_draw_line_numbers(true);
    code->set_code_completion_enabled(true);
    Ref<CodeHighlighter> highlighter;
    highlighter.instantiate();
    const Color keyword = EDITOR_GET("text_editor/theme/highlighting/keyword_color");
    for (const char *word : {"shader", "include", "import", "from", "as", "export", "function", "end", "local", "const", "if", "then", "else", "elseif", "for", "in", "to", "step", "do", "while", "repeat", "until", "return", "break", "continue", "and", "or", "not", "true", "false", "nil", "renderMode"})
        highlighter->add_keyword_color(word, keyword);
    const Color type = EDITOR_GET("text_editor/theme/highlighting/base_type_color");
    for (const char *word : {"number", "float", "boolean", "bool", "int", "uint", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4", "bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4", "sampler2D", "sampler2DArray", "sampler3D", "samplerCube", "samplerExternalOES", "DepthTexture", "ScreenTexture", "NormalRoughnessTexture"})
        highlighter->add_keyword_color(word, type);
    highlighter->set_symbol_color(EDITOR_GET("text_editor/theme/highlighting/symbol_color"));
    highlighter->set_number_color(EDITOR_GET("text_editor/theme/highlighting/number_color"));
    highlighter->set_function_color(EDITOR_GET("text_editor/theme/highlighting/function_color"));
    highlighter->set_member_variable_color(EDITOR_GET("text_editor/theme/highlighting/member_variable_color"));
    highlighter->add_color_region("--", "", EDITOR_GET("text_editor/theme/highlighting/comment_color"), true);
    highlighter->add_color_region("\"", "\"", EDITOR_GET("text_editor/theme/highlighting/string_color"));
    code->set_syntax_highlighter(highlighter);
    box->add_child(code);
    errors = memnew(Label);
    errors->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
    box->add_child(errors);
    validation = memnew(Timer);
    validation->set_one_shot(true);
    validation->set_wait_time(0.15);
    add_child(validation);
    validation->connect("timeout", callable_mp(this, &LuauShaderEditor::apply_shaders));
    code->connect("text_changed", callable_mp(this, &LuauShaderEditor::text_changed));
    code->connect("code_completion_requested", callable_mp(this, &LuauShaderEditor::complete));
}
void LuauShaderEditor::edit_shader(const Ref<Shader> &p_shader) {
    shader = p_shader;
    saved = shader->get_source();
    code->set_text(saved);
    errors->set_text(shader->get_diagnostics());
}
void LuauShaderEditor::text_changed() { validation->start(); }
void LuauShaderEditor::apply_shaders() {
    if (shader.is_null()) return;
    if (shader->get_source() != code->get_text()) shader->compile_source(code->get_text());
    errors->set_text(shader->get_diagnostics());
}
void LuauShaderEditor::save_external_data(const String &p_path) {
    apply_shaders();
    const String path = p_path.is_empty() ? shader->get_path() : p_path;
    if (ResourceSaver::save(shader, path) == OK) saved = code->get_text();
}
void LuauShaderEditor::complete() {
    for (const auto &word : Ludot::shaderCompletions()) {
        const String text = String::utf8(word.c_str());
        code->add_code_completion_option(CodeEdit::KIND_PLAIN_TEXT, text, text);
    }
    code->update_code_completion_options(true);
}
ShaderEditor *LuauShaderLanguagePlugin::edit_shader(const Ref<Shader> &p_shader) {
    auto editor = memnew(LuauShaderEditor);
    editor->edit_shader(p_shader);
    return editor;
}
Ref<Shader> LuauShaderLanguagePlugin::create_new_shader(int p_variation, Shader::Mode p_mode, int p_template) {
    Ref<LuauShader> shader;
    shader.instantiate();
    const String kind = p_mode == Shader::MODE_CANVAS_ITEM ? "canvas_item" : p_mode == Shader::MODE_PARTICLES ? "particles" : "spatial";
    const String entry = p_mode == Shader::MODE_PARTICLES ? "process" : "fragment";
    shader->set_source("shader " + kind + "\n\nfunction " + entry + "()\nend\n");
    return shader;
}
#endif
