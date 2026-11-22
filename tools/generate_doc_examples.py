#!/usr/bin/env python3
"""Embed the shared Luau examples without a runtime checkout dependency."""
import json
from pathlib import Path
import sys
from ludot_lsp.documentation import Documentation


def generate(target, source):
    data = Documentation(Path(source).resolve().parents[1]).examples
    payload = json.dumps(data, ensure_ascii=False).encode('utf-8') + b'\0'
    # A numeric byte array avoids MSVC's limit on individual string literals.
    lines = ['#pragma once', '#include <cstdint>', 'static const uint8_t ludot_doc_examples[] = {']
    for offset in range(0, len(payload), 32):
        lines.append('\t' + ', '.join(str(byte) for byte in payload[offset:offset + 32]) + ',')
    lines += ['};', '']
    Path(target).write_text('\n'.join(lines), encoding='utf-8')


def scons_action(target, source, env):
    generate(str(target[0]), str(source[0]))
    return 0


if __name__ == '__main__':
    generate(*sys.argv[1:])
