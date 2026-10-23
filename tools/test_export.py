#!/usr/bin/env python3
"""Check native Luau resources, shaders, inheritance and aliases inside an export."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('engine', type=Path)
parser.add_argument('--template', type=Path, help='Run the exported pack with this export template instead of the editor')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
engine = str(args.engine.resolve())
with tempfile.TemporaryDirectory(prefix='ludot-export-') as directory:
    pack = Path(directory) / 'game.pck'
    export = subprocess.run([engine, '--headless', '--editor', '--path', str(root / 'examples'), '--export-pack', 'Desktop test', str(pack)], capture_output=True, text=True, timeout=120)
    if export.returncode != 0 or not pack.is_file():
        raise SystemExit(export.stdout + export.stderr)
    if args.template:
        # Release templates ignore --main-pack, --script, and scene arguments. Ship the pack beside
        # the executable and select the smoke scene through override.cfg, as a deployed game would.
        runner = Path(directory) / ('game' + args.template.suffix)
        shutil.copy2(args.template, runner)
        (Path(directory) / 'override.cfg').write_text('[application]\nrun/main_scene="res://ExportSmoke.tscn"\n', encoding='utf-8')
        command = [str(runner), '--headless']
    else:
        command = [engine, '--headless', '--main-pack', str(pack), 'res://ExportSmoke.tscn']
    run = subprocess.run(command, capture_output=True, text=True, timeout=30, cwd=directory)
    print(run.stdout, end='')
    if run.returncode != 0 or 'LUDOT_EXPORT_TESTS_PASSED' not in run.stdout or 'SCRIPT ERROR:' in run.stderr:
        raise SystemExit(run.stdout + run.stderr)
