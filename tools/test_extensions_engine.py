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
	export Mode: Enum.MouseMode = Enum.MouseMode.Captured
	export Key: Enum.Key = Enum.Key.W
	export Axis: Enum.Axis = Enum.Axis.Z
	function modeName(): string
		return self.Mode.Name
	end
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
		assert(Input.MOUSE_MODE_CAPTURED == Enum.MouseMode.Captured)
		assert(Enum.Key.W.Value == 87 and Enum.MouseButton.Left.Value == 1)
		assert(Enum.VariantType.Int.Value == 2 and Vector3.AXIS_Z == 2)
		assert(Enum.Axis:fromValue(2) == Enum.Axis.Z)
		assert(Enum.MouseMode:fromValue(2) == Input.MOUSE_MODE_CAPTURED)
		assert(Enum.MouseMode:fromValue(999999) == nil)
		assert(Enum.MouseMode:fromName("Captured") == Enum.MouseMode.Captured and Enum.MouseMode:fromName("Nope") == nil)
		assert(Enum.LayoutPreset.TopLeft == Control.PRESET_TOP_LEFT)
		assert(Enum.MouseMode.Captured.EnumType == Enum.MouseMode and Enum.MouseMode.Captured.EnumType.Captured == Enum.MouseMode.Captured)
		assert(Enum.MouseMode.Captured.Name == "Captured" and Enum.MouseMode.Captured.Value == 2)
		assert(#Enum.MouseMode:getEnumItems() == 6 and #Enum:getEnums() > 500)
		assert(typeof(Enum.MouseMode.Captured) == "EnumItem" and typeof(Enum.MouseMode) == "Enum")
		assert(tostring(Enum.MouseMode.Captured) == "Enum.MouseMode.Captured" and tostring(Enum.MouseMode) == "Enum.MouseMode")
		assert(not pcall(function() return (Enum :: any).Local end))
		assert(not pcall(function() return (Enum.MouseMode :: any).Nope end))
		assert(self.Mode == Enum.MouseMode.Captured)
		assert(self.ProcessMode == Enum.NodeProcessMode.Inherit and self:getProcessMode() == Enum.NodeProcessMode.Inherit)
		const light = OmniLight3D.new()
		light:setParam(Enum.Light3DParam.Energy, 3)
		assert(light:getParam(Enum.Light3DParam.Energy) == 3)
		light.ShadowEnabled = true
		light:free()
		local seen = 0
		for name, item in Enum.MouseMode do
			assert(Enum.MouseMode[name] == item)
			seen += 1
		end
		assert(seen == 6)
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
	var expected = {"State": [1, 7], "Text": ["hello, world", "second: value"], "Local": [1, 9], "Mode": [0, 1, 2, 3, 4, 5]}
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
	node.set("Mode", 3)
	if node.call("modeName") != "Confined" or node.get("Mode") != 3:
		fail("An Inspector value did not become the matching EnumItem")
		return
	node.set("Mode", 2)
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
