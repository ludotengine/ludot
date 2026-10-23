import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from ludot_lsp.__main__ import Server, byte_column, utf16_column

class LspTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        suffix = '.exe' if sys.platform == 'win32' else ''
        self.server = Server(ROOT / 'build/luau' / ('luau-analyze' + suffix), ROOT / 'build' / ('ludot-shader' + suffix))
        self.messages = []
        self.server.send = self.messages.append
    def tearDown(self):
        self.server.workers.shutdown(wait=True)
        self.temp.cleanup()
    def open(self, text, name='Main.luau'):
        path = self.root / name
        path.write_text('-- Disk content deliberately differs from the open buffer.\n')
        self.server.documents[path.as_uri()] = {'version': 1, 'text': text}
        return path.as_uri()
    def test_unsaved_diagnostics(self):
        uri = self.open('--!strict\nlocal value: number = "wrong"\nreturn value')
        self.server.publish(uri)
        diagnostic = self.messages[-1]['params']['diagnostics'][0]
        self.assertIn('number', diagnostic['message'])
        self.assertEqual(diagnostic['range']['start']['line'], 1)
    def test_typed_completion(self):
        uri = self.open('--!strict\nlocal record = {Score = 1, Name = "test"}\nrecord.')
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 7}}})
        self.assertIn('Score', [entry['label'] for entry in result])
        self.assertIn('Name', [entry['label'] for entry in result])
    def test_engine_types_and_completion(self):
        self.server.definitions = ROOT / 'types/core.d.luau'
        uri = self.open('class Player extends CharacterBody2D = { function _ready() self:moveAndSlide() end }')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        text = 'class Player extends CharacterBody2D = { function _ready() self: end }'
        self.server.documents[uri] = {'version': 2, 'text': text}
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 0, 'character': text.index('self:') + 5}}})
        self.assertIn('moveAndSlide', [entry['label'] for entry in result])
    def test_hover(self):
        uri = self.open('--!strict\nlocal score: number = 1\nprint(score)')
        result = self.server.request({'method': 'textDocument/hover', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 8}}})
        self.assertIn('number', result['contents']['value'])
    def test_alias_completion_toggle(self):
        (self.root / '.luaurc').write_text(json.dumps({'aliases': {'helpers': './helpers'}}))
        (self.root / 'helpers').mkdir()
        (self.root / 'helpers/math.luau').write_text('return {}')
        uri = self.open('import "@helpers/ma')
        position = {'line': 0, 'character': len('import "@helpers/ma')}
        self.assertEqual(self.server.module_completions(uri, position)[0]['label'], 'math')
        self.server.require_completion = False
        self.assertIsNone(self.server.module_completions(uri, position))
    def test_utf16_conversion(self):
        text = 'local a = "😀"; value'
        end = len(text.encode('utf-16-le')) // 2
        self.assertEqual(byte_column(text, {'line': 0, 'character': end}), len(text.encode()))
        self.assertEqual(utf16_column(text, 0, len(text.encode())), end)
    def test_wire_protocol(self):
        requests = [{'jsonrpc': '2.0', 'id': 1, 'method': 'initialize', 'params': {}}, {'jsonrpc': '2.0', 'id': 2, 'method': 'shutdown'}, {'jsonrpc': '2.0', 'method': 'exit'}]
        wire = b''
        for request in requests:
            data = json.dumps(request).encode()
            wire += f'Content-Length: {len(data)}\r\n\r\n'.encode() + data
        result = subprocess.run([sys.executable, str(ROOT / 'tools/ludot-lsp')], input=wire, capture_output=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr.decode())
        self.assertIn(b'Ludot LSP', result.stdout)
        self.assertIn(b'"id": 2', result.stdout)
if __name__ == '__main__': unittest.main()
