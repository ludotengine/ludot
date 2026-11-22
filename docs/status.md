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
- Engine version 4.7.2-lu.10, set by the `status` field in `engine/version.py`.
- `.ludot` project file format (`project.ludot`) across core project settings, CLI,
  Project Manager, desktop MIME/bundle metadata, standalone LSP, and MCP tools, with
  backwards-compatible fallback loading for legacy `project.godot` projects.
- Project Manager update check repointed to Ludot's GitHub Releases (`ludotengine/ludot`),
  comparing `4.7.2-lu.<N>` versions and offering one-click install via the release's
  `install.sh` / `install.ps1` or opening the release page.
- One `Enum` global for every exposed, non-editor engine enum, modeled on Roblox's:
  `Enum.MouseMode.Captured` with `Name`, `Value`, and `EnumType`, plus `fromName`,
  `fromValue`, `getEnumItems`, and `Enum:getEnums()`. Engine methods and properties
  return EnumItems for non-bitfield enums and accept items or numbers. Native
  checking, the standalone LSP, and Inspector exports use `Enum.<Name>` types. The
  old `Class.Enums` namespaces, global enum tables, and `Class.Enum` annotations are
  gone. Signal arguments and virtual callback arguments still arrive as numbers.
- Native signals created with `signal with (param: Type, ...)` (modeled on Roblox's
  RBXScriptSignal): `fire`, `connect`, `once`, `wait`, `disconnectAll`, and
  `Connection` with `disconnect` and `Connected`. `Signal<T...>` is a type that
  checks fired arguments and infers handler parameters; `typeof` returns `"Signal"`
  and `"Connection"`. Handlers run in resumable threads from a snapshot, a failing
  handler is reported with its stack trace without stopping the others, and
  connections of freed objects are dropped. Engine signals use the same object,
  typed from ClassDB (`Signal<Node>` for `ChildEnteredTree`). On engine-backed
  classes, `public` signal fields are script signals: the Node dock lists them with
  their declared parameter names and types, scenes save their connections, and
  GDScript `connect`/`emit_signal` share handlers with Luau. `private` signal fields
  stay class-internal and Luau-only. The checker rejects exporting, `onready`, or
  reassigning signal fields, and the `signal_declaration` lint flags untyped signal
  parameters. Luau does not expose Godot's `Signal.emit`, `is_connected`, or
  `Signal.new`; GDScript retains its native Godot signal API.
- GDScript can construct engine-backed Luau scripts with `preload(...).new(...)`,
  including `__init` arguments. Constructor metadata keeps the script instance type
  in GDScript. Public Luau signals read as native Godot `Signal` values, supporting
  `.connect` and `.emit`, and method references read as `Callable` values. Typed
  returns retain their native types; dynamic and optional values cross as Variants.
  GDScript-declared signals also connect and emit through the Luau signal API.
  These changes leave Luau's syntax, signal methods, and API casing intact.
- A native `task` library (`spawn`, `defer`, `delay`, `wait`, `cancel`) with one
  scheduler per VM, stepped from SceneTree's process frame. Engine callbacks start in
  resumable threads, so `task.wait` works in `_ready`, `_process`, and signal
  handlers. Waits follow game time, pause, and time scale. Threads of freed objects
  are dropped, resumed errors print a stack trace, suspended threads keep their code
  through hot reload, and the debugger lists suspended tasks. The headless task suite
  and the debugger suite cover these behaviors. Waits always use game time; there is
  no real-time option.
- `onready` properties whose initializers can use `self`, such as
  `self:getNode("A/Child/Node")`, and run before `_ready`, base classes first.
  Unannotated onready properties take their initializer's type.
- Native parser support for brace classes, implicit `self`, per-instance defaults,
  exported fields, private field checking, and export range metadata.
- Path-first imports: side-effect, namespace, and `.{ }` item lists with renames,
  mixed value and type items, generic types, and erased type imports. The editor
  and LSP complete item names from the module's exports. Legacy `require` uses the same
  project and nested `.luaurc` aliases.
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
- Luau hovers in the native editor and standalone LSP use source-derived engine
  descriptions and a shared Luau example catalog. Examples follow API casing;
  native editor examples also follow lint naming and formatter indentation.
  Signal hovers resolve the signal's declaring class, and XML supplies named,
  typed connection examples. Public Luau signals and `---` comments appear in
  script class documentation; local signal hovers read unsaved comments. The
  packaged LSP includes descriptions and examples without a source checkout.
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
- `mcp/` holds Ludot Editor MCP (Ludot's fork of Godot Editor MCP, package/CLI
  `ludot-editor-mcp`, server ID `ludot-mcp`, stable 2026.11.04): a Python MCP
  server and an editor addon that lets an AI agent drive a live editor. Desktop
  releases bundle the `mcp/` server alongside the editor, and the editor embeds the
  matching addon directly. **Project > Setup Ludot MCP...** and **Project Settings >
  Plugins > Setup Ludot MCP...** open a built-in installer that extracts and enables
  `res://addons/godot_mcp` (with synchronous script class registration), detects
  installed AI providers (Claude Code, Claude Desktop, OpenAI Codex, Gemini CLI,
  Antigravity / ACP, OpenCode, Cursor, VS Code, Windsurf, Zed, GitHub Copilot CLI,
  Cline, Roo Code, Goose, Qwen Code, Amp, Kiro, Factory Droid, and project `.mcp.json`),
  greys out providers that already have Ludot MCP configured, and supports addon-only
  installation. The server finds `ludot` on PATH, and its always-on inspection toolset adds
  `get_ludot_conventions` (API casing, the user's naming casing per kind, lint,
  StyLua and indentation preferences, module aliases, and project `.luaurc`,
  selene and stylua files) and `list_settings` (project settings or editor
  preferences by prefix, with enum options and credential-like values withheld).
  It handles Luau through the editor's own `LuauScript` statics: the script tools
  read, write, patch, list, and check `.luau`; `luau_diagnose` returns checker
  errors, naming lint, Selene warnings with suppressions applied, and a StyLua diff;
  `luau_format` applies StyLua with the user's settings (`dry_run` returns the diff);
  `create_luau_script` uses the built-in templates; and the always-on `luau_api`
  (the engine API in Luau spelling under the project casing, with Godot's
  documentation and the `task`, `Signal`, `Connection`, and `Enum` libraries),
  `luau_enums`, `luau_resolve_module`, and `luau_inspect` (hover help and completions
  at a position) answer without running anything. The addon also works from an
  `Addons/` folder.

## Known limits

- Many upstream class-reference examples still need authored Luau equivalents.
  Luau hovers and reference pages mark those blocks as unavailable. Shared button,
  node lookup, and signal examples are available; signal connection examples derive
  their argument names from class XML.

- All Luau code shares one VM. Calls from different threads run one at a time, and a
  thread stopped at a breakpoint blocks other threads that call into Luau.
- A function keeps its old code after a reload while it is running, or when the
  variables it captures changed. Field values that cannot convert to a new type reset
  to the new default.
- Shader type checking beyond the Luau subset happens in the engine, after lowering.
- Scene-based node inference currently uses the active native editor scene and
  paths rooted at `self`. Dynamic paths, unrelated receiver variables, and standalone
  LSP sessions use the general engine return type.
- Values fired through an engine signal convert like any engine call. Tables and
  plain class objects travel as `LuauValue` references; other Luau values become
  engine values. `disconnectAll` on an engine signal also removes pending `wait`s made
  through the API, which leaves those threads suspended until their owner is freed.
- Mobile targets inherited from Godot are not supported. The web target is
  experimental. A single-threaded `template_release` web build compiles with
  Emscripten 4.0.23, using native wasm exceptions for Luau, and the exported Vesper
  demo runs in headless Chromium on WebGL 2 with the Compatibility renderer. Nothing
  else has been tested on the web: no threaded or debug templates, no other
  projects, no real GPUs or other browsers.

## Verification

Local Linux results:

- The documentation/interoperation changes were checked on 2026-11-22 with an
  editor build, the headless engine regression, native editor hover/class-reference
  checks, all nine API casing combinations, and the expanded signal integration
  suite. The language, shader, extension, LSP, and documentation CTest suites pass.
  Example checks cover authored snippets under all nine API casing combinations
  and generated signal handlers under all three casing styles. The packaged LSP
  passes its protocol and signal-hover checks from an unrelated project directory.

- `ctest`: extension (23 tests), language (17), shader (20), LSP (29), and
  documentation (5) suites
  pass. The extension suite checks the Signal usage cases, misuse errors, declaration lint, signal field
  rules, and method casing.
- `tools/test_signals_engine.py` passes. It runs the Signal usage cases in the engine
  and covers Luau fires reaching GDScript handlers and GDScript emits reaching Luau
  handlers, a scene-saved connection, the script signal list's names and types,
  inheritance, once, wait in a task, disconnect and Connected, disconnectAll keeping
  GDScript connections, a yielding and a failing handler, freed owners for engine and
  Luau signals, typed engine signals, and hot reload adding and removing signals.
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
- `tools/test_mcp_and_project_format.py` passes with the Linux editor build. It covers
  `project.ludot` loading/saving, legacy `project.godot` fallback loading, embedded
  MCP addon extraction and immediate plugin enablement, installed AI provider detection,
  already-configured provider detection and skipping, and non-destructive JSON/JSONC/TOML/YAML
  config merging across Claude Code, Codex, Gemini CLI, Antigravity, OpenCode, Zed,
  Goose, and VS Code.
- Local verification on 2026-11-22 passed `ctest` (4/4), `tests/test_install.py`,
  the updated MCP unit/contract suites (1,196 cases), and all 9 desktop suites via
  `tools/test_desktop.py` against `v4.7.2-lu.9`. The
  [lu.9 desktop CI run](https://github.com/ludotengine/ludot/actions/runs/45053367835)
  compiled all three editors, but the macOS and Windows jobs failed MCP provider
  fixture assertions. The lu.10 fixtures use macOS Application Support for VS Code,
  Windows app-data paths for Goose and Zed, and XDG paths for Unix Goose and Zed.
  Assertions check detected paths as well as configuration writes and preservation
  of existing entries. Use the Desktop builds workflow to check cross-platform
  results for lu.10.
- In `mcp/`, the upstream unit and contract tests pass, along with the live-editor
  inspection test against the Ludot editor, which covers the settings and
  conventions commands, and the live-editor Luau test, which diagnoses a script with
  a type error, a Selene warning, and bad formatting, formats it, and checks the
  API, enum, module, hover, and template lookups. Every Luau tool has a contract
  test. Upstream's tests of its own CI workflows were removed with
  those workflows.
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

### StyLua editor formatting

The script editor provides Format Document and Format Selection in the Luau
right-click menu. StyLua and format on save default to enabled. Editor Settings
under Text Editor > Luau > StyLua exposes the executable, indentation, line width,
quotes, call parentheses, simple statement collapsing, block spacing, function
name spacing, line endings, and require sorting. The syntax is fixed to Luau.
Desktop packages bundle StyLua 2.6.0. Source builds use `stylua` on PATH unless
an executable is selected in settings.

The native Ludot parser protects imports, class headers, property declarations,
and qualified methods while formatting the rest with StyLua. Protected declarations
retain their spelling and internal formatting. This is not a StyLua parser fork.
Invalid source or formatter errors leave the editor buffer unchanged. Selection
formatting follows StyLua's complete-statement range behavior.

The Linux editor build, headless engine regression, and headless editor regression
passed locally on 2026-11-12. Language, shader, and LSP regressions also passed.
Cross-platform CI results are not yet verified.

### Editor assistance

The CharacterBody3D movement template uses four-direction input on the X/Z plane,
with positive jump velocity. Movement templates use immutable bindings.
Native arithmetic signatures now come from Godot's operator table, including
Basis/Transform3D multiplied by Vector3. Hover cards also carry inferred variable
types and native property documentation, in addition to function signatures.
Luau block completion inserts indentation and `end` on Enter after block headers;
it can be disabled under Text Editor > Luau > Completion.

Editor Settings > Themes saves named themes and imports/exports compressed
`LDT1:` share codes. See [themes.md](themes.md) for palettes and customization.

The editor runs optional Selene 0.32.0 checks on unsaved buffers. Desktop packages
bundle Selene with its license. Source builds use `selene` on PATH. Text Editor >
Luau > Lint controls the executable, a custom Selene TOML configuration, and naming
rules for variables, constants, functions, methods, properties, and classes.
Bindings and functions default to camelCase, properties and classes to PascalCase.
Leading underscores are allowed. Naming diagnostics never rename symbols.

Selene receives a parser-identified compatibility view: imports, class properties,
and qualified methods are omitted, class braces become blocks, and `const` becomes
`local`. Native type checking still validates the original document. Selene's
undefined-global and standard-library checks default to disabled because the
native checker owns engine API types. This is an integration, not a Selene parser
fork; omitted regions do not receive Selene checks. Native naming checks cover
class declarations and properties. `-- selene: allow(rule)` and file-level
`--# selene: allow(rule)` suppress diagnostics; `naming_convention` controls naming.
StyLua's `-- stylua: ignore` and `-- stylua: ignore start` / `-- stylua: ignore end` comments pass through
to the formatter. Formatter and lint ignore directives have separate meanings.


### Project engine API casing

Project Settings > Luau > Api independently selects camelCase, PascalCase, or
snake_case for engine methods and engine properties/signals. Defaults remain
camelCase methods and PascalCase properties/signals. Builtin value components
follow property casing, while type/class names and `.new()` retain their established spelling. Native
enum constants use their original Godot names with compatible PascalCase aliases. User declarations remain exact and naming lint is
separate. Native bindings, editor checking/completion/hover/signature help, and
the standalone LSP use the same member-name conversion. Completion lists canonical spellings. The `isA`/`IsA` identity method and native
constant compatibility aliases follow the policy documented in the language guide. Casing snapshots require
an editor/game/LSP restart after setting changes; active bindings are not mutated.
See [language.md](language.md#how-names-are-converted) for acronym rules and compatibility.

The Linux native build and headless nine-combination runtime/editor-query matrix
passed locally on 2026-11-12. The LSP matrix, including hover and signature help,
also passed. Language/shader regressions and the full default desktop suite,
including debugger and exported-pack checks, passed. Cross-platform CI for this
addition has not yet been verified.


## Combined language extension implementation

The parser now lowers enums, match expressions/statements, optional property and
method chains, nil coalescing/assignment, inclusive ranges, and first-argument
pipelines into native AST operations. Compiler lifetime tracking accounts for
lowered expressions with non-lexical source ranges. Assertions remain enabled.
Enums have nominal member types, checked conversion, immutable namespaces, and
declaration-order iteration. Match analysis checks coverage, duplicate values,
case domains, and arm reachability, and refines matching enum values.

Godot Object identity checks support nodes, resources, input events, and registered
extension classes. Native checking and standalone LSP share their class-name
validation/refinement logic. Both `isA` and `IsA` work; completion uses the selected
project style. Engine enums now live under the `Enum` global described in the
language guide. Inspector exports retain the underlying numeric/string values and
display enum member names.

The editor preserves unsupported extended syntax through StyLua and supplies a
line-preserving view to Selene. Native match warnings support the editor's Selene
ignore directives. Match entry/arms support indentation and auto-end. The shader
subset rejects the new constructs with source diagnostics.

Local verification passed on 2026-11-13: the four language/extension/shader/LSP
CTest suites, 5,573 upstream unit cases with 35,373 assertions, a separate Linux
editor build, and the complete headless desktop suite. Desktop checks include
engine/debugger/exported-pack behavior, formatting and Inspector editing, enum
scene serialization, typed input-event identity, and all nine API casing
combinations. The user's existing engine executable was not replaced.

The detailed checklist is
[language-features-checklist.md](development/language-features-checklist.md).
Cross-platform CI for the combined revision is pending. No queued or unrun job
is counted as verified.

## Documentation website

`website/` provides a custom VitePress landing page and manual, with generated
references for the 1,070 classes and 774 native enum groups in the current API
manifest. The engine is labeled experimental and the site follows latest source
without a version picker. Luau and lushader use dedicated syntax highlighting.

The browser playground compiles the committed Luau compiler, VM, and analyzer to
WebAssembly. It runs and checks standalone examples in a worker; it does not run
the engine or resolve project imports. A separate engine checking mode loads
the API manifest for typed completion, hover, and diagnostics without executing
a scene. Runs have a
10-second time limit and a 256 MiB WebAssembly memory limit.

Local verification on 2026-11-21 passed the static production build and 13 site
regressions, covering runtime output, type errors, highlighting, reference
coverage, inherited documentation conversion, search entries, IntelliSense,
UTF-16/UTF-8 positions, and 27 maintained engine examples. Browser checks
verified execution and type checking with the built runtime. The GitHub Pages
workflow is configured; no deployed Pages result is claimed here.


## Desktop release automation

The desktop workflow packages checked builds into a draft release and publishes
only after all platform jobs pass. It includes Linux and macOS shell installation,
a macOS DMG, and Windows PowerShell and EXE installation, with package checksums.
Local Linux install/update and checksum-failure fixtures pass. A Linux editor
package was produced locally. Native builds and all eight desktop regression
suites pass locally after closing formatter/linter temporary files before calling
external tools. Windows and macOS packaging and release publication are not yet
verified; configured steps are not published releases.
