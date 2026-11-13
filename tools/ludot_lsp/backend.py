"""Serialized, persistent Luau Analysis session with framed buffer updates."""
import json
import queue
import struct
import subprocess
import tempfile
import threading
import time


class RequestCancelled(RuntimeError):
    pass


class Backend:
    def __init__(self, analyzer):
        self.analyzer = analyzer
        self.query_lock = threading.Lock()
        self.state_lock = threading.Lock()
        self.slot = None

    @property
    def process_id(self):
        with self.state_lock:
            return self.slot['process'].pid if self.slot else None

    def _start(self, path, definitions, api, casing):
        args = [str(self.analyzer), '--ludot-session', f'--ludot-method-casing={casing[0]}', f'--ludot-property-casing={casing[1]}']
        if definitions:
            args.append(f'--ludot-definitions={definitions}')
        if api:
            args.append(f'--ludot-api={api}')
        errors = tempfile.TemporaryFile()
        process = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=errors, cwd=path.parent)
        replies = queue.Queue()
        def read_replies():
            try:
                for line in process.stdout:
                    replies.put(line)
            finally:
                replies.put(None)
        reader = threading.Thread(target=read_replies, daemon=True)
        reader.start()
        return {'process': process, 'replies': replies, 'reader': reader, 'errors': errors, 'options': (definitions, api, casing)}

    def _discard(self, slot):
        with self.state_lock:
            if self.slot is slot:
                self.slot = None
        process = slot['process']
        if process.poll() is None:
            process.terminate()
        try:
            process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
        slot['reader'].join(timeout=1)
        process.stdin.close()
        process.stdout.close()
        slot['errors'].close()

    def close(self):
        with self.query_lock:
            with self.state_lock:
                slot = self.slot
            if slot:
                self._discard(slot)

    def query(self, path, line, column, overlays, definitions, api, cancelled=None, casing=('camelCase', 'PascalCase')):
        with self.query_lock:
            if cancelled is not None and cancelled.is_set():
                raise RequestCancelled('Request cancelled')
            with self.state_lock:
                slot = self.slot
            if slot and (slot['options'] != (definitions, api, casing) or slot['process'].poll() is not None):
                self._discard(slot)
                slot = None
            if slot is None:
                slot = self._start(path, definitions, api, casing)
                with self.state_lock:
                    self.slot = slot
            name = path.as_posix().encode('utf-8')
            payload = struct.pack('<IIII', len(name), line, column, len(overlays)) + name + overlays
            try:
                slot['process'].stdin.write(payload)
                slot['process'].stdin.flush()
                deadline = time.monotonic() + 15
                while True:
                    if cancelled is not None and cancelled.is_set():
                        raise RequestCancelled('Request cancelled')
                    if time.monotonic() >= deadline:
                        raise RuntimeError('Analyzer timed out')
                    try:
                        reply = slot['replies'].get(timeout=0.05)
                    except queue.Empty:
                        continue
                    if reply is None:
                        slot['errors'].seek(0)
                        raise RuntimeError(slot['errors'].read().decode('utf-8', errors='replace').strip() or 'Analyzer exited')
                    return json.loads(reply)
            except (OSError, ValueError, RuntimeError):
                self._discard(slot)
                raise
