# Implementation status

This file records what works, what has been verified, and the remaining practical
limits. A planned feature or an unrun CI job is not listed as verified.

## Implemented

- `.new()` constructors for Luau classes, native nodes/resources, and native value
  types, with typed results and initializer arguments in the editor and LSP.
  Normal Luau scripts use editor placeholders and do not run lifecycle callbacks
  while editing, so per-function `Engine:isEditorHint()` guards are unnecessary.
  Range keywords (`in`, `to`, `step`) and scalar/vector types are colored in the
  dedicated lushader editor. Native engine/editor and language/LSP regressions
  cover these behaviors.
- `demos/vesper` is a playable solar-system demo built entirely at startup from Luau
  and lushader: cube-sphere terrain with ridges, terraces, canyons, and craters,
  oceans, clouds, ray-marched atmosphere fog, a gas giant with rings, an asteroid
  belt, free flight that rides along with the nearest world, a planet camera, and a
  scripted tour. All eight Luau files pass native strict checking. Headless gameplay
  checks cover flight, the planet camera, riding along with a moving world, ground
  clearance, pause, and orbit visibility. A 54-second Forward+ capture covers the tour.
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
- Native objects created in field initializers are separate instances. The engine's
  existing owner is borrowed only for the script's own base constructor.
- Hot reload of scripts and modules. Live instances keep their identity and their
  field values, converted when a declared type changes. Functions created before the
  reload, such as connected callbacks, run the new code. Dependent scripts and derived
  classes reload. A failed edit keeps the previous version running.
- Native Luau syntax highlighting uses the language lexer, including interpolated
  strings, arbitrary long brackets, comments, and Unicode columns. Existing cached
  Plain Text defaults migrate once; future explicit highlighter choices are kept.
- Attach Script offers empty, Node lifecycle, and CharacterBody2D/3D movement
  templates. Luau resources use the official Luau icon. Native completion includes
  function/type/property/signal icons, parentheses, parameter hints, and `:` triggers.
- In the active editor scene, literal `self:getNode(...)` paths and `self:getParent()`
  infer the target native/script type, including chained literal lookups. Script
  class names in that scene can be used directly as instance annotations.
  `getNodeOrNull` keeps a nullable type. Custom script method names retain their
  original spelling when called through a node reference.
- Native editor function hover cards for GDScript and Luau show typed signatures,
  parameter and return sections, documentation comments, typed GDScript lambdas,
  selected Luau overloads, and a fixed footer linking
  documented Godot methods to their original API reference page. Luau names retain
  their binding casing; GDScript functions include default argument values.
  Native callback overrides retain their Godot documentation links.
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
  script builds a self-contained distribution with runtime and upstream license notices.
- Rounded Ludot logo with the Luau radial gradient, used by the project selector,
  default project icon, title bar, splash, desktop application and document artwork.
  `tools/generate_branding.py` regenerates desktop assets from `logo.svg`.
- Ludot product name, logo, and issue links. The upstream update check and official
  template downloads are disabled, since Godot builds lack the Luau module.
- Desktop distribution metadata uses Ludot names and identifiers. The macOS bundle
  generator packages the renamed executable, and Windows console wrappers use the
  Ludot icon. Original copyright notices remain intact.
- Deferred documentation-cache loads are cancelled after editor shutdown, so a
  command-line export cannot start a documentation worker on the freed filesystem.

## Known limits

- All Luau code shares one VM. Calls from different threads run one at a time, and a
  thread stopped at a breakpoint blocks other threads that call into Luau.
- A function keeps its old code after a reload while it is running, or when the
  variables it captures changed. Field values that cannot convert to a new type reset
  to the new default.
- Shader type checking beyond the Luau subset happens in the engine, after lowering.
- Scene-based node inference currently uses the active native editor scene and
  paths rooted at `self`. Dynamic paths, unrelated receiver variables, and standalone
  LSP sessions use the general engine return type.
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
- The optimized Linux SCU editor passes the engine, debugger, exported-pack, and
  native editor suites. The editor suite opens real Luau files, migrates a saved
  Plain Text choice, checks Unicode/interpolated/long-bracket coloring and typed
  completion icons, and creates/type-checks all four built-in templates. It also
  checks typed parent-script lookup, direct instance annotations, nullable narrowing,
  and invalidation when a parent script changes. Hover regressions inspect actual
  popups for both languages and verify signatures, comments, defaults, and exact
  native documentation URLs. A visible Linux run captures and checks compact card
  layouts with the real renderer. Runtime checks call a custom
  parent method through a child node lookup. Five fresh-cache editor startup/shutdown checks pass after the deferred
  documentation callback fix.

A loop of 100,000 iterations, each reading a `Vector2` field, reading a `Timer`
property, and calling a `Timer` method, took about 0.57 s in the unoptimized
development build. It took about 61.6 s before the member cache.

Linux, Windows, and macOS passed the complete desktop workflow at `8b23494`
([run](https://github.com/ludotengine/ludot/actions/runs/40046404832)). This includes
language/shader/LSP tests, standalone packaging, native engine checks, debugging,
exported-pack checks, and the nested native constructor regression. The macOS suites ran the executable selected by the
generated app bundle's `Info.plist`.
