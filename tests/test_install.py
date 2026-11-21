#!/usr/bin/env python3
"""Exercise the Unix installer against checksummed local release fixtures."""

import hashlib
import io
import os
from pathlib import Path
import subprocess
import sys
import tarfile
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


@unittest.skipUnless(
    sys.platform == "linux" and os.uname().machine == "x86_64",
    "Linux installer fixtures",
)
class InstallerTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.folder = Path(self.temporary.name)
        (self.folder / "bin").mkdir()
        (self.folder / "releases").mkdir()
        self.environment = {
            **os.environ,
            "HOME": str(self.folder / "home"),
            "XDG_DATA_HOME": str(self.folder / "home/.local/share"),
            "FIXTURE_RELEASE_DIR": str(self.folder / "releases"),
            "PATH": str(self.folder / "bin") + os.pathsep + os.environ["PATH"],
        }
        curl = self.folder / "bin/curl"
        curl.write_text(
            f"#!{sys.executable}\n"
            + """import os, pathlib, shutil, sys
args = sys.argv[1:]
root = pathlib.Path(os.environ['FIXTURE_RELEASE_DIR'])
if '--write-out' in args:
    print('https://github.com/ludotengine/ludot/releases/tag/' + (root / 'latest').read_text().strip(), end='')
else:
    url = next(value for value in args if value.startswith('https://'))
    output = pathlib.Path(args[args.index('--output') + 1])
    shutil.copyfile(root / url.rsplit('/', 1)[1], output)
"""
        )
        curl.chmod(0o755)

    def release(self, version, valid=True):
        asset = f"ludot-{version}-linux-x86_64"
        archive = self.folder / "releases" / (asset + ".tar.gz")
        with tarfile.open(archive, "w:gz") as bundle:
            data = f'#!/bin/sh\nprintf "{version}\\n"\n'.encode()
            info = tarfile.TarInfo(asset + "/ludot")
            info.mode = 0o755
            info.size = len(data)
            bundle.addfile(info, io.BytesIO(data))
        checksum = (
            hashlib.sha256(archive.read_bytes()).hexdigest() if valid else "0" * 64
        )
        (archive.parent / (asset + ".sha256")).write_text(
            checksum + "  " + archive.name + "\n"
        )
        (archive.parent / "latest").write_text("v" + version)

    def install(self):
        return subprocess.run(
            ["sh", str(ROOT / "install.sh")],
            env=self.environment,
            capture_output=True,
            text=True,
            timeout=15,
        )

    def test_install_and_update(self):
        for version in ["9.8.7-lu.1", "9.8.7-lu.2"]:
            self.release(version)
            result = self.install()
            self.assertEqual(result.returncode, 0, result.stderr)
            executable = self.folder / "home/.local/bin/ludot"
            self.assertTrue(executable.is_symlink())
            self.assertEqual(
                subprocess.check_output([str(executable)], text=True).strip(), version
            )
        self.assertTrue(
            (
                self.folder
                / "home/.local/share/applications/org.ludotengine.Ludot.desktop"
            ).exists()
        )

    def test_invalid_checksum_preserves_installed_version(self):
        self.release("9.8.7-lu.1")
        self.assertEqual(self.install().returncode, 0)
        self.release("9.8.7-lu.2", valid=False)
        self.assertNotEqual(self.install().returncode, 0)
        executable = self.folder / "home/.local/bin/ludot"
        self.assertEqual(
            subprocess.check_output([str(executable)], text=True).strip(), "9.8.7-lu.1"
        )


if __name__ == "__main__":
    unittest.main()
