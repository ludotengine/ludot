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


def path_from_uri(uri):
    parsed = urlparse(uri)
    if parsed.scheme != 'file':
        raise ValueError('Only file URIs are supported')
    path = unquote(parsed.path)
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


class Server:
    def __init__(self, analyzer, shader, definitions=None):
        self.analyzer = analyzer
        self.shader = shader
        self.definitions = definitions
        self.documents = {}
        self.lock = threading.RLock()
        self.output_lock = threading.Lock()
        self.workers = ThreadPoolExecutor(max_workers=2)
        self.require_completion = True
        self.shutdown = False
        self.timers = {}

    def send(self, message):
        data = json.dumps({'jsonrpc': '2.0', **message}, ensure_ascii=False).encode('utf-8')
        with self.output_lock:
            sys.stdout.buffer.write(f'Content-Length: {len(data)}\r\n\r\n'.encode() + data)
            sys.stdout.buffer.flush()

    def analyze(self, uri, position=None):
        with self.lock:
            documents = dict(self.documents)
        text = documents[uri]['text']
        path = path_from_uri(uri)
        position = position or {'line': 0, 'character': 0}
        with tempfile.TemporaryDirectory(prefix='ludot-lsp-') as directory:
            overlay = Path(directory) / 'overlays'
            content = bytearray()
            for document_uri, document in documents.items():
                if path_from_uri(document_uri).suffix == '.luau':
                    content += str(path_from_uri(document_uri)).encode() + b'\0' + document['text'].encode() + b'\0'
            overlay.write_bytes(content)
            args = [str(self.analyzer), f"--ludot-query={position['line']},{byte_column(text, position)}", f'--ludot-overlays={overlay}', str(path)]
            if self.definitions:
                args.insert(1, f'--ludot-definitions={self.definitions}')
            result = subprocess.run(args, capture_output=True, text=True, timeout=15, cwd=path.parent)
            if result.returncode != 0:
                raise RuntimeError(result.stderr.strip() or result.stdout.strip() or 'Analyzer failed')
            return json.loads(result.stdout), documents

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
                    source.write_text(document['text'])
                    result = subprocess.run([str(self.shader), str(source)], capture_output=True, text=True, timeout=10)
                    for line in result.stderr.splitlines():
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
                    diagnostics.append({'range': {'start': start, 'end': end}, 'severity': 1, 'source': 'ludot', 'message': error['message']})
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
        match = re.search(r'(?:\bfrom\s+|\bimport\s+(?:type\s+)?|\brequire\s*\(\s*)["\']([^"\']*)$', prefix)
        if match is None:
            return None
        requested = match[1]
        path = path_from_uri(uri)
        aliases = {}
        for parent in reversed([path.parent, *path.parent.parents]):
            config = parent / '.luaurc'
            if config.is_file():
                for name, target in json.loads(config.read_text()).get('aliases', {}).items():
                    aliases['@' + name] = parent / target
        if requested.startswith('@') and '/' not in requested:
            return [{'label': key, 'kind': 9, 'textEdit': {'range': {'start': {'line': position['line'], 'character': position['character'] - len(requested)}, 'end': position}, 'newText': key}} for key in aliases if key.startswith(requested)]
        base, _, tail = requested.rpartition('/')
        if requested.startswith('@'):
            alias, _, rest = base.partition('/')
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
            return {'capabilities': {'positionEncoding': 'utf-16', 'textDocumentSync': {'openClose': True, 'change': 1, 'save': {'includeText': True}}, 'completionProvider': {'triggerCharacters': ['.', ':', '"', "'", '/', '@']}, 'hoverProvider': True}, 'serverInfo': {'name': 'Ludot LSP', 'version': '0.1.0'}}
        if method == 'shutdown':
            self.shutdown = True
            return None
        if method == 'textDocument/completion':
            uri = params['textDocument']['uri']
            paths = self.module_completions(uri, params['position'])
            if paths is not None:
                return paths
            result, _ = self.analyze(uri, params['position'])
            return result['completions']
        if method == 'textDocument/hover':
            result, _ = self.analyze(params['textDocument']['uri'], params['position'])
            return {'contents': {'kind': 'markdown', 'value': '```luau\n' + result['hover'] + '\n```'}} if result['hover'] else None
        raise LookupError(method)

    def handle_request(self, message):
        try:
            self.send({'id': message['id'], 'result': self.request(message)})
        except LookupError as error:
            self.send({'id': message['id'], 'error': {'code': -32601, 'message': str(error)}})
        except (KeyError, OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
            self.send({'id': message['id'], 'error': {'code': -32603, 'message': str(error)}})

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
                # Initialize and shutdown must finish before following notifications.
                if message['method'] in ('initialize', 'shutdown'):
                    self.handle_request(message)
                else:
                    self.workers.submit(self.handle_request, message)
            else:
                self.notify(message)


def main():
    root = Path(__file__).resolve().parents[2]
    suffix = '.exe' if os.name == 'nt' else ''
    parser = argparse.ArgumentParser(description='Ludot stdio language server')
    analyzer = root / 'build/luau' / ('luau-analyze' + suffix)
    sibling = Path(sys.argv[0]).resolve().parent / ('luau-analyze' + suffix)
    if sibling.is_file():
        analyzer = sibling
    elif analyzer.is_file() is False and shutil.which('luau-analyze') is not None:
        analyzer = Path(shutil.which('luau-analyze'))
    parser.add_argument('--analyzer', type=Path, default=analyzer)
    shader = root / 'build' / ('ludot-shader' + suffix)
    sibling_shader = Path(sys.argv[0]).resolve().parent / ('ludot-shader' + suffix)
    if sibling_shader.is_file():
        shader = sibling_shader
    parser.add_argument('--shader-compiler', type=Path, default=shader)
    definitions = root / 'types/core.d.luau'
    installed_definitions = Path(sys.argv[0]).resolve().parents[1] / 'share/ludot/core.d.luau'
    if installed_definitions.is_file():
        definitions = installed_definitions
    parser.add_argument('--definitions', type=Path, default=definitions if definitions.is_file() else None)
    args = parser.parse_args()
    server = Server(args.analyzer.resolve(), args.shader_compiler.resolve(), args.definitions)
    try:
        server.run()
    finally:
        for timer in server.timers.values():
            timer.cancel()
        server.workers.shutdown(wait=True, cancel_futures=True)

if __name__ == '__main__':
    main()
