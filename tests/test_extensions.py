"""Regressions for Ludot syntax, nominal types, lowering, and evaluation order."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

BIN = Path(sys.argv.pop(1)).resolve()
ROOT = Path(__file__).resolve().parents[1]
SUFFIX = '.exe' if sys.platform == 'win32' else ''


class ExtensionTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)

    def tearDown(self):
        self.temp.cleanup()

    def run_source(self, source, tool='luau', flags=()):
        path = self.root / 'main.luau'
        path.write_text(source)
        return subprocess.run([str(BIN / (tool + SUFFIX)), *flags, str(path)], capture_output=True, text=True, cwd=self.root, timeout=20)

    def assert_passes(self, result):
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_combined_fixture_runtime(self):
        source = (ROOT / 'tests/fixtures/extensions/combined.luau').read_text()
        # Mutation is deliberately a type error; runtime immutability must also hold.
        for level in range(3):
            with self.subTest(optimization=level):
                result = self.run_source(source, flags=(f'-O{level}',))
                self.assert_passes(result)
                self.assertIn('COMBINED_FEATURES_PASSED', result.stdout)

    def test_combined_fixture_analysis(self):
        source = (ROOT / 'tests/fixtures/extensions/combined.luau').read_text()
        source = source.replace('assert(not pcall(function() Msg.Join = 7 end))', '')
        self.assert_passes(self.run_source(source, 'luau-analyze'))

    def test_contextual_match_calls_and_repeated_optional_types(self):
        source = """--!strict
const function match(value: any) return value end
local with = 0
assert(match("string") == "string")
assert(match "short string" == "short string")
const tableResult = match { Value = 7 }
assert(tableResult.Value == 7)
match(1)
with = 2
local optional: string??=nil
optional ??= "present"
const text: string = optional
assert(text == "present" and with == 2)
return { with = with, text = text }
"""
        for tool in ('luau', 'luau-analyze'):
            self.assert_passes(self.run_source(source, tool))

    def test_enum_nominality_conversion_and_exhaustiveness(self):
        source = '''--!strict
enum Player = { Idle, Run, Jump }
enum Enemy = { Idle, Run }
const a: Player = Enemy.Idle
const b: Player = 1
const function describe(value: Player)
	return match value with
		case Player.Idle then "idle"
	end
end
print(a, b, describe(Player.Run))
'''
        result = self.run_source(source, 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('Enemy.Idle', result.stderr)
        self.assertIn("got 'number'", result.stderr)
        self.assertIn('missing Player.Run, Player.Jump', result.stderr)
        self.assertIn('(7,9)', result.stderr)

    def test_enum_refinement_and_match_unions(self):
        source = """--!strict
enum State = { One, Two, Three }
const function choose(value: State): typeof(State.One)
	if value == 1 then return value end
	return State.One
end
const function other(value: State): typeof(State.Two) | typeof(State.Three)
	if value ~= State.One then return value end
	return State.Two
end
const function result(value: State): number | string
	return match value with
		case State.One then 1
		case State.Two then "two"
		case State.Three then 3
	end
end
return { choose, other, result }
"""
        self.assert_passes(self.run_source(source, 'luau-analyze'))
        mixed = source + '\nenum Other = { One, Two }\nconst function unsafe(value: State | Other): State\n\treturn match value with case State.One then value otherwise State.One end\nend\nreturn unsafe'
        # Replace the module return so all declarations remain reachable.
        mixed = mixed.replace('return { choose, other, result }', '')
        result = self.run_source(mixed, 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('Other.One', result.stderr)

    def test_coalesce_precise_types_and_stable_property_refinement(self):
        source = """--!strict
const numberValue: number = 1 ?? "unused"
const falseValue: boolean = false ?? "unused"
const object: { Value: string? } = { Value = nil }
object.Value ??= "assigned"
const value: string = object.Value
return { numberValue = numberValue, falseValue = falseValue, value = value }
"""
        for flags in ((), ('--solver=old',)):
            self.assert_passes(self.run_source(source, 'luau-analyze', flags))

    def test_enum_invalid_declarations(self):
        cases = {
            'enum E = { A = "a", B }': 'explicit string',
            'enum E = { A, B = "b" }': 'mix',
            'enum E = { A = 1.5 }': 'integer',
            'enum E = { A, A }': 'Duplicate',
            'enum E = { from }': 'reserved',
        }
        for source, message in cases.items():
            with self.subTest(source=source):
                result = self.run_source(source)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(message, result.stderr)

    def test_enum_export_and_class_fields(self):
        (self.root / 'state.luau').write_text('export enum State = { Idle, Running }')
        source = '''--!strict
import "./state" as states
class Player = { public State: states.State = states.State.Idle }
const player = Player.new()
assert(player.State == states.State.Idle)
'''
        for tool in ('luau', 'luau-analyze'):
            self.assert_passes(self.run_source(source, tool))

    def test_optional_laziness_and_parentheses(self):
        source = '''local calls = 0
const function argument() calls += 1 return 3 end
const missing = nil
assert(missing?:method(argument()).Value == nil)
assert(calls == 0)
assert(not pcall(function() return (missing?.Value).Other end))
const object = { child = { Value = 7 } }
assert(object?.child?.Value == 7)
const receiver = { method = function(self, value) assert(self ~= nil) return { Value = value } end }
assert(receiver?:method(argument()).Value == 3)
assert(calls == 1)
'''
        self.assert_passes(self.run_source(source))
        result = self.run_source('--!strict\nconst function f(v: { Name: string }?) return v?.Missing end\nreturn f', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Key 'Missing'", result.stderr)

    def test_coalesce_assignment_evaluates_receivers_and_keys_once(self):
        source = '''local receivers, keys, values = 0, 0, 0
const target = {}
const function receiver() receivers += 1 return target end
const function key() keys += 1 return "value" end
const function value() values += 1 return false end
receiver()[key()] ??= value()
receiver()[key()] ??= value()
assert(receivers == 2 and keys == 2 and values == 1)
assert(target.value == false)
'''
        self.assert_passes(self.run_source(source))
        result = self.run_source('const fixed = nil\nfixed ??= 1', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('const', result.stderr)

    def test_ranges_scope_closures_break_continue_and_zero_step(self):
        source = '''local evaluations = 0
const function bound(value) evaluations += 1 return value end
const closures = {}
for i in bound(10) to bound(1) step bound(-3) do
	if i == 7 then continue end
	table.insert(closures, function() return i end)
	if i == 4 then break end
end
assert(evaluations == 3 and #closures == 2)
assert(closures[1]() == 10 and closures[2]() == 4)
const ok, message = pcall(function() for i in 1 to 3 step 0 do error("body") end end)
assert(not ok and string.find(message, "step must not be zero"))
'''
        for flags in (('-O0',), ('-O2',), ('--codegen',)):
            self.assert_passes(self.run_source(source, flags=flags))

    def test_pipeline_precedence_order_multireturn_and_varargs(self):
        source = '''const order = {}
const function input() table.insert(order, "input") return nil, 99 end
const function stage() table.insert(order, "stage") return function(value, extra) table.insert(order, "call") return extra, 100 end end
const function argument() table.insert(order, "argument") return 3 end
const a, b = input() |> stage()(argument())
assert(a == 3 and b == nil)
assert(table.concat(order, ",") == "input,stage,argument,call")
const function double(value) return value * 2 end
const function nothing(value) return nil end
assert((1 |> nothing ?? 3 |> double) == 6)
assert((1 |> nothing ?? (4 |> double)) == 8)
assert((1 + 2 |> double) == 6)
assert((false or 3 |> double) == 6)
const function varargs(...) return nil ?? (...) |> double end
assert(varargs(4, 9) == 8)
const function id<T>(value: T): T return value end
const typed: string = "yes" |> id
assert(typed == "yes")
'''
        for flags in (('-O0',), ('-O2',)):
            self.assert_passes(self.run_source(source, flags=flags))

    def test_generic_pipeline_with_nullable_array_fallback(self):
        source = (ROOT / 'tests/fixtures/extensions/generic_pipeline.luau').read_text()
        for tool in ('luau', 'luau-analyze'):
            result = self.run_source(source, tool)
            self.assert_passes(result)
            self.assertNotIn('MultiLineStatement', result.stderr)
        empty = source.replace('Alive = true', 'Alive = false').replace('assert(result ~= nil and result.Name == "near")', 'assert(result == nil)')
        for tool in ('luau', 'luau-analyze'):
            self.assert_passes(self.run_source(empty, tool))
        invalid = source.replace('predicate: (T) -> boolean', 'predicate: (T) -> number', 1)
        self.assertNotEqual(self.run_source(invalid, 'luau-analyze').returncode, 0)

    def test_pipeline_stage_keeps_register_reuse_in_user_functions(self):
        body = []
        for index in range(1, 301):
            body.append(f'\tlocal value{index} = identity({index})\n\ttotal += value{index}')
        source = 'const function identity(value: number) return value end\nconst result = 0 |> function(input: number)\n\tlocal total = input\n' + '\n'.join(body) + '\n\treturn total\nend\nassert(result == 45150)'
        self.assert_passes(self.run_source(source))

    def test_match_order_aliases_and_control_flow(self):
        source = '''enum E = { A, Alias = 1, B }
local calls = 0
const function once() calls += 1 return E.A end
assert((match once() with case E.A then 1 case E.B then 2 end) == 1)
assert(calls == 1)
local count = 0
for i in 1 to 5 do
	match i with
		case 2 then continue
		case 4 then break
		otherwise count += 1
	end
end
assert(count == 2)
'''
        self.assert_passes(self.run_source(source))
        self.assert_passes(self.run_source(source, 'luau-analyze'))
        result = self.run_source('--!strict\nenum E = { A, B }\nconst v: E = E.A\nconst x = match v with case E.A then 1 case E.A then 2 otherwise 3 end\nreturn x', 'luau-analyze')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('Duplicate', result.stderr)

    def test_godot_global_and_builtin_enum_types(self):
        source = """--!strict
const key: Enum.Key = Enum.Key.W
const button: Enum.MouseButton = Enum.MouseButton.Left
const axis: Enum.Axis = Enum.Axis.Z
const kind: Enum.VariantType = Enum.VariantType.Int
const converted: Enum.Axis? = Enum.Axis:fromValue(2)
const named: Enum.MouseMode? = Enum.MouseMode:fromName("Captured")
const items: { Enum.MouseMode } = Enum.MouseMode:getEnumItems()
const enums: { Enum } = Enum:getEnums()
const item: EnumItem = Enum.MouseMode.Captured
const name: string = Enum.MouseMode.Captured.Name
const value: number = Enum.MouseMode.Captured.Value
const same: Enum.MouseMode = Enum.MouseMode.Captured.EnumType.Captured
const legacy: Enum.MouseMode = Input.MOUSE_MODE_CAPTURED
Input:setMouseMode(Enum.MouseMode.Captured)
const current: Enum.MouseMode = Input:getMouseMode()
const process: Enum.NodeProcessMode = Node.new().ProcessMode
const text = match current with
	case Enum.MouseMode.Visible then "visible"
	case Enum.MouseMode.Hidden then "hidden"
	case Enum.MouseMode.Captured then "captured"
	case Enum.MouseMode.Confined then "confined"
	case Enum.MouseMode.ConfinedHidden then "confined hidden"
	case Enum.MouseMode.Max then "max"
end
return { key, button, axis, kind, converted, named, items, enums, item, name, value, same, legacy, process, text }
"""
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))
        for wrong, message in (
            ('enum MyEnum = { Yes, No }\nreturn Enum.MyEnum', "MyEnum"),
            ('return Enum.MouseMode.Nope', "Nope"),
            ('const number: number = Enum.MouseMode.Captured\nreturn number', "number"),
            ('const old: Input.MouseMode = Input.MOUSE_MODE_CAPTURED\nreturn old', "Input.MouseMode"),
            ('return Key.KEY_W', "Key"),
            ('const mode: Enum.MouseMode = Enum.Key.W\nreturn mode', "Enum.Key"),
        ):
            result = self.run_source('--!strict\n' + wrong, 'luau-analyze', flags)
            self.assertNotEqual(result.returncode, 0, wrong)
            self.assertIn(message, result.stderr, wrong)

    def test_task_library_types(self):
        source = """--!strict
const worker: thread = task.spawn(function(value: number) print(value) end, 1)
const deferred: thread = task.defer(worker, "resumed")
const delayed: thread = task.delay(0.5, function() end)
const later: thread = task.delay(nil, worker)
const elapsed: number = task.wait(0.1)
const frame: number = task.wait()
task.cancel(delayed)
return { worker, deferred, later, elapsed, frame }
"""
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))
        for wrong in ('const text: string = task.wait()\nreturn text', 'task.cancel(5)', 'task.spawn(5)', 'task.delay("soon", function() end)'):
            result = self.run_source('--!strict\n' + wrong, 'luau-analyze', flags)
            self.assertNotEqual(result.returncode, 0, wrong)

    def test_signal_cases_type_check(self):
        source = (ROOT / 'tests/fixtures/signals/cases.luau').read_text()
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))

    def test_signal_misuse_is_reported(self):
        source = (ROOT / 'tests/fixtures/signals/errors.luau').read_text()
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        result = self.run_source(source, 'luau-analyze', flags)
        output = result.stdout + result.stderr
        self.assertNotEqual(result.returncode, 0)
        lines = source.splitlines()
        errors = {int(line) for line in __import__('re').findall(r'main\.luau\((\d+),\d+\): TypeError', output)}
        for needle in ('fire("full", 75)', 'fire(100)', 'function(previous: string', 'const name: string', ':emit(', 'p.Died:fire(5)'):
            line = next(i + 1 for i, text in enumerate(lines) if needle in text)
            self.assertIn(line, errors, needle + '\n' + output)
        untyped = next(i + 1 for i, text in enumerate(lines) if text.startswith('const Untyped'))
        self.assertIn(f'main.luau({untyped},30): SignalDeclaration', output)
        # The untyped parameter defaults to any, so its fires are not errors.
        self.assertNotIn(untyped + 1, errors)

    def test_signal_declaration_lint(self):
        source = """--!strict
const Typed = signal with (previous: number, current: number)
const Variadic = signal with (...any)
const Untyped = signal with (value)
const BareVariadic = signal with (...)
class Bag = {
	public Loose = signal with (item, count: number)
}
return { Typed, Variadic, Untyped, BareVariadic, Bag }
"""
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        result = self.run_source(source, 'luau-analyze', flags)
        output = result.stdout + result.stderr
        self.assertIn("main.luau(4,30): SignalDeclaration: Signal parameter 'value' has no type annotation, so its type defaults to any", output)
        self.assertIn("main.luau(5,35): SignalDeclaration: Variadic signal parameter '...' has no type annotation, so its type defaults to any", output)
        self.assertIn("main.luau(7,30): SignalDeclaration: Signal parameter 'item' has no type annotation, so its type defaults to any", output)
        self.assertEqual(output.count('SignalDeclaration'), 3, output)

    def test_signal_fields_on_engine_classes(self):
        source = """--!strict
class Player extends CharacterBody3D = {
	public Died = signal with ()
	public HealthChanged = signal with (previous: number, current: number)
	private Internal = signal with (code: number)
	export Shown = signal with ()
	onready public Late = signal with ()
	function _ready()
		self.Died = signal with ()
		self.Internal:fire(1)
		const connection: Connection = self.TreeExiting:connect(function() end)
		self.ChildEnteredTree:connect(function(node: Node) print(node.Name, connection.Connected) end)
		self.ChildEnteredTree:fire(5)
	end
}
class Bag = {
	public Changed = signal with ()
}
const bag = Bag.new()
bag.Changed = signal with ()
const player = Player.new()
player.Internal:fire(2)
return { Player, bag, player }
"""
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        result = self.run_source(source, 'luau-analyze', flags)
        output = result.stdout + result.stderr
        self.assertIn("main.luau(6,9): TypeError: Signal field 'Shown' cannot be exported", output)
        self.assertIn("main.luau(7,17): TypeError: Signal field 'Late' cannot be onready", output)
        self.assertIn("main.luau(9,3): TypeError: Signal field 'Died' is read-only", output)
        self.assertIn("main.luau(13,30): TypeError: Expected this to be 'Node', but got 'number'", output)
        self.assertIn("main.luau(20,1): TypeError: Signal field 'Changed' is read-only", output)
        self.assertIn("Internal", output)
        self.assertEqual(output.count('TypeError'), 6, output)

    def test_signal_method_casing(self):
        source = """--!strict
const Ping = signal with (text: string)
const connection: Connection = Ping:Connect(function(text) print(text) end)
Ping:Fire("x")
Ping:DisconnectAll()
print(connection.connected)
"""
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'), '--ludot-method-casing=PascalCase', '--ludot-property-casing=camelCase')
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))

    def test_godot_is_a_all_objects_and_invalid_names(self):
        source = '''--!strict
const function read(event: InputEvent): Vector2?
	if event:IsA("InputEventMouseMotion") then return event.Relative end
	return nil
end
const function move(node: Node)
	if node:isA("CharacterBody3D") then node.Velocity = Vector3.new() end
end
return { read, move }
'''
        flags = ('--ludot-api=' + str(ROOT / 'types/engine.api'),)
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))
        result = self.run_source(source.replace('InputEventMouseMotion', 'ImaginaryEvent'), 'luau-analyze', flags)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Unknown Godot class 'ImaginaryEvent'", result.stderr)
        source = """--!strict
const function read(event: InputEventMouseMotion | InputEventKey): number
	if event:isA("InputEventMouseMotion") then return event.Relative.X end
	return event.Keycode.Value
end
return read
"""
        self.assert_passes(self.run_source(source, 'luau-analyze', flags))
        dynamic = """--!strict
const function read(event: InputEvent, name: string)
	if event:isA(name) then return event.Relative end
	return nil
end
return read
"""
        result = self.run_source(dynamic, 'luau-analyze', flags)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Key 'Relative'", result.stderr)


if __name__ == '__main__':
    unittest.main()
