extends SceneTree

var server := TCPServer.new()
var peer := PacketPeerStream.new()
var child := -1
var deadline := 0
var stopped_thread = 0
var failed := false

func fail(message: String) -> void:
    push_error(message)
    failed = true
    if child != -1 and OS.is_process_running(child):
        OS.kill(child)
    server.stop()
    quit(1)

func send(command: String, arguments: Array = []) -> void:
    peer.put_var([command, stopped_thread, arguments])

func receive(expected: String) -> Array:
    while Time.get_ticks_msec() < deadline:
        while peer.get_available_packet_count() > 0:
            var packet = peer.get_var()
            if packet is Array and packet.size() == 3 and packet[0] == expected:
                return packet
        await process_frame
    fail("Timed out waiting for debugger message: " + expected)
    return []

func _init() -> void:
    call_deferred("run")

func run() -> void:
    if server.listen(0, "127.0.0.1") != OK:
        fail("Cannot listen for the debugger test")
        return
    deadline = Time.get_ticks_msec() + 30000
    var args := PackedStringArray([
        "--headless", "--path", ProjectSettings.globalize_path("res://"),
        "--remote-debug", "tcp://127.0.0.1:" + str(server.get_local_port()),
        "--breakpoints", "res://DebuggerProbe.luau:7,res://DebuggerProbe.luau:13,res://DebuggerProbe.luau:38", "--script", "../tests/debugger_target.gd"
    ])
    child = OS.create_process(OS.get_executable_path(), args)
    if child == -1:
        fail("Cannot launch the debugger target")
        return
    while not server.is_connection_available():
        if Time.get_ticks_msec() >= deadline:
            fail("The debugger target did not connect")
            return
        await process_frame
    peer.stream_peer = server.take_connection()
    var entered := await receive("debug_enter")
    if failed: return
    stopped_thread = entered[1]
    send("get_stack_dump")
    var stack := await receive("stack_dump")
    if failed: return
    if not str(stack[2]).contains("DebuggerProbe.luau"):
        fail("The stack does not identify the Luau source")
        return
    send("get_stack_frame_vars", [0])
    var variables := await receive("stack_frame_vars")
    if failed: return
    var count: int = variables[2][0]
    var has_health := false
    var has_amount := false
    var has_global := false
    for index in count:
        var variable := await receive("stack_frame_var")
        if failed: return
        var value: Array = variable[2]
        has_health = has_health or (value[0] == "Health" and value[1] == 1 and value[3] == 7.0)
        has_amount = has_amount or (value[0] == "amount" and value[1] == 0 and value[3] == 5.0)
        has_global = has_global or value[1] == 2
    if not has_health or not has_amount or not has_global:
        fail("Luau debugger members, locals or globals are missing")
        return
    send("evaluate", ["if amount == 5 then self.Health + amount else 0", 0])
    var evaluated := await receive("evaluation_return")
    if failed: return
    if evaluated[2][3] != 12.0:
        fail("The debugger did not evaluate a native Luau expression")
        return
    send("evaluate", ["self.Cycle", 0])
    var cyclic := await receive("evaluation_return")
    if failed: return
    if not cyclic[2][3] is Dictionary:
        fail("The debugger did not inspect a cyclic table")
        return
    send("next")
    entered = await receive("debug_enter")
    if failed: return
    stopped_thread = entered[1]
    send("evaluate", ["self.Health", 0])
    evaluated = await receive("evaluation_return")
    if failed: return
    if evaluated[2][3] != 12.0:
        fail("Stepping did not reach the following Luau line")
        return
    send("continue")
    await receive("debug_exit")
    if failed: return
    entered = await receive("debug_enter")
    if failed: return
    stopped_thread = entered[1]
    send("get_stack_frame_vars", [0])
    variables = await receive("stack_frame_vars")
    if failed: return
    count = variables[2][0]
    has_health = false
    for index in count:
        var variable := await receive("stack_frame_var")
        if failed: return
        var value: Array = variable[2]
        has_health = has_health or (value[0] == "Health" and value[1] == 1 and value[3] == 12.0)
    if not has_health:
        fail("A coroutine's captured self is missing from debugger members")
        return
    send("evaluate", ["if amount == 3 then self.Health + amount else 0", 0])
    evaluated = await receive("evaluation_return")
    if failed: return
    if evaluated[2][3] != 15.0:
        fail("The debugger did not evaluate coroutine locals and upvalues: " + str(evaluated))
        return
    send("continue")
    await receive("debug_exit")
    if failed: return
    entered = await receive("debug_enter")
    if failed: return
    var main_thread = stopped_thread
    stopped_thread = entered[1]
    if stopped_thread == main_thread:
        fail("The worker thread breakpoint reported the main thread")
        return
    send("get_stack_dump")
    stack = await receive("stack_dump")
    if failed: return
    if not str(stack[2]).contains("DebuggerProbe.luau"):
        fail("The worker thread stack does not identify the Luau source")
        return
    send("evaluate", ["amount", 0])
    evaluated = await receive("evaluation_return")
    if failed: return
    if evaluated[2][3] != 4.0:
        fail("The debugger did not evaluate worker thread locals: " + str(evaluated))
        return
    var worker_thread = stopped_thread
    send("next")
    entered = await receive("debug_enter")
    if failed: return
    if entered[1] != worker_thread:
        fail("Stepping left the worker thread")
        return
    send("evaluate", ["doubled + amount", 0])
    evaluated = await receive("evaluation_return")
    if failed: return
    if evaluated[2][3] != 12.0:
        fail("Stepping on the worker thread did not reach the next line: " + str(evaluated))
        return
    send("continue")
    await receive("debug_exit")
    if failed: return
    var profiled_luau := false
    var profiled_native := false
    var profiled_nested := false
    var profiled_coroutine := false
    var profiled_pcall := false
    while not profiled_luau or not profiled_native or not profiled_nested or not profiled_coroutine or not profiled_pcall:
        var signature := await receive("servers:function_signature")
        if failed: return
        var name := str(signature[2][0])
        if name.begins_with("native/"):
            fail("Generated native bindings appear as profiled Luau functions: " + name)
            return
        profiled_luau = profiled_luau or (name.begins_with("res://DebuggerProbe.luau::") and name.ends_with("::profiled"))
        profiled_pcall = profiled_pcall or (name.begins_with("res://DebuggerProbe.luau::") and name.ends_with("::guarded"))
        profiled_coroutine = profiled_coroutine or (name.begins_with("res://DebuggerProbe.luau::") and name.ends_with("::worker"))
        profiled_nested = profiled_nested or (name.begins_with("res://DebuggerProbe.luau::") and name.ends_with("::profiledHelper"))
        profiled_native = profiled_native or name == "res://DebuggerProbe.luau::0::Node.getChildCount"
    print("LUDOT_DEBUGGER_TESTS_PASSED")
    quit()
