# Ludot editor MCP

This directory is Ludot's copy of Godot Editor MCP. It is an MCP server written in
Python (`mcp_server/`) plus a GDScript editor addon (`godot/addons/godot_mcp/`) that
connects to the server over a WebSocket at `ws://127.0.0.1:9080`. Like `engine/` and
`luau/`, it is a hard fork: edit it here directly, record the upstream origin in
`../upstreams.json`, and keep the MIT license and its copyright notice.

Ludot changes from upstream:

- The server looks for `ludot` on PATH before `godot` when it needs an editor binary.
- `handlers/settings.gd` adds `cmd_list_settings` and `cmd_get_ludot_conventions`,
  exposed as the read-only `godot_inspection_list_settings` and
  `godot_inspection_get_ludot_conventions` tools. The server instructions tell
  agents to read the conventions before writing Luau.
- Luau support, through the Ludot editor's own `LuauScript` statics: the script
  tools read, write, patch, list (`language="luau"`) and check `.luau` files
  (`get_parse_errors` uses the Luau checker), and `handlers/luau.gd` adds
  `godot_scripts_luau_diagnose` (checker errors, naming lint, Selene with
  suppressions, and a StyLua diff), `godot_scripts_luau_format` (StyLua with the
  user's settings, `dry_run` returns the diff), `godot_scripts_create_luau_script`
  (Ludot's built-in templates), and the always-on `godot_inspection_luau_api` (the
  engine API in Luau spelling with Godot's documentation), `luau_enums`,
  `luau_resolve_module` and `luau_inspect` (hover help and completions at a
  position). They fail with `PRECONDITION_FAILED: required=ludot_editor` on stock
  Godot.
- Addon folders are matched case-insensitively, so a project can keep the addon in
  `res://Addons/godot_mcp`.
- Upstream's GitHub workflows, agent harness files and the tests that checked them
  are not copied.

Run `ludot --headless --path godot --import` once so the test project's class
cache exists, then run `uv run pytest tests/unit tests/contract -q` from this
directory. Live-editor tests in `tests/integration` use `GODOT_BIN` or `ludot`.

Install locally with `uv tool install --force .`, which puts `ludot-editor-mcp` in
`~/.local/bin`, and install the addon via Project → Setup Ludot MCP... or copy
`godot/addons/godot_mcp` into a project's `addons/`.

Upstream's own project guide is not copied here; see
https://github.com/hybridindie/godot-mcp for its architecture notes.
