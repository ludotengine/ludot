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

To run the upstream parser, compiler, and type-checker unit suite as well:

```sh
cmake -S . -B build -DLUAU_BUILD_TESTS=ON
cmake --build build --target Luau.UnitTest -j4
build/luau/Luau.UnitTest
```

## Editor

Godot's platform dependencies still apply. Install SCons 4.11.1 into a virtual
environment. Run SCons from `engine/`:

```sh
python -m venv .venv
.venv/bin/pip install scons==4.11.1
.venv/bin/scons -C engine platform=linuxbsd target=editor debug_symbols=no module_mono_enabled=no -j4
```

Rebuilds are much faster with a SCons cache, which reuses objects for unchanged files
even after switching branches or cleaning. Add `cache_path=../.scons_cache`.
`SCONS_CACHE` is deprecated. On Linux, `linker=mold` cuts link time. Use every core
with `-j$(nproc)`, provided there is enough RAM for the compiler processes. CI uses
the cache, all runner cores, and mold on Linux. Its Linux editor also uses
`scu_build=yes`, which combines source files to avoid parsing the same headers
repeatedly. This keeps the default runtime optimization and editor features.

For repeated language-tool builds, install ccache and configure CMake with
`-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache`.
CI restores this compiler cache on Linux and macOS. Windows uses sccache with MSVC
and GitHub's cache backend, configured through the same CMake launcher options.

Use `platform=windows` with an MSVC developer environment on Windows and
`platform=macos` on macOS. Ludot's module is desktop-only. GDScript stays enabled.
Luau sources use MSVC's `/fp:precise` mode, since their VM arithmetic pragmas cannot
compile with the floating-point exception mode enabled by Godot's `/fp:strict`.
Godot and the native binding keep their existing compilation settings.
The fork includes mobile and web source inherited from Godot, but Ludot does not
support or build those targets.

On macOS, add `generate_bundle=yes` to produce
`engine/bin/ludot_macos_editor.app`. The bundle uses the Ludot executable and icon.
`tools/test_desktop.py` accepts this `.app` path as well as a bare executable.
CI tests the generated app and uploads `ludot-macos-editor.zip`.

Linux desktop and MIME metadata live under `engine/misc/dist/linux/` with the
`org.ludotengine.Ludot` identifier. The Windows Inno Setup source is
`engine/misc/dist/windows/ludot.iss`; its app identifier is separate from Godot's.

The native module builds the monorepo's Luau sources directly. Editor builds also
include Luau Analysis and Config for type checking and completion. Export templates
include the VM and compiler without the editor analyzer. Engine binaries use the
`ludot` prefix in `engine/bin/`.

Luau translation units retain ordinary compiler warnings but do not inherit Godot's
shadowing warnings. Luau intentionally uses member names as constructor parameter
names, which otherwise produces thousands of repeated header diagnostics.
Compiler and AST assertions remain enabled in optimized native builds, as they do
in the standalone language tools.

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

A separate Linux GCC build with `scu_build=yes`, default runtime optimization, and
no compiler cache compiled 2,070 translation units in 16 minutes 52 seconds. The
initial link failed because mold was unavailable in that environment. Relinking
with the default linker took 18 seconds, and the headless engine regression passed.
This run and the Clang measurement above use different optimization settings and
are not a controlled timing comparison. No Windows or macOS SCU result is claimed.

A cache-backed macOS CI editor rebuild at `a25fa13` took 57.08 seconds. The complete
job, including language tools, standalone packaging, and all desktop regressions,
took 6 minutes 23 seconds
([job](https://github.com/ludotengine/ludot/actions/runs/40037145384/job/120701010724)).
This measures a rebuild using saved objects, rather than a clean build with an
empty cache.

The cached desktop rerun at `8b23494` included a changed native integration source
file and the full regression suites
([run](https://github.com/ludotengine/ludot/actions/runs/40046404832)):

| Platform | Editor build | Complete CI job |
| --- | --- | --- |
| Linux | 46 seconds | 2 minutes 7 seconds |
| macOS | 1 minute | 3 minutes 50 seconds |
| Windows | 2 minutes 7 seconds | 5 minutes 18 seconds |

The language-tool builds took 3 seconds on Linux, 4 seconds on macOS, and 43 seconds
on Windows. Windows reported 199 compiler-cache hits and no misses. First builds
with empty caches still take substantially longer.

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
the engine build, then the engine, remote debugger, and exported-pack regressions
through `python tools/test_desktop.py`. Pass an editor binary path to that command
when multiple builds are present locally. It also packages the standalone LSP,
checks its stdio protocol from an unrelated project directory, and uploads a
separate archive for each platform. A configured workflow is not evidence that
every platform has passed. Consult the workflow runs for actual results.

Pushes to one branch share a concurrency group. The running build finishes and
saves its cache; only the newest pending run waits behind it. This avoids losing
an almost-complete engine build each time another fix is pushed.

To regenerate desktop artwork after editing `logo.svg`, install Pillow in the
build virtual environment and run `.venv/bin/python tools/generate_branding.py`.
The generator also requires `rsvg-convert`. It writes the editor SVGs, application
PNGs, splash images, Windows ICOs, and macOS application/document ICNS files.

`python tools/test_editor.py <editor-binary>` runs the actual editor with a
temporary project. It checks Luau highlighter registration and cache migration,
Unicode and multiline coloring, completion kinds/icons, and template creation.
This check also runs as part of `tools/test_desktop.py`.

For visual hover-card checks on a desktop session, run
`python tools/test_editor.py <editor-binary> --screenshots build/hover-cards`.
This opens an isolated editor project, captures GDScript and Luau cards with the
real renderer, and verifies their maximum height and fixed documentation footer.
