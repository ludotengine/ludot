#!/usr/bin/env python3
"""Run the task scheduler in a headless SceneTree: waits, ordering, cancel, freed owners, errors, pause, time scale, and hot reload."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path)
args = parser.parse_args()

WAITER = '''class Waiter extends Node = {
	function _ready()
		TaskLog.add("ready start")
		task.spawn(function(value: string) TaskLog.add("spawn " .. value) end, "now")
		task.defer(function(value: string) TaskLog.add("defer " .. value) end, "later")
		task.delay(0, function() TaskLog.add("delay") end)
		TaskLog.add("before wait")
		const started = Engine:getProcessFrames()
		const elapsed = task.wait(0.5)
		TaskLog.add("after wait " .. tostring(elapsed >= 0.5 and elapsed < 0.55) .. " " .. tostring(Engine:getProcessFrames() - started))
		const frame = task.wait()
		TaskLog.add("frame " .. tostring(frame > 0 and frame < 0.02))
	end
}
'''

CANCEL = '''class Canceller extends Node = {
	function _ready()
		const delayed = task.delay(0.05, function() TaskLog.add("cancelled delay ran") end)
		task.cancel(delayed)
		const waiting = task.spawn(function()
			task.wait(0.05)
			TaskLog.add("cancelled wait resumed")
		end)
		task.cancel(waiting)
		const deferred = task.defer(function() TaskLog.add("cancelled defer ran") end)
		task.cancel(deferred)
		const ok, message = pcall(function() task.cancel(coroutine.running()) end)
		TaskLog.add("cancel running " .. tostring(not ok and string.find(message, "running") ~= nil))
		const parked = coroutine.create(function()
			TaskLog.add("thread resumed " .. tostring(task.wait(0.05) > 0))
		end)
		task.spawn(parked)
		task.delay(0.1, function() TaskLog.add("delay done") end)
	end
}
'''

DOOMED = '''class Doomed extends Node = {
	public Name2: string = "doomed"
	function _ready()
		self.TreeExiting:connect(function() TaskLog.add("doomed exiting") end)
		task.wait(0.1)
		TaskLog.add("doomed resumed " .. self.Name2)
	end
}
'''

FAILING = '''class Failing extends Node = {
	function _ready()
		task.delay(0, function()
			self:explode()
		end)
	end
	function explode()
		error("task boom")
	end
}
'''

PAUSED = '''class Paused extends Node = {
	function _ready()
		const started = Engine:getProcessFrames()
		task.wait(0.25)
		TaskLog.add(self.Name .. " " .. tostring(Engine:getProcessFrames() - started))
	end
}
'''

SIGNAL = '''class SignalWaiter extends Node = {
	function _ready()
		const timer = Timer.new()
		timer.WaitTime = 0.05
		timer.OneShot = true
		self:addChild(timer)
		timer.Timeout:connect(function()
			TaskLog.add("signal start")
			task.wait(0.05)
			TaskLog.add("signal resumed")
		end)
		timer:start()
	end
	function _process(delta: number)
		if not self.Waited then
			self.Waited = true
			task.wait(0.05)
			TaskLog.add("process resumed")
		end
	end
	public Waited: boolean = false
}
'''

RELOAD_V1 = '''class Reloaded extends Node = {
	function _ready()
		task.wait(0.1)
		TaskLog.add("resumed v1")
	end
	function version(): string return "v1" end
}
'''

RELOAD_V2 = '''class Reloaded extends Node = {
	function _ready()
		task.wait(0.1)
		TaskLog.add("resumed v2")
	end
	function version(): string return "v2" end
}
'''

LOG = '''const entries: { string } = {}
TaskLog = {
	add = function(entry: string) table.insert(entries, entry) end,
	text = function(): string return table.concat(entries, "|") end,
}
class LogReader extends Node = {
	function read(): string return TaskLog.text() end
	function topLevelWait(): string
		const ok, message = pcall(function()
			return coroutine.wrap(function() return task.wait() end)()
		end)
		return tostring(ok)
	end
}
'''

RUN = '''extends SceneTree
var frames := 0
var reader: Node
var doomed: Node
var reloaded_script: Script
var reloaded: Node
var always: Node
var failed := false

func fail(message: String) -> void:
	push_error(message)
	failed = true
	quit(1)

func spawn(path: String, name: String = "") -> Node:
	var node := Node.new()
	node.set_script(load(path))
	if name != "":
		node.name = name
	root.add_child(node)
	return node

func log_text() -> String:
	return reader.call("read")

func _initialize() -> void:
	reader = Node.new()
	reader.set_script(load("res://log.luau"))
	root.add_child(reader)
	spawn("res://waiter.luau")
	spawn("res://cancel.luau")
	doomed = spawn("res://doomed.luau")
	spawn("res://failing.luau")
	spawn("res://signal.luau")
	reloaded_script = load("res://reloaded.luau")
	reloaded = Node.new()
	reloaded.set_script(reloaded_script)
	root.add_child(reloaded)

func _process(_delta: float) -> bool:
	if failed:
		return true
	frames += 1
	if frames == 1:
		# _ready ran after _initialize. Spawn ran inside it, defer right after it returned.
		if not log_text().begins_with("ready start|spawn now|before wait|defer later"):
			fail("Spawn, defer and wait ran in the wrong order: " + log_text())
			return true
	if frames == 2:
		# The doomed node waits 0.1 seconds. Freeing it drops its thread.
		doomed.free()
		doomed = null
		# A suspended thread keeps running the code it started with.
		reloaded_script.source_code = """%s"""
		if reloaded_script.reload(true) != OK:
			fail("The hot reload failed")
			return true
		if reloaded.call("version") != "v2":
			fail("Hot reload did not update methods")
			return true
	if frames == 40:
		var text := log_text()
		var parts := text.split("|")
		# The wait starts during frame 0, so 0.5 seconds at 60 FPS ends on the 30th or 31st step.
		if not (parts.has("after wait true 30") or parts.has("after wait true 31")):
			fail("A 0.5 second wait did not take 30 frames: " + text)
			return true
		for expected in ["delay", "frame true", "cancel running true", "thread resumed true", "delay done", "doomed exiting", "signal start", "signal resumed", "process resumed", "resumed v1"]:
			if not text.split("|").has(expected):
				fail("Missing task log entry '" + expected + "': " + text)
				return true
		for unexpected in ["cancelled delay ran", "cancelled wait resumed", "cancelled defer ran", "resumed v2"]:
			if text.contains(unexpected):
				fail("Unexpected task log entry '" + unexpected + "': " + text)
				return true
		if text.contains("doomed resumed"):
			fail("A freed node resumed its wait: " + text)
			return true
		if text.find("|delay|") > text.find("after wait"):
			fail("Delay ran after a longer wait: " + text)
			return true
		if reader.call("topLevelWait") != "true":
			fail("task.wait inside a coroutine called from a callback failed")
			return true
		# Pause: default nodes stop waiting, PROCESS_MODE_ALWAYS nodes continue.
		paused = true
		spawn("res://paused.luau", "Pausable")
		always = Node.new()
		always.process_mode = Node.PROCESS_MODE_ALWAYS
		always.name = "Always"
		always.set_script(load("res://paused.luau"))
		root.add_child(always)
	if frames == 70:
		var text := log_text()
		if not text.contains("Always 15") or text.contains("Pausable"):
			fail("Pause did not stop default waits while ALWAYS waits continued: " + text)
			return true
		paused = false
	if frames == 100:
		var text := log_text()
		if not text.contains("Pausable 45"):
			fail("A paused wait did not resume after the pause: " + text)
			return true
		# Time scale: game time runs twice as fast, so a 0.25 second wait takes 8 frames.
		Engine.time_scale = 2.0
		spawn("res://paused.luau", "Scaled")
	if frames == 120:
		var text := log_text()
		if not text.contains("Scaled 8"):
			fail("Waits did not follow Engine.time_scale: " + text)
			return true
		Engine.time_scale = 1.0
		print("LUDOT_TASK_ENGINE_TESTS_PASSED")
		return true
	return false
''' % RELOAD_V2.replace('"', '\\"')

with tempfile.TemporaryDirectory(prefix='ludot-task-engine-') as directory:
    project = Path(directory)
    (project / 'project.ludot').write_text('config_version=5\n')
    files = {
        'log.luau': LOG, 'waiter.luau': WAITER, 'cancel.luau': CANCEL, 'doomed.luau': DOOMED,
        'failing.luau': FAILING, 'paused.luau': PAUSED, 'signal.luau': SIGNAL, 'reloaded.luau': RELOAD_V1, 'run.gd': RUN,
    }
    for name, text in files.items():
        (project / name).write_text(text)
    result = subprocess.run(
        [str(args.engine.resolve()), '--headless', '--fixed-fps', '60', '--path', str(project), '--script', 'run.gd'],
        capture_output=True, text=True, timeout=120,
    )
    print(result.stdout, end='')
    print(result.stderr, end='')
    output = result.stdout + result.stderr
    # The failing task reports its message and a stack trace through the failing method.
    reported = 'task boom' in output and 'stack traceback' in output and 'failing.luau:8' in output
    if not reported:
        print('The failing task did not report its error with a stack trace')
    clean = 'doomed resumed' not in output and 'attempt to index' not in output
    raise SystemExit(0 if result.returncode == 0 and 'LUDOT_TASK_ENGINE_TESTS_PASSED' in result.stdout and reported and clean else 1)
