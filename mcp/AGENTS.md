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
- Upstream's GitHub workflows, agent harness files and the tests that checked them
  are not copied.

Run `ludot --headless --path godot --import` once so the test project's class
cache exists, then run `uv run pytest tests/unit tests/contract -q` from this
directory. Live-editor tests in `tests/integration` use `GODOT_BIN` or `ludot`.

Install locally with `uv tool install --force .`, which puts `godot-editor-mcp` in
`~/.local/bin`, and copy `godot/addons/godot_mcp` into a project's `addons/`.

Upstream's own project guide is not copied here; see
https://github.com/hybridindie/godot-mcp for its architecture notes.
