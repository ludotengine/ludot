#include "luau_export_plugin.h"
#ifdef TOOLS_ENABLED
#include "core/io/dir_access.h"
#include "core/io/file_access.h"

void LuauExportPlugin::include_source(const String &p_path) {
    for (const String &pattern : get_export_preset()->get_exclude_filter().split(",", false)) {
        const String filter = pattern.strip_edges();
        if (p_path.matchn(filter) || p_path.trim_prefix("res://").matchn(filter)) return;
    }
    add_file(p_path, FileAccess::get_file_as_bytes(p_path), false);
}
void LuauExportPlugin::_export_file(const String &p_path, const String &p_type, const HashSet<String> &p_features) {
    if (p_path.get_extension() == "luau" || p_path.get_extension() == "lushader") skip();
}
void LuauExportPlugin::collect_sources(const String &p_directory) {
    const Ref<DirAccess> directory = DirAccess::open(p_directory);
    if (directory.is_null()) return;
    const String config = p_directory.path_join(".luaurc");
    if (FileAccess::exists(config)) include_source(config);
    directory->list_dir_begin();
    for (String entry = directory->get_next(); !entry.is_empty(); entry = directory->get_next()) {
        if (entry.begins_with(".")) continue;
        const String path = p_directory.path_join(entry);
        if (directory->current_is_dir()) collect_sources(path);
        else if (entry.get_extension() == "luau" || entry.get_extension() == "lushader") {
            // Imports and aliases are resolved at runtime, including dynamic require.
            // Keep the original paths and source in every desktop export.
            include_source(path);
        }
    }
    directory->list_dir_end();
}
void LuauExportPlugin::_export_begin(const HashSet<String> &p_features, bool p_debug, const String &p_path, int p_flags) {
    collect_sources("res://");
}
#endif
