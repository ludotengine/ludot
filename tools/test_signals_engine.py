#!/usr/bin/env python3
"""Run the Signal library in a headless SceneTree: Luau signals, engine signals, and script
signal fields seen by GDScript, scenes, inheritance, and hot reload."""
import argparse
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path)
args = parser.parse_args()

# The usage cases from tests/fixtures/signals/cases.luau, run as a module. A failing
# handler is reported by the engine, so the log has no line for it.
CASES = (ROOT / 'tests/fixtures/signals/cases.luau').read_text().replace(
    'print(table.concat(log, "\\n"))', 'return table.concat(log, "|")'
)
CASES_EXPECTED = '|'.join([
    'saved', 'health 100->75', 'health 10->5', 'inferred 15', 'tick 1', 'connected true', 'connected false',
    'self removing 3', 'once 5', 'waited level1 42', 'debug 3', 'forwarded hi', 'saved', 'count', 'count',
    'after error', 'alice died', 'bob 100->70', 'separate true',
])

FIGHTER = '''class Fighter extends Node = {
	public Died = signal with ()
	public HealthChanged = signal with (previous: number, current: number)
	public Hit = signal with (target: Node)
	private Internal = signal with (code: number)
	public Health: number = 100
	public Log: { string } = {}

	function _ready()
		self.Died:connect(function()
			table.insert(self.Log, "luau died")
		end)
		self.HealthChanged:connect(function(previous: number, current: number)
			table.insert(self.Log, `luau health {previous}->{current}`)
		end)
	end

	function damage(amount: number)
		const previous = self.Health
		self.Health -= amount
		self.HealthChanged:fire(previous, self.Health)
		if self.Health <= 0 then
			self.Died:fire()
		end
	end

	function text(): string
		return table.concat(self.Log, "|")
	end
}
'''

HERO = '''import "./fighter" as Fighter
class Hero extends Fighter = {
	public Leveled = signal with (level: number)

	function levelUp(level: number)
		self.Leveled:fire(level)
	end
}
return Hero
'''

HUB = '''const Hub = {
	Ping = signal with (),
	Count = 0,
}
return Hub
'''

LISTENER = '''import "./hub" as Hub
class Listener extends Node = {
	function listen(target: Node)
		const probe: any = target
		probe.Ping:connect(function()
			probe:note("listener heard")
		end)
		Hub.Ping:connect(function()
			Hub.Count += 1
		end)
	end
}
'''

PROBE = '''import "./hub" as Hub
class Probe extends Node = {
	public Ping = signal with ()
	public Pair = signal with (name: string, size: number)
	public Gone = signal with ()
	public Log: { string } = {}

	function note(entry: string)
		table.insert(self.Log, entry)
	end

	function text(): string
		return table.concat(self.Log, "|")
	end

	function hubCount(): number
		Hub.Ping:fire()
		return Hub.Count
	end

	function run()
		-- Engine signals are the same Signal type.
		const exiting = self.TreeExiting:connect(function() end)
		self:note(`exiting {typeof(self.TreeExiting)} {typeof(exiting)} {exiting.Connected}`)
		exiting:disconnect()
		exiting:disconnect()
		self:note(`exiting after {exiting.Connected}`)
		self.ChildEnteredTree:connect(function(node: Node)
			self:note(`child {node.Name}`)
		end)
		const child = Node.new()
		child.Name = "Kid"
		self:addChild(child)

		-- once runs a single time.
		local count = 0
		self.Ping:once(function()
			count += 1
		end)
		self.Ping:fire()
		self.Ping:fire()
		self:note(`once {count}`)

		-- wait inside a task resumes with the fired arguments.
		task.spawn(function()
			const name, size = self.Pair:wait()
			self:note(`waited {name} {size}`)
		end)
		self.Pair:fire("level", 3)

		-- A yielding handler continues on a later frame. A failing handler does not stop the others.
		const pair = self.Pair:connect(function(name: string)
			task.wait(0.05)
			self:note(`yielded {name}`)
		end)
		self.Pair:connect(function()
			error("signal boom")
		end)
		self.Pair:connect(function(name: string, size: number)
			self:note(`after boom {name} {size}`)
		end)
		self.Pair:fire("late", 9)
		pair:disconnect()
		self:note(`pair {pair.Connected}`)

		-- wait outside a yieldable thread, such as inside a sort comparator, is an error.
		const ok, message = pcall(function()
			table.sort({ 2, 1 }, function(): boolean
				self.Pair:wait()
				return false
			end)
		end)
		self:note(`wait error {not ok and string.find(tostring(message), "cannot yield") ~= nil}`)
	end

	function clearPing()
		self.Ping:disconnectAll()
		self.Ping:fire()
	end
}
'''

RECEIVER = '''extends Node
var heard := []
func on_died() -> void:
	heard.append("scene died")
'''

RUN = '''extends SceneTree
var frames := 0
var failed := false
var fighter: Node
var hero: Node
var probe: Node
var listener: Node
var probe_script: Script
var gd_log := []

func fail(message: String) -> void:
	push_error(message)
	failed = true
	quit(1)

func expect(condition: bool, message: String) -> bool:
	if not condition:
		fail(message)
	return condition

func make(path: String, name: String) -> Node:
	var node := Node.new()
	node.name = name
	node.set_script(load(path))
	root.add_child(node)
	return node

func on_health(previous, current) -> void:
	gd_log.append("gd health %d->%d" % [previous, current])

func on_died() -> void:
	gd_log.append("gd died")

func on_leveled(level) -> void:
	gd_log.append("gd level %d" % level)

func on_ping() -> void:
	gd_log.append("gd ping")

func on_extra(value) -> void:
	gd_log.append("gd extra %s" % value)

func signal_named(script: Script, name: String) -> Dictionary:
	for signal_info in script.get_script_signal_list():
		if signal_info.name == name:
			return signal_info
	return {}

func _initialize() -> void:
	var cases := Node.new()
	cases.set_script(load("res://cases_reader.luau"))
	root.add_child(cases)
	var text: String = cases.call("read")
	if not expect(text == "CASES_EXPECTED", "The usage cases produced the wrong log: " + text):
		return
	fighter = make("res://fighter.luau", "Fighter")
	hero = make("res://hero.luau", "Hero")
	probe = make("res://probe.luau", "Probe")
	listener = make("res://listener.luau", "Listener")
	probe_script = probe.get_script()

func _process(_delta: float) -> bool:
	if failed:
		return true
	frames += 1
	if frames == 1:
		var script: Script = fighter.get_script()
		# The Node dock lists Signal fields with their declared names and types.
		var health := signal_named(script, "HealthChanged")
		if not expect(not health.is_empty() and health.args.size() == 2, "HealthChanged is missing from the script signal list"):
			return true
		if not expect(health.args[0].name == "previous" and health.args[1].name == "current" and health.args[0].type == TYPE_FLOAT, "HealthChanged arguments are wrong: " + str(health.args)):
			return true
		var hit := signal_named(script, "Hit")
		if not expect(hit.args[0].name == "target" and hit.args[0].type == TYPE_OBJECT and hit.args[0].class_name == "Node", "Hit arguments are wrong: " + str(hit.args)):
			return true
		if not expect(fighter.has_signal("Died") and script.has_script_signal("Died"), "Died is not a script signal"):
			return true
		if not expect(not fighter.has_signal("Internal") and not script.has_script_signal("Internal"), "Private signal leaked to Godot script signals"):
			return true
		for property in fighter.get_property_list():
			if property.name == "Died" or property.name == "Internal":
				fail("A Signal field is listed as a property")
				return true
		# GDScript handlers receive Luau fires, and Luau handlers receive GDScript emits.
		fighter.connect("HealthChanged", on_health)
		fighter.connect("Died", on_died)
		fighter.call("damage", 25.0)
		fighter.emit_signal("HealthChanged", 75.0, 60.0)
		# The emit only runs handlers, so Health is still 75.
		fighter.call("damage", 75.0)
		var luau_text: String = fighter.call("text")
		if not expect(gd_log == ["gd health 100->75", "gd health 75->60", "gd health 75->0", "gd died"], "GDScript handlers missed Luau fires: " + str(gd_log)):
			return true
		if not expect(luau_text == "luau health 100->75|luau health 75->60|luau health 75->0|luau died", "Luau handlers missed GDScript emits: " + luau_text):
			return true
		gd_log.clear()
		# Inheritance: base signals plus the derived class's own.
		var hero_script: Script = hero.get_script()
		if not expect(not signal_named(hero_script, "Died").is_empty() and signal_named(hero_script, "Leveled").args[0].name == "level", "Hero is missing inherited or own signals"):
			return true
		hero.connect("Died", on_died)
		hero.connect("Leveled", on_leveled)
		hero.call("damage", 100.0)
		hero.call("levelUp", 2.0)
		if not expect(gd_log == ["gd died", "gd level 2"], "Inherited signals did not reach GDScript: " + str(gd_log)):
			return true
		gd_log.clear()
		# A connection saved in a scene reaches its target after loading.
		var scene_root := Node.new()
		scene_root.name = "SceneRoot"
		scene_root.set_script(load("res://receiver.gd"))
		var saved_fighter := Node.new()
		saved_fighter.name = "SavedFighter"
		saved_fighter.set_script(load("res://fighter.luau"))
		scene_root.add_child(saved_fighter)
		saved_fighter.owner = scene_root
		saved_fighter.connect("Died", Callable(scene_root, "on_died"), CONNECT_PERSIST)
		var packed := PackedScene.new()
		packed.pack(scene_root)
		ResourceSaver.save(packed, "res://saved.tscn")
		scene_root.free()
		var saved_text := FileAccess.get_file_as_string("res://saved.tscn")
		if not expect(saved_text.contains('[connection signal="Died" from="SavedFighter" to="." method="on_died"'), "The scene did not save the connection: " + saved_text):
			return true
		var loaded: Node = load("res://saved.tscn").instantiate()
		root.add_child(loaded)
		loaded.get_node("SavedFighter").call("damage", 100.0)
		if not expect(loaded.heard == ["scene died"], "The saved connection did not run: " + str(loaded.heard)):
			return true
		# Probe: engine signals, once, wait, failing and yielding handlers.
		probe.connect("Ping", on_ping)
		listener.call("listen", probe)
		probe.call("run")
	if frames == 2:
		var text: String = probe.call("text")
		for expected in ["exiting Signal Connection true", "exiting after false", "child Kid", "once 1", "waited level 3", "after boom late 9", "pair false", "wait error true"]:
			if not expect(text.split("|").has(expected), "Missing probe entry '" + expected + "': " + text):
				return true
		# The listener's connections end when it is freed, for engine and Luau signals.
		if not expect(probe.call("hubCount") == 1, "The hub signal did not reach the listener"):
			return true
		var heard_before := String(probe.call("text")).count("listener heard")
		listener.free()
		listener = null
		probe.emit_signal("Ping")
		if not expect(String(probe.call("text")).count("listener heard") == heard_before, "A freed listener still handled an engine signal: " + probe.call("text")):
			return true
		if not expect(probe.call("hubCount") == 1, "A freed listener still handled a Luau signal"):
			return true
		# disconnectAll leaves GDScript connections in place.
		gd_log.clear()
		probe.call("clearPing")
		if not expect(gd_log == ["gd ping"], "disconnectAll removed a GDScript connection: " + str(gd_log)):
			return true
	if frames == 10:
		var text: String = probe.call("text")
		if not expect(text.contains("yielded late"), "A yielding handler did not resume: " + text):
			return true
		# Hot reload adds Extra and removes Gone while connections exist.
		probe.connect("Gone", on_ping)
		var source: String = probe_script.source_code
		probe_script.source_code = source.replace('public Gone = signal with ()', 'public Extra = signal with (value: string)')
		if not expect(probe_script.reload(true) == OK, "The probe did not reload"):
			return true
		if not expect(probe_script.has_script_signal("Extra") and not probe_script.has_script_signal("Gone"), "The reload did not update the signal list"):
			return true
		gd_log.clear()
		probe.connect("Extra", on_extra)
		probe.emit_signal("Ping")
		probe.emit_signal("Extra", "new")
		if not expect(gd_log == ["gd ping", "gd extra new"], "Connections did not survive the reload: " + str(gd_log)):
			return true
		print("LUDOT_SIGNAL_ENGINE_TESTS_PASSED")
		return true
	return false
'''.replace('CASES_EXPECTED', CASES_EXPECTED)

CASES_READER = '''const text = require("./cases")
class CasesReader extends Node = {
	function read(): string
		return text
	end
}
'''

with tempfile.TemporaryDirectory(prefix='ludot-signal-engine-') as directory:
    project = Path(directory)
    (project / 'project.ludot').write_text('config_version=5\n')
    files = {
        'cases.luau': CASES, 'cases_reader.luau': CASES_READER, 'fighter.luau': FIGHTER, 'hero.luau': HERO,
        'hub.luau': HUB, 'listener.luau': LISTENER, 'probe.luau': PROBE, 'receiver.gd': RECEIVER, 'run.gd': RUN,
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
    # Both failing handlers report their message with a stack trace and the rest still ran.
    reported = all(message in output for message in ('boom', 'signal boom', 'stack traceback'))
    if not reported:
        print('A failing signal handler was not reported with a stack trace')
    clean = not re.search(r'Error calling from signal|attempt to index', output)
    raise SystemExit(0 if result.returncode == 0 and 'LUDOT_SIGNAL_ENGINE_TESTS_PASSED' in result.stdout and reported and clean else 1)
