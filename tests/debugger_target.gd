extends SceneTree

var probe: Node
var frames := 0

func _init() -> void:
    probe = Node.new()
    probe.set_script(load("res://DebuggerProbe.luau"))
    root.add_child.call_deferred(probe)
    probe.call("startTasks")
    var result = probe.call("run", 5.0)
    var coroutine_result = probe.call("runCoroutine")
    if result != 12.0 or coroutine_result != 15.0:
        probe.free()
        quit(1)
        return
    var worker := Thread.new()
    worker.start(func(): return probe.call("threaded", 4.0))
    var threaded_result = worker.wait_to_finish()
    if threaded_result != 8.0:
        probe.free()
        quit(1)
        return
    print("LUDOT_DEBUGGER_TARGET_PASSED")
    # Native call timing is the second option, after the frame function limit.
    EngineDebugger.profiler_enable("servers", true, [16, true])

func _process(_delta: float) -> bool:
    frames += 1
    if frames > 30:
        return true
    probe.call("profiled")
    if frames == 30:
        probe.queue_free()
    return false
