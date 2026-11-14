"""Contract tests for the Ludot Luau tools.

Each tool routes to its cmd_luau_* command, maps the response to its typed model,
and turns the addon's before/after texts into a unified diff.
"""

from __future__ import annotations

import pytest
from fastmcp import Client, FastMCP

from mcp_server.bridge import Bridge
from mcp_server.config import ServerConfig
from mcp_server.models.envelope import CommandEnvelope, ResponseEnvelope
from mcp_server.server import create_server
from tests.fakes import FakeAddonConnection, connector_for

pytestmark = pytest.mark.asyncio

BAD = "const x: number = 'oops'\nprint(  x  )\n"
GOOD = 'const x: number = "oops"\nprint(x)\n'


def _responder(cmd: CommandEnvelope) -> ResponseEnvelope | None:
    params = cmd.params or {}
    match cmd.command:
        case "cmd_luau_api":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "query": params["symbol"],
                    "found": True,
                    "kind": "method",
                    "name": "Node.addChild",
                    "godot_name": "add_child",
                    "type": "object:addChild(node: Node): ()",
                    "casing": {"method": "camelCase", "property": "PascalCase"},
                    "description": "Adds a child node.",
                },
            )
        case "cmd_luau_enums":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "query": params["query"],
                    "enums": [{"name": "Enum.MouseMode", "items": [{"name": "Captured", "value": 2}]}],
                },
            )
        case "cmd_luau_resolve_module":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "path": params["path"],
                    "from_path": params["from_path"],
                    "resolved": "res://helpers/mathutils.luau",
                    "found": True,
                },
            )
        case "cmd_luau_inspect":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "script_path": params["script_path"],
                    "line": params["line"],
                    "column": params["column"],
                    "symbol": "getNode",
                    "help": {"signature": "getNode(path: string) -> Node"},
                    "completions": [{"label": "getNode", "kind": 2}],
                    "completions_truncated": False,
                },
            )
        case "cmd_luau_diagnose":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "script_path": params["script_path"],
                    "ok": False,
                    "errors": [{"line": 1, "column": 19, "message": "Expected this to be 'number'"}],
                    "warnings": [{"line": 1, "end_line": 1, "code": "unused_variable", "message": "x is unused"}],
                    "format": {"ok": True, "changed": True, "error": None},
                    "original": BAD,
                    "formatted": GOOD,
                },
            )
        case "cmd_luau_format":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "script_path": params["script_path"],
                    "changed": True,
                    "original": BAD,
                    "formatted": GOOD,
                    "receipt": {"status": "noop" if params["dry_run"] else "updated", "changes": [], "truncated": False},
                },
            )
        case "cmd_luau_create_script":
            return ResponseEnvelope.success(
                cmd.id,
                {
                    "script_path": params["script_path"],
                    "created": not params["dry_run"],
                    "content": "class Mover extends Node3D = {\n}\n",
                    "templates": [{"name": "Empty", "inherit": "Object", "description": "No callbacks."}],
                    "previous_existed": False,
                },
            )
    return ResponseEnvelope.failure(cmd.id, "VALIDATION_ERROR", "unexpected command")


def _build(conn: FakeAddonConnection) -> FastMCP:
    bridge = Bridge(ServerConfig().bridge, connector=connector_for(conn))
    return create_server(ServerConfig(), bridge=bridge)


async def _enable_scripts(client: Client) -> None:
    await client.call_tool("godot_enable_toolset", {"category": "scripts"})


async def test_luau_api_routes_symbol() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        result = await client.call_tool("godot_inspection_luau_api", {"symbol": "Node.add_child"})
    assert conn.last_command().params == {"symbol": "Node.add_child"}
    docs = result.structured_content
    assert docs["found"] is True and docs["name"] == "Node.addChild"
    assert docs["godot_name"] == "add_child"  # extra fields pass through
    assert docs["casing"]["method"] == "camelCase"


async def test_luau_enums_routes_query() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        result = await client.call_tool("godot_inspection_luau_enums", {"query": "MouseMode"})
    assert conn.last_command().params == {"query": "MouseMode"}
    assert result.structured_content["enums"][0]["items"][0] == {"name": "Captured", "value": 2}


async def test_luau_resolve_module_defaults_from_path() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        result = await client.call_tool("godot_inspection_luau_resolve_module", {"module": "@helpers/mathutils"})
    assert conn.last_command().params == {"path": "@helpers/mathutils", "from_path": "res://main.luau"}
    assert result.structured_content["resolved"] == "res://helpers/mathutils.luau"


async def test_luau_inspect_passes_position() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        result = await client.call_tool(
            "godot_inspection_luau_inspect", {"script_path": "res://player.luau", "line": 3, "column": 9}
        )
    assert conn.last_command().params == {
        "script_path": "res://player.luau",
        "line": 3,
        "column": 9,
        "max_completions": 100,
    }
    assert result.structured_content["completions"][0]["label"] == "getNode"


async def test_luau_diagnose_returns_errors_warnings_and_diff() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        await _enable_scripts(client)
        result = await client.call_tool("godot_scripts_luau_diagnose", {"script_path": "res://bad.luau"})
    body = result.structured_content
    assert body["ok"] is False and body["errors"][0]["column"] == 19
    assert body["warnings"][0]["code"] == "unused_variable"
    assert body["format"]["changed"] is True
    assert "-print(  x  )" in body["format"]["diff"] and "+print(x)" in body["format"]["diff"]
    assert "original" not in body and "formatted" not in body


async def test_get_parse_errors_uses_the_luau_checker() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        await _enable_scripts(client)
        result = await client.call_tool("godot_scripts_get_parse_errors", {"script_path": "res://bad.luau"})
    assert conn.last_command().command == "cmd_luau_diagnose"
    body = result.structured_content
    assert body["ok"] is False and body["errors"][0]["line"] == 1


async def test_luau_format_dry_run_and_real_run() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        await _enable_scripts(client)
        preview = await client.call_tool("godot_scripts_luau_format", {"script_path": "res://bad.luau", "dry_run": True})
        assert conn.last_command().params == {"script_path": "res://bad.luau", "dry_run": True}
        applied = await client.call_tool("godot_scripts_luau_format", {"script_path": "res://bad.luau"})
    assert preview.structured_content["dry_run"] is True and "+print(x)" in preview.structured_content["diff"]
    assert applied.structured_content["dry_run"] is False
    assert applied.structured_content["receipt"]["status"] == "updated"


async def test_create_luau_script_lists_templates() -> None:
    conn = FakeAddonConnection(responder=_responder)
    async with Client(_build(conn)) as client:
        await _enable_scripts(client)
        result = await client.call_tool(
            "godot_scripts_create_luau_script",
            {"script_path": "res://mover.luau", "base_class": "Node3D", "dry_run": True},
        )
    assert conn.last_command().params["base_class"] == "Node3D"
    body = result.structured_content
    assert body["created"] is False and body["dry_run"] is True
    assert body["templates"][0]["name"] == "Empty"
    assert body["content"].startswith("class Mover extends Node3D")


async def test_luau_tool_safety_classes() -> None:
    async with Client(_build(FakeAddonConnection(responder=_responder))) as client:
        await _enable_scripts(client)
        tools = {tool.name: tool for tool in await client.list_tools()}
    for name in (
        "godot_inspection_luau_api",
        "godot_inspection_luau_enums",
        "godot_inspection_luau_resolve_module",
        "godot_inspection_luau_inspect",
        "godot_scripts_luau_diagnose",
    ):
        assert tools[name].meta.get("safety_class") == "read_only", name
    for name in ("godot_scripts_luau_format", "godot_scripts_create_luau_script"):
        assert tools[name].meta.get("safety_class") == "mutating", name
