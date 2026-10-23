#!/usr/bin/env python3
"""Build a desktop LSP bundle containing Python, Luau Analysis and shader tools."""
import argparse
from importlib.metadata import distribution
from pathlib import Path
import shutil
import platform
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('--build-dir', type=Path, default=Path('build'))
parser.add_argument('--output-dir', type=Path, default=Path('dist'))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
build = args.build_dir.resolve()
output = args.output_dir.resolve()
suffix = '.exe' if sys.platform == 'win32' else ''
work = build / 'lsp-package'
work.mkdir(parents=True, exist_ok=True)
command = [sys.executable, '-m', 'PyInstaller', '--noconfirm', '--clean', '--onedir', '--name', 'ludot-lsp', '--paths', str(root / 'tools'), '--distpath', str(output), '--workpath', str(work / 'objects'), '--specpath', str(work)]
for source, destination, kind in [
    (build / 'luau' / ('luau-analyze' + suffix), 'bin', 'binary'),
    (build / ('ludot-shader' + suffix), 'bin', 'binary'),
    (root / 'types/engine.api', 'share/ludot', 'data'),
    (root / 'LICENSE', 'share/licenses/ludot', 'data'),
    (root / 'luau/LICENSE.txt', 'share/licenses/luau', 'data'),
    (root / 'luau/lua_LICENSE.txt', 'share/licenses/luau', 'data'),
    (root / 'engine/LICENSE.txt', 'share/licenses/godot', 'data'),
]:
    command += ['--add-' + kind, str(source) + ':' + destination]
license_paths = [Path(sys.base_prefix) / 'LICENSE.txt', Path('/usr/share/licenses/python/LICENSE'), Path('/usr/share/doc/libstdc++6/copyright'), Path('/usr/share/licenses/gcc-libs/COPYING.RUNTIME'), Path('/usr/share/licenses/gcc-libs/COPYING3')]
for license_path in license_paths:
    if license_path.is_file():
        command += ['--add-data', str(license_path) + ':share/licenses/runtime']
for item in distribution('pyinstaller').files or []:
    if 'licenses' in item.parts and distribution('pyinstaller').locate_file(item).is_file():
        command += ['--add-data', str(distribution('pyinstaller').locate_file(item)) + ':share/licenses/pyinstaller']
command.append(str(root / 'tools/ludot-lsp'))
subprocess.run(command, check=True, cwd=root)
subprocess.run([sys.executable, str(root / 'tools/verify_lsp.py'), str(output / 'ludot-lsp' / ('ludot-lsp' + suffix))], check=True)

archive_name = 'ludot-lsp-' + platform.system().lower() + '-' + platform.machine().lower()
archive = shutil.make_archive(str(output / archive_name), 'zip' if sys.platform == 'win32' else 'gztar', output, 'ludot-lsp')
print(archive)
