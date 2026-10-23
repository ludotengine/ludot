#include "luau_script.h"
#include "lobject.h"
#include "lstate.h"
#include "lualib.h"

namespace {
int get_field(lua_State *L) { lua_pushvalue(L, 2); lua_gettable(L, 1); return 1; }
void inspect_field(lua_State *L, int index, const char *name) {
    const int absolute = lua_absindex(L, index);
    lua_pushcfunction(L, get_field, "debugger field");
    lua_pushvalue(L, absolute);
    lua_pushstring(L, name);
    const auto hook = lua_callbacks(L)->debugprotectederror;
    lua_callbacks(L)->debugprotectederror = nullptr;
    lua_pcall(L, 2, 1, 0);
    lua_callbacks(L)->debugprotectederror = hook;
}
bool push_self(lua_State *L, int level) {
    for (int i = 1;; ++i) {
        const char *name = lua_getlocal(L, level, i);
        if (name == nullptr) break;
        if (strcmp(name, "self") == 0 && lua_isobject(L, -1)) return true;
        lua_pop(L, 1);
    }
    lua_Debug frame = {};
    if (!lua_getinfo(L, level, "f", &frame)) return false;
    for (int i = 1;; ++i) {
        const char *name = lua_getupvalue(L, -1, i);
        if (name == nullptr) break;
        if (strcmp(name, "self") == 0 && lua_isobject(L, -1)) { lua_remove(L, -2); return true; }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return false;
}
Variant inspect_value(lua_State *L, int index, int limit, int depth) {
    const int absolute = lua_absindex(L, index);
    const int kind = lua_type(L, absolute);
    if (kind == LUA_TFUNCTION) return String("<Luau function>");
    if (kind == LUA_TCLASS) {
        const auto cls = static_cast<const LuauClass *>(lua_topointer(L, absolute));
        return "<class " + String::utf8(getstr(cls->name)) + ">";
    }
    if (kind == LUA_TTHREAD) return String("<coroutine>");
    if (kind == LUA_TUSERDATA) {
        if (lua_getmetatable(L, absolute)) {
            luaL_getmetatable(L, "Ludot.Variant");
            const bool native = lua_rawequal(L, -1, -2);
            lua_pop(L, 2);
            if (native) return LuauLanguage::to_variant(L, absolute);
        }
        return String("<userdata>");
    }
    if (kind == LUA_TOBJECT) {
        const auto object = static_cast<const LuauObject *>(lua_topointer(L, absolute));
        Dictionary members;
        if (depth <= 0) return "<" + String::utf8(getstr(object->lclass->name)) + ">";
        for (uint32_t i = 0; i < object->numberofmembers && i < uint32_t(limit); ++i) {
            const char *name = getstr(object->lclass->offsettomember[i]);
            inspect_field(L, absolute, name);
            members[String::utf8(name)] = inspect_value(L, -1, limit, depth - 1);
            lua_pop(L, 1);
        }
        return members;
    }
    if (kind == LUA_TTABLE) {
        if (depth <= 0) return String("<table>");
        Dictionary table;
        lua_pushnil(L);
        int count = 0;
        while (lua_next(L, absolute)) {
            if (count++ >= limit) { lua_pop(L, 2); break; }
            const Variant key = inspect_value(L, -2, limit, 0);
            table[key] = inspect_value(L, -1, limit, depth - 1);
            lua_pop(L, 1);
        }
        return table;
    }
    if (kind == LUA_TLIGHTUSERDATA || kind == LUA_TBUFFER) return String("<") + lua_typename(L, kind) + ">";
    return LuauLanguage::to_variant(L, absolute);
}
}
void LuauLanguage::debug_get_stack_level_locals(int p_level, List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) {
    MutexLock lock(mutex);
    lua_State *L = debug_state != nullptr ? debug_state : state;
    if (L == nullptr) return;
    const int limit = p_max_subitems > 0 ? p_max_subitems : 64;
    for (int i = 1;; ++i) {
        const char *name = lua_getlocal(L, p_level, i);
        if (name == nullptr) break;
        r_names->push_back(String::utf8(name));
        r_values->push_back(inspect_value(L, -1, limit, (p_max_depth >= 0 ? p_max_depth : 3)));
        lua_pop(L, 1);
    }
}
void LuauLanguage::debug_get_stack_level_members(int p_level, List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) {
    MutexLock lock(mutex);
    lua_State *L = debug_state != nullptr ? debug_state : state;
    if (L == nullptr) return;
    const int limit = p_max_subitems > 0 ? p_max_subitems : 64;
    if (!push_self(L, p_level)) return;
    const auto object = static_cast<const LuauObject *>(lua_topointer(L, -1));
    for (uint32_t member = 0; member < object->numberofmembers && member < uint32_t(limit); ++member) {
        const char *field = getstr(object->lclass->offsettomember[member]);
        r_names->push_back(String::utf8(field));
        inspect_field(L, -1, field);
        r_values->push_back(inspect_value(L, -1, limit, p_max_depth >= 0 ? p_max_depth : 3));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}
void LuauLanguage::debug_get_globals(List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) {
    MutexLock lock(mutex);
    lua_State *L = debug_state != nullptr ? debug_state : state;
    if (L == nullptr) return;
    const int limit = p_max_subitems > 0 ? p_max_subitems : 64;
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            r_names->push_back(String::utf8(lua_tostring(L, -2)));
            r_values->push_back(inspect_value(L, -1, limit, (p_max_depth >= 0 ? p_max_depth : 3)));
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}
ScriptInstance *LuauLanguage::debug_get_stack_level_instance(int p_level) {
    MutexLock lock(mutex);
    lua_State *L = debug_state != nullptr ? debug_state : state;
    if (L == nullptr) return nullptr;
    if (!push_self(L, p_level)) return nullptr;
    inspect_field(L, -1, "_native");
    Object *owner = nullptr;
    if (lua_isuserdata(L, -1)) owner = static_cast<Object *>(LuauLanguage::to_variant(L, -1));
    lua_pop(L, 2);
    return owner != nullptr ? owner->get_script_instance() : nullptr;
}
String LuauLanguage::debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems, int p_max_depth) {
    Variant value;
    String error;
    debug_evaluate_expression(p_level, p_expression, value, error);
    return error.is_empty() ? value.get_construct_string() : error;
}
bool LuauLanguage::debug_evaluate_expression(int p_level, const String &p_expression, Variant &r_value, String &r_error) {
    MutexLock lock(mutex);
    lua_State *L = debug_state != nullptr ? debug_state : state;
    if (L == nullptr) { r_error = "The Luau VM is not running"; return true; }
    const int top = lua_gettop(L);
    lua_newtable(L);
    const int environment = lua_absindex(L, -1);
    lua_newtable(L);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, environment);
    lua_Debug frame = {};
    if (lua_getinfo(L, p_level, "f", &frame)) {
        for (int i = 1;; ++i) {
            const char *name = lua_getupvalue(L, -1, i);
            if (name == nullptr) break;
            lua_setfield(L, environment, name);
        }
        lua_pop(L, 1);
    }
    for (int i = 1;; ++i) {
        const char *name = lua_getlocal(L, p_level, i);
        if (name == nullptr) break;
        lua_setfield(L, environment, name);
    }
    const String saved_error = last_error;
    const Error compiled = load_chunk("return " + p_expression, debug_get_stack_level_source(p_level), L);
    if (compiled != OK) {
        const String error = last_error;
        last_error = saved_error;
        lua_settop(L, top);
        r_error = error;
        return true;
    }
    lua_pushvalue(L, environment);
    lua_setfenv(L, -2);
    const bool stepping = L->singlestep;
    lua_singlestep(L, false);
    const auto error_hook = lua_callbacks(L)->debugprotectederror;
    lua_callbacks(L)->debugprotectederror = nullptr;
    const int status = lua_pcall(L, 0, 1, 0);
    lua_callbacks(L)->debugprotectederror = error_hook;
    lua_singlestep(L, stepping);
    if (status == LUA_OK) r_value = inspect_value(L, -1, 64, 3);
    else r_error = String::utf8(lua_tostring(L, -1));
    lua_settop(L, top);
    last_error = saved_error;
    return true;
}
