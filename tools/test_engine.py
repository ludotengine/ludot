#!/usr/bin/env python3
import argparse
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('engine', type=Path, nargs='?')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
engine = args.engine
if engine is None:
    candidates = [file for file in (root / 'engine/bin').glob('ludot.*') if file.is_file() and not file.name.endswith(('.pdb', '.exp', '.lib', '.console.exe'))]
    if len(candidates) != 1:
        parser.error('Pass the editor binary path when multiple builds are present')
    engine = candidates[0]
result = subprocess.run([str(engine.resolve()), '--headless', '--path', str(root / 'examples'), '--script', '../tests/engine_integration.gd'], capture_output=True, text=True, timeout=60)
sys.stdout.write(result.stdout)
sys.stderr.write(result.stderr)
sys.exit(0 if result.returncode == 0 and 'LUDOT_ENGINE_TESTS_PASSED' in result.stdout else 1)
