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
        # The server reports canonical paths: macOS temp folders live under /private, and
        # Windows expands short names such as RUNNER~1.
        self.root = Path(self.temp.name).resolve()
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
        uri = self.open('--!strict\nlocal camera = Camera3D.new()\ncamera.Fov = 60\nlocal p: Vector2 = Vector2.new(1, 2) * 2\nreturn camera')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        text = 'local camera = Camera3D.new()\ncamera:'
        self.server.documents[uri] = {'version': 2, 'text': text}
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 7}}})
        self.assertIn('projectRayNormal', [entry['label'] for entry in result])
    def test_transform_operator_results(self):
        self.server.api = ROOT / 'types/engine.api'
        uri = self.open('--!strict\nconst direction = Basis.new() * Vector3.new(1, 2, 3)\nconst x: number = direction.X\nconst translated: Vector3 = Transform3D.new() * direction\nreturn translated')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        self.server.documents[uri] = {'version': 2, 'text': '--!strict\nconst direction = Basis.new() * Vector3.new(1, 2, 3)\nreturn direction.X.X'}
        checked, _ = self.server.analyze(uri)
        self.assertTrue(checked['diagnostics'])

    def test_project_api_casing_matrix(self):
        self.server.api = ROOT / 'types/engine.api'
        styles = ('camelCase', 'PascalCase', 'snake_case')
        def name(raw, style):
            if style == 'snake_case':
                return raw
            result = ''.join(word.title() for word in raw.split('_'))
            return result[:1].lower() + result[1:] if style == 'camelCase' else result
        for method_style in styles:
            for property_style in styles:
                with self.subTest(methods=method_style, properties=property_style):
                    project = self.root / (method_style + '-' + property_style)
                    project.mkdir()
                    (project / 'project.godot').write_text('config_version=5\n[luau]\napi/method_casing=' + json.dumps(method_style) + '\napi/property_casing=' + json.dumps(property_style) + '\n')
                    method = name('get_instance_id', method_style)
                    prop = name('position', property_style)
                    component = name('x', property_style)
                    text = '--!strict\nconst node = Node3D.new()\nnode.' + prop + ' = Vector3.new()\nconst component: number = node.' + prop + '.' + component + '\nconst id: number = node:' + method + '()\nconst q: Quaternion = Quaternion.' + name('from_euler', method_style) + '(Vector3.new())\nreturn component + id'
                    text = text.replace('\nreturn component + id', '')
                    text += '\nconst rid: RID = Resource.new():' + name('get_rid', method_style) + '()\nconst http: HTTPClient = HTTPClient.new()'
                    uri = self.open(text, str(project.relative_to(self.root) / 'Main.luau'))
                    checked, _ = self.server.analyze(uri)
                    self.assertEqual(checked['diagnostics'], [])
                    completion = 'const node = Node3D.new()\nnode.'
                    self.server.documents[uri] = {'version': 2, 'text': completion}
                    result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 5}}})
                    labels = [item['label'] for item in result]
                    self.assertEqual(len(labels), len(set(labels)))
                    self.assertEqual(labels.count(prop), 1)
                    self.assertEqual(labels.count(method), 1)
                    for alternative in styles:
                        if name('position', alternative) != prop:
                            self.assertNotIn(name('position', alternative), labels)
                        if name('get_instance_id', alternative) != method:
                            self.assertNotIn(name('get_instance_id', alternative), labels)
                    hover_text = 'const node = Node3D.new()\nprint(node.' + prop + ')'
                    self.server.documents[uri] = {'version': 3, 'text': hover_text}
                    hover = self.server.request({'method': 'textDocument/hover', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 12}}})
                    self.assertIn('Vector3', hover['contents']['value'])
                    call = 'const node = Node.new()\nnode:' + name('set_name', method_style) + '("test")'
                    self.server.documents[uri] = {'version': 4, 'text': call}
                    signature = self.server.request({'method': 'textDocument/signatureHelp', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': call.split('\n')[1].index('(') + 1}}})
                    self.assertIsNotNone(signature)
                    self.assertIn('string', signature['signatures'][0]['label'])
                    # User identifiers are independent of engine and lint casing.
                    self.server.documents[uri] = {'version': 3, 'text': '--!strict\nclass Custom_Name = { public custom_field: number = 1 function Do_Not_Change(): number return self.custom_field end }\nreturn Custom_Name.new():Do_Not_Change()'}
                    checked, _ = self.server.analyze(uri)
                    self.assertEqual(checked['diagnostics'], [])
                    (project / 'project.godot').write_text('config_version=5\n[luau]\napi/method_casing="snake_case"\napi/property_casing="snake_case"\n')
                    self.assertEqual(self.server.api_casing(project / 'Main.luau'), (method_style, property_style))

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
    def test_shader_include_diagnostics(self):
        (self.root / 'shared.lushader').write_text('shader include\nlocal Half: number = 0.5\n')
        uri = self.open('shader canvas_item\nimport "./shared"\nfunction fragment() Color = vec4(Half) end', 'includes.lushader')
        self.server.publish(uri)
        self.assertEqual(self.messages[-1]['params']['diagnostics'], [])
        (self.root / 'shared.lushader').write_text('shader include\nlocal Half = {}\n')
        self.server.publish(uri)
        diagnostics = self.messages[-1]['params']['diagnostics']
        self.assertEqual(len(diagnostics), 1)
        self.assertEqual(diagnostics[0]['range']['start']['line'], 1)
        self.assertIn('shared.lushader', diagnostics[0]['message'])
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
    def test_extensions_unsaved_enum_exhaustiveness_and_utf16(self):
        text = '--!strict\nenum Action = { Join, Chat, Leave }\nconst value: Action = Action.Join\nconst emoji = "😀"; const result = match value with case Action.Join then 1 end\nreturn { emoji, result }'
        uri = self.open(text)
        self.server.publish(uri)
        errors = self.messages[-1]['params']['diagnostics']
        missing = next(error for error in errors if 'Non-exhaustive match' in error['message'])
        self.assertIn('Action.Chat', missing['message'])
        self.assertIn('Action.Leave', missing['message'])
        self.assertEqual(missing['range']['start'], {'line': 3, 'character': 35})
        self.server.documents[uri] = {'version': 2, 'text': text.replace('then 1 end', 'then 1 otherwise 0 end')}
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])

    def test_is_a_resource_refinement_and_class_string_completion(self):
        self.server.api = ROOT / 'types/engine.api'
        text = '--!strict\nconst function mouse(event: InputEvent): Vector2?\n\tif event:IsA("InputEventMouseMotion") then\n\t\treturn event.Relative\n\tend\n\treturn nil\nend\nreturn mouse'
        uri = self.open(text)
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        result = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 24}}})
        self.assertIn('InputEventMouseMotion', [entry['label'] for entry in result])
        hover = self.server.request({'method': 'textDocument/hover', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 3, 'character': 18}}})
        self.assertIn('Vector2', str(hover))
        self.server.documents[uri] = {'version': 2, 'text': '--!strict\nconst event = InputEventMouseMotion.new()\nevent.'}
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 6}}})
        labels = [entry['label'] for entry in completions]
        self.assertIn('isA', labels)
        self.assertNotIn('IsA', labels)

    def test_native_enum_constants_and_checked_conversion(self):
        self.server.api = ROOT / 'types/engine.api'
        uri = self.open('--!strict\nconst mode: Enum.MouseMode = Input.MOUSE_MODE_CAPTURED\nInput:setMouseMode(Enum.MouseMode.Captured)\nreturn mode')
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])
        self.server.documents[uri] = {'version': 2, 'text': '--!strict\nreturn Input.'}
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 13}}})
        labels = [entry['label'] for entry in completions]
        self.assertIn('MOUSE_MODE_CAPTURED', labels)
        self.assertNotIn('MouseModeCaptured', labels)
        self.server.documents[uri] = {'version': 3, 'text': '--!strict\nreturn Enum.MouseMode.'}
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 22}}})
        labels = [entry['label'] for entry in completions]
        self.assertIn('Captured', labels)
        self.assertIn('fromName', labels)
        self.server.documents[uri] = {'version': 4, 'text': '--!strict\nreturn task.'}
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': 12}}})
        labels = [entry['label'] for entry in completions]
        for name in ('spawn', 'defer', 'delay', 'wait', 'cancel'):
            self.assertIn(name, labels)

    def test_signal_completion_and_declaration_warning(self):
        self.server.api = ROOT / 'types/engine.api'
        uri = self.open('--!strict\nconst Ping: Signal<number> = Signal.new()\nPing:')
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 2, 'character': 5}}})
        labels = [entry['label'] for entry in completions]
        for name in ('fire', 'connect', 'once', 'wait', 'disconnectAll'):
            self.assertIn(name, labels)
        self.assertNotIn('emit', labels)
        text = '--!strict\nconst Loose = Signal.new()\nreturn Loose'
        self.server.documents[uri] = {'version': 2, 'text': text}
        self.server.publish(uri)
        warning = next(d for d in self.messages[-1]['params']['diagnostics'] if 'Signal.new() has no type annotation' in d['message'])
        self.assertEqual(warning['severity'], 2)
        self.server.documents[uri] = {'version': 3, 'text': text.replace('const Loose', '-- selene: allow(signal_declaration)\nconst Loose')}
        checked, _ = self.server.analyze(uri)
        self.assertEqual(checked['diagnostics'], [])

    def test_match_warning_severity_and_selene_ignores(self):
        text = '--!strict\nenum Action = { Join, Chat }\nconst action: Action = Action.Join\nmatch action with case Action.Join then print(1) end'
        uri = self.open(text)
        self.server.publish(uri)
        warning = next(d for d in self.messages[-1]['params']['diagnostics'] if 'Non-exhaustive match' in d['message'])
        self.assertEqual(warning['severity'], 2)
        for ignored in (
            '--# selene: allow(match_exhaustiveness)\n' + text,
            text.replace('match action with', '-- selene: allow(match_exhaustiveness)\nmatch action with'),
        ):
            self.server.documents[uri] = {'version': 2, 'text': ignored}
            checked, _ = self.server.analyze(uri)
            self.assertEqual(checked['diagnostics'], [])
        literal = text.replace('const action:', 'const ignoredText = "--# selene: allow(match_exhaustiveness)"\nconst action:')
        self.server.documents[uri] = {'version': 3, 'text': literal}
        checked, _ = self.server.analyze(uri)
        self.assertTrue(any('Non-exhaustive match' in d['message'] for d in checked['diagnostics']))

    def test_completion_recovers_inside_unfinished_match_and_class_name(self):
        self.server.api = ROOT / 'types/engine.api'
        text = 'enum Action = { Join, Chat }\nconst function read(action: Action)\n\tconst result = match action with\n\t\tcase Action.'
        uri = self.open(text)
        line = text.splitlines()[-1]
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 3, 'character': len(line)}}})
        labels = [entry['label'] for entry in completions]
        self.assertIn('Join', labels)
        self.assertIn('Chat', labels)
        text = 'const function read(event: InputEvent)\n\tevent:IsA("InputEventMo'
        self.server.documents[uri] = {'version': 2, 'text': text}
        completions = self.server.request({'method': 'textDocument/completion', 'params': {'textDocument': {'uri': uri}, 'position': {'line': 1, 'character': len(text.splitlines()[-1])}}})
        self.assertIn('InputEventMouseMotion', [entry['label'] for entry in completions])

if __name__ == '__main__': unittest.main()
