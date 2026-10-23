"""Executable language regressions against the fork's compiler, VM, and analyzer."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

BIN = Path(sys.argv.pop(1)).resolve()
SUFFIX = '.exe' if sys.platform == 'win32' else ''

class LanguageTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
    def tearDown(self):
        self.temp.cleanup()
    def write(self, name, source):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(source)
        return path
    def run_luau(self, source, tool='luau'):
        path = self.write('main.luau', source)
        return subprocess.run([str(BIN / (tool + SUFFIX)), str(path)], capture_output=True, text=True, cwd=self.root)
    def test_class_defaults_and_implicit_self(self):
        result = self.run_luau('''
class Player = {
    export Speed: number = 1 in 0 to 1 step 0.1
    private Health: number = 100
    function damage(amount: number) self.Health -= amount end
    function health(): number return self.Health end
}
local a = Player()
local b = Player()
a:damage(5)
assert(a:health() == 95)
assert(b:health() == 100)
assert(a.Speed == 1)
''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_defaults_run_per_instance(self):
        result = self.run_luau('''
class Bag = { private Items: {number} = {} }
local a = Bag()
local b = Bag()
table.insert(a.Items, 1)
assert(#b.Items == 0)
''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_private_range_is_rejected(self):
        result = self.run_luau('class Player = { private Health: number = 100 in 0 to 100 }')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('only allowed on exported properties', result.stderr)
    def test_value_import_forms_and_alias_require(self):
        self.write('.luaurc', json.dumps({'aliases': {'helpers': './helpers'}}))
        self.write('helpers/math.luau', 'return { add = function(a, b) return a + b end, subtract = function(a, b) return a - b end }')
        result = self.run_luau('''
import (add as plus), subtract from "@helpers/math"
import "@helpers/math" as math
import "@helpers/math"
assert(plus(2, 3) == 5)
assert(subtract(8, 3) == 5)
assert(math.add(3, 4) == 7)
assert(require("@helpers/math") == math)
''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_type_import_has_no_runtime_effect(self):
        # The missing module must never be loaded by the VM.
        result = self.run_luau('''
import type Person from "./missing"
import type "./also_missing" as people
local p: Person? = nil
local q: people.Person? = nil
assert(p == q)
''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_type_import_cannot_be_read_as_value(self):
        result = self.run_luau('import type "./missing" as types\nprint(types)')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('cannot be used as a value', result.stderr)
    def test_type_import_forms(self):
        self.write('.luaurc', json.dumps({'aliases': {'helpers': './helpers'}}))
        self.write('helpers/person.luau', 'export type Person = {Name: string}\nexport type Female = Person & {Age: number}\nreturn {createPerson = function(): Person return {Name = "Ada"} end}')
        result = self.run_luau('''--!strict
import type (Person as Guy), Female from "@helpers/person"
import type "@helpers/person" as utils
import "@helpers/person" as people
local a: Guy = {Name = "Ada"}
local b: utils.Person = a
local c: people.Person = people.createPerson()
local d: Female = {Name = "Ada", Age = 30}
return {a, b, c, d}
''', 'luau-analyze')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_full_type_system_still_checks(self):
        result = self.run_luau('--!strict\ntype Box<T> = {value: T}\nlocal box: Box<number> = {value = "wrong"}\nreturn box', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('TypeError', result.stderr)
    def test_private_access_is_checked(self):
        result = self.run_luau('--!strict\nclass Counter = { private Value: number = 1 }\nlocal c = Counter()\nprint(c.Value)', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('private member', result.stderr)
    def test_inheritance_and_default_captures(self):
        result = self.run_luau('local value = 7\nclass Base = { public X: number = value }\nclass Child extends Base = { public Y: number = value + 1 }\nlocal child = Child()\nassert(child.Y == 8)')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
    def test_class_types(self):
        result = self.run_luau('''--!strict
class Counter = {
    private Value: number = 0
    function increment(amount: number) self.Value += amount end
}
local c = Counter()
c:increment("wrong")
''', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('TypeError', result.stderr)
        self.assertNotIn('SyntaxError', result.stderr)

if __name__ == '__main__': unittest.main()
