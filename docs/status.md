# Implementation status

This file records what works, what has been verified, and the remaining practical
limits. A planned feature or an unrun CI job is not listed as verified.

## Implemented

- Vendored Godot 4.7.2-stable and Luau 0.743 source with pinned provenance.
- Native parser support for brace classes, implicit `self`, per-instance defaults,
  exported fields, private field checking, and export range metadata.
- Named, renamed, namespace, side-effect, generic, and erased type imports. Legacy
  `require` uses the same project and nested `.luaurc` aliases.
- Native class types support structural table intersections. A field supplied by
  the table shape is accepted; a field missing from a union alternative is rejected.
- Native engine module: Luau script and shader resources, inspector exports, script
  inheritance between Luau resources, global class discovery, and Luau-cased engine
  API (methods, properties, signals, static methods, constants, Luau functions as
  `Callable`s, and collection indexing, length, and iteration).
- Direct native property access with a per-class member cache.
- Hot reload of scripts and modules. Live instances keep their identity and their
  field values, converted when a declared type changes. Functions created before the
  reload, such as connected callbacks, run the new code. Dependent scripts and derived
  classes reload. A failed edit keeps the previous version running.
- Native editor checking and completion using Luau Analysis and reflected engine
  signatures, including alias-aware import and `require` path completion. Project
  setting `luau/editor/require_autocomplete` turns path completion off.
- Remote debugger support: breakpoints, stepping, stack frames, locals, members,
  globals, Luau expression evaluation, coroutine frames, worker threads, and runtime
  error breaks.
- Script profiler rows for every Luau function, including calls between Luau
  functions, `pcall` targets, metamethods, and coroutine bodies, plus native calls
  made from Luau.
- `.lushader` compiler for canvas, spatial, and particle shaders with uniforms, range
  hints, varyings with interpolation qualifiers, constants, helper functions, loops,
  `discard()`, render modes, and `shader include` libraries imported by path or alias.
  Editing a library recompiles the shaders that include it.
  A dedicated editor provides delayed validation and completion, and GPU diagnostics
  map back to source lines.
- Standalone stdio LSP with a persistent analyzer session, the complete engine API,
  diagnostics, completion, hover, definitions, references, rename, signature help,
  cancellation, unsaved-buffer overlays, and UTF-16 position conversion. A packaging
  script builds a self-contained distribution.
- Ludot product name, logo, and issue links. The upstream update check and official
  template downloads are disabled, since Godot builds lack the Luau module.
- Deferred documentation-cache loads are cancelled after editor shutdown, so a
  command-line export cannot start a documentation worker on the freed filesystem.

## Known limits

- All Luau code shares one VM. Calls from different threads run one at a time, and a
  thread stopped at a breakpoint blocks other threads that call into Luau.
- A function keeps its old code after a reload while it is running, or when the
  variables it captures changed. Field values that cannot convert to a new type reset
  to the new default.
- Shader type checking beyond the Luau subset happens in the engine, after lowering.
- Mobile and web targets inherited from Godot are not supported.

## Verification

Local Linux results:

- `ctest`: language (13 tests), shader (18), and LSP (16) suites pass.
- The upstream Luau unit suite passes all 5,573 cases and 35,369 assertions.
- `tools/test_engine.py` passes with the development editor build. It covers scripts,
  exports, inheritance, native API casing, reloads (including field type changes and
  callbacks connected before the reload), module invalidation, shaders (loops,
  helpers, interpolated varyings, render modes, and includes, validated by the engine
  shader compiler, plus recompiling when an include changes), checker diagnostics,
  and native completion.
- `tools/test_debugger.py` passes. It drives the remote debugger protocol through
  breakpoints, stepping, evaluation, coroutine frames, worker threads, and the scripts
  profiler (including nested Luau calls, `pcall` targets, and coroutine bodies).
- `tools/test_export.py` passes, both with the editor binary and with a Linux
  `template_release` build running the exported pack the way a shipped game does
  (pack beside the executable, scene selected through `override.cfg`).
- `tools/package_lsp.py` builds the standalone LSP and passes its protocol check.
- The optimized Linux SCU editor passes the engine, debugger, and exported-pack
  suites. Five fresh-cache editor startup/shutdown checks pass after the deferred
  documentation callback fix.

A loop of 100,000 iterations, each reading a `Vector2` field, reading a `Timer`
property, and calling a `Timer` method, took about 0.57 s in the unoptimized
development build. It took about 61.6 s before the member cache.

The macOS editor build and native engine integration suite passed in GitHub Actions
at `7ebd0c7` ([run](https://github.com/ludotengine/ludot/actions/runs/40034285979)).
Windows still needs validation of the MSVC floating-point mode fix in `4c08027`
or later. The workflow's debugger, exported-pack, and standalone-package checks are
newer than that macOS result; only their Linux results are verified above.
