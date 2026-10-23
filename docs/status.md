# Implementation status

Ludot is an initial hard-fork implementation. It does not yet meet the complete
engine specification. This file records practical limits rather than treating
repository structure or planned features as delivered behavior.

## Implemented

- Vendored Godot 4.7.2-stable and Luau 0.743 source with pinned provenance.
- Native parser support for brace classes, implicit `self`, per-instance defaults,
  exported fields, private field checking, and export range metadata.
- Named, renamed, namespace, side-effect, and erased type imports.
- Alias-aware CLI imports and legacy `require` through `.luaurc`.
- A restricted `.lushader` compiler for canvas, spatial, and particle shaders.
- A native engine module with Luau script and shader resource loaders/savers,
  reflected API casing, script instances, inspector exports, staged script reloads, and
  transitive module cache invalidation with dependent script reloads.
- Native editor checker/completion integration using Luau Analysis and reflected
  engine signatures. Analysis initializes lazily.
- Standalone stdio LSP with checker-backed diagnostics, completion, hover, open-buffer
  overlays, Unicode position conversion, and import path completion settings.
- Debugger hooks for runtime errors, source breakpoints, stepping, stack frames,
  and locals. Interactive debugger behavior still needs manual verification.
- Ludot product name and supplied logo in engine version metadata and major desktop assets.

## Incomplete

- Attached script inheritance across Luau resources and script global class discovery.
- Complete native API coverage, static methods/constants, Luau callbacks as Callables,
  signal conveniences, typed collection conversion, and native ownership/refcount auditing.
- Direct native property proxies. The current binding synchronizes reflected
  properties at callback boundaries and needs replacement for production performance.
- Hot reload closure identity, external references to replaced script objects, dependency
  graph pruning, and safe migration when field types or inheritance change. Basic
  module dependency invalidation and dependent script reloads work.
- Generic named type imports need further work. Ordinary Luau generics and type
  namespaces remain available.
- Native import path completion and nested alias configuration parity with the CLI.
- Dedicated native `.lushader` code editor, shader completions, source mapping for GPU
  diagnostics, loops/helpers/varyings/render modes, and shader export packaging tests.
- Complete debugger expression evaluation, member/global inspection, profiling, and
  breakpoint behavior under coroutine and multithreaded workloads.
- Full standalone engine API declarations. The shipped core declarations cover a
  selected group of runtime classes; generating the entire API exceeds checker limits.
- Persistent incremental standalone LSP sessions, cancellation, definitions, rename,
  signature help, and packaged standalone distributions.
- Startup and reload performance targets, release builds, export templates, and
  platform certification. CI covers three desktop operating systems but must actually run.

## Verification

The local language suite has 11 tests, the shader suite has 9, and the LSP suite
has 7. These tests and the Linux editor build pass. The headless native integration
suite also passes, covering exports, methods, native API casing, script reloads,
Luau table preservation, module invalidation, failed edits, shaders, native type
errors, and native completion. The tests intentionally produce an invalid-script
diagnostic while checking that the previous implementation remains active.

Three local headless example startup-and-exit runs took 529.5, 526.1, and 543.8 ms
with an unoptimized development editor. Those numbers include process teardown.
They are not a release startup benchmark or a comparison against upstream Godot.
No Windows or macOS success is claimed from Linux compilation.
