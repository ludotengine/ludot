#pragma once
#ifdef TOOLS_ENABLED
#include "editor/shader/editor_shader_language_plugin.h"
#include "luau_shader.h"
#include "scene/gui/code_edit.h"
#include "scene/gui/label.h"
#include "scene/main/timer.h"

class LuauShaderEditor : public ShaderEditor {
    GDCLASS(LuauShaderEditor, ShaderEditor);
    Ref<LuauShader> shader;
    CodeEdit *code;
    Label *errors;
    Timer *validation;
    String saved;
    void text_changed();
    void complete();
public:
    LuauShaderEditor();
    void edit_shader(const Ref<Shader> &p_shader) override;
    void use_menu_bar(MenuButton *p_menu) override {}
    void apply_shaders() override;
    bool is_unsaved() const override { return code->get_text() != saved; }
    void save_external_data(const String &p_path = "") override;
    void set_toggle_list_control(Control *p_control) override {}
    void update_toggle_files_button() override {}
    void validate_script() override { apply_shaders(); }
};
class LuauShaderLanguagePlugin : public EditorShaderLanguagePlugin {
    GDCLASS(LuauShaderLanguagePlugin, EditorShaderLanguagePlugin);
public:
    bool handles_shader(const Ref<Shader> &p_shader) const override { return Object::cast_to<LuauShader>(p_shader.ptr()) != nullptr; }
    ShaderEditor *edit_shader(const Ref<Shader> &p_shader) override;
    Ref<Shader> create_new_shader(int p_variation, Shader::Mode p_mode, int p_template) override;
    PackedStringArray get_language_variations() const override { return {"Luau shader"}; }
    String get_file_extension(int p_variation) const override { return "lushader"; }
};
#endif
