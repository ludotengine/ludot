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
    static bool equal(const CallableCustom *p_a, const CallableCustom *p_b) {
        const auto a = static_cast<const LuauCallable *>(p_a);
        const auto b = static_cast<const LuauCallable *>(p_b);
        return a->generation == b->generation && a->identity == b->identity;
    }
    static bool less(const CallableCustom *p_a, const CallableCustom *p_b) {
        const auto a = static_cast<const LuauCallable *>(p_a);
        const auto b = static_cast<const LuauCallable *>(p_b);
        if (a->generation != b->generation) return a->generation < b->generation;
        return uintptr_t(a->identity) < uintptr_t(b->identity);
    }
public:
    LuauCallable(lua_State *p_state, int p_index) {
        reference = lua_ref(p_state, p_index);
        generation = LuauLanguage::get_singleton()->generation;
        identity = lua_topointer(p_state, p_index);
        owner = LuauLanguage::get_singleton()->current_task_owner();
    }
    ~LuauCallable() {
        auto language = LuauLanguage::get_singleton();
        if (language == nullptr) return;
        MutexLock lock(language->mutex);
        if (language->state != nullptr && language->generation == generation) lua_unref(language->state, reference);
    }
    uint32_t hash() const override { const auto id = uintptr_t(identity); return uint32_t(id ^ (id >> 16) ^ generation); }
    String get_as_text() const override { return "Luau function"; }
    CompareEqualFunc get_compare_equal_func() const override { return equal; }
    CompareLessFunc get_compare_less_func() const override { return less; }
    ObjectID get_object() const override { return ObjectID(); }
    bool is_valid() const override {
        auto language = LuauLanguage::get_singleton();
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
            // invoke already reported the error with its stack trace.
            r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
        }
        lua_settop(L, top);
    }
};
}
Variant ludot_callable(lua_State *p_state, int p_index) { return Callable(memnew(LuauCallable(p_state, p_index))); }
