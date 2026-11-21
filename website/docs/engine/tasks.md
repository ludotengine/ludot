---
source: docs/language.md
---

# Tasks and scheduling

## Tasks

The `task` library schedules threads, modeled on Roblox's:

| Function | Behavior |
| --- | --- |
| `task.spawn(fnOrThread, ...)` | Runs it now in a new thread, or resumes the given thread, and returns the thread. |
| `task.defer(fnOrThread, ...)` | Same, but runs it after the current engine callback returns, later in the same frame. |
| `task.delay(seconds, fnOrThread, ...)` | Resumes it after the delay and returns the thread. |
| `task.wait(seconds?)` | Yields the current thread and returns the time that actually passed. With no argument, waits one frame. |
| `task.cancel(thread)` | Stops a scheduled or suspended thread. It cannot cancel a running thread. |

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

Engine callbacks, such as `_ready`, `_process`, and signal handlers, start in a
resumable thread, so they can call `task.wait` directly. When a callback yields, the
engine gets `nil` back and the rest of the callback runs when the thread resumes.
Module top-level code and field initializers cannot yield.

Each VM has one scheduler, stepped at the start of every SceneTree process frame,
before `_process`. Waits and delays use game time: they follow `Engine.time_scale`
and stop like a Timer while their node cannot process, such as during a pause. A
node with `PROCESS_MODE_ALWAYS` keeps waiting through a pause. Threads with no node
stop during a pause. Deferred threads that defer again run in the next round, for at
most 64 rounds a frame.

A thread belongs to the object whose callback started it. Threads it spawns, and
signal handlers connected from its code, belong to the same object. When the object
is freed, its scheduled threads are dropped, never resumed with a freed `self`. An
error in a resumed thread is printed with its stack trace, and the debugger breaks on
it when one is attached.

Starting callbacks in threads costs little. Finished threads are reused, so a plain
callback allocates nothing. On Linux, a GDScript call to a Luau method measured
0.74 µs, against 0.69 µs with the previous `pcall` path.
