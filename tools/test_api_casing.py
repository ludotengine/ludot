#!/usr/bin/env python3
"""Exercise project API casing in isolated headless engines, including editor queries."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path)
args = parser.parse_args()
styles = ('camelCase', 'PascalCase', 'snake_case')


def member(raw, style):
    words = raw.split('_')
    if style == 'snake_case':
        return raw
    result = ''.join(word.title() for word in words)
    return result[:1].lower() + result[1:] if style == 'camelCase' else result


for method_style in styles:
    for property_style in styles:
        with tempfile.TemporaryDirectory(prefix='ludot-api-casing-') as directory:
            project = Path(directory)
            method = lambda name: member(name, method_style)
            prop = lambda name: member(name, property_style)
            callback = '_' + method('ready')
            (project / 'project.ludot').write_text('config_version=5\n[luau]\napi/method_casing=' + json.dumps(method_style) + '\napi/property_casing=' + json.dumps(property_style) + '\n')
            source = f'''class ApiCaseProbe extends Node3D = {{
    export custom_value: number = 7
    export noticed: boolean = false
    function _{method("notification")}(what: number) if what == 12345 then self.noticed = true end end
    function {callback}() self.custom_value = 11 end
    function Do_Not_Change(value: number): number return self.custom_value + value end
    function check(object: any): boolean
        const body = CharacterBody3D.new()
        body.{prop('velocity')} = Vector3.new(1, 2, 3)
        const component = body.{prop('velocity')}.{prop('x')}
        const signal = body.{prop('ready')}
        const invoked = {{ value = false }}
        signal:{method('connect')}(function() invoked.value = true end)
        body:{method('emit_signal')}("ready")
        const node = Node.new()
        node:{method('set_name')}("Casing")
        const name = node.{prop('name')}
        const id = node:{method('get_instance_id')}()
        assert(self:{method('is_a')}("Node3D") and self:IsA("Node") and self:isA("Object"))
        const mode: Enum.MouseMode = Input.MOUSE_MODE_CAPTURED
        assert(Enum.MouseMode:{method('from_value')}(2) == mode and mode.{prop('name')} == "Captured")
        assert(Enum.Key.W.{prop('value')} == 87 and Vector3.AXIS_Z == 2 and #Enum:{method('get_enums')}() > 500)
        const resource = Resource.new()
        const rid = resource:{method('get_rid')}()
        const rotation = Quaternion.{method('from_euler')}(Vector3.new())
        object.custom_value = 13
        const custom = object:Do_Not_Change(1)
        body:{method('free')}()
        node:{method('free')}()
        return component == 1 and name == "Casing" and id > 0 and rotation.{prop('w')} == 1 and custom == 14 and invoked.value
    end
}}
'''
            (project / 'probe.luau').write_text(source)
            # A native object with a Luau script must expose the script's exact
            # member names, not aliases produced by the selected engine style.
            query = f'class Query extends Node = {{ function test() const body = CharacterBody3D.new() body.'
            wrong_property = 'Velocity' if prop('velocity') != 'Velocity' else 'velocity'
            wrong_method = 'GetName' if method('get_name') != 'GetName' else 'getName'
            gd = f'''extends SceneTree
func _init():
    run.call_deferred()
func run():
    var script = load("res://probe.luau")
    assert(script.get_diagnostics().is_empty(), str(script.get_diagnostics()))
    var node = Node3D.new()
    node.set_script(script)
    root.add_child(node)
    var camera = Camera3D.new()
    camera.name = "Camera"
    node.add_child(camera)
    assert(node.get("custom_value") == 11, "Selected callback spelling runs")
    node.notification(12345)
    assert(node.get("noticed"), "Object core notification callback uses the selected style")
    assert(node.call("check", node), "Selected runtime API, static methods, components, and custom members work")
    assert(node.has_method("Do_Not_Change") and not node.has_method("doNotChange"))
    var query = {json.dumps(query)}
    var labels = script.get_completions(query + String.chr(0xffff) + " end }}")
    var unique = {{}}
    for label in labels: unique[label] = true
    assert(labels.size() == unique.size(), "No duplicate native completion suggestions")
    assert(labels.count("{prop('velocity')}") == 1 and not labels.has("{wrong_property}"), str(labels))
    assert(labels.count("{method('get_name')}") == 1 and not labels.has("{wrong_method}"), str(labels))
    for alias in ["isA", "IsA", "is_a"]:
        assert(labels.has(alias) == (alias == "{method('is_a')}"), "Only the selected identity spelling is completed: " + alias)
    var hover = query + "{prop('velocity')}" + String.chr(0xffff) + " end }}"
    var help = script.get_symbol_help(hover, "{prop('velocity')}")
    assert(help.get("value_type") == "Vector3" and help.get("native_member") == "velocity", str(help))
    var call = 'const node = Node.new() node:{method('set_name')}' + String.chr(0xffff) + '("name")'
    help = script.get_symbol_help(call, "{method('set_name')}")
    assert(help.get("native_member") == "set_name" and help.get("parameters", []).size() == 1, str(help))
    var scene_query = 'class ApiCaseProbe extends Node3D = {{ function check() const camera = self:{method("get_node")}("Camera") print(camera.{prop("fov")}' + String.chr(0xffff) + ') end }}'
    help = script.get_symbol_help(scene_query, "{prop('fov')}", node)
    assert(help.get("value_type") == "number" and help.get("native_member") == "fov", "Selected scene lookup retains the concrete child type: " + str(help))
    var invalid = ClassDB.instantiate("LuauScript")
    invalid.source_code = 'return CharacterBody3D.new().{wrong_property}'
    assert(not invalid.get_diagnostics().is_empty(), "No old property alias")
    invalid.source_code = 'return Node.new():{wrong_method}()'
    assert(not invalid.get_diagnostics().is_empty(), "No old method alias")
    ProjectSettings.set_setting("luau/api/method_casing", "{('snake_case' if method_style != 'snake_case' else 'PascalCase')}")
    ProjectSettings.set_setting("luau/api/property_casing", "{('snake_case' if property_style != 'snake_case' else 'PascalCase')}")
    assert(node.call("check", node), "Project setting changes do not mix active runtime spellings")
    assert(script.get_completions(query + String.chr(0xffff) + " end }}").has("{prop('velocity')}"))
    node.queue_free()
    print("LUDOT_API_CASING_PASSED")
    quit()
'''
            (project / 'check.gd').write_text(gd)
            result = subprocess.run([str(args.engine.resolve()), '--headless', '--path', directory, '--script', 'check.gd', '--quit-after', '10'], capture_output=True, text=True, timeout=30)
            if result.returncode or 'LUDOT_API_CASING_PASSED' not in result.stdout:
                raise SystemExit(f'{method_style}/{property_style}\n{result.stdout}\n{result.stderr}')
            print(f'{method_style}/{property_style}: passed')
