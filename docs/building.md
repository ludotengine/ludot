# Building Ludot

The source snapshots are committed in this repository. Builds do not fetch Godot
or Luau. `upstreams.json` records their original tags and commit IDs. Upstream
licenses and attribution remain in their source directories.

## Language tools

Install a C++17 compiler, CMake 3.25 or newer, Ninja, and Python 3.12 or newer.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLUAU_BUILD_TESTS=OFF
cmake --build build --target Luau.Repl.CLI Luau.Analyze.CLI Luau.Compile.CLI ludot-shader -j4
ctest --test-dir build --output-on-failure
```

The binaries live in `build/luau/` and `build/ludot-shader`. On Windows, append
`.exe`. The Luau CLI filenames remain recognizable while the dialect evolves.

## Editor

Godot's platform dependencies still apply. Install SCons 4.11.1 into a virtual
environment. Run SCons from `engine/`:

```sh
python -m venv .venv
.venv/bin/pip install scons==4.11.1
.venv/bin/scons -C engine platform=linuxbsd target=editor debug_symbols=no module_mono_enabled=no -j4
```

Rebuilds are much faster with a SCons cache, which reuses objects for unchanged files
even after switching branches or cleaning. Add `cache_path=../.scons_cache` (or set
`SCONS_CACHE`). On Linux, `linker=mold` cuts link time. Use every core with
`-j$(nproc)`. CI uses the cache, all cores, and mold.

Use `platform=windows` with an MSVC developer environment on Windows and
`platform=macos` on macOS. Ludot's module is desktop-only. GDScript stays enabled.
The fork includes mobile and web source inherited from Godot, but Ludot does not
support or build those targets.

The native module builds the monorepo's Luau sources directly. Editor builds also
include Luau Analysis and Config for type checking and completion. Export templates
include the VM and compiler without the editor analyzer. Engine binaries use the
`ludot` prefix in `engine/bin/`.

For a headless engine integration check, use the editor binary:

```sh
python tools/test_engine.py engine/bin/ludot.linuxbsd.editor.x86_64
```

A development build adds `.dev` to the filename. The test intentionally attempts
invalid script and shader edits, so those diagnostics are expected.

### Clean build timing

On 2026-10-23, a full editor C/C++ rebuild compiled 3,077 source files in
609.75 seconds of wall time on an Intel Core i9-11900F with 16 GB RAM:

```sh
.venv/bin/scons -C engine platform=linuxbsd target=editor use_llvm=yes linker=lld optimize=none debug_symbols=no module_mono_enabled=no extra_suffix=fastbuild -j8
python tools/test_engine.py engine/bin/ludot.linuxbsd.editor.x86_64.llvm.fastbuild
```

The build used a fresh output suffix without a compiler cache. Existing generated
headers were present, and other desktop applications were running. All default
editor features remained enabled except Mono. The headless integration regression
passed. This is one measurement, not a controlled comparison against GCC or the
documented build command.

This configuration did not meet a 200-second target for a threefold reduction
from a 10-minute build. `optimize=none` also reduces runtime performance, so it is
not the recommended default. Increasing parallelism, changing the compiler, and
using LLD together were insufficient in this experiment. Compiler caching can help
repeat builds but does not shorten the first build with an empty cache.

## Standalone LSP

```sh
python tools/ludot-lsp
```

Configure an editor to launch this command over stdio for `luau` and `lushader`
files. Optional arguments are `--analyzer`, `--shader-compiler`, and `--definitions`.
The server supports full-document synchronization, diagnostics, typed completion,
hover, and alias-aware import/require completion. It converts LSP UTF-16 positions
to Luau UTF-8 byte columns. Set `ludot.requireAutocomplete` to false to turn off
module path completion.

The standalone LSP loads the complete engine API from `types/engine.api` directly into
the checker. Regenerate it after changing engine classes:

```sh
engine/bin/ludot.linuxbsd.editor.x86_64 --headless --dump-extension-api
python tools/generate_api_manifest.py extension_api.json types/engine.api
```

`--definitions` adds a Luau declaration file on top of the engine API.

One analyzer process stays alive for the whole session. It reuses type information
and module dependencies, so editing an unsaved import updates the diagnostics of
files that depend on it. The server also supports request cancellation,
go-to-definition (including through imports), references, rename, and signature
help.

The native editor derives its signatures directly from ClassDB and Variant
reflection. It does not use `engine.api`.

To package the server as a self-contained directory, with Python, `luau-analyze`,
`ludot-shader`, and the API data, install PyInstaller into the virtual environment
and run:

```sh
.venv/bin/pip install pyinstaller
.venv/bin/python tools/package_lsp.py
```

The script writes `dist/ludot-lsp/` plus an archive, and then runs the packaged server
through a protocol check from an unrelated directory.

## Export templates

Godot's official export templates do not contain the Luau module, so the export
template manager does not offer downloads. Build templates from this tree:

```sh
.venv/bin/scons -C engine platform=linuxbsd target=template_release module_mono_enabled=no -j4
.venv/bin/scons -C engine platform=linuxbsd target=template_debug module_mono_enabled=no -j4
```

Select the resulting binaries in `engine/bin/` as custom templates in the export
preset. Template builds include the Luau VM and compiler, without the editor analyzer.

## Continuous integration

`.github/workflows/desktop.yml` builds language tools and the editor on Linux,
Windows, and macOS. It runs the language, shader, and LSP regressions before
uploading binaries. A configured workflow is not evidence that every platform
has passed. Consult the workflow runs for actual results.
