extends SceneTree

func _init() -> void:
    var probe := Node.new()
    probe.set_script(load("res://DebuggerProbe.luau"))
    var result = probe.call("run", 5.0)
    var coroutine_result = probe.call("runCoroutine")
    probe.free()
    if result == 12.0 and coroutine_result == 15.0:
        print("LUDOT_DEBUGGER_TARGET_PASSED")
        quit()
    else:
        quit(1)
