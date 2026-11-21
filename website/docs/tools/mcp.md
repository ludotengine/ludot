# Editor MCP

<p class="lead">Ludot includes a fork of Godot Editor MCP for inspecting and editing a live desktop editor from an MCP client.</p>

The server and addon live in the repository's `mcp/` folder. The package is still named `godot-editor-mcp`, and tool names keep their `godot_` prefix.

::: tip
Install from the `mcp/` folder. The upstream release doesn't include Ludot's Luau tools.
:::

## Install the local fork

Install the server from the local checkout into an environment with Python 3.11 or newer:

```bash
python -m venv build/mcp-venv
build/mcp-venv/bin/pip install ./mcp
```

On Windows, use `build\mcp-venv\Scripts\pip` instead.

Then:

1. Copy `mcp/godot/addons/godot_mcp/` into your project's `addons/` (or `Addons/`) folder.
2. Enable it in Project Settings > Plugins.
3. Point your MCP client at the virtual environment's `godot-editor-mcp` command.

The server finds `ludot` on your PATH. Use a server and editor from the same Ludot version. The [server README](https://github.com/ludotengine/ludot/blob/main/mcp/README.md) covers client configuration in detail.

## Read before editing

Two tools give an agent the context it needs before changing anything:

- `get_ludot_conventions` returns the project's API casing, naming rules, lint and formatting settings, module aliases, and config files.
- `list_settings` reads project settings or editor preferences by prefix. It hides values that look like credentials.

Tools that change things are grouped into toolsets, which are off by default. Enable a toolset before calling its tools.

## Luau tools

| Tool | Purpose |
| --- | --- |
| `luau_api` | Query engine types using project Luau spelling and inherited documentation. |
| `luau_enums` | Inspect the engine enum catalog. |
| `luau_resolve_module` | Resolve an import or module path. |
| `luau_inspect` | Read hover help or completions at a script position. |
| `luau_diagnose` | Read checker diagnostics, naming lint, Selene output, and formatting differences. |
| `luau_format` | Apply StyLua using editor settings; `dry_run` returns a diff. |
| `create_luau_script` | Create a script using the built-in templates. |

The API and inspection tools never run game code. Script editing and checking go through the engine's own `LuauScript` support.

## More detail

- [Full tool catalog and architecture](https://github.com/ludotengine/ludot/tree/main/mcp/docs/site)
- [Implementation status](../reference/status.md): which integration tests pass, and known limits
