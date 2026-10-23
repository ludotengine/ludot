#!/usr/bin/env python3
"""Generate Luau declarations from this fork's --dump-extension-api output."""
import argparse
import json
from pathlib import Path
import re


def casing(name, property=False):
    private = name.startswith('_')
    parts = name.lstrip('_').split('_')
    result = ''.join(part[:1].upper() + part[1:] for part in parts)
    if not property:
        result = result[:1].lower() + result[1:]
    return ('_' if private else '') + result


def generate(api, selected=None):
    classes = {entry['name']: entry for entry in api['classes']}
    if selected is not None:
        included = set(selected)
        for name in list(included):
            current = name
            while current in classes and classes[current].get('inherits'):
                current = classes[current]['inherits']
                included.add(current)
        unknown = included - classes.keys()
        if unknown:
            raise ValueError('Unknown engine classes: ' + ', '.join(sorted(unknown)))
        classes = {name: entry for name, entry in classes.items() if name in included}
    builtins = {entry['name']: entry for entry in api['builtin_classes'] if entry['name'] not in {'Nil', 'bool', 'int', 'float', 'String', 'StringName', 'NodePath'}}
    known = classes.keys() | builtins.keys()
    def type_name(name):
        if name is None or name == 'void': return '()'
        if name in ('int', 'float'): return 'number'
        if name == 'bool': return 'boolean'
        if name in ('String', 'StringName', 'NodePath'): return 'string'
        if name == 'Nil': return 'nil'
        if name.startswith(('enum::', 'bitfield::')): return 'number'
        if name.startswith('typedarray::'): return 'Array'
        return name if name in known else 'any'
    output = ['-- Generated from Ludot reflection. Run tools/generate_api_types.py to update.', '']
    done = set()
    def emit(name):
        if name in done: return
        entry = classes.get(name, builtins.get(name))
        parent = entry.get('inherits')
        if parent in classes: emit(parent)
        done.add(name)
        output.append(f'declare extern type {name}' + (f' extends {parent}' if parent else '') + ' with')
        names = set()
        for prop in entry.get('properties', []) + entry.get('members', []):
            renamed = casing(prop['name'], True)
            if not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', renamed): continue
            if renamed in names: raise ValueError(f'Property name collision: {name}.{renamed}')
            names.add(renamed)
            output.append(f"    {renamed}: {type_name(prop.get('type', 'Variant'))}")
        for method in entry.get('methods', []):
            renamed = casing(method['name'])
            if not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', renamed) or renamed in {'__init', 'and', 'or', 'not', 'type', 'end'}: continue
            if renamed in names: continue
            names.add(renamed)
            args = ['self']
            for index, arg in enumerate(method.get('arguments', [])):
                annotation = type_name(arg.get('type', 'Variant'))
                if 'default_value' in arg: annotation = f'({annotation})?'
                args.append(f'arg{index}: {annotation}')
            if method.get('is_vararg'): args.append('...: any')
            returns = type_name(method.get('return_value', {}).get('type', method.get('return_type')))
            output.append(f"    function {renamed}({', '.join(args)}): {returns}")
        output.extend(['end', ''])
    for name in [*builtins, *classes]: emit(name)
    for singleton in api.get('singletons', []):
        if singleton['type'] in known: output.append(f"declare {singleton['name']}: {singleton['type']}")
    return '\n'.join(output) + '\n'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('api', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--classes', help='Comma-separated class subset, including its ancestors')
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(generate(json.loads(args.api.read_text()), args.classes.split(',') if args.classes else None))
if __name__ == '__main__': main()
