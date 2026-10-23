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

The standalone LSP loads `types/core.d.luau` by default. It covers common runtime
classes and built-in value types. Extend that subset by generating declarations
from this fork's extension API, then pass the file with `--definitions`:

```sh
engine/bin/ludot.linuxbsd.editor.x86_64 --headless --dump-extension-api
python tools/generate_api_types.py extension_api.json types/custom.d.luau --classes Node,CharacterBody2D,SceneTree,Resource,Input
python tools/ludot-lsp --definitions types/custom.d.luau
```

Generating the entire engine API currently exceeds the definition checker's
complexity limits. Use a class subset with its ancestors. References to classes
outside the selected subset become `any`. Full standalone engine API coverage
remains unfinished.

The native editor derives its signatures directly from ClassDB and Variant
reflection. It does not need this declaration file. The standalone LSP currently
starts a checker process per request. Incremental native LSP sessions, cancellation,
go-to-definition, rename, and signature help remain unfinished.

## Continuous integration

`.github/workflows/desktop.yml` builds language tools and the editor on Linux,
Windows, and macOS. It runs the language, shader, and LSP regressions before
uploading binaries. A configured workflow is not evidence that every platform
has passed. Consult the workflow runs for actual results.
