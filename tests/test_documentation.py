import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from ludot_lsp.documentation import Documentation, example, select_examples


class DocumentationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.docs = Documentation()

    def test_examples_adapt_identifiers_without_changing_messages(self):
        code = example('Button', self.docs.examples, ('snake_case', 'camelCase'), {'constant': 'SCREAMING_SNAKE_CASE'}, '  ')
        self.assertIn('    const BUTTON = Button.new()', code)
        self.assertIn('BUTTON.text = "Click me"', code)
        self.assertIn('BUTTON.pressed:connect', code)
        self.assertIn('self:add_child(BUTTON)', code)
        self.assertIn('function _ready()', code)
        pascal = example('Button', self.docs.examples, ('PascalCase', 'PascalCase'))
        self.assertIn('function _Ready()', pascal)
        self.assertNotIn('function _ready()', pascal)

    def test_signals_include_inherited_docs_and_argument_names(self):
        owner, member, detail = self.docs.resolve('Button', 'Pressed', ('camelCase', 'PascalCase'))
        self.assertEqual((owner, member, detail['kind']), ('BaseButton', 'pressed', 'signal'))
        code = example('Node.child_entered_tree', self.docs.examples)
        self.assertIn('const function onSignal(node: Node)', code)
        self.assertIn('object.ChildEnteredTree:connect(onSignal)', code)
        self.assertIn('print(node)', code)

    def test_foreign_examples_are_never_presented_as_luau(self):
        source = '[codeblocks][gdscript]var node = Node.new()[/gdscript][csharp]new Node();[/csharp][/codeblocks]'
        text = select_examples(source, '')
        self.assertNotIn('var node', text)
        self.assertNotIn('new Node', text)
        self.assertIn('not available yet', text)

    def test_generated_signal_examples_typecheck(self):
        analyzer = ROOT / 'build/luau' / ('luau-analyze.exe' if sys.platform == 'win32' else 'luau-analyze')
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'Signals.luau'
            for style in ('camelCase', 'PascalCase', 'snake_case'):
                snippets = []
                for key in self.docs.examples:
                    if self.docs.examples[key].get('generated'):
                        snippets.append('do\n' + example(key, self.docs.examples, (style, style)) + '\nend')
                path.write_text('--!strict\n' + '\n'.join(snippets))
                result = subprocess.run([str(analyzer), '--ludot-api=' + str(ROOT / 'types/engine.api'), '--ludot-method-casing=' + style, '--ludot-property-casing=' + style, str(path)], capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_authored_examples_typecheck_in_every_api_style(self):
        authored = json.loads((ROOT / 'docs/luau_examples.json').read_text())
        analyzer = ROOT / 'build/luau' / ('luau-analyze.exe' if sys.platform == 'win32' else 'luau-analyze')
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'Example.luau'
            for method in ('camelCase', 'PascalCase', 'snake_case'):
                for prop in ('camelCase', 'PascalCase', 'snake_case'):
                    for key in authored:
                        with self.subTest(method=method, property=prop, example=key):
                            path.write_text('--!strict\n' + example(key, authored, (method, prop)) + '\n')
                            result = subprocess.run([str(analyzer), '--ludot-api=' + str(ROOT / 'types/engine.api'), '--ludot-method-casing=' + method, '--ludot-property-casing=' + prop, str(path)], capture_output=True, text=True)
                            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
