# Ludot development

Ludot is a monorepo with independent Godot and Luau hard forks. Modify the committed
source directly. Do not replace these trees with submodules or add automatic upstream
updates. Original versions and commits are recorded in `upstreams.json`.

- `engine/modules/ludot/` owns native integration. GDScript and Godot's internal API
  names remain compatible. Apply Luau casing in the binding layer.
- `luau/` owns language syntax, analysis, compilation, and VM behavior. Native class
  and import changes belong here, not in regex preprocessing.
- `language/` owns the shared shader subset compiler. Reject unsupported AST nodes
  with source diagnostics. The engine validates generated shader types separately.
- `tools/ludot_lsp/` owns the standalone LSP protocol. Keep UTF-16/UTF-8 conversion
  and unsaved document overlays covered when editing protocol behavior.
- Preserve upstream copyright notices, dependency licenses, and logo attribution.
- Update `docs/status.md` when a feature becomes usable or a known limit changes.
  Never describe a planned feature or an unrun CI job as verified.

Use the commands in `docs/building.md`. Run the relevant language, shader, or LSP
regressions for behavioral changes. Native integration changes also need an engine
build and the headless engine regression. Keep compiler assertions enabled in the
language tools so optimized builds still catch AST and register-lifetime mistakes.
