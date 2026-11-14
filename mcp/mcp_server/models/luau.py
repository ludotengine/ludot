"""Typed results for the Ludot Luau tools.

The addon answers through the editor's own LuauScript statics, so names, checks,
and formatting match what the editor does with the user's settings.
"""

from __future__ import annotations

from typing import Any, Literal

from pydantic import BaseModel, ConfigDict, Field

from mcp_server.models.receipt import ChangeReceipt


class LuauError(BaseModel):
    """A checker or syntax error. Lines and columns are 1-based."""

    line: int
    column: int = 0
    message: str


class LuauWarning(BaseModel):
    """A lint warning: naming rules, Selene, match exhaustiveness, or signal declarations."""

    line: int
    end_line: int | None = None
    code: str = ""
    message: str


class LuauFormatState(BaseModel):
    """Whether StyLua, with the user's editor settings, would change the file."""

    ok: bool = False
    changed: bool = False
    error: str | None = None
    diff: str | None = None


class LuauDiagnosis(BaseModel):
    """Everything the editor reports for a Luau script, without running it."""

    script_path: str
    ok: bool
    errors: list[LuauError] = Field(default_factory=list)
    warnings: list[LuauWarning] = Field(default_factory=list)
    format: LuauFormatState = Field(default_factory=LuauFormatState)


class LuauFormatResult(BaseModel):
    """A StyLua format. ``diff`` is the change; ``dry_run`` previews without writing."""

    script_path: str
    changed: bool = False
    diff: str = ""
    dry_run: bool = False
    receipt: ChangeReceipt | None = None
    reversible_by: Literal["undo", "vcs", "none"] = "undo"


class LuauTemplate(BaseModel):
    name: str
    inherit: str = ""
    description: str = ""


class CreateLuauScriptResult(BaseModel):
    """A new Luau script from one of Ludot's built-in templates."""

    script_path: str
    created: bool = False
    content: str = ""
    templates: list[LuauTemplate] = Field(default_factory=list)
    previous_existed: bool = False
    dry_run: bool = False
    receipt: ChangeReceipt | None = None
    reversible_by: Literal["undo", "vcs", "none"] = "undo"


class LuauApiDocs(BaseModel):
    """The engine API as Luau code spells it under the project's casing settings.

    ``kind`` is class, method, callback, property, signal, constant, library,
    function, enum, or enum_item. Classes list their own ``members``; libraries
    (task, Signal, Connection, Enum) list theirs. ``found=false`` comes with
    ``suggestions``.
    """

    model_config = ConfigDict(extra="allow")

    query: str
    found: bool = False
    kind: str | None = None
    name: str | None = None
    casing: dict[str, Any] = Field(default_factory=dict)
    description: str | None = None
    members: list[dict[str, Any]] | None = None
    suggestions: list[Any] | None = None


class LuauEnumList(BaseModel):
    """Engine enums under ``Enum``. An exact name lists every item with its value."""

    query: str
    enums: list[dict[str, Any]] = Field(default_factory=list)


class LuauModuleResolution(BaseModel):
    """The file an import path resolves to, by the engine's rules."""

    path: str
    from_path: str
    resolved: str | None = None
    found: bool = False


class LuauInspection(BaseModel):
    """Hover help for the identifier at a position, and the completions there."""

    script_path: str
    line: int
    column: int
    symbol: str = ""
    help: dict[str, Any] = Field(default_factory=dict)
    completions: list[dict[str, Any]] = Field(default_factory=list)
    completions_truncated: bool = False
