#!/usr/bin/env python3
"""Package tested desktop builds into versioned GitHub Release assets."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import runpy
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def version():
    data = runpy.run_path(str(ROOT / "engine/version.py"))
    return f"{data['major']}.{data['minor']}.{data['patch']}-{data['status']}"


def package(system, destination, editor=None):
    release = version()
    architecture = "arm64" if system == "macos" else "x86_64"
    asset = f"ludot-{release}-{system}-{architecture}"
    stage = ROOT / "build/release-stage" / asset
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)
    destination.mkdir(parents=True, exist_ok=True)
    if system == "macos":
        shutil.copytree(
            ROOT / "engine/bin/ludot_macos_editor.app",
            stage / "Ludot.app",
            symlinks=True,
        )
    else:
        candidates = [
            path
            for path in (ROOT / "engine/bin").glob(
                f"ludot.{'windows' if system == 'windows' else 'linuxbsd'}.editor.x86_64*"
            )
            if path.is_file()
            and not path.name.endswith((".pdb", ".lib", ".exp", ".console.exe"))
        ]
        if editor is not None:
            candidates = [editor.resolve()]
        if len(candidates) != 1:
            raise RuntimeError(f"Expected one release editor, got {candidates}")
        shutil.copy2(
            candidates[0], stage / ("ludot.exe" if system == "windows" else "ludot")
        )
        for path in (ROOT / "engine/bin").iterdir():
            if (
                path.name.startswith(("ludot.stylua", "ludot.selene"))
                or path.suffix.lower() == ".dll"
            ):
                shutil.copy2(path, stage / path.name)
    shutil.copy2(ROOT / "logo.svg", stage / "ludot.svg")
    licenses = stage / "licenses"
    licenses.mkdir()
    for source, name in [
        ("LICENSE", "Ludot.txt"),
        ("engine/LICENSE.txt", "Godot.txt"),
        ("engine/COPYRIGHT.txt", "Godot-dependencies.txt"),
        ("luau/LICENSE.txt", "Luau.txt"),
        ("luau/lua_LICENSE.txt", "Lua.txt"),
    ]:
        shutil.copy2(ROOT / source, licenses / name)
    sha = (
        os.environ.get("GITHUB_SHA")
        or subprocess.check_output(
            ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True
        ).strip()
    )
    (stage / "release.json").write_text(
        json.dumps(
            {
                "version": release,
                "commit": sha,
                "platform": system,
                "architecture": architecture,
            },
            indent=2,
        )
        + "\n"
    )
    if system == "macos":
        archive = destination / (asset + ".zip")
        subprocess.run(
            [
                "ditto",
                "-c",
                "-k",
                "--sequesterRsrc",
                "--keepParent",
                str(stage),
                str(archive),
            ],
            check=True,
        )
    else:
        archive = Path(
            shutil.make_archive(
                str(destination / asset),
                "gztar" if system == "linux" else "zip",
                stage.parent,
                stage.name,
            )
        )
    products = [archive]
    if system == "macos":
        image_source = ROOT / "build/release-dmg"
        if image_source.exists():
            shutil.rmtree(image_source)
        image_source.mkdir()
        shutil.copytree(stage / "Ludot.app", image_source / "Ludot.app", symlinks=True)
        (image_source / "Applications").symlink_to("/Applications")
        image = destination / (asset + ".dmg")
        subprocess.run(
            [
                "hdiutil",
                "create",
                "-ov",
                "-volname",
                "Ludot",
                "-srcfolder",
                str(image_source),
                "-format",
                "UDZO",
                str(image),
            ],
            check=True,
        )
        products.append(image)
    if system == "windows":
        installer = destination / (asset + "-setup.exe")
        script = ROOT / "engine/misc/dist/windows/ludot.iss"
        subprocess.run(
            [
                "ISCC.exe",
                f"/DMyAppVersion={release}",
                f"/DMyAppNumericVersion={release.split('-')[0]}.{release.rsplit('.', 1)[1]}",
                f"/DSourceDirectory={stage}",
                f"/O{destination}",
                f"/F{installer.stem}",
                str(script),
            ],
            check=True,
        )
        products.append(installer)
    # Per-platform lists avoid simultaneous matrix jobs overwriting one checksum asset.
    checksums = destination / f"{asset}.sha256"
    checksums.write_text(
        "".join(
            hashlib.file_digest(path.open("rb"), "sha256").hexdigest()
            + "  "
            + path.name
            + "\n"
            for path in products
        )
    )
    print("\n".join(str(path) for path in products + [checksums]))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--print-version", action="store_true")
    parser.add_argument(
        "--platform",
        choices=["linux", "macos", "windows"],
        default={"Linux": "linux", "Darwin": "macos", "Windows": "windows"}[
            platform.system()
        ],
    )
    parser.add_argument("--editor", type=Path)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/release")
    args = parser.parse_args()
    if args.print_version:
        print(version())
    else:
        package(args.platform, args.output.resolve(), args.editor)
