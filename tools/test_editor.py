#!/usr/bin/env python3
"""Exercise Luau's registered highlighter, completion icons and script templates in the editor."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path)
parser.add_argument('--screenshots', type=Path, help='Run the visible editor and save rendered hover cards')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='ludot-editor-tests-') as directory:
    project = Path(directory)
    plugin = project / 'addons/editor_test'
    plugin.mkdir(parents=True)
    (plugin / 'plugin.cfg').write_text('[plugin]\nname="Editor regression"\ndescription=""\nauthor="Ludot"\nversion="1"\nscript="plugin.gd"\n')
    (plugin / 'plugin.gd').write_text((root / 'tests/editor_integration.gd').read_text())
    (project / 'project.godot').write_text('config_version=5\n[application]\nconfig/name="Ludot editor tests"\n[editor_plugins]\nenabled=PackedStringArray("res://addons/editor_test/plugin.cfg")\n[rendering]\nrenderer/rendering_method="gl_compatibility"\n')
    (project / 'Player.luau').write_text('class Player extends Node = {\n    export Speed: number = 1\n    function _ready()\n        print("Ready!")\n    end\n}\n')
    (project / 'Hover.gd').write_text('extends Node\n## Adds two numbers.\nfunc add(a: int, b: int = 2) -> int:\n    return a + b\nfunc _ready() -> void:\n    get_node(".")\n    add(1, 2)\n')
    (project / 'NodeNode.luau').write_text('class NodeNode extends Node3D = { export Calls: number = 0 function Start(): number self.Calls += 1 return self.Calls end function Add(amount: number): number return amount + 1 end }')
    (project / 'main.tscn').write_text('[gd_scene load_steps=3 format=3]\n[ext_resource type="Script" path="res://NodeNode.luau" id="1"]\n[ext_resource type="Script" path="res://Player.luau" id="2"]\n[node name="NodeNode" type="Node3D"]\nscript = ExtResource("1")\n[node name="Node3D" type="Node3D" parent="."]\nscript = ExtResource("2")\n')
    cache = project / '.godot/editor'
    cache.mkdir(parents=True)
    (cache / 'script_editor_cache.cfg').write_text('[res://Player.luau]\nstate={"syntax_highlighter": "Plain Text"}\n')
    command = [str(args.engine.resolve()), '--editor', '--path', str(project)]
    env = os.environ.copy()
    if args.screenshots:
        args.screenshots.mkdir(parents=True, exist_ok=True)
        env['LUDOT_HOVER_SCREENSHOTS'] = str(args.screenshots.resolve())
    else:
        env.pop('LUDOT_HOVER_SCREENSHOTS', None)
        command.append('--headless')
    result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=60)
    print(result.stdout, end='')
    print(result.stderr, end='')
    raise SystemExit(0 if result.returncode == 0 and 'LUDOT_EDITOR_TESTS_PASSED' in result.stdout else 1)
