#!/usr/bin/env python3
import argparse
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('engine', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
result = subprocess.run([str(args.engine.resolve()), '--headless', '--path', str(root / 'examples'), '--script', '../tests/debugger_host.gd'], capture_output=True, text=True, timeout=45)
sys.stdout.write(result.stdout)
sys.stderr.write(result.stderr)
sys.exit(0 if result.returncode == 0 and 'LUDOT_DEBUGGER_TESTS_PASSED' in result.stdout else 1)
