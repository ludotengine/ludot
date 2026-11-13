#pragma once
#ifdef TOOLS_ENABLED
#include "core/string/ustring.h"
class ScriptTextEditor;
namespace LuauFormatter {
void initialize();
bool enabled();
String selene_source(const String &source);
void format(ScriptTextEditor *editor, bool selection, bool on_save = false);
} //namespace LuauFormatter
#endif
