#!/usr/bin/env python3
"""Run desktop integration suites against the editor produced by CI."""
import argparse
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('engine', type=Path, nargs='?')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
engine = args.engine
if engine is None:
    candidates = [
        file for file in (root / 'engine/bin').glob('ludot.*.editor.*')
        if file.is_file() and not file.name.endswith(('.pdb', '.exp', '.lib', '.console.exe'))
    ]
    if len(candidates) != 1:
        parser.error('Pass the editor binary path when multiple builds are present')
    engine = candidates[0]
for suite in ('test_engine.py', 'test_debugger.py', 'test_export.py'):
    print(f'Running {suite} with {engine.name}', flush=True)
    subprocess.run([sys.executable, str(root / 'tools' / suite), str(engine.resolve())], check=True)
