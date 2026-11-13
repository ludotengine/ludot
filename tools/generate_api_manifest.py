#!/usr/bin/env python3
"""Emit the nominal engine type database consumed directly by Luau Analysis."""
import argparse
import json
from pathlib import Path
from generate_api_types import casing


def generate(api):
    rows = ['LUDOT_API_1']
    primitives = {'Nil', 'bool', 'int', 'float', 'String', 'StringName', 'NodePath'}
    for entry in api['builtin_classes'] + api['classes']:
        name = entry['name']
        if name in primitives:
            continue
        builtin = entry in api['builtin_classes']
        rows.append('\t'.join(['C', name, entry.get('inherits', ''), str(int(builtin))]))
        operators = {'+': '__add', '-': '__sub', '*': '__mul', '/': '__div'}
        for operator in entry.get('operators', []):
            if operator['name'] in operators and operator.get('right_type'):
                right = 'float' if operator['right_type'] == 'int' else operator['right_type']
                row = '\t'.join(['O', name, operators[operator['name']], right, operator['return_type']])
                if row not in rows:
                    rows.append(row)
        for prop in entry.get('members', []) + entry.get('properties', []):
            renamed = casing(prop['name'], True)
            if renamed.isidentifier():
                rows.append('\t'.join(['P', name, renamed, prop.get('type', 'Variant')]))
        for method in entry.get('methods', []):
            renamed = casing(method['name'])
            if not renamed.isidentifier() or renamed.startswith('__'):
                continue
            result = method.get('return_value', {}).get('type', method.get('return_type', 'void'))
            args = [arg.get('type', 'Variant') + ('?' if 'default_value' in arg else '') for arg in method.get('arguments', [])]
            rows.append('\t'.join(['F', name, renamed, result, str(int(method.get('is_static', False))), str(int(method.get('is_vararg', False))), *args]))
        for constant in entry.get('constants', []):
            rows.append('\t'.join(['K', name, casing(constant['name'], True), constant.get('type', 'int')]))
        for enum in entry.get('enums', []):
            for constant in enum.get('values', []):
                rows.append('\t'.join(['K', name, casing(constant['name'], True), 'int']))
        for signal in entry.get('signals', []):
            rows.append('\t'.join(['P', name, casing(signal['name'], True), 'Signal']))
    for singleton in api.get('singletons', []):
        rows.append('\t'.join(['S', singleton['name'], singleton['type']]))
    return '\n'.join(rows) + '\n'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('api', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    args.output.write_text(generate(json.loads(args.api.read_text())), encoding='utf-8')


if __name__ == '__main__':
    main()
