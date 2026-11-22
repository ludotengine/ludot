#!/usr/bin/env python3
"""Verify .ludot/.godot project loading and the built-in Ludot MCP installer."""

import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import tomllib

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("engine", type=Path)
args = parser.parse_args()
engine = str(args.engine.resolve())


def run_editor_script(project: Path, script_body: str, env_overrides: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    addon_dir = project / "addons" / "mcp_test"
    addon_dir.mkdir(parents=True, exist_ok=True)
    (addon_dir / "plugin.cfg").write_text(
        '[plugin]\nname="MCP Test"\ndescription="test"\nauthor="Ludot"\nversion="1.0"\nscript="plugin.gd"\n',
        encoding="utf-8",
    )
    (addon_dir / "plugin.gd").write_text(script_body, encoding="utf-8")
    env = os.environ.copy()
    if env_overrides:
        env.update(env_overrides)
    result = subprocess.run(
        [engine, "--headless", "--editor", "--path", str(project), "--quit-after", "20"],
        capture_output=True,
        text=True,
        timeout=90,
        env=env,
    )
    return result


# 1. Verify project.ludot loads, saves back to project.ludot, and supports the MCP installer.
with tempfile.TemporaryDirectory(prefix="ludot-mcp-installer-") as tmp:
    base = Path(tmp)
    project = base / "game"
    project.mkdir()
    (project / "project.ludot").write_text(
        'config_version=5\n'
        '[application]\n'
        'config/name="Ludot MCP Test"\n'
        '[editor_plugins]\n'
        'enabled=PackedStringArray("res://addons/mcp_test/plugin.cfg")\n'
        '[rendering]\n'
        'renderer/rendering_method="gl_compatibility"\n',
        encoding="utf-8",
    )

    fake_home = base / "home"
    fake_xdg = fake_home / ".config"
    fake_bin = base / "bin"
    fake_home.mkdir()
    fake_xdg.mkdir()
    fake_bin.mkdir()

    # Create installed providers in the sandboxed environment:
    # - Claude Code: installed, NOT configured yet (has another server)
    (fake_home / ".claude").mkdir()
    (fake_home / ".claude.json").write_text(
        json.dumps({"mcpServers": {"other": {"command": "other-mcp"}}}, indent=2),
        encoding="utf-8",
    )
    # - Codex: installed, ALREADY configured via legacy godot-editor-mcp
    (fake_home / ".codex").mkdir()
    (fake_home / ".codex" / "config.toml").write_text(
        '[other]\nvalue = 1\n\n[mcp_servers.godot-mcp]\ncommand = "godot-editor-mcp"\nargs = []\n',
        encoding="utf-8",
    )
    # - Gemini CLI: installed, NOT configured yet
    (fake_home / ".gemini").mkdir()
    (fake_home / ".gemini" / "settings.json").write_text(json.dumps({"theme": "dark"}), encoding="utf-8")
    # - Antigravity: installed, NOT configured yet
    (fake_home / ".gemini" / "config").mkdir()
    # - OpenCode: installed, NOT configured yet
    (fake_xdg / "opencode").mkdir()
    (fake_xdg / "opencode" / "opencode.json").write_text(
        json.dumps({"mcp": {"existing": {"type": "local", "command": ["foo"]}}}),
        encoding="utf-8",
    )
    # - Zed: installed with JSONC comments, NOT configured yet
    (fake_xdg / "zed").mkdir()
    (fake_xdg / "zed" / "settings.json").write_text(
        '// Zed settings\n{"theme": "One Dark", /* block comment */}\n',
        encoding="utf-8",
    )
    # - Goose: installed, NOT configured yet
    (fake_xdg / "goose").mkdir()
    (fake_xdg / "goose" / "config.yaml").write_text(
        "GOOSE_PROVIDER: anthropic\nextensions:\n  developer:\n    enabled: true\n",
        encoding="utf-8",
    )
    # - VS Code: installed, NOT configured yet
    (fake_xdg / "Code" / "User").mkdir(parents=True)
    # - Cursor: NOT installed (should not be marked installed)

    # Put a fake ludot-editor-mcp binary in fake_bin so command resolution picks it up.
    fake_mcp_bin = fake_bin / ("ludot-editor-mcp.exe" if sys.platform == "win32" else "ludot-editor-mcp")
    fake_mcp_bin.write_text("#!/bin/sh\n", encoding="utf-8")
    fake_mcp_bin.chmod(0o755)

    report_path = base / "report.json"
    plugin_code = f"""@tool
extends EditorPlugin

func _enter_tree() -> void:
\tcall_deferred("_run")

func _run() -> void:
\tvar report := {{}}
\treport["before_addon"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"get_addon_status")
\treport["before_providers"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"detect_providers")
\treport["resolved"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"resolve_server_command")
\treport["install_addon"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"install_addon")
\treport["after_addon"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"get_addon_status")

\t# Configure several providers with different formats, and verify already-installed codex is skipped.
\tvar cmd := "ludot-editor-mcp"
\tvar args := PackedStringArray()
\treport["cfg_claude"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "claude-code", cmd, args)
\treport["cfg_codex"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "codex", cmd, args)
\treport["cfg_gemini"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "gemini", cmd, args)
\treport["cfg_antigravity"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "antigravity", cmd, args)
\treport["cfg_opencode"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "opencode", cmd, args)
\treport["cfg_zed"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "zed", cmd, args)
\treport["cfg_goose"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "goose", cmd, args)
\treport["cfg_vscode"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "vscode", cmd, args)
\treport["after_providers"] = ClassDB.class_call_static(&"LudotMcpInstaller", &"detect_providers")

\tvar f := FileAccess.open({json.dumps(str(report_path))}, FileAccess.WRITE)
\tf.store_string(JSON.stringify(report))
\tf.close()
\tprint("MCP_INSTALLER_TEST_OK")
"""

    env_overrides = {
        "HOME": str(fake_home),
        "USERPROFILE": str(fake_home),
        "XDG_CONFIG_HOME": str(fake_xdg),
        "APPDATA": str(fake_xdg),
        "CODEX_HOME": str(fake_home / ".codex"),
        "PATH": str(fake_bin),
    }
    res = run_editor_script(project, plugin_code, env_overrides=env_overrides)
    if res.returncode != 0 or "MCP_INSTALLER_TEST_OK" not in res.stdout:
        print(res.stdout, file=sys.stderr)
        print(res.stderr, file=sys.stderr)
        raise SystemExit(f"MCP installer test failed with code {res.returncode}")

    report = json.loads(report_path.read_text(encoding="utf-8"))

    # Addon was not installed before, and IS installed + enabled after.
    assert report["before_addon"]["installed"] is False
    assert report["install_addon"]["ok"] is True
    assert report["install_addon"]["files_written"] > 50
    assert report["after_addon"]["installed"] is True
    assert report["after_addon"]["enabled"] is True
    assert (project / "addons" / "godot_mcp" / "plugin.cfg").is_file()
    assert (project / "addons" / "godot_mcp" / "handlers" / "luau.gd").is_file()
    assert 'name="Ludot Editor MCP - Addon"' in (project / "addons" / "godot_mcp" / "plugin.cfg").read_text(encoding="utf-8")
    # Saved into project.ludot, and did NOT create a stray project.godot.
    assert (project / "project.ludot").is_file()
    assert not (project / "project.godot").exists()
    assert "res://addons/godot_mcp/plugin.cfg" in (project / "project.ludot").read_text(encoding="utf-8")

    # Command resolved to ludot-editor-mcp.
    assert report["resolved"]["command"] == "ludot-editor-mcp"

    # Provider detection before install.
    before = {p["id"]: p for p in report["before_providers"]}
    assert before["claude-code"]["installed"] is True and before["claude-code"]["already_configured"] is False
    assert before["codex"]["installed"] is True and before["codex"]["already_configured"] is True
    assert before["gemini"]["installed"] is True and before["gemini"]["already_configured"] is False
    assert before["antigravity"]["installed"] is True and before["antigravity"]["already_configured"] is False
    assert before["opencode"]["installed"] is True and before["opencode"]["already_configured"] is False
    assert before["zed"]["installed"] is True and before["zed"]["already_configured"] is False
    assert before["goose"]["installed"] is True and before["goose"]["already_configured"] is False
    assert before["vscode"]["installed"] is True and before["vscode"]["already_configured"] is False
    assert before["cursor"]["installed"] is False

    # Codex was already configured, so configure_provider skipped it without duplicating.
    assert report["cfg_codex"]["ok"] is True and report["cfg_codex"]["skipped"] is True
    codex_data = tomllib.loads((fake_home / ".codex" / "config.toml").read_text(encoding="utf-8"))
    assert "godot-mcp" in codex_data["mcp_servers"] and "ludot-mcp" not in codex_data["mcp_servers"]

    # Every other selected provider was merged without clobbering existing entries.
    for key in ("cfg_claude", "cfg_gemini", "cfg_antigravity", "cfg_opencode", "cfg_zed", "cfg_goose", "cfg_vscode"):
        assert report[key]["ok"] is True and report[key]["skipped"] is False, (key, report[key])

    claude_cfg = json.loads((fake_home / ".claude.json").read_text(encoding="utf-8"))
    assert set(claude_cfg["mcpServers"]) == {"other", "ludot-mcp"}
    assert claude_cfg["mcpServers"]["ludot-mcp"]["command"] == "ludot-editor-mcp"

    opencode_cfg = json.loads((fake_xdg / "opencode" / "opencode.json").read_text(encoding="utf-8"))
    assert set(opencode_cfg["mcp"]) == {"existing", "ludot-mcp"}
    assert opencode_cfg["mcp"]["ludot-mcp"] == {"type": "local", "command": ["ludot-editor-mcp"]}

    zed_cfg = json.loads((fake_xdg / "zed" / "settings.json").read_text(encoding="utf-8"))
    assert zed_cfg["theme"] == "One Dark"
    assert zed_cfg["context_servers"]["ludot-mcp"]["command"]["path"] == "ludot-editor-mcp"

    vscode_cfg = json.loads((fake_xdg / "Code" / "User" / "mcp.json").read_text(encoding="utf-8"))
    assert vscode_cfg["servers"]["ludot-mcp"]["type"] == "stdio"
    assert vscode_cfg["servers"]["ludot-mcp"]["command"] == "ludot-editor-mcp"

    agy_cfg = json.loads((fake_home / ".gemini" / "config" / "mcp_config.json").read_text(encoding="utf-8"))
    assert agy_cfg["mcpServers"]["ludot-mcp"]["command"] == "ludot-editor-mcp"

    goose_text = (fake_xdg / "goose" / "config.yaml").read_text(encoding="utf-8")
    assert "developer:" in goose_text and "ludot-mcp:" in goose_text and "ludot-editor-mcp" in goose_text

    # Re-detection after install marks all newly configured providers as already_configured.
    after = {p["id"]: p for p in report["after_providers"]}
    for pid in ("claude-code", "codex", "gemini", "antigravity", "opencode", "zed", "goose", "vscode"):
        assert after[pid]["already_configured"] is True, (pid, after[pid])

    # Project .mcp.json was NOT touched (verifying unselected providers stay untouched).
    assert not (project / ".mcp.json").exists()

# 2. Verify legacy project.godot projects still open cleanly for backwards compatibility.
with tempfile.TemporaryDirectory(prefix="ludot-legacy-project-") as tmp:
    legacy = Path(tmp) / "legacy"
    legacy.mkdir()
    (legacy / "project.godot").write_text('config_version=5\n[application]\nconfig/name="Legacy"\n', encoding="utf-8")
    (legacy / "check.gd").write_text(
        'extends SceneTree\nfunc _init():\n\tassert(ProjectSettings.get_setting("application/config/name") == "Legacy")\n\tprint("LEGACY_GODOT_OK")\n\tquit(0)\n',
        encoding="utf-8",
    )
    res = subprocess.run(
        [engine, "--headless", "--path", str(legacy), "--script", "check.gd"],
        capture_output=True,
        text=True,
        timeout=60,
    )
    if res.returncode != 0 or "LEGACY_GODOT_OK" not in res.stdout:
        print(res.stdout, file=sys.stderr)
        print(res.stderr, file=sys.stderr)
        raise SystemExit(f"Legacy project.godot compatibility check failed with code {res.returncode}")

# 3. Verify addon-only installation leaves all provider configs untouched, and fresh Codex TOML writing works.
with tempfile.TemporaryDirectory(prefix="ludot-mcp-addon-only-") as tmp:
    base = Path(tmp)
    project = base / "game"
    project.mkdir()
    (project / "project.ludot").write_text(
        'config_version=5\n'
        '[editor_plugins]\n'
        'enabled=PackedStringArray("res://addons/mcp_test/plugin.cfg")\n'
        '[rendering]\n'
        'renderer/rendering_method="gl_compatibility"\n',
        encoding="utf-8",
    )
    fake_home = base / "home"
    fake_home.mkdir()
    (fake_home / ".codex").mkdir()
    (fake_home / ".codex" / "config.toml").write_text('model = "gpt-5"\n', encoding="utf-8")
    plugin_code = """@tool
extends EditorPlugin

func _enter_tree() -> void:
\tcall_deferred("_run")

func _run() -> void:
\tvar r: Dictionary = ClassDB.class_call_static(&"LudotMcpInstaller", &"install_addon")
\tassert(r["ok"] == true)
\tvar c: Dictionary = ClassDB.class_call_static(&"LudotMcpInstaller", &"configure_provider", "codex", "uv", PackedStringArray(["run", "--directory", "/opt/ludot/mcp", "ludot-editor-mcp"]))
\tassert(c["ok"] == true and c["skipped"] == false)
\tprint("CODEX_TOML_OK")
"""
    res = run_editor_script(
        project,
        plugin_code,
        env_overrides={
            "HOME": str(fake_home),
            "USERPROFILE": str(fake_home),
            "XDG_CONFIG_HOME": str(fake_home / ".config"),
            "CODEX_HOME": str(fake_home / ".codex"),
            "PATH": "",
        },
    )
    if res.returncode != 0 or "CODEX_TOML_OK" not in res.stdout:
        print(res.stdout, file=sys.stderr)
        print(res.stderr, file=sys.stderr)
        raise SystemExit(f"Codex TOML test failed with code {res.returncode}")
    parsed = tomllib.loads((fake_home / ".codex" / "config.toml").read_text(encoding="utf-8"))
    assert parsed["model"] == "gpt-5"
    assert parsed["mcp_servers"]["ludot-mcp"]["command"] == "uv"
    assert parsed["mcp_servers"]["ludot-mcp"]["args"] == ["run", "--directory", "/opt/ludot/mcp", "ludot-editor-mcp"]

print("Ludot .ludot project format and MCP installer checks passed.")
