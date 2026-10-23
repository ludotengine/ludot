import pathlib
import subprocess
import sys
import tempfile
import unittest
BIN = pathlib.Path(sys.argv.pop(1)).resolve()
class ShaderTests(unittest.TestCase):
    def compile(self, source):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / 'test.lushader'
            path.write_text(source)
            return subprocess.run([str(BIN), str(path)], capture_output=True, text=True)
    def test_canvas(self):
        r = self.compile('shader canvas_item\nexport Tint: vec4 = vec4(1, 1, 1, 1)\nfunction fragment() Color = texture(Texture, UV) * Tint end')
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn('uniform vec4 Tint', r.stdout)
        self.assertIn('texture(TEXTURE, UV)', r.stdout)
    def test_spatial(self):
        r = self.compile('shader spatial\nexport Tint: vec3 = vec3(1, 0, 0)\nfunction fragment() Albedo = Tint end\nfunction light() Light = vec3(1, 1, 1) end')
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn('ALBEDO = Tint', r.stdout)
    def test_particles(self):
        r = self.compile('shader particles\nfunction start() Velocity = vec3(0, 1, 0) end\nfunction process() Velocity = Velocity + vec3(0, Delta, 0) end')
        self.assertEqual(r.returncode, 0, r.stderr)
    def test_samplers_and_matrices(self):
        r = self.compile('shader spatial\nexport Map: sampler2D\nexport Matrix: mat4 = mat4(1)\nfunction vertex() Position = Matrix * vec4(Vertex, 1) end')
        self.assertEqual(r.returncode, 0, r.stderr)
    def test_reject_dynamic_constructs(self):
        for body in ['local t = {}', 'while true do end', 'print("hello")', 'local x: any = 1', 'return 1']:
            with self.subTest(body=body):
                r = self.compile('shader canvas_item\nfunction fragment() ' + body + ' end')
                self.assertNotEqual(r.returncode, 0)
                self.assertEqual(r.stdout, '')
    def test_integer_uniform(self):
        r = self.compile('shader spatial\nexport Count: int = 3\nexport Seed: uint = 7\nfunction vertex() end')
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn('uniform int Count = 3;', r.stdout)
        self.assertIn('uniform uint Seed = 7u;', r.stdout)
    def test_reject_wrong_entry(self):
        r = self.compile('shader particles\nfunction fragment() end')
        self.assertNotEqual(r.returncode, 0)
        self.assertIn('not supported for particles', r.stderr)
    def test_lexical_rewrite(self):
        r = self.compile('shader canvas_item\n-- export is a comment\nexport Exposure: number = 1\nfunction fragment() Color = vec4(Exposure) end')
        self.assertEqual(r.returncode, 0, r.stderr)
    def test_reject_tables_and_imports(self):
        for source in ['import "./x"', 'class X = {}', 'local x: number = 1']:
            r = self.compile('shader canvas_item\n' + source)
            self.assertNotEqual(r.returncode, 0)
if __name__ == '__main__': unittest.main()
