// Ludot Signal library, modeled on Roblox's RBXScriptSignal. Licensed under the MIT license.
//
// One Signal object type serves three cases:
// - Signal.new() creates a Luau signal. Its connections live in a table held by the
//   object's own metatable, so the garbage collector sees handler cycles.
// - Engine signals, such as Node.tree_exiting, are Signal objects bound to an object and
//   a signal name. Handlers become Godot connections, and fire emits the engine signal.
// - A Signal field of a script class starts as a Luau signal and is bound to the engine
//   signal of the same name when the instance is created, so GDScript and the editor see it.
#include "luau_callable.h"
#include "luau_script.h"

#include "core/object/class_db.h"
#include "lualib.h"

namespace {
constexpr int LUDOT_SIGNAL_TAG = 103;
constexpr int LUDOT_CONNECTION_TAG = 104;
constexpr const char *SIGNAL_METHODS = "Ludot.SignalMethods";
constexpr const char *ENGINE_SIGNAL_META = "Ludot.EngineSignal";
constexpr const char *ENGINE_CONNECTION_META = "Ludot.EngineConnection";
constexpr const char *SIGNAL_EQ = "Ludot.SignalEq";
constexpr const char *SIGNAL_TOSTRING = "Ludot.SignalToString";
constexpr const char *CONNECTION_INDEX = "Ludot.ConnectionIndex";
constexpr const char *CONNECTION_TOSTRING = "Ludot.ConnectionToString";

struct SignalData {
	bool engine = false;
	ObjectID object;
	StringName name;
};

struct ConnectionData {
	bool engine = false;
	ObjectID object;
	StringName name;
	Callable callable;
};

SignalData *check_signal(lua_State *L, int p_index) {
	auto *data = static_cast<SignalData *>(lua_touserdatatagged(L, p_index, LUDOT_SIGNAL_TAG));
	if (data == nullptr) luaL_typeerror(L, p_index, "Signal");
	return data;
}

ConnectionData *check_connection(lua_State *L, int p_index) {
	auto *data = static_cast<ConnectionData *>(lua_touserdatatagged(L, p_index, LUDOT_CONNECTION_TAG));
	if (data == nullptr) luaL_typeerror(L, p_index, "Connection");
	return data;
}

Object *signal_object(lua_State *L, const SignalData *p_data) {
	Object *object = ObjectDB::get_instance(p_data->object);
	if (object == nullptr) luaL_error(L, "The object that owns signal '%s' has been freed", String(p_data->name).utf8().get_data());
	return object;
}

void push_owner_id(lua_State *L, ObjectID p_owner) {
	lua_pushlightuserdata(L, reinterpret_cast<void *>(uintptr_t(uint64_t(p_owner))));
}

ObjectID owner_id(lua_State *L, int p_index) {
	return ObjectID(uint64_t(reinterpret_cast<uintptr_t>(lua_tolightuserdata(L, p_index))));
}

// Fields shared by the metatables of Signal and Connection objects.
void set_shared_fields(lua_State *L, const char *p_type, const char *p_tostring) {
	lua_pushstring(L, p_type);
	lua_setfield(L, -2, "__type");
	lua_pushstring(L, "The metatable is locked");
	lua_setfield(L, -2, "__metatable");
	lua_getfield(L, LUA_REGISTRYINDEX, p_tostring);
	lua_setfield(L, -2, "__tostring");
}

// Pushes the slots table of a Luau signal at p_index.
void push_slots(lua_State *L, int p_index) {
	lua_getmetatable(L, p_index);
	lua_getfield(L, -1, "slots");
	lua_remove(L, -2);
}

// Removes the slot at the top of the stack from the slots table below it.
void remove_slot(lua_State *L, int p_slots, int p_slot) {
	const int count = lua_objlen(L, p_slots);
	for (int i = 1; i <= count; ++i) {
		lua_rawgeti(L, p_slots, i);
		const bool found = lua_rawequal(L, -1, p_slot);
		lua_pop(L, 1);
		if (!found) continue;
		for (int j = i; j < count; ++j) {
			lua_rawgeti(L, p_slots, j + 1);
			lua_rawseti(L, p_slots, j);
		}
		lua_pushnil(L);
		lua_rawseti(L, p_slots, count);
		return;
	}
}

// A slot bound to the engine keeps the Godot connection it moved to.
bool slot_engine_connection(lua_State *L, int p_slot, Object *&r_object, StringName &r_name, Callable &r_callable) {
	lua_getfield(L, p_slot, "bound");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return false;
	}
	r_callable = LuauLanguage::to_variant(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, p_slot, "object");
	r_object = ObjectDB::get_instance(owner_id(L, -1));
	lua_pop(L, 1);
	lua_getfield(L, p_slot, "signal");
	r_name = String::utf8(lua_tostring(L, -1));
	lua_pop(L, 1);
	return true;
}

bool slot_connected(lua_State *L, int p_slot) {
	Object *object = nullptr;
	StringName name;
	Callable callable;
	if (slot_engine_connection(L, p_slot, object, name, callable)) return object != nullptr && object->is_connected(name, callable);
	lua_getfield(L, p_slot, "on");
	const bool on = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return on;
}

void disconnect_slot(lua_State *L, int p_slots, int p_slot) {
	Object *object = nullptr;
	StringName name;
	Callable callable;
	if (slot_engine_connection(L, p_slot, object, name, callable)) {
		if (object != nullptr && object->is_connected(name, callable)) object->disconnect(name, callable);
		return;
	}
	lua_pushboolean(L, false);
	lua_setfield(L, p_slot, "on");
	remove_slot(L, p_slots, p_slot);
}

void push_engine_connection(lua_State *L, const SignalData *p_signal, const Callable &p_callable) {
	auto *data = new (lua_newuserdatatagged(L, sizeof(ConnectionData), LUDOT_CONNECTION_TAG)) ConnectionData;
	data->engine = true;
	data->object = p_signal->object;
	data->name = p_signal->name;
	data->callable = p_callable;
	luaL_getmetatable(L, ENGINE_CONNECTION_META);
	lua_setmetatable(L, -2);
}

// Adds a slot for the handler at p_handler to the Luau signal at p_signal and pushes its Connection.
void push_luau_connection(lua_State *L, int p_signal, int p_handler, bool p_once) {
	push_slots(L, p_signal);
	const int slots = lua_gettop(L);
	lua_createtable(L, 1, 3);
	lua_pushvalue(L, p_handler);
	lua_rawseti(L, -2, 1);
	lua_pushboolean(L, true);
	lua_setfield(L, -2, "on");
	lua_pushboolean(L, p_once);
	lua_setfield(L, -2, "once");
	push_owner_id(L, LuauLanguage::get_singleton()->current_task_owner());
	lua_setfield(L, -2, "owner");
	const int slot = lua_gettop(L);
	lua_pushvalue(L, slot);
	lua_rawseti(L, slots, lua_objlen(L, slots) + 1);
	new (lua_newuserdatatagged(L, sizeof(ConnectionData), LUDOT_CONNECTION_TAG)) ConnectionData;
	lua_createtable(L, 0, 6);
	set_shared_fields(L, "Connection", CONNECTION_TOSTRING);
	lua_getfield(L, LUA_REGISTRYINDEX, CONNECTION_INDEX);
	lua_setfield(L, -2, "__index");
	lua_pushvalue(L, slot);
	lua_setfield(L, -2, "slot");
	lua_pushvalue(L, slots);
	lua_setfield(L, -2, "slots");
	lua_setmetatable(L, -2);
	lua_replace(L, slots);
	lua_settop(L, slots);
}

// Connects the handler at index 2 to the signal at index 1 and pushes the Connection.
int connect_handler(lua_State *L, bool p_once) {
	SignalData *data = check_signal(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	if (!data->engine) {
		push_luau_connection(L, 1, 2, p_once);
		return 1;
	}
	Object *object = signal_object(L, data);
	Callable callable = ludot_signal_callable(L, 2);
	const Error error = object->connect(data->name, callable, p_once ? Object::CONNECT_ONE_SHOT : 0);
	if (error != OK) {
		callable = Callable();
		luaL_error(L, "Cannot connect to signal '%s'", String(data->name).utf8().get_data());
	}
	push_engine_connection(L, data, callable);
	return 1;
}

int signal_connect(lua_State *L) {
	return connect_handler(L, false);
}

int signal_once(lua_State *L) {
	return connect_handler(L, true);
}

// Tables and plain class objects cross the engine as LuauValue, so Luau handlers receive
// the same value. Other values convert as they do for any engine call.
Variant signal_argument(lua_State *L, int p_index) {
	bool wrap = lua_istable(L, p_index);
	if (lua_type(L, p_index) == LUA_TOBJECT) {
		lua_getfield(L, p_index, "_native");
		wrap = !lua_isuserdata(L, -1);
		lua_pop(L, 1);
	}
	if (!wrap) return LuauLanguage::to_variant(L, p_index);
	Ref<LuauValue> value;
	value.instantiate();
	value->store(L, p_index);
	return value;
}

int signal_fire(lua_State *L) {
	SignalData *data = check_signal(L, 1);
	const int count = lua_gettop(L) - 1;
	if (data->engine) {
		Object *object = signal_object(L, data);
		Vector<Variant> values;
		Vector<const Variant *> arguments;
		values.resize(count);
		arguments.resize(count);
		for (int i = 0; i < count; ++i) {
			values.write[i] = signal_argument(L, i + 2);
			arguments.write[i] = &values[i];
		}
		object->emit_signalp(data->name, arguments.ptrw(), count);
		return 0;
	}
	auto language = LuauLanguage::get_singleton();
	lua_State *main = language->state;
	// Handlers run from a snapshot, so they can connect and disconnect while firing.
	push_slots(L, 1);
	const int slots = lua_gettop(L);
	const int total = lua_objlen(L, slots);
	lua_createtable(L, total, 0);
	const int snapshot = lua_gettop(L);
	for (int i = 1; i <= total; ++i) {
		lua_rawgeti(L, slots, i);
		lua_rawseti(L, snapshot, i);
	}
	for (int i = 1; i <= total; ++i) {
		lua_rawgeti(L, snapshot, i);
		const int slot = lua_gettop(L);
		lua_getfield(L, slot, "on");
		const bool on = lua_toboolean(L, -1);
		lua_getfield(L, slot, "owner");
		const ObjectID owner = owner_id(L, -1);
		lua_getfield(L, slot, "once");
		const bool once = lua_toboolean(L, -1);
		lua_pop(L, 3);
		if (!on) {
			lua_settop(L, snapshot);
			continue;
		}
		// A handler whose owner was freed never runs again.
		if ((owner.is_valid() && ObjectDB::get_instance(owner) == nullptr) || once) disconnect_slot(L, slots, slot);
		if (owner.is_valid() && ObjectDB::get_instance(owner) == nullptr) {
			lua_settop(L, snapshot);
			continue;
		}
		lua_rawgeti(L, slot, 1);
		for (int argument = 0; argument < count; ++argument) lua_pushvalue(L, argument + 2);
		lua_xmove(L, main, count + 1);
		// invoke reports a failing handler with its stack trace and the rest still run.
		language->invoke(count, owner, "Signal handler");
		lua_pop(main, 1);
		lua_settop(L, snapshot);
	}
	return 0;
}

// Resumes a thread waiting in Signal:wait with the fired arguments.
int signal_resume_waiter(lua_State *L) {
	auto language = LuauLanguage::get_singleton();
	lua_State *thread = lua_tothread(L, lua_upvalueindex(1));
	const uint64_t order = uint64_t(lua_tonumber(L, lua_upvalueindex(2)));
	LuauLanguage::Task *task = language->tasks.getptr(thread);
	if (task == nullptr || task->kind != LuauLanguage::Task::SIGNAL || task->order != order) return 0;
	const int count = lua_gettop(L);
	lua_xmove(L, thread, count);
	task->arguments = count;
	language->resume_task(thread);
	return 0;
}

int signal_wait(lua_State *L) {
	SignalData *data = check_signal(L, 1);
	if (!lua_isyieldable(L)) luaL_error(L, "Signal:wait cannot yield here. Call it from a script callback, a signal handler, or a task thread");
	auto language = LuauLanguage::get_singleton();
	Object *object = data->engine ? signal_object(L, data) : nullptr;
	lua_settop(L, 1);
	language->schedule_task(L, LuauLanguage::Task::SIGNAL, 0.0, 0);
	lua_pushthread(L);
	lua_pushnumber(L, double(language->tasks[L].order));
	lua_pushcclosure(L, signal_resume_waiter, "Signal:wait", 2);
	if (data->engine) {
		Callable callable = ludot_signal_callable(L, 2);
		object->connect(data->name, callable, Object::CONNECT_ONE_SHOT);
	} else {
		push_luau_connection(L, 1, 2, true);
	}
	lua_settop(L, 0);
	return lua_yield(L, 0);
}

int signal_disconnect_all(lua_State *L) {
	SignalData *data = check_signal(L, 1);
	if (data->engine) {
		Object *object = signal_object(L, data);
		// Only connections made through this API. Editor and GDScript connections stay.
		List<Object::Connection> connections;
		object->get_signal_connection_list(data->name, &connections);
		for (const Object::Connection &connection : connections) {
			if (ludot_is_signal_callable(connection.callable)) object->disconnect(data->name, connection.callable);
		}
		return 0;
	}
	push_slots(L, 1);
	const int slots = lua_gettop(L);
	const int total = lua_objlen(L, slots);
	lua_createtable(L, total, 0);
	const int snapshot = lua_gettop(L);
	for (int i = 1; i <= total; ++i) {
		lua_rawgeti(L, slots, i);
		lua_rawseti(L, snapshot, i);
	}
	for (int i = 1; i <= total; ++i) {
		lua_rawgeti(L, snapshot, i);
		disconnect_slot(L, slots, lua_gettop(L));
		lua_pop(L, 1);
	}
	return 0;
}

int signal_eq(lua_State *L) {
	const auto *a = static_cast<const SignalData *>(lua_touserdatatagged(L, 1, LUDOT_SIGNAL_TAG));
	const auto *b = static_cast<const SignalData *>(lua_touserdatatagged(L, 2, LUDOT_SIGNAL_TAG));
	// Engine signals are values: two reads of node.TreeExiting are equal.
	lua_pushboolean(L, a != nullptr && b != nullptr && (a == b || (a->engine && b->engine && a->object == b->object && a->name == b->name)));
	return 1;
}

int signal_tostring(lua_State *L) {
	const SignalData *data = check_signal(L, 1);
	if (data->engine) {
		Object *object = ObjectDB::get_instance(data->object);
		const String owner = object != nullptr ? String(object->get_class_name()) : String("<freed>");
		lua_pushstring(L, ("Signal " + owner + "." + String(data->name)).utf8().get_data());
	} else {
		lua_pushstring(L, "Signal");
	}
	return 1;
}

int connection_disconnect(lua_State *L) {
	ConnectionData *data = check_connection(L, 1);
	if (data->engine) {
		Object *object = ObjectDB::get_instance(data->object);
		if (object != nullptr && object->is_connected(data->name, data->callable)) object->disconnect(data->name, data->callable);
		return 0;
	}
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, "slots");
	lua_getfield(L, -2, "slot");
	disconnect_slot(L, lua_gettop(L) - 1, lua_gettop(L));
	return 0;
}

int connection_index(lua_State *L) {
	ConnectionData *data = check_connection(L, 1);
	const String key = String::utf8(luaL_checkstring(L, 2));
	if (key == LuauLanguage::api_name("disconnect")) {
		lua_pushcfunction(L, connection_disconnect, "Connection:disconnect");
		return 1;
	}
	if (key == LuauLanguage::api_name("connected", true)) {
		if (data->engine) {
			Object *object = ObjectDB::get_instance(data->object);
			lua_pushboolean(L, object != nullptr && object->is_connected(data->name, data->callable));
		} else {
			lua_getmetatable(L, 1);
			lua_getfield(L, -1, "slot");
			lua_pushboolean(L, slot_connected(L, lua_gettop(L)));
		}
		return 1;
	}
	luaL_error(L, "'%s' is not a member of Connection", key.utf8().get_data());
}

int connection_tostring(lua_State *L) {
	check_connection(L, 1);
	lua_pushstring(L, "Connection");
	return 1;
}

// Signal.new(...names). The names are read from the source by the engine, for script signals.
int signal_new(lua_State *L) {
	for (int i = 1; i <= lua_gettop(L); ++i) luaL_checkstring(L, i);
	new (lua_newuserdatatagged(L, sizeof(SignalData), LUDOT_SIGNAL_TAG)) SignalData;
	lua_createtable(L, 0, 6);
	set_shared_fields(L, "Signal", SIGNAL_TOSTRING);
	luaL_getmetatable(L, SIGNAL_METHODS);
	lua_setfield(L, -2, "__index");
	lua_getfield(L, LUA_REGISTRYINDEX, SIGNAL_EQ);
	lua_setfield(L, -2, "__eq");
	lua_newtable(L);
	lua_setfield(L, -2, "slots");
	lua_setmetatable(L, -2);
	return 1;
}
} // namespace

void LuauValue::_bind_methods() {
	ClassDB::bind_method(D_METHOD("to_variant"), &LuauValue::to_variant);
}

void LuauValue::store(lua_State *L, int p_index) {
	reference = lua_ref(L, p_index);
	generation = LuauLanguage::get_singleton()->generation;
}

void LuauValue::push(lua_State *L) const {
	if (reference == LUA_NOREF || LuauLanguage::get_singleton()->generation != generation) {
		lua_pushnil(L);
		return;
	}
	lua_getref(L, reference);
}

Variant LuauValue::to_variant() const {
	auto language = LuauLanguage::get_singleton();
	MutexLock lock(language->mutex);
	if (language->state == nullptr) return Variant();
	lua_State *L = language->state;
	push(L);
	Variant result;
	LuauLanguage::try_variant(L, -1, result);
	lua_pop(L, 1);
	return result;
}

LuauValue::~LuauValue() {
	auto language = LuauLanguage::get_singleton();
	if (language == nullptr || reference == LUA_NOREF) return;
	MutexLock lock(language->mutex);
	if (language->state != nullptr && language->generation == generation) lua_unref(language->state, reference);
}

void ludot_open_signal(lua_State *L) {
	lua_setuserdatadtor(L, LUDOT_SIGNAL_TAG, [](lua_State *, void *p_data) { static_cast<SignalData *>(p_data)->~SignalData(); });
	lua_setuserdatadtor(L, LUDOT_CONNECTION_TAG, [](lua_State *, void *p_data) { static_cast<ConnectionData *>(p_data)->~ConnectionData(); });
	lua_pushcfunction(L, signal_eq, "Signal.__eq");
	lua_setfield(L, LUA_REGISTRYINDEX, SIGNAL_EQ);
	lua_pushcfunction(L, signal_tostring, "Signal.__tostring");
	lua_setfield(L, LUA_REGISTRYINDEX, SIGNAL_TOSTRING);
	lua_pushcfunction(L, connection_index, "Connection.__index");
	lua_setfield(L, LUA_REGISTRYINDEX, CONNECTION_INDEX);
	lua_pushcfunction(L, connection_tostring, "Connection.__tostring");
	lua_setfield(L, LUA_REGISTRYINDEX, CONNECTION_TOSTRING);

	// Method names follow the project's method casing, like engine methods.
	luaL_newmetatable(L, SIGNAL_METHODS);
	const struct {
		const char *name;
		lua_CFunction function;
	} methods[] = {
		{ "fire", signal_fire },
		{ "connect", signal_connect },
		{ "once", signal_once },
		{ "wait", signal_wait },
		{ "disconnect_all", signal_disconnect_all },
	};
	for (const auto &method : methods) {
		const CharString name = LuauLanguage::api_name(method.name).utf8();
		lua_pushcfunction(L, method.function, name.get_data());
		lua_setfield(L, -2, name.get_data());
	}
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	luaL_newmetatable(L, ENGINE_SIGNAL_META);
	set_shared_fields(L, "Signal", SIGNAL_TOSTRING);
	luaL_getmetatable(L, SIGNAL_METHODS);
	lua_setfield(L, -2, "__index");
	lua_getfield(L, LUA_REGISTRYINDEX, SIGNAL_EQ);
	lua_setfield(L, -2, "__eq");
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	luaL_newmetatable(L, ENGINE_CONNECTION_META);
	set_shared_fields(L, "Connection", CONNECTION_TOSTRING);
	lua_getfield(L, LUA_REGISTRYINDEX, CONNECTION_INDEX);
	lua_setfield(L, -2, "__index");
	lua_setreadonly(L, -1, true);
	lua_pop(L, 1);

	lua_createtable(L, 0, 1);
	lua_pushcfunction(L, signal_new, "Signal.new");
	lua_setfield(L, -2, "new");
	lua_setreadonly(L, -1, true);
	lua_setglobal(L, "Signal");
}

void ludot_push_engine_signal(lua_State *L, Object *p_object, const StringName &p_name) {
	auto *data = new (lua_newuserdatatagged(L, sizeof(SignalData), LUDOT_SIGNAL_TAG)) SignalData;
	data->engine = true;
	data->object = p_object->get_instance_id();
	data->name = p_name;
	luaL_getmetatable(L, ENGINE_SIGNAL_META);
	lua_setmetatable(L, -2);
}

bool ludot_signal_variant(lua_State *L, int p_index, Variant &r_value) {
	const auto *data = static_cast<const SignalData *>(lua_touserdatatagged(L, p_index, LUDOT_SIGNAL_TAG));
	if (data == nullptr) return false;
	Object *object = data->engine ? ObjectDB::get_instance(data->object) : nullptr;
	r_value = object != nullptr ? Variant(Signal(object, data->name)) : Variant();
	return true;
}

void ludot_bind_signal_fields(lua_State *L, int p_object, Object *p_owner, const HashMap<StringName, MethodInfo> &p_signals) {
	const int object = lua_absindex(L, p_object);
	for (const KeyValue<StringName, MethodInfo> &signal : p_signals) {
		const CharString field = String(signal.key).utf8();
		lua_getfield(L, object, field.get_data());
		auto *data = static_cast<SignalData *>(lua_touserdatatagged(L, -1, LUDOT_SIGNAL_TAG));
		if (data == nullptr || data->engine) {
			lua_pop(L, 1);
			continue;
		}
		const int value = lua_gettop(L);
		// Connections made while constructing, such as in __init, move to the engine signal.
		push_slots(L, value);
		const int slots = lua_gettop(L);
		const int total = lua_objlen(L, slots);
		for (int i = 1; i <= total; ++i) {
			lua_rawgeti(L, slots, i);
			const int slot = lua_gettop(L);
			lua_getfield(L, slot, "once");
			const bool once = lua_toboolean(L, -1);
			lua_rawgeti(L, slot, 1);
			Callable callable = ludot_signal_callable(L, -1);
			lua_pop(L, 2);
			p_owner->connect(signal.key, callable, once ? Object::CONNECT_ONE_SHOT : 0);
			LuauLanguage::push_variant(L, callable);
			lua_setfield(L, slot, "bound");
			push_owner_id(L, p_owner->get_instance_id());
			lua_setfield(L, slot, "object");
			lua_pushstring(L, field.get_data());
			lua_setfield(L, slot, "signal");
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		data->engine = true;
		data->object = p_owner->get_instance_id();
		data->name = signal.key;
		luaL_getmetatable(L, ENGINE_SIGNAL_META);
		lua_setmetatable(L, value);
		lua_pop(L, 1);
	}
}
