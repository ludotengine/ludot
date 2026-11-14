"""Ludot Luau tools: checks, formatting, templates, and the engine API in Luau spelling.

Every tool routes to the addon, which calls the editor's own LuauScript statics, so
results match the editor with the user's settings (API casing, naming rules,
Selene, and StyLua). Nothing runs the script. The read-only lookups live in the
always-on ``inspection`` toolset; diagnose, format, and create live in ``scripts``.
On stock Godot every tool fails with ``PRECONDITION_FAILED: required=ludot_editor``.
"""

from __future__ import annotations

import difflib
from typing import Annotated, Any

from fastmcp import FastMCP
from pydantic import Field

from mcp_server.bridge import Bridge
from mcp_server.categories import INSPECTION_TAG, SCRIPTS_TAG
from mcp_server.models.luau import (
    CreateLuauScriptResult,
    LuauApiDocs,
    LuauDiagnosis,
    LuauEnumList,
    LuauFormatResult,
    LuauInspection,
    LuauModuleResolution,
)
from mcp_server.paths import ResPath
from mcp_server.safety import MUTATING, READ_ONLY
from mcp_server.tools._route import route

INSPECTION = {INSPECTION_TAG}
SCRIPTS = {SCRIPTS_TAG}


def unified_diff(path: str, before: str, after: str) -> str:
    """A unified diff from the file's current text to the formatted text."""
    return "".join(
        difflib.unified_diff(
            before.splitlines(keepends=True),
            after.splitlines(keepends=True),
            fromfile=path,
            tofile=path + " (formatted)",
        )
    )


def register_luau(mcp: FastMCP, bridge: Bridge) -> None:
    """Register the Ludot Luau tools."""

    @mcp.tool(meta=READ_ONLY, tags=INSPECTION)
    async def luau_api(symbol: str) -> LuauApiDocs:
        """Engine API in Luau spelling with Godot's docs: ``Node``,
        ``Node.addChild``, ``Vector3``, ``task.wait``, ``Signal``, ``Enum.MouseMode``."""
        return LuauApiDocs(**await route(bridge, "cmd_luau_api", {"symbol": symbol}))

    @mcp.tool(meta=READ_ONLY, tags=INSPECTION)
    async def luau_enums(query: str = "") -> LuauEnumList:
        """List ``Enum`` items by exact name, or search enum names."""
        return LuauEnumList(**await route(bridge, "cmd_luau_enums", {"query": query}))

    @mcp.tool(meta=READ_ONLY, tags=INSPECTION)
    async def luau_resolve_module(module: str, from_path: ResPath = "res://main.luau") -> LuauModuleResolution:
        """Resolve an import path like ``@helpers/mathutils`` to its file,
        by the engine's rules."""
        params = {"path": module, "from_path": from_path}
        return LuauModuleResolution(**await route(bridge, "cmd_luau_resolve_module", params))

    @mcp.tool(meta=READ_ONLY, tags=INSPECTION)
    async def luau_inspect(
        script_path: ResPath,
        line: Annotated[int, Field(ge=1)],
        column: Annotated[int, Field(ge=1)],
        max_completions: Annotated[int, Field(ge=0, le=1000)] = 100,
    ) -> LuauInspection:
        """Hover help and completions at a 1-based position in a ``.luau`` file."""
        params = {
            "script_path": script_path,
            "line": line,
            "column": column,
            "max_completions": max_completions,
        }
        return LuauInspection(**await route(bridge, "cmd_luau_inspect", params))

    @mcp.tool(meta=READ_ONLY, tags=SCRIPTS)
    async def luau_diagnose(script_path: ResPath) -> LuauDiagnosis:
        """Check a ``.luau`` script: checker ``errors``, lint and Selene
        ``warnings``, and the StyLua ``format`` diff."""
        body = await route(bridge, "cmd_luau_diagnose", {"script_path": script_path})
        original = body.pop("original", None)
        formatted = body.pop("formatted", None)
        if original is not None and formatted is not None:
            body.setdefault("format", {})["diff"] = unified_diff(script_path, original, formatted)
        return LuauDiagnosis(**body)

    @mcp.tool(meta=MUTATING, tags=SCRIPTS)
    async def luau_format(script_path: ResPath, dry_run: bool = False) -> LuauFormatResult:
        """Format a ``.luau`` script with StyLua and the user's settings,
        through undo. ``dry_run`` returns the ``diff`` only."""
        params: dict[str, Any] = {"script_path": script_path, "dry_run": dry_run}
        body = await route(bridge, "cmd_luau_format", params)
        diff = unified_diff(script_path, body.pop("original", ""), body.pop("formatted", ""))
        return LuauFormatResult(**body, diff=diff, dry_run=dry_run)

    @mcp.tool(meta=MUTATING, tags=SCRIPTS)
    async def create_luau_script(
        script_path: ResPath,
        base_class: str = "Node",
        class_name: str = "",
        template: str = "",
        dry_run: bool = False,
    ) -> CreateLuauScriptResult:
        """Create a ``.luau`` script from a built-in template for
        ``base_class``; ``dry_run`` shows content and ``templates``."""
        params = {
            "script_path": script_path,
            "base_class": base_class,
            "class_name": class_name,
            "template": template,
            "dry_run": dry_run,
        }
        body = await route(bridge, "cmd_luau_create_script", params)
        return CreateLuauScriptResult(**body, dry_run=dry_run)
