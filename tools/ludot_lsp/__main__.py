import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import threading
from urllib.parse import unquote, urlparse
from .backend import Backend, RequestCancelled


def path_from_uri(uri):
    parsed = urlparse(uri)
    if parsed.scheme != 'file':
        raise ValueError('Only file URIs are supported')
    path = unquote(parsed.path)
    if os.name == 'nt' and parsed.netloc and parsed.netloc != 'localhost':
        path = '//' + parsed.netloc + path
    if os.name == 'nt' and re.match(r'^/[A-Za-z]:', path):
        path = path[1:]
    return Path(path).resolve()


def byte_column(text, position):
    lines = text.split('\n')
    line = lines[position['line']] if position['line'] < len(lines) else ''
    prefix = line.encode('utf-16-le')[:position['character'] * 2].decode('utf-16-le', errors='ignore')
    return len(prefix.encode('utf-8'))


def utf16_column(text, line, column):
    lines = text.split('\n')
    value = lines[line] if line < len(lines) else ''
    return len(value.encode('utf-8')[:column].decode('utf-8', errors='ignore').encode('utf-16-le')) // 2


class ProtocolError(RuntimeError):
    def __init__(self, code, message):
        super().__init__(message)
        self.code = code


class Server:
    def __init__(self, analyzer, shader, definitions=None, api=None):
        self.analyzer = analyzer
        self.shader = shader
        self.definitions = definitions
        self.api = api
        self.backend = Backend(analyzer)
        self.request_context = threading.local()
        self.cancellations = {}
        self.documents = {}
        self.lock = threading.RLock()
        self.output_lock = threading.Lock()
        self.workers = ThreadPoolExecutor(max_workers=2)
        self.project_casing = {}
        self.require_completion = True
        self.shader_words = None
        self.shutdown = False
        self.timers = {}

    def send(self, message):
        data = json.dumps({'jsonrpc': '2.0', **message}, ensure_ascii=False).encode('utf-8')
        with self.output_lock:
            sys.stdout.buffer.write(f'Content-Length: {len(data)}\r\n\r\n'.encode() + data)
            sys.stdout.buffer.flush()

    def api_casing(self, path):
        project = next((parent / 'project.godot' for parent in path.parents if (parent / 'project.godot').is_file()), None)
        with self.lock:
            if project in self.project_casing:
                return self.project_casing[project]
            selected = ['camelCase', 'PascalCase']
            if project:
                section = ''
                for raw in project.read_text(encoding='utf-8').splitlines():
                    line = raw.strip()
                    if line.startswith('['):
                        section = line
                    if section != '[luau]' or '=' not in line:
                        continue
                    key, value = line.split('=', 1)
                    if key.strip() not in ('api/method_casing', 'api/property_casing'):
                        continue
                    style = json.loads(value.split(';', 1)[0].strip())
                    if style not in ('camelCase', 'PascalCase', 'snake_case'):
                        print('Invalid Luau API casing in project.godot; using the default.', file=sys.stderr)
                        continue
                    selected[0 if key.strip() == 'api/method_casing' else 1] = style
            self.project_casing[project] = tuple(selected)
            return tuple(selected)

    def analyze(self, uri, position=None):
        with self.lock:
            documents = dict(self.documents)
        text = documents[uri]['text']
        path = path_from_uri(uri)
        position = position or {'line': 0, 'character': 0}
        content = bytearray()
        for document_uri, document in documents.items():
            if path_from_uri(document_uri).suffix == '.luau':
                content += path_from_uri(document_uri).as_posix().encode() + b'\0' + document['text'].encode() + b'\0'
        request_id = getattr(self.request_context, 'request_id', None)
        with self.lock:
            cancelled = self.cancellations.get(request_id)
        result = self.backend.query(path, position['line'], byte_column(text, position), content, self.definitions, self.api, cancelled, self.api_casing(path))
        return result, documents

    def close(self):
        with self.lock:
            for timer in self.timers.values():
                timer.cancel()
            for event in self.cancellations.values():
                event.set()
        self.workers.shutdown(wait=True, cancel_futures=True)
        self.backend.close()

    def publish(self, uri):
        try:
            with self.lock:
                document = self.documents.get(uri)
            if document is None:
                return
            diagnostics = []
            path = path_from_uri(uri)
            if path.suffix == '.lushader':
                with tempfile.TemporaryDirectory(prefix='ludot-shader-') as directory:
                    source = Path(directory) / 'shader.lushader'
                    source.write_text(document['text'], encoding='utf-8')
                    # Imports resolve against the document's real location, not the temporary copy.
                    result = subprocess.run([str(self.shader), '--origin', str(path), str(source)], capture_output=True, text=True, timeout=10)
                    for line in result.stderr.splitlines():
                        # Errors inside included files also produce one at the importing line.
                        if not line.startswith(str(source) + ':'):
                            continue
                        match = re.match(r'^.*?:(\d+):(\d+): (.*)$', line)
                        if match:
                            row, column = int(match[1]) - 1, int(match[2]) - 1
                            diagnostics.append({'range': {'start': {'line': row, 'character': utf16_column(document['text'], row, column)}, 'end': {'line': row, 'character': utf16_column(document['text'], row, column) + 1}}, 'severity': 1, 'source': 'ludot', 'message': match[3]})
            else:
                result, _ = self.analyze(uri)
                for error in result['diagnostics']:
                    if Path(error['path']).resolve() != path:
                        continue
                    start = {'line': error['line'], 'character': utf16_column(document['text'], error['line'], error['column'])}
                    end = {'line': error['endLine'], 'character': utf16_column(document['text'], error['endLine'], error['endColumn'])}
                    diagnostics.append({'range': {'start': start, 'end': end}, 'severity': error.get('severity', 1), 'source': 'ludot', 'message': error['message']})
            with self.lock:
                if self.documents.get(uri) != document:
                    return
            self.send({'method': 'textDocument/publishDiagnostics', 'params': {'uri': uri, 'version': document['version'], 'diagnostics': diagnostics}})
        except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
            self.send({'method': 'window/logMessage', 'params': {'type': 1, 'message': str(error)}})

    def schedule_diagnostics(self, uri):
        with self.lock:
            if uri in self.timers:
                self.timers[uri].cancel()
            timer = threading.Timer(0.12, lambda: self.workers.submit(self.publish, uri))
            timer.daemon = True
            self.timers[uri] = timer
            timer.start()

    def module_completions(self, uri, position):
        if self.require_completion is False:
            return None
        with self.lock:
            text = self.documents[uri]['text']
        line = text.split('\n')[position['line']]
        prefix = line.encode('utf-16-le')[:position['character'] * 2].decode('utf-16-le', errors='ignore')
        match = re.search(r'(?:\bimport\s+(?:type\s+)?|\brequire\s*\(\s*)["\']([^"\']*)$', prefix)
        if match is None:
            return None
        requested = match[1]
        path = path_from_uri(uri)
        aliases = {}
        for parent in reversed([path.parent, *path.parent.parents]):
            config = parent / '.luaurc'
            if config.is_file():
                for name, target in json.loads(config.read_text(encoding='utf-8')).get('aliases', {}).items():
                    aliases['@' + name.lower()] = parent / target
        if requested.startswith('@') and '/' not in requested:
            return [{'label': key, 'kind': 9, 'textEdit': {'range': {'start': {'line': position['line'], 'character': position['character'] - len(requested)}, 'end': position}, 'newText': key}} for key in aliases if key.startswith(requested.lower())]
        base, _, tail = requested.rpartition('/')
        if requested.startswith('@'):
            alias, _, rest = base.partition('/')
            alias = alias.lower()
            if alias not in aliases:
                return []
            directory = aliases[alias] / rest
        else:
            directory = path.parent / base
        items = []
        if directory.is_dir():
            for entry in sorted(directory.iterdir()):
                if entry.name.startswith('.'):
                    continue
                if entry.is_dir() or entry.suffix == '.luau':
                    name = entry.name if entry.is_dir() else entry.stem
                    if name.startswith(tail):
                        replacement = {'range': {'start': {'line': position['line'], 'character': position['character'] - len(tail)}, 'end': position}, 'newText': name}
                        items.append({'label': name, 'kind': 19 if entry.is_dir() else 9, 'textEdit': replacement})
        return items

    def request(self, message):
        method = message['method']
        params = message.get('params', {})
        if method == 'initialize':
            return {'capabilities': {'positionEncoding': 'utf-16', 'textDocumentSync': {'openClose': True, 'change': 1, 'save': {'includeText': True}}, 'completionProvider': {'triggerCharacters': ['.', ':', '"', "'", '/', '@']}, 'hoverProvider': True, 'definitionProvider': True, 'referencesProvider': True, 'renameProvider': {'prepareProvider': True}, 'signatureHelpProvider': {'triggerCharacters': ['(', ',']}}, 'serverInfo': {'name': 'Ludot LSP', 'version': '0.1.0'}}
        if method == 'shutdown':
            self.shutdown = True
            return None
        if method == 'textDocument/completion':
            uri = params['textDocument']['uri']
            paths = self.module_completions(uri, params['position'])
            if paths is not None:
                return paths
            if path_from_uri(uri).suffix == '.lushader':
                if self.shader_words is None:
                    words = subprocess.run([str(self.shader), '--completion-list'], capture_output=True, text=True, timeout=5, check=True)
                    self.shader_words = words.stdout.splitlines()
                with self.lock:
                    text = self.documents[uri]['text']
                declared = re.findall(r'\b(?:export|local|const|function)\s+([A-Za-z_]\w*)', text)
                return [{'label': word, 'kind': 6} for word in sorted(set(self.shader_words + declared))]
            result, _ = self.analyze(uri, params['position'])
            return result['completions']
        if method in ('textDocument/definition', 'textDocument/references', 'textDocument/prepareRename', 'textDocument/rename', 'textDocument/signatureHelp'):
            uri = params['textDocument']['uri']
            if path_from_uri(uri).suffix == '.lushader':
                return [] if method in ('textDocument/definition', 'textDocument/references') else None
            result, documents = self.analyze(uri, params['position'])
            def location(site):
                target = Path(site['path']).resolve()
                target_uri = target.as_uri()
                text = None
                for document_uri, document in documents.items():
                    if path_from_uri(document_uri) == target:
                        target_uri = document_uri
                        text = document['text']
                        break
                if text is None:
                    text = target.read_text(encoding='utf-8')
                return {'uri': target_uri, 'range': {
                    'start': {'line': site['line'], 'character': utf16_column(text, site['line'], site['column'])},
                    'end': {'line': site['endLine'], 'character': utf16_column(text, site['endLine'], site['endColumn'])}}}
            if method == 'textDocument/definition':
                site = result.get('definition')
                return [location(site)] if site and Path(site['path']).is_file() else []
            if method == 'textDocument/signatureHelp':
                return result.get('signatureHelp')
            declaration = result.get('rename')
            if method == 'textDocument/references':
                sites = result.get('references', [])
                if params.get('context', {}).get('includeDeclaration', True) is False:
                    sites = [site for site in sites if site != declaration]
                return [location(site) for site in sites]
            if method == 'textDocument/prepareRename':
                if declaration is None:
                    return None
                for site in result['references']:
                    item = location(site)
                    if item['uri'] == uri and (item['range']['start']['line'], item['range']['start']['character']) <= (params['position']['line'], params['position']['character']) <= (item['range']['end']['line'], item['range']['end']['character']):
                        return item['range']
                return None
            if declaration is None:
                raise ProtocolError(-32602, 'This symbol cannot be renamed')
            name = params['newName']
            keywords = {'and', 'break', 'continue', 'do', 'else', 'elseif', 'end', 'false', 'for', 'function', 'if', 'in', 'local', 'nil', 'not', 'or', 'repeat', 'return', 'then', 'true', 'until', 'while', 'const', 'class', 'import', 'export', 'onready', 'private', 'public'}
            if re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', name) is None or name in keywords:
                raise ProtocolError(-32602, 'The new name must be a Luau identifier')
            if name in result['renameConflicts']:
                raise ProtocolError(-32602, 'The new name would capture another binding')
            changes = {}
            for site in result['references']:
                item = location(site)
                replacement = name
                if site == declaration and result.get('importName'):
                    prefix = documents[uri]['text'].split('\n')[site['line']].encode('utf-8')[:site['column']].decode('utf-8')
                    if re.search(r'\bas\s*$', prefix) is None:
                        replacement = f"{result['importName']} as {name}"
                changes.setdefault(item['uri'], []).append({'range': item['range'], 'newText': replacement})
            return {'changes': changes}
        if method == 'textDocument/hover':
            uri = params['textDocument']['uri']
            if path_from_uri(uri).suffix == '.lushader':
                return None
            result, _ = self.analyze(params['textDocument']['uri'], params['position'])
            return {'contents': {'kind': 'markdown', 'value': '```luau\n' + result['hover'] + '\n```'}} if result['hover'] else None
        raise LookupError(method)

    def handle_request(self, message):
        request_id = message['id']
        self.request_context.request_id = request_id
        with self.lock:
            cancelled = self.cancellations.setdefault(request_id, threading.Event())
        try:
            if cancelled.is_set():
                raise RequestCancelled('Request cancelled')
            result = self.request(message)
            if cancelled.is_set():
                raise RequestCancelled('Request cancelled')
            self.send({'id': request_id, 'result': result})
        except RequestCancelled as error:
            self.send({'id': request_id, 'error': {'code': -32800, 'message': str(error)}})
        except ProtocolError as error:
            self.send({'id': request_id, 'error': {'code': error.code, 'message': str(error)}})
        except LookupError as error:
            self.send({'id': request_id, 'error': {'code': -32601, 'message': str(error)}})
        except (KeyError, OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
            self.send({'id': request_id, 'error': {'code': -32603, 'message': str(error)}})
        finally:
            with self.lock:
                self.cancellations.pop(request_id, None)
            self.request_context.request_id = None

    def notify(self, message):
        method = message['method']
        params = message.get('params', {})
        if method in ('textDocument/didOpen', 'textDocument/didChange', 'textDocument/didSave'):
            doc = params['textDocument']
            uri = doc['uri']
            with self.lock:
                if method == 'textDocument/didOpen':
                    self.documents[uri] = {'text': doc['text'], 'version': doc['version']}
                elif method == 'textDocument/didChange':
                    self.documents[uri] = {'text': params['contentChanges'][-1]['text'], 'version': doc['version']}
                elif 'text' in params:
                    self.documents[uri] = {**self.documents[uri], 'text': params['text']}
            self.schedule_diagnostics(uri)
        elif method == 'textDocument/didClose':
            uri = params['textDocument']['uri']
            with self.lock:
                self.documents.pop(uri, None)
                if uri in self.timers:
                    self.timers.pop(uri).cancel()
            self.send({'method': 'textDocument/publishDiagnostics', 'params': {'uri': uri, 'diagnostics': []}})
        elif method == '$/cancelRequest':
            with self.lock:
                event = self.cancellations.get(params['id'])
                if event is not None:
                    event.set()
        elif method == 'workspace/didChangeWatchedFiles':
            self.backend.close()
            for uri in list(self.documents):
                self.schedule_diagnostics(uri)
        elif method == 'workspace/didChangeConfiguration':
            self.require_completion = params.get('settings', {}).get('ludot', {}).get('requireAutocomplete', True)

    def run(self):
        while True:
            headers = {}
            while True:
                line = sys.stdin.buffer.readline()
                if line == b'':
                    return
                if line in (b'\r\n', b'\n'):
                    break
                key, _, value = line.decode('ascii').partition(':')
                headers[key.lower()] = value.strip()
            length = int(headers.get('content-length', 0))
            if length <= 0 or length > 16 * 1024 * 1024:
                raise ValueError('Invalid Content-Length')
            message = json.loads(sys.stdin.buffer.read(length))
            if message.get('method') == 'exit':
                return
            if 'id' in message:
                with self.lock:
                    self.cancellations[message['id']] = threading.Event()
                # Initialize and shutdown must finish before following notifications.
                if message['method'] in ('initialize', 'shutdown'):
                    self.handle_request(message)
                else:
                    self.workers.submit(self.handle_request, message)
            else:
                self.notify(message)


def main():
    root = Path(__file__).resolve().parents[2]
    bundle = Path(getattr(sys, '_MEIPASS', root))
    suffix = '.exe' if os.name == 'nt' else ''
    parser = argparse.ArgumentParser(description='Ludot stdio language server')
    analyzer = root / 'build/luau' / ('luau-analyze' + suffix)
    sibling = Path(sys.argv[0]).resolve().parent / ('luau-analyze' + suffix)
    if sibling.is_file():
        analyzer = sibling
    elif analyzer.is_file() is False and shutil.which('luau-analyze') is not None:
        analyzer = Path(shutil.which('luau-analyze'))
    bundled_analyzer = bundle / 'bin' / ('luau-analyze' + suffix)
    if bundled_analyzer.is_file():
        analyzer = bundled_analyzer
    parser.add_argument('--analyzer', type=Path, default=analyzer)
    shader = root / 'build' / ('ludot-shader' + suffix)
    sibling_shader = Path(sys.argv[0]).resolve().parent / ('ludot-shader' + suffix)
    if sibling_shader.is_file():
        shader = sibling_shader
    bundled_shader = bundle / 'bin' / ('ludot-shader' + suffix)
    if bundled_shader.is_file():
        shader = bundled_shader
    parser.add_argument('--shader-compiler', type=Path, default=shader)
    definitions = root / 'types/core.d.luau'
    installed_definitions = Path(sys.argv[0]).resolve().parents[1] / 'share/ludot/core.d.luau'
    if installed_definitions.is_file():
        definitions = installed_definitions
    api = root / 'types/engine.api'
    installed_api = Path(sys.argv[0]).resolve().parents[1] / 'share/ludot/engine.api'
    if installed_api.is_file():
        api = installed_api
    bundled_api = bundle / 'share/ludot/engine.api'
    if bundled_api.is_file():
        api = bundled_api
    parser.add_argument('--api', type=Path, default=api if api.is_file() else None)
    parser.add_argument('--definitions', type=Path, default=None)
    args = parser.parse_args()
    server = Server(args.analyzer.resolve(), args.shader_compiler.resolve(), args.definitions, args.api)
    try:
        server.run()
    finally:
        server.close()

if __name__ == '__main__':
    main()
