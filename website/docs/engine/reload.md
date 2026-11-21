---
source: docs/language.md
---

# Hot reload and debugging

## Hot reload

Saving a script or module reloads it in place, while the game runs.

### Objects and fields

Live instances stay the same objects, so references held elsewhere stay valid.
Fields keep their values. When you change a field's declared type:

- If the old value still fits the new type, it's kept.
- If a lossless conversion exists, it's converted. That covers strings and numbers,
  and engine value types such as `Vector2` and `Vector2i`.
- Otherwise the field resets to its new default, with a warning.

Scripts that import a changed module, and scripts that extend a changed class,
reload along with it.

Adding or removing a `Signal` field updates the script's signal list. Existing
connections on the object stay connected.

### Functions

Functions created by the old version keep working and run the new code. That
includes signal callbacks and functions stored in fields or tables. Functions are
matched by name and by position inside their enclosing function.

A function keeps its old code when:

- it is running at the moment of the reload
- the variables it captures have changed

A thread paused in `task.wait` counts as running, so it finishes with the code it
started with. Functions it calls later, and all new calls, use the new code.

If the edit has an error, the previous version keeps running.

## Debugging and profiling

### Debugger

Breakpoints, stepping, stack inspection, and expression evaluation work on the main
thread, in coroutines, and on worker threads.

Watch expressions are Luau and can read the frame's locals, upvalues, and `self`.
Scripts compile without optimization while a debugger is attached, so `const` locals
stay visible.

The debugger's globals include `[suspended tasks]`. It lists every scheduled thread
with:

- its kind: `wait`, `delay`, or `defer`
- where it's paused
- its owner
- for waits and delays, the duration and the time elapsed

### Threads and the VM

All Luau code runs on one VM, guarded by a lock. Calls from different threads run
one at a time. While one thread is stopped at a breakpoint, other threads that call
into Luau wait for it.

### Profiler

The editor's script profiler shows every Luau function, whether the engine called
it or another Luau function did. That includes `pcall` targets, metamethods, and
coroutine bodies.

- Coroutine time excludes time spent suspended.
- Code resumed by the task scheduler is profiled the same way, and waiting time is
  excluded.
- With native call profiling on, engine methods called from Luau get their own rows.

## Function hover help

Hover over a function in the native editor to see its signature, parameters, return
types, and documentation.

For documented engine methods, the card links to the matching Godot API page. The
link stays visible below the scrolling description. Luau names such as `getNode`
link to the original `get_node` page.

To document your own Luau functions, put `---` comments directly above them.
GDScript uses its normal `##` comments. Named script functions use the editor's
current text, including unsaved edits.
