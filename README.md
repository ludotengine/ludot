# Ludot

<img src="logo.svg" width="160" alt="Ludot logo">

Ludot is a private engine monorepo containing independent hard forks of Godot
4.7.2-stable and Luau 0.743. GDScript remains available. Desktop targets are Linux,
Windows, and macOS. Original source provenance is recorded in `upstreams.json`.

This is an early implementation, not a finished engine distribution. Native class
syntax and imports, a restricted shader compiler, an engine module, and a standalone
LSP are under development. Read [the implementation status](docs/status.md) before
using it for a project.

| Directory | Contents |
| --- | --- |
| `engine/` | Godot-derived engine, editor, and native Ludot module |
| `luau/` | Luau-derived parser, checker, compiler, and VM |
| `language/` | Shared `.lushader` compiler |
| `tools/` | Standalone LSP, shader CLI, and API type generator |
| `examples/` | Class script, aliased module, and canvas shader |
| `tests/` | Language, shader, LSP, and engine integration regressions |

[Build instructions](docs/building.md) · [Language syntax](docs/language.md) ·
[Shader syntax](docs/shaders.md)

There are no Godot or Luau submodules, automatic upstream merges, or build-time
upstream downloads. Source development takes place directly in these two trees.
Upstream copyright notices and dependency licenses remain intact. The supplied
logo carries its attribution in `logo.svg`.
