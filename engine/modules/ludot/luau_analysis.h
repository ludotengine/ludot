#pragma once
#include "core/object/script_language.h"
bool ludot_validate(const String &source, const String &path, List<ScriptLanguage::ScriptError> *errors);
Error ludot_complete(const String &source, const String &path, List<ScriptLanguage::CodeCompletionOption> *options);
void ludot_clear_analysis();
