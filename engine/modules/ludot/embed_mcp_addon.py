#!/usr/bin/env python3
"""Pack mcp/godot/addons/godot_mcp into a zlib-compressed C++ header for the editor."""

from pathlib import Path
import struct
import sys
import zlib


def build_header(target_path: Path, addon_dir: Path) -> None:
    files = sorted(
        path
        for path in addon_dir.rglob("*")
        if path.is_file()
        and not path.name.endswith(".import")
        and "__pycache__" not in path.parts
        and not any(part.startswith(".") for part in path.relative_to(addon_dir).parts)
    )
    payload = bytearray()
    payload.extend(struct.pack("<I", len(files)))
    for path in files:
        rel = path.relative_to(addon_dir).as_posix().encode("utf-8")
        data = path.read_bytes()
        payload.extend(struct.pack("<I", len(rel)))
        payload.extend(rel)
        payload.extend(struct.pack("<I", len(data)))
        payload.extend(data)

    compressed = zlib.compress(bytes(payload), level=9)
    target_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "#pragma once",
        "#include <cstddef>",
        "#include <cstdint>",
        "",
        f"static constexpr size_t LUDOT_MCP_ADDON_RAW_SIZE = {len(payload)};",
        f"static constexpr size_t LUDOT_MCP_ADDON_COMPRESSED_SIZE = {len(compressed)};",
        "static const uint8_t LUDOT_MCP_ADDON_ZLIB[] = {",
    ]
    for offset in range(0, len(compressed), 16):
        chunk = ", ".join(str(byte) for byte in compressed[offset : offset + 16])
        lines.append(f"\t{chunk},")
    lines.append("};")
    lines.append("")
    target_path.write_text("\n".join(lines), encoding="utf-8")


def scons_action(target, source, env):
    addon_dir = Path(env["LUDOT_MCP_ADDON_DIR"])
    build_header(Path(str(target[0])), addon_dir)
    return 0


if __name__ == "__main__":
    build_header(Path(sys.argv[1]), Path(sys.argv[2]))
