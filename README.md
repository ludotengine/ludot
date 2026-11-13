# Ludot

<img src="logo.svg" width="160" alt="Ludot logo">

Ludot is a private engine monorepo containing independent hard forks of Godot
4.7.2-stable and Luau 0.743. GDScript remains available. Desktop targets are Linux,
Windows, and macOS. Original source provenance is recorded in `upstreams.json`.

The current release is Ludot 4.7.2-lu.3. Ludot versions keep the Godot version they
are based on and replace Godot's release status with a Ludot release number, set
as `status` in `engine/version.py`. The editor shows it as `4.7.2.lu.3`, and
`Engine.get_version_info()` reports `4.7.2-lu.3`. Bump the number after `lu.` for
each Ludot release; rebasing on a newer Godot release resets it to `lu.1`.

Luau runs alongside GDScript as a native scripting language, with brace classes,
`import` syntax, user-defined module aliases, hot reload, editor checking and
completion, debugging, and profiling. `.lushader` files compile a typed Luau subset to
canvas, spatial, and particle shaders. A standalone LSP serves other editors. Read
[the implementation status](docs/status.md) for verified behavior and known limits.

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
