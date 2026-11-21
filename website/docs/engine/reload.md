---
source: docs/language.md
---

# Hot reload and debugging

## Hot reload

Saving a script or module reloads it in place. Live instances keep their identity, so
references held elsewhere stay valid. Fields keep their values. When a field's
declared type changes, the value is kept if it still fits the new type, converted when
a lossless conversion exists (between strings and numbers, or between engine value
types such as `Vector2` and `Vector2i`), and otherwise reset to the new default with a
warning. Scripts that import a changed module, and scripts that extend a changed
class, reload with it.

Functions created by the previous version keep working after a reload and run the
new code. That includes callbacks connected to signals and functions stored in
fields or tables. Adding or removing a `Signal` field updates the script's signal
list; existing connections on the object stay. Functions match by name and position within their enclosing
function. A function keeps its old code while it is running, or if the variables it
captures changed. A failed edit leaves the previous version running. A thread
suspended in `task.wait` is still running, so it finishes with the code it started
with. Functions it calls later, and new calls, use the new code.

## Debugging and profiling

Breakpoints, stepping, stack inspection, and expression evaluation work on the main
thread, in coroutines, and on worker threads. Expressions are Luau and can read the
frame's locals, upvalues, and `self`. Constant locals stay inspectable because scripts
compile without optimization while a debugger is attached.

All Luau code runs on one VM, guarded by a lock. Calls from different threads run one
at a time, so while one thread is stopped at a breakpoint, other threads that call
into Luau wait for it.

The editor's scripts profiler shows every Luau function, whether the engine called it
or another Luau function did, including `pcall` targets, metamethods, and coroutine
bodies. Coroutine time excludes the time spent suspended. With native call profiling
on, engine methods called from Luau get their own rows. Code resumed by the task
scheduler is profiled the same way, and time spent waiting is excluded.

The debugger's globals include `[suspended tasks]`, which lists every scheduled
thread with its kind (`wait`, `delay`, or `defer`), where it is suspended, its owner,
and for waits and delays the duration and elapsed time.

## Function hover help

Hover over a function in the native editor to see its signature, parameters, return
types, and documentation. Documented engine methods offer a link to the matching
Godot API page. The link stays visible below the scrolling description. Luau
binding names such as `getNode` link to the original `get_node` documentation.

Put adjacent `---` comments above Luau functions to describe them in the card.
GDScript uses its normal `##` documentation comments. Named script functions use
the current editor source, including edits that have not been saved.
