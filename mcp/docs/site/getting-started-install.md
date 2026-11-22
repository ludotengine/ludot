---
type: index
title: "Install the MCP server"
description: "Ways to run ludot-editor-mcp — local install, Docker, or from source."
created: 2026-09-19
updated: 2026-10-18
---

# Install the MCP server

::: info For developers
This is the manual setup path. In Ludot, you can also open **Project → Setup Ludot MCP...** to install the addon and configure your AI clients automatically.
:::

## Option A: Install with uv or pip (recommended)

```bash
uv tool install --force ./mcp       # or: pip install ./mcp
ludot-editor-mcp                    # starts the MCP server over stdio
```

## Option B: Docker

```bash
docker run -d -p 9090:9090 -p 9080:9080 \
  -e GODOT_MCP_AUTH_TOKEN=$(openssl rand -hex 32) \
  ghcr.io/hybridindie/godot-mcp:latest
```

Docker binds `0.0.0.0` inside the container, so a **bearer token is required**
(#226). The server refuses to start on a non-loopback bind without one.

## Option C: From source (development)

```bash
cd mcp
uv sync
uv run ludot-editor-mcp   # stdio mode (default)
```

## Choose a transport

| Transport | Use case | How |
|-----------|----------|-----|
| `stdio` (default) | A single local agent | the client spawns `ludot-editor-mcp` itself |
| `http` | Shared service; several clients on one editor; remote/web UIs | `GODOT_MCP_TRANSPORT=http` or `scripts/serve-http.sh` |

::: tip Service mode for multiple clients
The bridge to the editor is a single connection — one server process owns
it at a time. To give **several** clients (OpenCode, Claude, OpenWebUI, an
eval harness) the same live editor, run **one** HTTP service and point
every client at `http://<host>:9090/mcp`:

```bash
GODOT_MCP_TRANSPORT=http GODOT_MCP_AUTH_TOKEN=<token> \
  uv run ludot-editor-mcp
```

Non-loopback HTTP binds **require** a `GODOT_MCP_AUTH_TOKEN` — the server
refuses to start without one (fail-fast, #226).
:::

## Verify

```bash
GODOT_MCP_BRIDGE_URL=ws://127.0.0.1:9080 uv run ludot-editor-mcp
# logs are JSON on stderr; the MCP protocol is on stdout
```

The server binds the bridge listener immediately — before Ludot exists.
Next: [install the addon](getting-started-addon.md).