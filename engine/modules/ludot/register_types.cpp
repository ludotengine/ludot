#include "register_types.h"
#include "core/object/class_db.h"
#include "luau_script.h"
#include "luau_shader.h"
#ifdef TOOLS_ENABLED
#include "editor_theme_tools.h"
#include "luau_editor_assist.h"
#include "luau_enum_editor.h"
#include "luau_export_plugin.h"
#include "luau_formatter.h"
#include "luau_highlighter.h"
#include "luau_lint.h"
#include "luau_shader_editor.h"

#include "editor/editor_node.h"
#include "editor/export/editor_export.h"
#include "editor/script/script_editor_plugin.h"
static void initialize_luau_shader_editor() {
    LuauFormatter::initialize();
    ludot_initialize_editor_assist();
    ludot_initialize_lint();
	ludot_initialize_enum_editor();
	Ref<LuauSyntaxHighlighter> highlighter;
    highlighter.instantiate();
    ScriptEditor::get_singleton()->register_syntax_highlighter(highlighter);
    Ref<LuauShaderLanguagePlugin> plugin;
    plugin.instantiate();
    EditorShaderLanguagePlugin::register_shader_language(plugin);
    Ref<LuauExportPlugin> exporter;
    exporter.instantiate();
    EditorExport::get_singleton()->add_export_plugin(exporter);
}
#endif
static LuauLanguage *language;
static Ref<ResourceFormatLoaderLuau> script_loader;
static Ref<ResourceFormatSaverLuau> script_saver;
static Ref<ResourceFormatLoaderLuauShader> shader_loader;
static Ref<ResourceFormatSaverLuauShader> shader_saver;
void initialize_ludot_module(ModuleInitializationLevel level) {
#ifdef TOOLS_ENABLED
    if (level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        GDREGISTER_CLASS(LuauSyntaxHighlighter);
        GDREGISTER_INTERNAL_CLASS(LudotThemeTools);
        EditorNode::add_init_callback(initialize_luau_shader_editor);
    }
#endif
    if (level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
        GDREGISTER_CLASS(LuauScript);
        GDREGISTER_ABSTRACT_CLASS(LuauValue);
        language = memnew(LuauLanguage);
        ScriptServer::register_language(language);
        script_loader.instantiate(); script_saver.instantiate();
        ResourceLoader::add_resource_format_loader(script_loader, true);
        ResourceSaver::add_resource_format_saver(script_saver, true);
    }
    if (level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        GDREGISTER_CLASS(LuauShader);
        shader_loader.instantiate();
        shader_saver.instantiate();
        ResourceLoader::add_resource_format_loader(shader_loader, true);
        ResourceSaver::add_resource_format_saver(shader_saver, true);
    }
}
void uninitialize_ludot_module(ModuleInitializationLevel level) {
    if (level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
        ResourceLoader::remove_resource_format_loader(script_loader);
        ResourceSaver::remove_resource_format_saver(script_saver);
        script_loader.unref(); script_saver.unref();
        ScriptServer::unregister_language(language);
        memdelete(language);
    }
    if (level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ResourceLoader::remove_resource_format_loader(shader_loader);
        ResourceSaver::remove_resource_format_saver(shader_saver);
        shader_loader.unref();
        shader_saver.unref();
    }
}
