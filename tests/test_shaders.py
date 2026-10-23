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
        for body in ['local t = {}', 'repeat until true', 'for k, v in t do end', 'print("hello")', 'local x: any = 1', 'return 1', 'local s: number = 1 .. 2']:
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
        for source in ['import "./x"', 'class X = {}', 'local x = 1', 'x = 1']:
            r = self.compile('shader canvas_item\n' + source)
            self.assertNotEqual(r.returncode, 0)
    def test_loops_and_control_flow(self):
        r = self.compile("""shader canvas_item
export Count: int = 4
function fragment()
    local total: number = 0
    for i = 0, Count - 1 do
        if i == 2 then continue elseif i > 3 then break else total += 0.25 end
    end
    for j = 10, 0, -2 do total -= 0.01 end
    local k: int = 0
    while k < 3 do k += 1 end
    Color = vec4(total % 1, total ^ 2, if k == 3 then 1 else 0, 1)
end""")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn('for (int i = 0; i <= (Count - 1); i += 1)', r.stdout)
        self.assertIn('for (int j = 10; j >= 0; j += (-2))', r.stdout)
        self.assertIn('else if ((i > 3))', r.stdout)
        self.assertIn('int k = 0;', r.stdout)
        self.assertIn('while ((k < 3))', r.stdout)
        self.assertIn('k += 1;', r.stdout)
        self.assertIn('total += 0.25;', r.stdout)
        self.assertIn('mod(total, 1.0)', r.stdout)
        self.assertIn('pow(total, 2.0)', r.stdout)
        self.assertIn('((k == 3) ? 1.0 : 0.0)', r.stdout)
    def test_helpers_varyings_constants(self):
        r = self.compile("""shader spatial
local Tint: vec3
local Scale: number = 2
local function brighten(color: vec3, amount: number): vec3
    return color * amount
end
function wave(x: number): number
    return sin(x * Scale)
end
function vertex() Tint = vec3(wave(Time), 0, 0) end
function fragment() Albedo = brighten(Tint, 1.5) end""")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn('varying vec3 Tint;', r.stdout)
        self.assertIn('const float Scale = 2.0;', r.stdout)
        self.assertIn('vec3 brighten(vec3 color, float amount) {', r.stdout)
        self.assertIn('float wave(float x) {', r.stdout)
        self.assertIn('ALBEDO = brighten(Tint, 1.5);', r.stdout)
    def test_render_modes_discard_and_ranges(self):
        r = self.compile('shader spatial\nrenderMode("unshaded", "cull_disabled")\nexport Cutoff: number = 0.5 in 0 to 1 step 0.05\nexport Steps: int = 3 in 1 to 8\nfunction fragment() if Alpha < Cutoff then discard() end end')
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertTrue(r.stdout.startswith('shader_type spatial;\nrender_mode unshaded, cull_disabled;\n'), r.stdout)
        self.assertIn('uniform float Cutoff : hint_range(0, 1, 0.05) = 0.5;', r.stdout)
        self.assertIn('uniform int Steps : hint_range(1, 8) = 3;', r.stdout)
        self.assertIn('discard;', r.stdout)
    def test_reject_invalid_functions(self):
        cases = {
            'function f(x: number): number return f(x) end\nfunction fragment() end': 'cannot recurse',
            'function fragment() Color = vec4(g(1)) end\nfunction g(x: number): number return x end': 'declared before use',
            'function f(x: number): number if x > 0 then return x end end\nfunction fragment() end': 'must end with a return',
            'function f(x) end\nfunction fragment() end': 'requires a scalar',
            'function fragment() for i = 0, 3, Time do end end': 'numeric literals',
            'export Tint: vec3 = vec3(1) in 0 to 1\nfunction fragment() end': 'Range hints',
            'renderMode(Unshaded)\nfunction fragment() end': 'string literals',
            'function fragment() discard(1) end': 'statements',
        }
        for source, message in cases.items():
            with self.subTest(source=source):
                r = self.compile('shader canvas_item\n' + source)
                self.assertNotEqual(r.returncode, 0)
                self.assertIn(message, r.stderr)
        r = self.compile('shader particles\nlocal Shared: vec3\nfunction process() end')
        self.assertIn('only available in canvas_item and spatial', r.stderr)
if __name__ == '__main__': unittest.main()
