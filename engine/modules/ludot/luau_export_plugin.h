#pragma once
#ifdef TOOLS_ENABLED
#include "editor/export/editor_export_plugin.h"

class LuauExportPlugin : public EditorExportPlugin {
    GDCLASS(LuauExportPlugin, EditorExportPlugin);
    void include_source(const String &p_path);
    void collect_sources(const String &p_directory);
protected:
    void _export_file(const String &p_path, const String &p_type, const HashSet<String> &p_features) override;
    void _export_begin(const HashSet<String> &p_features, bool p_debug, const String &p_path, int p_flags) override;
public:
    String get_name() const override { return "Ludot Luau"; }
};
#endif
