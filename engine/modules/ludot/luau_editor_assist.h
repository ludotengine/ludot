#pragma once
#ifdef TOOLS_ENABLED
class CodeEdit;
void ludot_initialize_editor_assist();
bool ludot_complete_block(CodeEdit *editor);
#endif
