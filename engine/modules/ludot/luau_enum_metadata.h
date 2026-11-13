#pragma once
#include "core/variant/dictionary.h"
namespace Luau {
class AstStatBlock;
class AstType;
} //namespace Luau
Dictionary ludot_enum_metadata(Luau::AstStatBlock *root, Luau::AstType *type, const String &path);
