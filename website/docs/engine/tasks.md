---
source: docs/language.md
---

# Tasks and scheduling

## Tasks

The `task` library schedules threads. It's modeled on Roblox's.

| Function | What it does |
| --- | --- |
| `task.spawn(fnOrThread, ...)` | Runs it now in a new thread, or resumes the given thread. Returns the thread. |
| `task.defer(fnOrThread, ...)` | Same as `spawn`, but runs after the current engine callback returns, later in the same frame. |
| `task.delay(seconds, fnOrThread, ...)` | Resumes it after the delay. Returns the thread. |
| `task.wait(seconds?)` | Pauses the current thread and returns how much time actually passed. With no argument, waits one frame. |
| `task.cancel(thread)` | Stops a scheduled or suspended thread. It can't cancel a thread that is running. |

```luau
class Door extends Node3D = {
	function _ready()
		task.wait(1)
		self:open()

		task.delay(5, function()
			self:close()
		end)
	end
}
```

### Waiting inside callbacks

Engine callbacks such as `_ready`, `_process`, and signal handlers each start in a
thread that can pause, so they can call `task.wait` directly. When a callback pauses,
the engine gets `nil` back right away, and the rest of the callback runs when the
thread resumes.

Module top-level code and field initializers can't pause.

### Timing

- Each VM has one scheduler. It steps at the start of every SceneTree process frame,
  before `_process`.
- Waits and delays use game time. They follow `Engine.time_scale`.
- Like a Timer, they stop while their node can't process, such as during a pause. A
  node with `PROCESS_MODE_ALWAYS` keeps waiting through a pause.
- Threads that don't belong to a node stop during a pause.
- A deferred thread that defers again runs in the next round, up to 64 rounds a
  frame.

### Ownership

A thread belongs to the object whose callback started it. Threads it spawns, and
signal handlers its code connects, belong to the same object.

When the object is freed, its scheduled threads are dropped. They never resume with
a freed `self`.

An error in a resumed thread prints with its stack trace. If a debugger is attached,
it breaks on the error.

### Cost

Starting callbacks in threads is cheap. Finished threads are reused, so an ordinary
callback allocates nothing. On Linux, a GDScript call into a Luau method measured
0.74 µs, against 0.69 µs with the previous `pcall` path.
