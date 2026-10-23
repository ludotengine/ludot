#include "register_types.h"
#include "core/object/class_db.h"
#include "luau_script.h"
#include "luau_shader.h"
#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/export/editor_export.h"
#include "luau_export_plugin.h"
#include "luau_shader_editor.h"
static void initialize_luau_shader_editor() {
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
    if (level == MODULE_INITIALIZATION_LEVEL_EDITOR) EditorNode::add_init_callback(initialize_luau_shader_editor);
#endif
    if (level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
        GDREGISTER_CLASS(LuauScript);
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
