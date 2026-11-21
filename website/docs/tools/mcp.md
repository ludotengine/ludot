# Editor MCP

<p class="lead">Ludot includes a fork of Godot Editor MCP for inspecting and editing a live desktop editor from an MCP client.</p>

Use the server and addon in this repository's `mcp/` tree for Ludot-specific tools. The upstream package name is still `godot-editor-mcp`, and tool names keep their `godot_` prefix for compatibility. Installing upstream releases alone does not provide this fork's Luau additions.

## Install the local fork

Install the server from the local checkout into an environment with Python 3.11 or newer:

```bash
python -m venv build/mcp-venv
build/mcp-venv/bin/pip install ./mcp
```

On Windows, use the virtual environment's `Scripts` directory. Install the matching addon from the local `mcp/godot/addons/godot_mcp/` tree into your project and enable it in Project Settings > Plugins. Configure your MCP client to run the environment's `godot-editor-mcp` command. See the [server README](https://github.com/ludotengine/ludot/blob/main/mcp/README.md) for bridge and client configuration.

The fork discovers `ludot` on PATH. Keep editor and server versions matched.

## Read before editing

`get_ludot_conventions` returns the project API casing, naming rules, lint and formatting preferences, module aliases, and relevant configuration files. `list_settings` inspects project settings or editor preferences by prefix, withholding credential-like values.

Toolsets gate mutation tools. Enable the required toolset before trying to call a tool that is not exposed. Inspect the connected editor and current project before changing scene or script state.

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

The API and inspection tools answer without running game code. Script editing and checking go through the native `LuauScript` integration. The addon can also live in an `Addons/` directory.

## More detail

The full MCP tool catalog and architecture remain in [the repository's MCP docs](https://github.com/ludotengine/ludot/tree/main/mcp/docs/site). Ludot's [implementation status](../reference/status.md) records which integration tests have passed and which runtime limits remain.
