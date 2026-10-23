# Implementation status

This file records what works, what has been verified, and the remaining practical
limits. A planned feature or an unrun CI job is not listed as verified.

## Implemented

- Vendored Godot 4.7.2-stable and Luau 0.743 source with pinned provenance.
- Native parser support for brace classes, implicit `self`, per-instance defaults,
  exported fields, private field checking, and export range metadata.
- Named, renamed, namespace, side-effect, generic, and erased type imports. Legacy
  `require` uses the same project and nested `.luaurc` aliases.
- Native engine module: Luau script and shader resources, inspector exports, script
  inheritance between Luau resources, global class discovery, and Luau-cased engine
  API (methods, properties, signals, static methods, constants, Luau functions as
  `Callable`s, and collection indexing, length, and iteration).
- Direct native property access with a per-class member cache.
- Hot reload of scripts and modules. Live instances keep their identity and the
  fields whose type is unchanged. Dependent scripts and derived classes reload. A
  failed edit keeps the previous version running.
- Native editor checking and completion using Luau Analysis and reflected engine
  signatures, including alias-aware import and `require` path completion. Project
  setting `luau/editor/require_autocomplete` turns path completion off.
- Remote debugger support: breakpoints, stepping, stack frames, locals, members,
  globals, Luau expression evaluation, coroutine frames, and runtime error breaks.
- Script profiler data for Luau functions called from the engine, Luau callables, and
  native calls made from Luau.
- `.lushader` compiler for canvas, spatial, and particle shaders with uniforms, range
  hints, varyings, constants, helper functions, loops, `discard()`, and render modes.
  A dedicated editor provides delayed validation and completion, and GPU diagnostics
  map back to source lines.
- Standalone stdio LSP with a persistent analyzer session, the complete engine API,
  diagnostics, completion, hover, definitions, references, rename, signature help,
  cancellation, unsaved-buffer overlays, and UTF-16 position conversion. A packaging
  script builds a self-contained distribution.
- Ludot product name, logo, and issue links. The upstream update check and official
  template downloads are disabled, since Godot builds lack the Luau module.

## Known limits

- Profiling times engine-to-Luau and Luau-to-native boundaries. Luau functions called
  directly from other Luau code are included in their caller's time. They do not get
  their own rows.
- Closures created before a reload, such as callbacks already connected to signals,
  keep running the code they were created with until they are reconnected.
- Fields whose declared type changes reset to the new default on reload.
- Shader includes and custom interpolation qualifiers are not implemented. Shader
  type checking beyond the Luau subset happens in the engine, after lowering.
- Debugger stepping and breakpoints run on the main Luau state. Scripts executing on
  other threads are not covered.
- Mobile and web targets inherited from Godot are not supported.

## Verification

Local Linux results:

- `ctest`: language (12 tests), shader (13), and LSP (15) suites pass.
- `tools/test_engine.py` passes with the development editor build. It covers scripts,
  exports, inheritance, native API casing, reloads, module invalidation, shaders
  (including loops, helpers, varyings, and render modes validated by the engine
  shader compiler), checker diagnostics, and native completion.
- `tools/test_debugger.py` passes. It drives the remote debugger protocol through
  breakpoints, stepping, evaluation, coroutine frames, and the scripts profiler.
- `tools/test_export.py` passes, both with the editor binary and with a Linux
  `template_release` build running the exported pack the way a shipped game does
  (pack beside the executable, scene selected through `override.cfg`).
- `tools/package_lsp.py` builds the standalone LSP and passes its protocol check.

A loop of 100,000 iterations, each reading a `Vector2` field, reading a `Timer`
property, and calling a `Timer` method, took about 0.57 s in the unoptimized
development build. It took about 61.6 s before the member cache.

The GitHub workflow builds and tests Linux, Windows, and macOS. Its results are on
the repository's Actions page. Only the Linux results above were checked locally.
No Windows or macOS success is claimed from Linux compilation.
