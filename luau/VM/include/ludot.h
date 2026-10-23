// Ludot embedding extensions. Licensed under the MIT license.
#pragma once
#include "lua.h"

// Replace an object's class and fields while preserving its VM identity.
// Both objects remain valid and rooted by the caller during the operation.
LUA_API void lua_migrateobject(lua_State* L, int object, int replacement);
// Replace an existing static class member. Consumes the value on the stack.
LUA_API void lua_setclassmember(lua_State* L, int cls, const char* name);
// Run a C callback with Luau error recovery. On failure the error is on the stack.
LUA_API int lua_rawrunprotected(lua_State* L, void (*callback)(lua_State* L, void* context), void* context);
