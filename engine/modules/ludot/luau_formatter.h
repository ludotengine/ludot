#pragma once
#ifdef TOOLS_ENABLED
#include "core/string/ustring.h"
class ScriptTextEditor;
namespace LuauFormatter {
void initialize();
bool enabled();
String selene_source(const String &source);
void format(ScriptTextEditor *editor, bool selection, bool on_save = false);
// Formats source with StyLua and the user's editor settings. Returns false with r_error
// when the source has syntax errors or StyLua fails.
bool format_source(const String &source, String &r_result, String &r_error);
} //namespace LuauFormatter
#endif
