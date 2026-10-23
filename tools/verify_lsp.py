#!/usr/bin/env python3
"""Exercise a standalone LSP executable from an unrelated project directory."""
import argparse
import json
from pathlib import Path
import queue
import subprocess
import tempfile
import threading

parser = argparse.ArgumentParser()
parser.add_argument('executable', type=Path)
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix='ludot-lsp-project-') as directory:
    path = Path(directory) / 'Player.luau'
    text = '--!strict\nlocal camera = Camera3D()\ncamera:'
    path.write_text(text, encoding='utf-8')
    process = subprocess.Popen([str(args.executable.resolve())], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=directory)
    replies = queue.Queue()
    def read():
        try:
            while True:
                headers = {}
                while True:
                    line = process.stdout.readline()
                    if not line:
                        return
                    if line in (b'\r\n', b'\n'):
                        break
                    key, _, value = line.decode('ascii').partition(':')
                    headers[key.lower()] = value.strip()
                replies.put(json.loads(process.stdout.read(int(headers['content-length']))))
        finally:
            replies.put(None)
    reader = threading.Thread(target=read, daemon=True)
    reader.start()
    def send(message):
        data = json.dumps({'jsonrpc': '2.0', **message}).encode()
        process.stdin.write(f'Content-Length: {len(data)}\r\n\r\n'.encode() + data)
        process.stdin.flush()
    def response(request_id):
        while True:
            message = replies.get(timeout=20)
            if message is None:
                raise RuntimeError(process.stderr.read().decode(errors='replace'))
            if message.get('id') == request_id:
                if 'error' in message:
                    raise RuntimeError(message['error'])
                return message['result']
    try:
        send({'id': 1, 'method': 'initialize', 'params': {'rootUri': Path(directory).as_uri()}})
        assert response(1)['serverInfo']['name'] == 'Ludot LSP'
        send({'method': 'textDocument/didOpen', 'params': {'textDocument': {'uri': path.as_uri(), 'languageId': 'luau', 'version': 1, 'text': text}}})
        send({'id': 2, 'method': 'textDocument/completion', 'params': {'textDocument': {'uri': path.as_uri()}, 'position': {'line': 2, 'character': 7}}})
        assert 'projectRayNormal' in [entry['label'] for entry in response(2)]
        send({'id': 3, 'method': 'shutdown'})
        response(3)
        send({'method': 'exit'})
        process.wait(timeout=10)
        assert process.returncode == 0
        print('LUDOT_STANDALONE_LSP_TESTS_PASSED')
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
        reader.join(timeout=1)
        process.stdin.close()
        process.stdout.close()
        process.stderr.close()
