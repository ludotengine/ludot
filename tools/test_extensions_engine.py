#!/usr/bin/env python3
"""Run language extensions through native bindings and enum Inspector metadata."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='ludot-extension-engine-') as directory:
    project = Path(directory)
    (project / 'project.godot').write_text('config_version=5\n')
    (project / 'states.luau').write_text('export enum State = { Idle, Running = 7 }\nexport enum Text = { First = "hello, world", Second = "second: value" }')
    (project / 'pipeline.luau').write_text((root / 'tests/fixtures/extensions/generic_pipeline.luau').read_text())
    (project / 'probe.luau').write_text('''import "./states" as states
import "./pipeline" as pipelineResult
enum Local = { First, Second = 9 }
class FeatureProbe extends Node = {
	export State: states.State = states.State.Running
	export Text: states.Text = states.Text.First
	export Local: Local = Local.Second
	export Mode: Input.MouseMode = Input.MOUSE_MODE_CAPTURED
	export Key: Key = Key.KEY_W
	export Axis: Vector3.Axis = Vector3.AXIS_Z
	function check(event: InputEvent): boolean
		assert(pipelineResult ~= nil and pipelineResult.Name == "near")
		assert(self:IsA("FeatureProbe") and self:isA("Node") and self:isA("Object"))
		assert(event:IsA("InputEvent") and event:isA("Resource"))
		if event:IsA("InputEventMouseMotion") then
			const movement: Vector2 = event.Relative
			assert(movement.X == 4)
		else
			error("Mouse motion did not narrow")
		end
		assert(not event:IsA("Node"))
		assert(Input.MOUSE_MODE_CAPTURED == 2)
		assert(Key.KEY_W == 87 and MouseButton.MOUSE_BUTTON_LEFT == 1)
		assert(Variant.Type.TYPE_INT == 2 and Vector3.AXIS_Z == 2)
		assert(Vector3.Enums.Axis.from(2) == Vector3.AXIS_Z)
		assert(Input.Enums.MouseMode.from(2) == Input.MOUSE_MODE_CAPTURED)
		assert(Input.Enums.MouseMode.from(999999) == nil)
		assert(Control.Enums.LayoutPreset.PRESET_TOP_LEFT == Control.PRESET_TOP_LEFT)
		const name: string = self?:getParent()?.Name ?? "No parent"
		assert(name == "No parent")
		return true
	end
}
''')
    (project / 'run.gd').write_text('''extends SceneTree
func fail(message: String) -> void:
	push_error(message)
	quit(1)
func _initialize() -> void:
	var script = load("res://probe.luau")
	if script == null or not script.can_instantiate():
		fail("Probe did not load")
		return
	var errors = script.get_diagnostics()
	if not errors.is_empty():
		fail(str(errors))
		return
	var expected = {"State": [1, 7], "Text": ["hello, world", "second: value"], "Local": [1, 9]}
	for name in expected:
		var metadata = script.get_enum_property(name)
		if metadata.get("values") != expected[name]:
			fail("Wrong enum metadata for " + name + ": " + str(metadata))
			return
	var node = Node.new()
	node.set_script(script)
	if node.get("State") != 7 or node.get("Text") != "hello, world" or node.get("Local") != 9 or node.get("Mode") != 2:
		fail("Wrong enum field defaults")
		return
	var event = InputEventMouseMotion.new()
	event.relative = Vector2(4, 5)
	if node.call("check", event) != true:
		fail("Native extension check failed")
		return
	var properties = script.get_script_property_list()
	for property in properties:
		if property.name == "State" and (property.type != TYPE_INT or property.hint != PROPERTY_HINT_ENUM):
			fail("Numeric enum did not become an Inspector dropdown")
			return
		if property.name == "Text" and property.type != TYPE_STRING:
			fail("String enum has the wrong Inspector type")
			return
	node.set("State", 1)
	node.set("Text", "second: value")
	var packed = PackedScene.new()
	if packed.pack(node) != OK or ResourceSaver.save(packed, "res://roundtrip.tscn") != OK:
		fail("Enum scene could not be saved")
		return
	var restored = load("res://roundtrip.tscn").instantiate()
	if restored.get("State") != 1 or restored.get("Text") != "second: value":
		fail("Enum scene values did not survive serialization: " + str(restored.get("State")) + ", " + str(restored.get("Text")) + "\n" + FileAccess.get_file_as_string("res://roundtrip.tscn"))
		return
	restored.free()
	node.free()
	print("LUDOT_EXTENSION_ENGINE_TESTS_PASSED")
	quit()
''')
    result = subprocess.run([str(args.engine.resolve()), '--headless', '--path', str(project), '--script', 'run.gd'], capture_output=True, text=True, timeout=60)
    print(result.stdout, end='')
    print(result.stderr, end='')
    raise SystemExit(0 if result.returncode == 0 and 'LUDOT_EXTENSION_ENGINE_TESTS_PASSED' in result.stdout else 1)
