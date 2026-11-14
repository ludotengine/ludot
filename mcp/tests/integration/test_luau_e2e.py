"""End-to-end Luau tools: live Python bridge ↔ live Ludot editor.

Writes a script with a type error, a Selene warning, and bad formatting, then checks
that diagnose reports all three through the editor's own analyzer, lint, and
StyLua, that format fixes the layout, and that the API, enum, and module lookups
answer in Luau spelling. Skipped without a Godot binary; on stock Godot (no
LuauScript) the test checks the precondition failure instead.
"""

from __future__ import annotations

import asyncio
import os
import subprocess
import uuid

from mcp_server.bridge import Bridge
from mcp_server.config import BridgeConfig
from tests.integration._godot import (
    GODOT_BIN,
    GODOT_PROJECT,
    e2e_bridge_url,
    serve_and_await_editor,
)

import pytest

pytestmark = pytest.mark.skipif(GODOT_BIN is None, reason="Godot binary not installed")

BRIDGE_URL = e2e_bridge_url()

# Line 3 has a type error, line 4 an unused variable (Selene), line 5 bad spacing.
SOURCE = """class LuauProbe extends Node = {
\tfunction _ready()
\t\tconst count: number = "three"
\t\tlocal unusedValue = 5
\t\tprint(  count   )
\tend
}
"""


async def _run_checks(script_path: str) -> None:
    bridge = Bridge(BridgeConfig(url=BRIDGE_URL))
    if not await serve_and_await_editor(bridge):
        raise AssertionError("the addon never connected to the bridge")
    try:
        conventions = await bridge.send("cmd_get_ludot_conventions")
        assert conventions.ok and conventions.result is not None, conventions
        if not conventions.result["is_ludot"]:
            refused = await bridge.send("cmd_luau_enums", {"query": "MouseMode"})
            assert refused.ok is False and refused.error == "PRECONDITION_FAILED"
            return

        written = await bridge.send("cmd_write_script", {"script_path": script_path, "content": SOURCE})
        assert written.ok, written

        diagnosis = await bridge.send("cmd_luau_diagnose", {"script_path": script_path})
        assert diagnosis.ok and diagnosis.result is not None, diagnosis
        body = diagnosis.result
        assert body["ok"] is False
        assert any(error["line"] == 3 and "number" in error["message"] for error in body["errors"]), body
        assert any(warning["line"] == 4 and warning["code"] == "unused_variable" for warning in body["warnings"]), body
        assert body["format"]["changed"] is True, body["format"]
        assert "print(count)" in body["formatted"]

        preview = await bridge.send("cmd_luau_format", {"script_path": script_path, "dry_run": True})
        assert preview.ok and preview.result["changed"] is True, preview
        formatted = await bridge.send("cmd_luau_format", {"script_path": script_path, "dry_run": False})
        assert formatted.ok and formatted.result["changed"] is True, formatted
        for _ in range(40):
            again = await bridge.send("cmd_luau_diagnose", {"script_path": script_path})
            if again.ok and not again.result["format"]["changed"]:
                break
            await asyncio.sleep(0.1)
        assert again.ok and again.result["format"]["changed"] is False, again
        assert any(error["line"] == 3 for error in again.result["errors"]), again

        api = await bridge.send("cmd_luau_api", {"symbol": "Node.add_child"})
        assert api.ok and api.result["found"] is True, api
        assert api.result["name"] == "Node.addChild" and "node: Node" in api.result["type"], api.result
        node = await bridge.send("cmd_luau_api", {"symbol": "Node"})
        members = {member["name"]: member for member in node.result["members"]}
        assert members["TreeExiting"]["type"] == "Signal<>", members["TreeExiting"]
        assert members["ChildEnteredTree"]["type"] == "Signal<Node>", members["ChildEnteredTree"]
        signal = await bridge.send("cmd_luau_api", {"symbol": "Signal.connect"})
        assert signal.ok and signal.result["found"] is True and "Connection" in signal.result["signature"], signal

        enums = await bridge.send("cmd_luau_enums", {"query": "MouseMode"})
        items = {item["name"]: item["value"] for item in enums.result["enums"][0]["items"]}
        assert items["Captured"] == 2, items

        module = await bridge.send("cmd_luau_resolve_module", {"path": "./nothing_here", "from_path": script_path})
        assert module.ok and module.result["found"] is False, module

        inspected = await bridge.send("cmd_luau_inspect", {"script_path": script_path, "line": 5, "column": 4})
        assert inspected.ok and inspected.result["symbol"] == "print", inspected

        created = await bridge.send(
            "cmd_luau_create_script", {"script_path": script_path.replace(".luau", "_new.luau"), "base_class": "CharacterBody3D", "dry_run": True}
        )
        assert created.ok and created.result["content"].startswith("class "), created
        assert any(template["name"] for template in created.result["templates"]), created
    finally:
        await bridge.send("cmd_delete_resource_file", {"path": script_path, "confirm": True})
        await bridge.close()


def test_live_editor_luau_tools() -> None:
    assert GODOT_BIN is not None
    script_path = f"res://tmp_luau_probe_{uuid.uuid4().hex[:8]}.luau"
    editor = subprocess.Popen(
        [GODOT_BIN, "--headless", "--editor", "--path", str(GODOT_PROJECT)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        env={**os.environ, "GODOT_MCP_BRIDGE_URL": BRIDGE_URL},
    )
    try:
        asyncio.run(_run_checks(script_path))
    finally:
        editor.terminate()
        try:
            editor.wait(timeout=10)
        except subprocess.TimeoutExpired:
            editor.kill()
