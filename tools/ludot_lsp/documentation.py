"""Source documentation shared by the editor, language server, and manual.

Examples are authored Luau. Only explicitly marked identifiers are renamed;
strings, comments, callbacks, and user API names keep their meaning.
"""

import json
from pathlib import Path
import re
import textwrap
import xml.etree.ElementTree as ET


ROOT = Path(__file__).resolve().parents[2]


def name_case(name, style):
    if style == 'Any':
        return name
    prefix = name[:len(name) - len(name.lstrip('_'))]
    words = re.sub(r'([A-Z])([A-Z][a-z])', r'\1_\2', name.lstrip('_'))
    words = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', words).lower().split('_')
    if style == 'snake_case':
        return prefix + '_'.join(words)
    if style == 'SCREAMING_SNAKE_CASE':
        return prefix + '_'.join(words).upper()
    value = ''.join(word.capitalize() for word in words)
    return prefix + (value[:1].lower() + value[1:] if style == 'camelCase' else value)


def example(key, examples, casing=('camelCase', 'PascalCase'), naming=None, indent='\t'):
    if key.startswith('Signal.') and key not in examples:
        key = 'Signal'
    entry = examples.get(key)
    if entry is None:
        return ''
    styles = {
        'api_method': casing[0], 'api_property': casing[1], 'api_callback': casing[0],
        'class': 'PascalCase', 'property': 'PascalCase',
        'constant': 'camelCase', 'variable': 'camelCase',
        'function': 'camelCase', 'method': 'camelCase',
    }
    styles.update(naming or {})
    names = {
        token: token if styles.get(kind, 'Any') == 'Any' else name_case(native, styles[kind])
        for token, (kind, native) in entry['names'].items()
    }
    # Authored examples use ordinary quoted strings. Match strings and comments
    # before identifiers so example messages and node paths are never renamed.
    def rename(match):
        token = match[0]
        if token not in names:
            return token
        kind = entry['names'][token][0]
        previous = entry['code'][:match.start()].rstrip()[-1:]
        if kind == 'api_property' and previous != '.':
            return token
        if kind == 'api_method' and previous not in ('.', ':'):
            return token
        return names[token]

    code = re.sub(
        r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|--[^\n]*|\b[A-Za-z_][A-Za-z_0-9]*\b',
        rename,
        entry['code'],
    )
    return '\n'.join(
        indent * (len(line) - len(line.lstrip('\t'))) + line.lstrip('\t')
        for line in code.splitlines()
    )


def select_examples(text, code):
    """Keep prose, replacing foreign-language blocks with authored Luau only."""
    pattern = r'\[(codeblocks|codeblock|gdscript|csharp)(?:\s[^\]]*)?\]([\s\S]*?)\[/\1\]'
    inserted = False

    def replace(match):
        nonlocal inserted
        if match[0].startswith('[codeblock lang=luau]'):
            inserted = True
            return match[0]
        if not inserted and code:
            inserted = True
            return '[codeblock lang=luau]\n' + code + '\n[/codeblock]'
        return '[i]A Luau translation of this example is not available yet.[/i]'

    text = re.sub(pattern, replace, text)
    if code and not inserted:
        text += '\n[codeblock lang=luau]\n' + code + '\n[/codeblock]'
    return text


class Documentation:
    def __init__(self, root=ROOT, data=None):
        if data is not None:
            self.classes, self.examples = data['classes'], data['examples']
            return
        self.examples = json.loads((root / 'docs/luau_examples.json').read_text())
        self.classes = {}
        paths = sorted((root / 'engine/doc/classes').glob('*.xml'))
        paths += sorted((root / 'engine/modules').glob('*/doc_classes/*.xml'))
        for path in paths:
            tree = ET.parse(path).getroot()
            if tree.tag != 'class':
                continue
            entry = {
                'parent': tree.get('inherits', ''),
                'description': tree.findtext('description', ''),
                'brief': tree.findtext('brief_description', ''),
                'members': {},
            }
            for kind, query in [
                ('method', 'methods/method'),
                ('property', 'members/member'),
                ('signal', 'signals/signal'),
            ]:
                for node in tree.findall(query):
                    entry['members'][node.get('name')] = {
                        'kind': kind,
                        'description': (
                            node.findtext('description', '')
                            if kind == 'method' else ''.join(node.itertext())
                        ),
                        'arguments': [param.attrib for param in node.findall('param')],
                    }
            self.classes[tree.get('name')] = entry
            for native, member in entry['members'].items():
                if member['kind'] != 'signal':
                    continue
                key = tree.get('name') + '.' + native
                parameters = [param['name'] for param in member['arguments']]
                arguments = ', '.join(parameters)
                primitives = {
                    'int': 'number', 'float': 'number', 'bool': 'boolean',
                    'String': 'string', 'StringName': 'string', 'Variant': 'any',
                }
                typed_arguments = ', '.join(
                    param['name'] + ': ' + (
                        'Array' if param['type'].endswith('[]')
                        else primitives.get(param['type'], param['type'])
                    )
                    for param in member['arguments']
                )
                signal = name_case(native, 'PascalCase')
                self.examples.setdefault(key, {
                    'generated': True,
                    'code': (
                        f"const function watch(object: {tree.get('name')})\n"
                        f'\tconst function onSignal({typed_arguments})\n'
                        f'\t\tprint({arguments})\n'
                        '\tend\n\n'
                        f'\tconst connection = object.{signal}:connect(onSignal)\n'
                        '\treturn connection\n'
                        'end'
                    ),
                    'names': {
                        'watch': ['function', 'watch'],
                        'onSignal': ['function', 'on_signal'],
                        'object': ['variable', 'object'],
                        'connection': ['constant', 'connection'],
                        signal: ['api_property', native],
                        'connect': ['api_method', 'connect'],
                        **{name: ['variable', name] for name in parameters},
                    },
                })
        signal = self.examples['Signal']
        self.classes['Signal'] = {
            'parent': '',
            'brief': '',
            'description': signal['description'],
            'members': {
                name: {'kind': 'method', 'description': description, 'arguments': []}
                for name, description in signal['members'].items()
            },
        }
        self.classes['Connection'] = {
            'parent': '', 'brief': '', 'description': 'A Luau signal connection returned by connect or once.',
            'members': {
                'disconnect': {'kind': 'method', 'description': 'Disconnects the handler. Calling it twice does nothing.', 'arguments': []},
                'connected': {'kind': 'property', 'description': 'Whether the handler is still connected.', 'arguments': []},
            },
        }

    def resolve(self, owner, member, casing):
        while owner in self.classes:
            entry = self.classes[owner]
            for native, value in entry['members'].items():
                style = casing[0] if value['kind'] == 'method' else casing[1]
                if name_case(native, style) == member:
                    return owner, native, value
            owner = entry['parent']
        return None

    def markdown(self, symbol, casing, naming=None, indent='\t'):
        owner, member = symbol.get('class', ''), symbol.get('member', '')
        if owner not in self.classes:
            return ''
        entry = self.classes[owner]
        text = (
            entry['brief'] + '\n' + entry['description']
            if not member else entry['members'].get(member, {}).get('description', '')
        )
        code = example(owner + ('.' + member if member else ''), self.examples, casing, naming, indent)
        text = select_examples(textwrap.dedent(text).strip(), code)
        snippets = []

        def block(match):
            snippets.append('\n```luau\n' + match[1].strip('\n') + '\n```\n')
            return f'LUDOTBLOCK{len(snippets) - 1}END'

        text = re.sub(r'\[codeblock lang=luau\]([\s\S]*?)\[/codeblock\]', block, text)
        def reference(match):
            kind, target = match[1], match[2]
            if kind in ('method', 'member', 'signal'):
                owner, separator, native = target.rpartition('.')
                target = (owner + separator if separator else '') + name_case(
                    native, casing[0] if kind == 'method' else casing[1]
                )
            return '`' + target + '`'

        text = re.sub(r'\[(method|member|signal|param|constant|enum) ([^\]]+)\]', reference, text)
        text = text.replace('[code]', '`').replace('[/code]', '`').replace('[b]', '**').replace('[/b]', '**')
        text = re.sub(r'\[url[^\]]*\](.*?)\[/url\]', r'\1', text, flags=re.S)
        text = re.sub(r'\[/?(?:i|codeblocks)\]', '', text)
        text = re.sub(r'\[([A-Za-z_][\w]*)\]', r'`\1`', text)
        for index, snippet in enumerate(snippets):
            text = text.replace(f'LUDOTBLOCK{index}END', snippet)
        return text.strip()
