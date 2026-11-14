#pragma once
#include "core/variant/variant.h"
#include "lua.h"
Variant ludot_callable(lua_State *p_state, int p_index);
// A handler for the Signal API: unique per connection, owned by the current task owner,
// and reporting its own errors so the emitter continues with the other handlers.
Callable ludot_signal_callable(lua_State *p_state, int p_index);
// True for callables made by ludot_signal_callable, which disconnectAll may remove.
bool ludot_is_signal_callable(const Callable &p_callable);
