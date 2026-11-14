#include "luau_callable.h"
#include "luau_script.h"
#include "lualib.h"
#include <cstdint>

namespace {
class LuauCallable : public CallableCustom {
    int reference;
    uint64_t generation;
    // The object whose code created this callable. Its tasks end when it is freed.
    ObjectID owner;
    const void *identity;
    // Signal API handlers are distinct per connection, so the same function can connect
    // twice. Their owner is the Godot target, so freeing it drops the connection.
    uint64_t connection = 0;
public:
    static bool equal(const CallableCustom *p_a, const CallableCustom *p_b) {
        const auto a = static_cast<const LuauCallable *>(p_a);
        const auto b = static_cast<const LuauCallable *>(p_b);
        return a->generation == b->generation && a->identity == b->identity && a->connection == b->connection;
    }
    static bool less(const CallableCustom *p_a, const CallableCustom *p_b) {
        const auto a = static_cast<const LuauCallable *>(p_a);
        const auto b = static_cast<const LuauCallable *>(p_b);
        if (a->generation != b->generation) return a->generation < b->generation;
        if (a->connection != b->connection) return a->connection < b->connection;
        return uintptr_t(a->identity) < uintptr_t(b->identity);
    }
    LuauCallable(lua_State *p_state, int p_index, bool p_signal_handler = false) {
        reference = lua_ref(p_state, p_index);
        generation = LuauLanguage::get_singleton()->generation;
        identity = lua_topointer(p_state, p_index);
        owner = LuauLanguage::get_singleton()->current_task_owner();
        static uint64_t next_connection = 0;
        if (p_signal_handler) connection = ++next_connection;
    }
    bool is_signal_handler() const { return connection != 0; }
    ~LuauCallable() {
        auto language = LuauLanguage::get_singleton();
        if (language == nullptr) return;
        MutexLock lock(language->mutex);
        if (language->state != nullptr && language->generation == generation) lua_unref(language->state, reference);
    }
    uint32_t hash() const override { const auto id = uintptr_t(identity); return uint32_t(id ^ (id >> 16) ^ generation ^ connection); }
    String get_as_text() const override { return "Luau function"; }
    CompareEqualFunc get_compare_equal_func() const override { return equal; }
    CompareLessFunc get_compare_less_func() const override { return less; }
    ObjectID get_object() const override { return connection != 0 ? owner : ObjectID(); }
    bool is_valid() const override {
        auto language = LuauLanguage::get_singleton();
        if (connection != 0 && owner.is_valid() && ObjectDB::get_instance(owner) == nullptr) return false;
        return language != nullptr && language->state != nullptr && language->generation == generation;
    }
    void call(const Variant **p_args, int p_count, Variant &r_result, Callable::CallError &r_error) const override {
        auto language = LuauLanguage::get_singleton();
        if (language == nullptr) { r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL; return; }
        MutexLock lock(language->mutex);
        if (!is_valid()) { r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL; return; }
        lua_State *L = language->state;
        const int top = lua_gettop(L);
        lua_getref(L, reference);
        for (int i = 0; i < p_count; ++i) LuauLanguage::push_variant(L, *p_args[i]);
        // Signal handlers run in a resumable thread, so they can call task.wait.
        const int status = language->invoke(p_count, owner) ? LUA_OK : LUA_ERRRUN;
        if (status == LUA_OK && LuauLanguage::try_variant(L, -1, r_result)) {
            r_error.error = Callable::CallError::CALL_OK;
        } else if (status == LUA_OK) {
            ERR_PRINT(language->last_error);
            r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
        } else {
            // invoke already reported the error with its stack trace. A failing signal
            // handler must not make the emitter report the connection as broken.
            r_error.error = connection != 0 ? Callable::CallError::CALL_OK : Callable::CallError::CALL_ERROR_INVALID_METHOD;
        }
        lua_settop(L, top);
    }
};
}
Variant ludot_callable(lua_State *p_state, int p_index) { return Callable(memnew(LuauCallable(p_state, p_index))); }
Callable ludot_signal_callable(lua_State *p_state, int p_index) { return Callable(memnew(LuauCallable(p_state, p_index, true))); }
bool ludot_is_signal_callable(const Callable &p_callable) {
    if (!p_callable.is_custom() || p_callable.get_custom()->get_compare_equal_func() != &LuauCallable::equal) return false;
    return static_cast<const LuauCallable *>(p_callable.get_custom())->is_signal_handler();
}
