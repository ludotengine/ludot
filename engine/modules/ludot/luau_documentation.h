#pragma once
#ifdef TOOLS_ENABLED
#include "core/string/ustring.h"

// Member names are native ClassDB names. Select examples at the point of use,
// never in the shared GDScript/C# documentation cache.
String ludot_luau_doc(const String &p_text, const String &p_class, const String &p_member = String());
String ludot_luau_example(const String &p_key);
#endif
