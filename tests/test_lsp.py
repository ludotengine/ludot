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
        self.server.close()
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
    def test_full_engine_api(self):
        self.server.api = ROOT / 'types/engine.api'
        uri = self.open('--!strict\nlocal camera = Camera3D()\ncamera.Fov = 60\nlocal p: Vector2 = Vector2(1, 2) * 2\nreturn camera')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        text = 'local camera = Camera3D()\ncamera:'
        self.server.documents[uri] = {'version': 2, 'text': text}
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 7}}})
        self.assertIn('projectRayNormal', [entry['label'] for entry in result])
    def test_persistent_session_and_import_invalidation(self):
        (self.root / 'helper.luau').write_text('return {Value = 1}')
        uri = self.open('--!strict\nimport "./helper" as helper\nlocal value: number = helper.Value\nreturn value')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        pid = self.server.backend.process_id
        helper_uri = (self.root / 'helper.luau').as_uri()
        self.server.documents[helper_uri] = {'version': 1, 'text': 'return {Value = "changed"}'}
        checked, _ = self.server.analyze(uri)
        self.assertTrue(checked['diagnostics'])
        again, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], again['diagnostics'])
        self.assertEqual(pid, self.server.backend.process_id)
        del self.server.documents[helper_uri]
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
    def test_cancel_request(self):
        event = __import__('threading').Event()
        self.server.cancellations[42] = event
        self.server.notify({'method': '$/cancelRequest', 'params': {'id': 42}})
        self.server.handle_request({'id': 42, 'method': 'initialize'})
        self.assertEqual(self.messages[-1]['error']['code'], -32800)
    def test_definition_references_and_rename(self):
        uri = self.open('local value = 1\nlocal function nested() local value = 2 return value end\nprint(value)')
        params = {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 8}}
        definition = self.server.request({'method': 'textDocument/definition', 'params': params})
        self.assertEqual(definition[0]['range']['start'], {'line': 0, 'character': 6})
        references = self.server.request({'method': 'textDocument/references', 'params': params})
        self.assertEqual(len(references), 2)
        prepared = self.server.request({'method': 'textDocument/prepareRename', 'params': params})
        self.assertEqual(prepared['start'], {'line': 2, 'character': 6})
        renamed = self.server.request({'method': 'textDocument/rename', 'params': {**params, 'newName': 'score'}})
        self.assertEqual(len(renamed['changes'][uri]), 2)
        self.assertEqual({item['newText'] for item in renamed['changes'][uri]}, {'score'})
    def test_import_alias_rename(self):
        (self.root / 'helper.luau').write_text('return {add = function(a: number): number return a + 1 end}')
        uri = self.open('import add from "./helper"\nprint(add(1))')
        renamed = self.server.request({'method': 'textDocument/rename', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 8}, 'newName': 'plus'}})
        edits = renamed['changes'][uri]
        self.assertEqual(edits[0]['newText'], '(add as plus)')
        self.assertEqual(edits[1]['newText'], 'plus')
    def test_signature_help(self):
        uri = self.open('local function add(a: number, b: number): number return a + b end\nadd(1, 2)')
        result = self.server.request({'method': 'textDocument/signatureHelp', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 7}}})
        self.assertIn('number', result['signatures'][0]['label'])
        self.assertEqual(len(result['signatures'][0]['parameters']), 2)
        self.assertEqual(result['activeParameter'], 1)
    def test_import_definition(self):
        (self.root / 'helper.luau').write_text('return {add = function(a: number): number return a + 1 end}')
        uri = self.open('import add from "./helper"\nprint(add(1))')
        result = self.server.request({'method': 'textDocument/definition', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 8}}})
        self.assertEqual(result[0]['uri'], (self.root / 'helper.luau').as_uri())
    def test_shader_completion_and_unicode_diagnostics(self):
        uri = self.open('shader canvas_item\nexport Tint: vec4 = vec4(1)\n-- 😀\nfunction fragment() Color = Tint end', 'tint.lushader')
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 3, 'character': 28}}})
        labels = [item['label'] for item in result]
        self.assertIn('Tint', labels)
        self.assertIn('texture', labels)
        self.server.publish(uri)
        self.assertEqual(self.messages[-1]['params']['diagnostics'], [])
        self.server.documents[uri] = {'version': 2, 'text': 'shader canvas_item\n-- 😀\nfunction fragment() local unsupported = {} end'}
        self.server.publish(uri)
        self.assertTrue(self.messages[-1]['params']['diagnostics'])
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
