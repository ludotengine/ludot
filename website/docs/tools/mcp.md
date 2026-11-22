# Editor MCP

<p class="lead">Ludot includes Ludot Editor MCP (a fork of Godot Editor MCP) for inspecting and editing a live desktop editor from an MCP client.</p>

The server and addon live in the repository's `mcp/` folder and are bundled with desktop releases. The package and CLI command are named `ludot-editor-mcp` (server ID `ludot-mcp`), and tool names keep their `godot_` prefix for protocol compatibility.

::: tip
Use Ludot's built-in installer or install from the `mcp/` folder. Upstream releases don't include Ludot's Luau tools.
:::

## Automatic setup from the editor

Open your project in Ludot and choose **Project > Setup Ludot MCP...** (or click **Setup Ludot MCP...** under **Project Settings > Plugins**):

1. Check **Install and enable Ludot Editor MCP addon in this project** to extract the matching `godot_mcp` addon into `res://addons/godot_mcp/` and enable it automatically.
2. Select any detected AI providers you want to configure (Claude Code, Claude Desktop, OpenAI Codex, Gemini CLI, Antigravity / ACP, OpenCode, Cursor, VS Code, Windsurf, Zed, GitHub Copilot CLI, Cline, Roo Code, Goose, Qwen Code, Amp, Kiro, Factory Droid, or project-local `.mcp.json`).
   - Leave all providers unchecked (or click **Addon Only**) to install only the project addon.
   - Providers that already have Ludot MCP configured are automatically detected and greyed out.
3. Click **Install**.

## Manual install from source

To install the server CLI manually from a local checkout into an environment with Python 3.11 or newer:

```bash
uv tool install --force ./mcp
# or inside a virtual environment:
python -m venv build/mcp-venv
build/mcp-venv/bin/pip install ./mcp
```

On Windows, use `build\mcp-venv\Scripts\pip` instead.

Then point your MCP client at `ludot-editor-mcp` (or run `ludot-editor-mcp config --client <client> --write`).

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
