// Ludot task scheduler, modeled on Roblox's task library. Licensed under the MIT license.
#include "luau_script.h"

#include "core/debugger/engine_debugger.h"
#include "core/debugger/script_debugger.h"
#include "core/os/os.h"
#include "scene/main/node.h"
#include "scene/main/scene_tree.h"
#include "lualib.h"

#include <algorithm>

namespace {
// Leaves the target thread at index 1 of L and moves the function, if any, and the
// arguments from p_first onwards onto it. Returns the argument count for lua_resume.
lua_State *prepare_thread(lua_State *L, int p_first, int &r_arguments) {
	auto language = LuauLanguage::get_singleton();
	const int top = lua_gettop(L);
	lua_State *thread = nullptr;
	if (lua_isfunction(L, p_first)) {
		thread = lua_newthread(L);
		lua_insert(L, p_first);
		lua_xmove(L, thread, top - p_first + 1);
		r_arguments = top - p_first;
	} else if (lua_isthread(L, p_first)) {
		thread = lua_tothread(L, p_first);
		const int status = lua_costatus(L, thread);
		if (status == LUA_CORUN || status == LUA_CONOR) luaL_error(L, "Cannot schedule a running thread");
		if (status == LUA_COFIN || status == LUA_COERR) luaL_error(L, "Cannot schedule a dead thread");
		language->unschedule_task(thread);
		lua_xmove(L, thread, top - p_first);
		r_arguments = top - p_first;
	} else {
		luaL_typeerror(L, p_first, "function or thread");
	}
	if (p_first != 1) lua_replace(L, 1);
	lua_settop(L, 1);
	return thread;
}
int task_spawn(lua_State *L) {
	auto language = LuauLanguage::get_singleton();
	int arguments = 0;
	lua_State *thread = prepare_thread(L, 1, arguments);
	if (language->resume_thread(thread, L, arguments, language->current_task_owner()) == LUA_OK) lua_settop(thread, 0);
	return 1;
}
int task_defer(lua_State *L) {
	int arguments = 0;
	lua_State *thread = prepare_thread(L, 1, arguments);
	LuauLanguage::get_singleton()->schedule_task(thread, LuauLanguage::Task::DEFER, 0.0, arguments);
	return 1;
}
int task_delay(lua_State *L) {
	const double seconds = MAX(0.0, luaL_optnumber(L, 1, 0.0));
	int arguments = 0;
	lua_State *thread = prepare_thread(L, 2, arguments);
	LuauLanguage::get_singleton()->schedule_task(thread, LuauLanguage::Task::DELAY, seconds, arguments);
	return 1;
}
int task_wait(lua_State *L) {
	const double seconds = MAX(0.0, luaL_optnumber(L, 1, 0.0));
	if (!lua_isyieldable(L)) luaL_error(L, "task.wait cannot yield here. Call it from a script callback, a signal handler, or a task thread");
	LuauLanguage::get_singleton()->schedule_task(L, LuauLanguage::Task::WAIT, seconds, 0);
	return lua_yield(L, 0);
}
int task_cancel(lua_State *L) {
	luaL_checktype(L, 1, LUA_TTHREAD);
	lua_State *thread = lua_tothread(L, 1);
	const int status = lua_costatus(L, thread);
	if (status == LUA_CORUN || status == LUA_CONOR) luaL_error(L, "task.cancel cannot cancel a running thread");
	LuauLanguage::get_singleton()->drop_task(thread);
	lua_resetthread(thread);
	return 0;
}
// The first Luau frame of a suspended thread, for debugger listings.
String thread_location(lua_State *p_thread) {
	lua_Debug info = {};
	for (int level = 0; lua_getinfo(p_thread, level, "sl", &info); ++level) {
		if (info.currentline > 0 && info.source != nullptr) return String::utf8(info.source).trim_prefix("@") + ":" + itos(info.currentline);
	}
	return "<not started>";
}
} // namespace

void ludot_open_task(lua_State *L) {
	static const luaL_Reg functions[] = {
		{ "spawn", task_spawn },
		{ "defer", task_defer },
		{ "delay", task_delay },
		{ "wait", task_wait },
		{ "cancel", task_cancel },
		{ nullptr, nullptr },
	};
	lua_newtable(L);
	luaL_register(L, nullptr, functions);
	lua_setreadonly(L, -1, true);
	lua_setglobal(L, "task");
}

int LuauLanguage::resume_thread(lua_State *p_thread, lua_State *p_from, int p_arguments, ObjectID p_owner, const String &p_where) {
	++call_depth;
	task_owners.push_back(p_owner);
	running_threads.insert(p_thread);
	const int status = lua_resume(p_thread, p_from, p_arguments);
	running_threads.erase(p_thread);
	task_owners.resize(task_owners.size() - 1);
	--call_depth;
	if (status != LUA_OK && status != LUA_YIELD) {
		// Close profiled frames that the error unwound without reaching RETURN.
		if (profiling) profile_unwind(p_thread, 1);
		report_thread_error(p_thread, p_where);
	}
	return status;
}

void LuauLanguage::report_thread_error(lua_State *p_thread, const String &p_where) {
	const String message = lua_isstring(p_thread, -1) ? String::utf8(lua_tostring(p_thread, -1)) : String("Luau runtime error");
	last_error = message;
	String trace = String::utf8(lua_debugtrace(p_thread)).strip_edges();
	ERR_PRINT((p_where.is_empty() ? message : p_where + ": " + message) + (trace.is_empty() ? String() : "\nstack traceback:\n" + trace));
	ScriptDebugger *debugger = EngineDebugger::get_script_debugger();
	if (debugger != nullptr && !debugger->is_ignoring_error_breaks()) {
		// The failed thread keeps its frames, so the debugger can inspect them.
		lua_State *previous = debug_state;
		debug_state = p_thread;
		debugger->debug(this, false, true);
		debug_state = previous;
	}
}

bool LuauLanguage::invoke(int p_count, ObjectID p_owner, const String &p_where) {
	lua_State *L = state;
	lua_State *thread = nullptr;
	int reference = LUA_NOREF;
	if (!idle_threads.is_empty()) {
		reference = idle_threads[idle_threads.size() - 1];
		idle_threads.resize(idle_threads.size() - 1);
		lua_getref(L, reference);
		thread = lua_tothread(L, -1);
		lua_pop(L, 1);
	} else {
		thread = lua_newthread(L);
		reference = lua_ref(L, -1);
		lua_pop(L, 1);
	}
	lua_xmove(L, thread, p_count + 1);
	const int status = resume_thread(thread, active_state, p_count, p_owner, p_where);
	if (status == LUA_OK) {
		if (lua_gettop(thread) > 0) {
			lua_pushvalue(thread, 1);
			lua_xmove(thread, L, 1);
		} else {
			lua_pushnil(L);
		}
		lua_settop(thread, 0);
		// Finished threads are reused, so a plain callback allocates no thread.
		if (idle_threads.size() < 16) idle_threads.push_back(reference);
		else lua_unref(L, reference);
	} else {
		// A yielded thread now belongs to the scheduler. A failed one is dead.
		lua_unref(L, reference);
		lua_pushnil(L);
	}
	if (call_depth == 0) run_deferred();
	return status == LUA_OK || status == LUA_YIELD;
}

void LuauLanguage::schedule_task(lua_State *p_thread, Task::Kind p_kind, double p_duration, int p_arguments) {
	unschedule_task(p_thread);
	Task task;
	lua_pushthread(p_thread);
	lua_xmove(p_thread, state, 1);
	task.ref = lua_ref(state, -1);
	lua_pop(state, 1);
	task.kind = p_kind;
	task.duration = p_duration;
	task.arguments = p_arguments;
	task.owner = current_task_owner();
	task.order = ++task_order;
	tasks.insert(p_thread, task);
}

void LuauLanguage::unschedule_task(lua_State *p_thread) {
	Task *task = tasks.getptr(p_thread);
	if (task == nullptr) return;
	if (task->arguments > 0) lua_pop(p_thread, task->arguments);
	lua_unref(state, task->ref);
	tasks.erase(p_thread);
}

void LuauLanguage::drop_task(lua_State *p_thread) {
	if (!tasks.has(p_thread)) return;
	const int reference = tasks[p_thread].ref;
	tasks.erase(p_thread);
	if (!running_threads.has(p_thread)) lua_resetthread(p_thread);
	lua_unref(state, reference);
}

void LuauLanguage::drop_owner_tasks(ObjectID p_owner) {
	if (state == nullptr || tasks.is_empty()) return;
	LocalVector<lua_State *> owned;
	for (const KeyValue<lua_State *, Task> &entry : tasks) {
		if (entry.value.owner == p_owner) owned.push_back(entry.key);
	}
	for (lua_State *thread : owned) drop_task(thread);
}

void LuauLanguage::resume_task(lua_State *p_thread) {
	Task *found = tasks.getptr(p_thread);
	if (found == nullptr) return;
	const Task task = *found;
	tasks.erase(p_thread);
	// Keep the thread alive while it runs. A wait inside it takes a new reference.
	int arguments = task.arguments;
	if (task.kind == Task::WAIT) {
		lua_pushnumber(p_thread, task.elapsed);
		arguments = 1;
	}
	const int status = resume_thread(p_thread, nullptr, arguments, task.owner);
	if (status == LUA_OK) lua_settop(p_thread, 0);
	lua_unref(state, task.ref);
}

void LuauLanguage::run_deferred() {
	if (call_depth > 0 || running_deferred || state == nullptr) return;
	running_deferred = true;
	// Deferred threads that defer again run in the next round. Bounded so a loop cannot hang the frame.
	for (int round = 0; round < 64; ++round) {
		LocalVector<Pair<uint64_t, lua_State *>> due;
		for (const KeyValue<lua_State *, Task> &entry : tasks) {
			if (entry.value.kind == Task::DEFER) due.push_back({ entry.value.order, entry.key });
		}
		if (due.is_empty()) break;
		std::sort(due.ptr(), due.ptr() + due.size(), [](const Pair<uint64_t, lua_State *> &a, const Pair<uint64_t, lua_State *> &b) { return a.first < b.first; });
		for (const auto &entry : due) {
			const Task *task = tasks.getptr(entry.second);
			if (task == nullptr || task->order != entry.first) continue;
			if (task->owner.is_valid() && ObjectDB::get_instance(task->owner) == nullptr) {
				drop_task(entry.second);
				continue;
			}
			resume_task(entry.second);
		}
	}
	running_deferred = false;
}

void LuauLanguage::step_tasks() {
	MutexLock lock(mutex);
	if (state == nullptr || tasks.is_empty()) return;
	SceneTree *tree = Object::cast_to<SceneTree>(OS::get_singleton()->get_main_loop());
	if (tree == nullptr) return;
	// Game time: SceneTree already scales the step by Engine.time_scale.
	const double delta = tree->get_process_time();
	const bool paused = tree->is_paused();
	LocalVector<Pair<uint64_t, lua_State *>> due;
	LocalVector<lua_State *> stale;
	for (KeyValue<lua_State *, Task> &entry : tasks) {
		Task &task = entry.value;
		Object *owner = task.owner.is_valid() ? ObjectDB::get_instance(task.owner) : nullptr;
		if (task.owner.is_valid() && owner == nullptr) {
			stale.push_back(entry.key);
			continue;
		}
		// A signal wait resumes when its signal fires, never on a timer.
		if (task.kind == Task::SIGNAL) continue;
		if (task.kind != Task::DEFER) {
			// Like a Timer, a wait stops while its node cannot process, such as during a pause.
			const Node *node = Object::cast_to<Node>(owner);
			const bool advances = node != nullptr && node->is_inside_tree() ? node->can_process() : !paused;
			if (!advances) continue;
			task.elapsed += delta;
			if (task.elapsed < task.duration) continue;
		}
		due.push_back({ task.order, entry.key });
	}
	for (lua_State *thread : stale) drop_task(thread);
	std::sort(due.ptr(), due.ptr() + due.size(), [](const Pair<uint64_t, lua_State *> &a, const Pair<uint64_t, lua_State *> &b) { return a.first < b.first; });
	for (const auto &entry : due) {
		const Task *task = tasks.getptr(entry.second);
		if (task == nullptr || task->order != entry.first) continue;
		resume_task(entry.second);
	}
	run_deferred();
}

void LuauLanguage::debug_get_tasks(List<String> *r_names, List<Variant> *r_values) {
	Array listed;
	for (const KeyValue<lua_State *, Task> &entry : tasks) {
		Dictionary item;
		const Task &task = entry.value;
		item["kind"] = task.kind == Task::DEFER ? "defer" : task.kind == Task::WAIT ? "wait" : task.kind == Task::SIGNAL ? "signal" : "delay";
		item["location"] = thread_location(entry.key);
		if (task.kind == Task::WAIT || task.kind == Task::DELAY) {
			item["duration"] = task.duration;
			item["elapsed"] = task.elapsed;
		}
		Object *owner = task.owner.is_valid() ? ObjectDB::get_instance(task.owner) : nullptr;
		if (owner != nullptr) item["owner"] = owner;
		listed.push_back(item);
	}
	r_names->push_back("[suspended tasks]");
	r_values->push_back(listed);
}
