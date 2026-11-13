#pragma once
#ifdef TOOLS_ENABLED
#include "core/object/script_language.h"
namespace Luau {
struct ParseResult;
}
void ludot_initialize_lint();
void ludot_lint(const String &source, const Luau::ParseResult &parsed, List<ScriptLanguage::Warning> *warnings);
#endif
