# Ludot language

Ludot's language is a fork of Luau 0.743. The parser, analyzer, compiler, and VM
implement every extension on this page directly, so there is no transpiler between
your code and the compiler.

Everything from standard Luau still works: unions, intersections, generics, type
packs, refinements, and type functions. `const` keeps Luau's meaning: the binding
can't be reassigned, but the table or object it points to can still change.

## Classes

```lua
class Player extends CharacterBody2D = {
    export Speed: number = 1 in 0 to 1 step 0.1
    private Health: number = 100

    function _ready()
        print("Ready!")
    end

    function damage(amount: number)
        self.Health -= amount
    end
}
```

A class is a brace block of fields and methods.

- Methods receive `self` automatically. You don't declare it.
- Field defaults run once per instance, so a `{}` default gives each instance its
  own table.
- An optional `__init` method runs after the field defaults. Its parameters become
  the constructor's parameters.
- Classes compile to native class bytecode and VM objects, not tables with
  metatables.

The older experimental `class ... end` syntax still exists internally.

### Creating instances

Use `.new()` for Luau classes, native nodes and resources, and value types:

```lua
const node = Node3D.new()
const position = Vector3.new(1, 2, 3)
const player = Player.new()
```

The checker and completion know the instance type `.new()` returns and the
arguments it takes. Because of this, `new` is reserved and can't be used as a member
name. Older direct calls such as `Vector3(1, 2, 3)` still work.

### Field visibility

| Keyword | Inspector | Access |
| --- | --- | --- |
| `export` | Shown and editable | Anywhere |
| `public` | Hidden | Anywhere |
| `private` | Hidden | Only inside the declaring class |

Privacy is a type-checking rule. It doesn't stop reflective engine calls or
unchecked bytecode from reaching the field.

Range hints such as `in 0 to 1 step 0.1` only work on `export` fields, since they
only affect the Inspector. Ludot has no decorators; `export`, `onready`, and the
visibility words are plain keywords.

### Onready properties

`onready` delays a field's initializer until the node is ready, like GDScript's
`@onready`. The initializer can use `self`, so it can look up child nodes that don't
exist yet when the instance is created:

```lua
class Player extends CharacterBody3D = {
    onready public Head = self:getNode("Head") :: Node3D
    onready public Camera = self.Head:getNode("Camera") :: Camera3D
    onready private Muzzle = self:getNode("Arm/Hand/Muzzle")

    function _ready()
        self.Camera.Fov = 70
    end
}
```

`getNode` takes a Godot node path, so `"Arm/Hand/Muzzle"` reaches a
great-grandchild.

**When they run:**

- Immediately before `_ready`, in declaration order. A later initializer can read an
  earlier one, and `_ready` sees all of them.
- Base class initializers run before derived class initializers.
- They run even if the class has no `_ready` method.
- Until then, the fields are `nil`.

**Types:**

- Without an annotation, the field takes its initializer's type.
- A `::` assertion, such as `:: Node3D`, narrows `getNode`'s general `Node` result.
- When the editor knows the attached scene, `getNode` with a literal path already
  returns the right node type, so the assertion is optional.
- An annotation such as `onready public Head: Node3D = ...` must match the
  initializer's type.

**Rules:**

- Write `onready` before `public` or `private`, and always give it an initializer.
- It only works in brace classes.
- `export` fields can't be onready, because the initializer would overwrite the
  value set in the Inspector.
- Hot reload keeps current onready values. The initializers run again only the next
  time the node becomes ready.

### Scripts in the editor

Luau node scripts don't run while you edit a scene. The editor shows placeholders
for exported fields, and your code runs only when the project runs. You don't need
`Engine:isEditorHint()` checks in `_ready`, `_process`, or input callbacks.

Editor tool scripts aren't supported yet.

### Inheritance

A script attached to an object extends either a native engine class, such as
`Node`, `CharacterBody2D`, or `Resource`, or another Luau script class imported from
its module.

Derived scripts inherit fields, defaults, and methods. Reloading a base script also
reloads every live instance of its derived scripts.

Ordinary Luau modules that aren't attached to anything can return tables,
functions, or classes.

## Imports

Every import starts with `import` and a module path. What comes after the path
decides what you get.

```lua
import "@helpers/setup"
import "@helpers/mathutils" as math
import "@helpers/mathutils".{ add, subtract }
import "@helpers/mathutils".{ add as plus, subtract }
import "@helpers/personutils".{ type Person, createPerson }
import "@helpers/personutils" as people.{ type Person }
import type "@helpers/personutils" as utils
```

| Form | What you get |
| --- | --- |
| `import "path"` | Runs the module for its side effects |
| `import "path" as name` | The whole module, including its exported types |
| `import "path".{ a, b }` | Named values, as immutable locals |
| `import "path".{ a as b }` | A named value under a different local name |
| `import "path".{ type A }` | A named type |
| `import "path" as name.{ a }` | The whole module and named items in one statement |
| `import type "path" as name` | A type-only namespace, which can't be read as a value |

Each import statement loads its module once.

A module exports types with `export type`. It exports values by returning a table or
with Luau's value export syntax.

### Named items

The braces list the exports you want, much like `use module::{a, b}` in Rust. The
path comes first, so the editor can complete item names from the module's exports.

- `as` renames the name to its left, whether that's an item or the module.
- Mark types with `type`, as in TypeScript's `import { type A }`. Values and types
  can share one list. If every item is a type and there's no namespace, the module
  isn't loaded at runtime.
- A long list can span several lines and end with a trailing comma:

```lua
import "@pkgs/ui".{
	Button,
	type ButtonProps,
	Label as Text,
}
```

`type` still works as an ordinary name. `.{ type }` and `.{ type as kind }` import a
value called `type`. `type` followed by another name marks a type.

### Namespaces

`as name` binds the whole module to one local, like `import module as name` in
Python. Exported types are available as `name.Type`.

`import type "path" as name` binds a namespace for types only. The module isn't
loaded at runtime, and reading `name` as a value is an error. A type-only import
can't take an item list. Mark the items with `type` instead.

### Aliases

Define aliases in a `.luaurc` file at the project root:

```json
{
	"languageMode": "strict",
	"aliases": {
		"helpers": "./src/helpers",
		"pkgs": "./packages"
	}
}
```

The engine also reads a `luau/modules/aliases` dictionary from Project Settings.
Both `import` and the older `require()` use these aliases.

### Module resolution

- A path resolves to a `.luau` file, or to `init.luau` inside a directory.
- The engine and the CLI tools read every `.luaurc` from the importing file's
  directory up through its ancestors. Nearer files override outer ones.
- The engine applies the Project Settings aliases first.

## Enums

An enum declares a fixed set of named values and gives them their own type.

```lua
enum Action = {
	Join,
	Chat,
	Leave = 10,
}

enum Message = {
	Join = "join",
	Chat = "chat",
}

const action: Action = Action.Chat
const checked: Action? = Action.from(2)
```

### Values

- Numeric members start at 1 and count up.
- An explicit integer, like `Leave = 10`, restarts the count for the members after
  it. Integers must fit exactly in a Luau number.
- In a string enum, every member needs an explicit string.
- An enum can't mix numbers and strings.

### Type safety

An enum is a distinct type. These are type errors:

- assigning a plain number or string to an enum variable
- assigning one enum type to another

To turn an unchecked value into an enum, use `Action.from(value)`. It returns the
matching member or `nil`. Because of this, `from` is reserved and can't be a member
name.

At runtime, members are still plain numbers or strings, including when they're
passed to Godot. Comparing a member with its underlying value using `==` is allowed.
Two members with the same value are aliases. They compare equal and count as the
same case in a `match`.

### Exporting and the Inspector

- Use `export enum` to share an enum, then import it like any other module export.
- Enum types can annotate class fields, including `export` fields.
- Exported enum fields show a dropdown of member names. Scenes store the underlying
  values.
- String values can contain commas or colons. The dropdown keeps names and values
  separate instead of parsing display text.

### Iteration

Looping over an enum yields its members in declaration order, aliases included. The
`from` function isn't part of the loop:

```lua
for name, value in Action do
	print(name, value)
end
```

### Engine enums

Every engine enum lives under one global, `Enum`, modeled on Roblox's. Each enum is
an `Enum` object, and each value is an `EnumItem`:

```lua
Input:setMouseMode(Enum.MouseMode.Captured)

print(Enum.MouseMode.Captured)          -- Enum.MouseMode.Captured
print(Enum.MouseMode.Captured.Name)     -- Captured
print(Enum.MouseMode.Captured.Value)    -- 2
print(Enum.MouseMode.Captured.EnumType) -- Enum.MouseMode

print(Enum.MouseMode:fromName("Captured")) -- Enum.MouseMode.Captured, or nil
print(Enum.MouseMode:fromValue(2))         -- Enum.MouseMode.Captured, or nil
print(#Enum.MouseMode:getEnumItems())      -- 6
print(#Enum:getEnums())                    -- every engine enum

for name, item in Enum.MouseMode do
	print(name, item.Value)
end
```

- `typeof` returns `"EnumItem"` or `"Enum"`.
- Each item is a single shared value, so `==` compares identity.
  `Enum.MouseMode.Captured.EnumType.Captured` is the same item.
- Indexing a missing enum or item is an error.
- Your own `enum` declarations are separate and don't appear under `Enum`.

#### How names are built

Item names drop the words every constant in the enum shares, then use PascalCase.
`MOUSE_MODE_CONFINED_HIDDEN` becomes `ConfinedHidden`. A name that would start with
a digit keeps the last shared word, so `KEY_0` becomes `Enum.Key.Key0`.

Enum names follow these rules:

| Case | Result | Examples |
| --- | --- | --- |
| The Godot name is unique | Kept as is | `Enum.Key`, `Enum.MouseButton`, `Enum.LayoutPreset` |
| Same name, same items | Merged into one enum | `Input.MouseMode` and `DisplayServer.MouseMode` are both `Enum.MouseMode` |
| Same name, different items | Prefixed with the class | `Enum.Light3DParam`, `Enum.NodeProcessMode`, `Enum.VariantType` |

`Control`, `Input`, and `DisplayServer` all share `Enum.CursorShape`. Editor-only
enums are left out, so the editor and exported games see the same names.

#### Passing and returning enums

- Engine methods and properties that take an enum accept an `EnumItem` or its number.
- Those that return an ordinary enum return the `EnumItem`, for example
  `Input:getMouseMode()` and `node.ProcessMode`.
- A value with no matching item, such as a key code with modifier bits, stays a
  number.
- Bitfield enums such as `Enum.MouseButtonMask` return numbers, because their values
  combine. Use `.Value` with `bit32` to combine items. Bitfield parameters and
  properties still accept numeric masks.

#### Types

Annotate engine enums as `Enum.<Name>`, such as `Enum.MouseMode`. `EnumItem` and
`Enum` are the general types.

Engine methods, properties, `fromName`, and `getEnumItems` are typed with the
specific enum, and a `match` over an engine enum checks that every case is covered.
An `export` field typed `Enum.<Name>` shows a dropdown and stores the number in the
scene.

#### Class constants

Class constants keep Godot's names, such as `Input.MOUSE_MODE_CAPTURED`.

- Non-bitfield constants are the matching `EnumItem`, so
  `Input.MOUSE_MODE_CAPTURED == Enum.MouseMode.Captured`.
- Bitfield constants, such as `Control.SIZE_EXPAND_FILL`, stay numbers.
- Value type constants, such as `Vector3.AXIS_Z`, stay numbers.

#### Casing

Method names (`fromName`, `getEnumItems`) follow the project's method casing.
`Name`, `Value`, and `EnumType` follow the property casing. Enum and item names
never change with casing.

#### Replaced forms

`Enum` replaces these older forms, which no longer exist:

- class enum namespaces, such as `Input.Enums.MouseMode`
- global enum tables, such as `Key.KEY_W`, `MouseButton`, and `Variant.Type`
- annotations in the `Input.MouseMode` style

## Tasks

The `task` library schedules threads. It's modeled on Roblox's.

| Function | What it does |
| --- | --- |
| `task.spawn(fnOrThread, ...)` | Runs it now in a new thread, or resumes the given thread. Returns the thread. |
| `task.defer(fnOrThread, ...)` | Same as `spawn`, but runs after the current engine callback returns, later in the same frame. |
| `task.delay(seconds, fnOrThread, ...)` | Resumes it after the delay. Returns the thread. |
| `task.wait(seconds?)` | Pauses the current thread and returns how much time actually passed. With no argument, waits one frame. |
| `task.cancel(thread)` | Stops a scheduled or suspended thread. It can't cancel a thread that is running. |

```lua
class Door extends Node3D = {
	function _ready()
		task.wait(1)
		self:open()

		task.delay(5, function()
			self:close()
		end)
	end
}
```

### Waiting inside callbacks

Engine callbacks such as `_ready`, `_process`, and signal handlers each start in a
thread that can pause, so they can call `task.wait` directly. When a callback pauses,
the engine gets `nil` back right away, and the rest of the callback runs when the
thread resumes.

Module top-level code and field initializers can't pause.

### Timing

- Each VM has one scheduler. It steps at the start of every SceneTree process frame,
  before `_process`.
- Waits and delays use game time. They follow `Engine.time_scale`.
- Like a Timer, they stop while their node can't process, such as during a pause. A
  node with `PROCESS_MODE_ALWAYS` keeps waiting through a pause.
- Threads that don't belong to a node stop during a pause.
- A deferred thread that defers again runs in the next round, up to 64 rounds a
  frame.

### Ownership

A thread belongs to the object whose callback started it. Threads it spawns, and
signal handlers its code connects, belong to the same object.

When the object is freed, its scheduled threads are dropped. They never resume with
a freed `self`.

An error in a resumed thread prints with its stack trace. If a debugger is attached,
it breaks on the error.

### Cost

Starting callbacks in threads is cheap. Finished threads are reused, so an ordinary
callback allocates nothing. On Linux, a GDScript call into a Luau method measured
0.74 µs, against 0.69 µs with the previous `pcall` path.

## Signals

Signals are created with `signal with (...)`, modeled on Roblox's RBXScriptSignal.
The parameter list names and types the arguments the signal fires with, and its
type is `Signal<T...>`:

```lua
const HealthChanged = signal with (previous: number, current: number)

const connection: Connection = HealthChanged:connect(function(previous, current)
	print(previous, current)
end)

HealthChanged:fire(100, 75)
print(connection.Connected) --> true
connection:disconnect()

const Saved = signal with ()
const Debug = signal with (...any)
```

| Form / Member | What it does |
| --- | --- |
| `signal with (param: Type, ...)` | Creates a signal. Parameter names also label its arguments for the engine. |
| `signal:connect(handler)` | Runs `handler` on every fire. Returns a `Connection`. |
| `signal:once(handler)` | Runs `handler` on the next fire only. Returns a `Connection`. |
| `signal:fire(...)` | Calls every connected handler with the arguments. |
| `signal:wait()` | Pauses the current thread until the next fire, then returns its arguments. |
| `signal:disconnectAll()` | Disconnects every handler connected through this API. |
| `connection:disconnect()` | Disconnects the handler. Calling it twice does nothing. |
| `connection.Connected` | Whether the handler is still connected. |

### Names and types

- Methods follow `luau/api/method_casing`, so they become `Fire` and `Connect` in
  PascalCase. `Connected` follows `luau/api/property_casing`.
- Luau uses `fire`. GDScript keeps Godot's `Signal.emit`.
- `Signal` is a type only (`Signal<number, number>`, `Signal<>`, `Signal<...any>`).
  `typeof` returns `"Signal"` or `"Connection"`.
- Handler parameter types come from the signal, so you can leave them off.
- A signal parameter without a type annotation, such as `signal with (value)`,
  defaults to `any` and triggers the `signal_declaration` warning.

### How handlers run

- Each handler runs in its own thread, so it can call `task.wait`.
- Handlers run in connection order. The list is copied when a fire starts, so a
  handler can connect or disconnect others safely.
- Connecting the same function twice makes it run twice.
- If a handler errors, the error prints with its stack trace and the remaining
  handlers still run.
- `wait` errors outside a thread that can pause, such as module top-level code.
- A handler belongs to the object whose code connected it. When that object is
  freed, its connections are dropped.

### Engine signals

Engine signals use the same type. `self.TreeExiting` is a `Signal<>`, and
`self.ChildEnteredTree` is a `Signal<Node>`, typed from the engine's argument
information.

- `connect` returns a `Connection`.
- `fire` emits the engine signal, so GDScript handlers run too.
- `disconnectAll` removes only connections made through this API. It never touches
  editor or GDScript connections.

### Signal fields

In a class that extends an engine class, a `public` field initialized with
`signal with (...)` also becomes a script signal with the same name:

```lua
class Player extends CharacterBody3D = {
	public Died = signal with ()
	public HealthChanged = signal with (previous: number, current: number)
	private InternalTick = signal with (dt: number)
	public Health: number = 100

	function damage(amount: number)
		const previous = self.Health
		self.Health -= amount
		self.HealthChanged:fire(previous, self.Health)
	end
}
```

These signals work everywhere a GDScript signal does:

- The Node dock lists them with their parameter names and types, and scenes save
  their connections.
- GDScript can call `connect("Died", ...)` and `emit_signal("HealthChanged", 1.0, 2.0)`.
- Firing from Luau and emitting from GDScript reach the same handlers.

`private` signal fields stay internal to the declaring class: they are not
registered as engine signals and cannot be accessed from other classes.

Signal fields are read-only, cannot be `export`ed or `onready`, and do not appear
in the Inspector.

### GDScript interoperation

GDScript keeps Godot syntax and native APIs, including `Node.new()`, `@export`,
`_ready`, `signal`, `await`, `Signal.connect`, and `Signal.emit`. The Luau API
casing settings do not rename GDScript's engine methods or properties. Public
members declared in a Luau script keep their authored names in both languages.

Given the `Player` class above saved as `player.luau`, GDScript can instantiate it
and use its signals directly:

```gdscript
extends Node

const Player = preload("res://player.luau")

func _ready() -> void:
	var player = Player.new()
	player.HealthChanged.connect(_on_health_changed)
	add_child(player)
	player.damage(25.0)
	player.HealthChanged.emit(75.0, 60.0)
	player.queue_free()

func _on_health_changed(previous: float, current: float) -> void:
	print(previous, " -> ", current)
```

The `.emit` call notifies listeners; it does not change the player's health field.
GDScript's string-based `connect` and `emit_signal` APIs also work. A public Luau
method read from GDScript, such as `player.damage`, is a Godot `Callable`.
Engine-backed Luau script resources accept `.new(...)` from GDScript and forward
constructor arguments to the Luau class's `__init` method. Nodes need the usual
`free` or `queue_free`; RefCounted instances use Godot reference counting.

In Luau, the same field remains a `Signal<number, number>`. Use
`self.HealthChanged:fire(previous, current)` and the Luau `Connection` API.
GDScript connections and Luau connections receive emissions from either language.

Public Luau signal declarations and adjacent `---` documentation comments also
appear in the editor's script class reference. Private signals remain internal.

### Passing values

Values passed through an engine signal convert like any other engine call. Tables
and plain class objects travel as `LuauValue` references. Luau handlers receive the
same table or object. GDScript handlers receive a `LuauValue` and can call
`to_variant()` on it.

## Match expressions and statements

A `match` compares one value against a list of cases, top to bottom. Only the first
matching case runs, and there is no fallthrough.

```lua
const text = match action with
	case Action.Join then "joined"
	case Action.Chat then "chatting"
	case Action.Leave then "left"
end

match health with
	case 0 then
		print("Dead")
	case if health < 25 then
		print("Critical health")
	otherwise
		print("Alive")
end
```

The value after `match` is evaluated once.

### Cases

| Case | Matches when |
| --- | --- |
| `case 0`, `case "idle"`, `case Action.Join` | The value equals this literal or enum member |
| `case if condition` | The condition is true (a guard) |
| `otherwise` | Nothing above matched |

### Expressions and statements

**Match expressions** produce a value. Each case holds one expression, and the
result type is the union of all of them. An expression must cover every case or end
with `otherwise`. If it doesn't, the error lists the missing enum members.

**Match statements** run code. Missing cases produce a `match_exhaustiveness`
warning instead of an error. `return`, `break`, and `continue` inside a statement
act on the enclosing function or loop as usual. A match expression is its own
boundary.

### Checks

The checker reports:

- duplicate values
- values that can't match the subject's type
- cases written after `otherwise`

Aliases with the same value count as one case. Guards never count toward complete
coverage, even a guard that is always true. Such a guard does make every later case
unreachable.

### Reserved words

`case` and `otherwise` are only keywords at the start of a case inside a match.
Elsewhere they are ordinary names. A function named `match` can still be called with
parentheses.

## Optional access and nil coalescing

```lua
const parentName = node?:getParent()?.Name ?? "No parent"
const enabled = settings.Enabled ?? true

local value: boolean? = nil
value ??= false
```

| Operator | Meaning |
| --- | --- |
| `a?.b` | Read `b`, or skip if `a` is nil |
| `a?:m()` | Call `m`, or skip if `a` is nil |
| `a ?? b` | `a`, unless it's nil, then `b` |
| `a ??= b` | Assign `b` only if `a` is nil |

### Optional chains

When the receiver is nil, the rest of the chain is skipped, including any argument
expressions, and the result is nil. Each optional receiver is evaluated once.

Only receivers marked with `?` are guarded:

- `node?.Child.Name` guards `node`, but `Child` must exist.
- `node?.Child?.Name` guards both.
- Parentheses end a chain. `(node?.Child).Name` reads `Name` from the result
  normally.

Reading a member that doesn't exist on a valid receiver is still a type error.

### Nil coalescing

`??` only uses the fallback for `nil`. Unlike `or`, it keeps `false`:

```lua
const settings = { Sound = false }
print(settings.Sound ?? true) --> false
print(settings.Sound or true) --> true
```

The fallback is only evaluated when it's needed.

`??=` reads its target once and assigns only when the target is nil. Indexed targets
compute their table and key once. Assigning to a `const` or to something that isn't
assignable is an error.

The checker narrows assigned locals and stable indexed locations after `??=`. It
doesn't assume that an expression with side effects returns the same object twice.

## Inclusive ranges

```lua
for i in 1 to 10 do
	print(i) -- 1, 2, ..., 10
end

for i in 1 to 10 step 5 do
	print(i) -- 1, then 6
end

for i in 10 to 1 step -3 do
	print(i) -- 10, 7, 4, 1
end
```

- Both ends are included.
- The start, end, and step are evaluated once, in that order. The default step is 1.
- A step of zero raises a runtime error before the loop body runs.
- The loop variable has the same scope and closure behavior as a numeric `for`.

The classic `for i = start, limit, step` and generic `for` loops still work.

## Pipelines and precedence

The pipeline operator `|>` is a convenience. It doesn't add anything you couldn't
write before. It changes the order you write function calls in, so a chain of steps
reads in the order it runs.

```lua
x |> f(a, b)   -- same as f(x, a, b)
x |> f()       -- same as f(x)
x |> f         -- same as f(x)
```

The value on the left becomes the first argument of the function on the right.

### What it replaces

Say you want the closest living enemy. With three small functions, `filter`, `sort`,
and `first`, you'd usually write one of these:

```lua
-- Nested calls: read from the inside out.
const target = first(sort(filter(enemies, isAlive), byDistance))

-- Temporary variables: clear, but every step needs a name.
const alive = filter(enemies, isAlive)
const sorted = sort(alive, byDistance)
const target = first(sorted)
```

The pipeline version makes the same calls in the same order:

```lua
const target = enemies
	|> filter(isAlive)
	|> sort(byDistance)
	|> first()
```

It reads top to bottom: take the enemies, keep the living ones, sort them by
distance, take the first. You can add, remove, or reorder a step by editing one line.

<details>
<summary>The helper functions used above</summary>

```lua
type Enemy = { Name: string, Alive: boolean, Distance: number }

const function filter<T>(values: { T }, keep: (T) -> boolean): { T }
	const result: { T } = {}
	for _, value in values do
		if keep(value) then
			table.insert(result, value)
		end
	end
	return result
end

const function sort<T>(values: { T }, before: (T, T) -> boolean): { T }
	table.sort(values, before)
	return values
end

const function first<T>(values: { T }): T?
	return values[1]
end

const function isAlive(enemy: Enemy)
	return enemy.Alive
end

const function byDistance(a: Enemy, b: Enemy)
	return a.Distance < b.Distance
end
```

</details>

### Another example: damage

Pipelines work for any value, not just lists. Here a hit's damage goes through
armor, a critical bonus, rounding, and a cap:

```lua
-- Without a pipeline
const damage = math.clamp(math.floor(applyCritical(applyArmor(40, 25), true)), 0, 999)

-- With a pipeline
const damage = 40
	|> applyArmor(25)
	|> applyCritical(true)
	|> math.floor()
	|> math.clamp(0, 999)
```

In the nested version, the `25` and `true` are far from the functions they belong
to, and the closing parentheses pile up. The pipeline keeps each argument next to
its step.

### When to use one

A pipeline helps when:

- you apply three or more steps to one value
- the steps are plain functions that take that value first
- you expect to add or reorder steps later

### When to skip it

- **One or two calls.** `first(alive)` is already clear.
- **The value has methods.** Use `:` instead. `name:lower():sub(1, 12)` already
  reads left to right. Pipelines are for free functions, which can't chain that way.
  Method calls aren't allowed as stages, so `x |> obj:method()` is a compile error.
- **The value isn't the first argument.** There's no placeholder syntax for putting
  it anywhere else, so write a normal call.

### Evaluation rules

- The input is evaluated once, before the stage's function and its other arguments.
- A stage can be a function name, a call, a parenthesized function reference, or a
  call with explicit generic arguments.
- Each stage takes one value and returns one value. If a function returns several
  values, the next stage only gets the first.
- If a stage's last explicit argument is a call or `...`, it expands normally, as in
  any Lua call.
- `...` inside a pipeline stage, a `??` fallback, or an optional method call refers
  to the enclosing function's varargs.

### Precedence

`|>` and `??` share the lowest precedence and group left to right. Arithmetic,
`..`, comparisons, `and`, and `or` all bind more tightly. That lets you put a
fallback in the middle of a chain:

```lua
enemies |> filter(isAlive) ?? {} |> sort(byDistance)
-- ((enemies |> filter(isAlive)) ?? {}) |> sort(byDistance)
```

Use parentheses to keep later steps inside the fallback:

```lua
enemies |> filter(isAlive) ?? (fallback |> sort(byDistance))
```

## Godot class identity

```lua
if event:IsA("InputEventMouseMotion") then
	print(event.Relative)
end
```

`isA` checks whether an object is an instance of a class or inherits from it. Every
Godot object supports it: nodes, resources, input events, and extension classes
registered in ClassDB.

### Type narrowing

With a literal class name, the checker narrows the object's type inside the `if`, so
`event.Relative` above is known to exist.

- Misspelled class names are reported, and completion suggests class names inside
  the string.
- A string built at runtime still returns a boolean, but doesn't narrow the type.
- In the `else` branch, the checker keeps every type that could still match.

Imported script classes are checked by their declared class name, not their import
alias.

### Runtime behavior

At runtime, `isA` checks the native class, the attached script's inheritance chain,
and Luau class ancestry. Pure Luau classes that don't extend a Godot class don't get
`isA` automatically.

### Spelling

Both `isA` and `IsA` work in every project. Completion only shows the spelling that
matches your method casing, which is `is_a` in snake_case projects. This is the one
exception to the rule that each engine method has a single spelling.

## Tooling for the extended syntax

The native editor and the standalone LSP share the same parser and analyzer, so both
understand everything on this page. Typing a `match` block indents its cases and
completes the closing `end`.

### Formatting and linting

- **StyLua** formats the statements it understands and leaves statements with Ludot
  syntax byte for byte. StyLua ignore comments still work.
- **Selene** gets a copy of the file with Ludot syntax hidden and line numbers kept.
  The native checker validates the hidden parts.

Naming lint rules are separate from the engine casing settings.

### Suppressing warnings

To silence `match_exhaustiveness` or `signal_declaration`:

```lua
-- selene: allow(match_exhaustiveness)    -- for the next statement
--# selene: allow(match_exhaustiveness)   -- for the whole file, at the top
```

### Shaders

The shader subset rejects these extensions with a source diagnostic. Its numeric
`for` loop still works.

## Engine naming

Godot's API uses snake_case. Ludot lets you pick how engine methods and properties
are spelled in Luau. Set it under Project Settings > Luau > Api:

| Setting | Choices | Default |
| --- | --- | --- |
| `luau/api/method_casing` | `camelCase`, `PascalCase`, `snake_case` | `camelCase` |
| `luau/api/property_casing` | `camelCase`, `PascalCase`, `snake_case` | `PascalCase` |

### Examples

| Godot member | camelCase | PascalCase | snake_case |
| --- | --- | --- | --- |
| `move_and_slide` | `moveAndSlide` | `MoveAndSlide` | `move_and_slide` |
| `get_instance_id` | `getInstanceId` | `GetInstanceId` | `get_instance_id` |
| `velocity` | `velocity` | `Velocity` | `velocity` |
| `tree_entered` | `treeEntered` | `TreeEntered` | `tree_entered` |
| Vector3 `x`, `y`, `z` | `x`, `y`, `z` | `X`, `Y`, `Z` | `x`, `y`, `z` |
| `_physics_process` | `_physicsProcess` | `_PhysicsProcess` | `_physics_process` |

### What each setting covers

**Method casing** covers instance methods, static methods, and signal methods such
as `connect`.

**Property casing** covers properties, signal names such as `self.TreeExiting`, and
value type components such as `Vector3.X`.

These never change:

- Class and type names keep their engine spelling, including acronyms like
  `HTTPClient`, `RID`, and `AABB`.
- Constructors are always `.new()`.
- Enum constants keep Godot's spelling and their existing PascalCase aliases.
- Other built-in constants keep their fixed PascalCase spelling.
- GDScript and the engine's internals keep Godot's original names.

### How names are converted

1. Split the name into words at underscores, at lowercase-to-uppercase changes, and
   at acronym boundaries (`HTTPResponse` becomes `HTTP` + `Response`).
2. Lowercase every word. Digits stay with their word.
3. Join the words in the chosen style.

So `get_rid` becomes `getRid`, `GetRid`, or `get_rid`, never `getRID`. Leading
underscores are kept: `_ready` stays `_ready` in camelCase and snake_case, and
becomes `_Ready` in PascalCase.

The engine and the standalone analyzer share this conversion. Both start from the
raw engine names, never from already-converted ones.

### Compatibility and setting changes

Projects without these settings use the defaults.

Each engine method and property has exactly one spelling, the selected one, except
the `isA`/`IsA` pair. Completion doesn't list other spellings. That means changing a
setting can require edits to your code. Ludot never renames your source for you.

Settings are read once, when the language runtime starts:

- **Editor:** restart the editor after changing a casing setting.
- **Running games:** restart them too. Reloading a script or changing
  ProjectSettings at runtime doesn't switch the spelling midway.
- **Standalone LSP:** it reads the nearest `project.ludot` (or legacy `project.godot`) and needs a restart after
  a change. One LSP session can serve projects with different styles.

An invalid style name in the settings falls back to the default with a warning.

### Your own names

Names you declare yourself, such as classes, functions, methods, properties,
parameters, and table keys, stay exactly as written. Naming lint preferences are
separate and don't affect the engine API.

### Engine callbacks

Native virtual callbacks, such as `_ready` and `_physics_process`, are where the two
naming worlds meet. Godot calls its original name, and Ludot forwards the call to
your Luau spelling. If you declare both spellings, the exact match wins.

Only real engine callbacks get this forwarding. Your own methods have no casing
aliases. Script templates and generated callback stubs use your selected style.

### Names inside strings

Strings passed to Godot's reflective APIs keep Godot's original names.
`EmitSignal("ready")` uses the original signal name even when the property is
`Ready`. The same goes for your own method and property names passed as strings,
node paths, and resource paths. Casing only changes identifiers in code.

### How the binding works

The binding reflects methods, properties, signals, static methods, and constants.

- Property reads and writes go straight to the engine object, so `self.Name = "x"`
  is visible to `self:getName()` on the next line.
- Member lookups use a per-class cache built from ClassDB. Script and dynamic
  properties fall back to a slower reflective scan.
- Luau functions convert to engine `Callable` values.
- Signals are `Signal` objects, so `timer.Timeout:connect(onTimeout)` returns a
  `Connection`.
- Arrays, dictionaries, and packed arrays support indexing from 1, `#`, and
  iteration.

## Hot reload

Saving a script or module reloads it in place, while the game runs.

### Objects and fields

Live instances stay the same objects, so references held elsewhere stay valid.
Fields keep their values. When you change a field's declared type:

- If the old value still fits the new type, it's kept.
- If a lossless conversion exists, it's converted. That covers strings and numbers,
  and engine value types such as `Vector2` and `Vector2i`.
- Otherwise the field resets to its new default, with a warning.

Scripts that import a changed module, and scripts that extend a changed class,
reload along with it.

Adding or removing a `Signal` field updates the script's signal list. Existing
connections on the object stay connected.

### Functions

Functions created by the old version keep working and run the new code. That
includes signal callbacks and functions stored in fields or tables. Functions are
matched by name and by position inside their enclosing function.

A function keeps its old code when:

- it is running at the moment of the reload
- the variables it captures have changed

A thread paused in `task.wait` counts as running, so it finishes with the code it
started with. Functions it calls later, and all new calls, use the new code.

If the edit has an error, the previous version keeps running.

## Debugging and profiling

### Debugger

Breakpoints, stepping, stack inspection, and expression evaluation work on the main
thread, in coroutines, and on worker threads.

Watch expressions are Luau and can read the frame's locals, upvalues, and `self`.
Scripts compile without optimization while a debugger is attached, so `const` locals
stay visible.

The debugger's globals include `[suspended tasks]`. It lists every scheduled thread
with:

- its kind: `wait`, `delay`, or `defer`
- where it's paused
- its owner
- for waits and delays, the duration and the time elapsed

### Threads and the VM

All Luau code runs on one VM, guarded by a lock. Calls from different threads run
one at a time. While one thread is stopped at a breakpoint, other threads that call
into Luau wait for it.

### Profiler

The editor's script profiler shows every Luau function, whether the engine called
it or another Luau function did. That includes `pcall` targets, metamethods, and
coroutine bodies.

- Coroutine time excludes time spent suspended.
- Code resumed by the task scheduler is profiled the same way, and waiting time is
  excluded.
- With native call profiling on, engine methods called from Luau get their own rows.

## Function hover help

Hover over a function in the native editor to see its signature, parameters, return
types, and documentation.

For documented engine methods, the card links to the matching Godot API page. The
link stays visible below the scrolling description. Luau names such as `getNode`
link to the original `get_node` page.

To document your own Luau functions, put `---` comments directly above them.
GDScript uses its normal `##` comments. Named script functions use the editor's
current text, including unsaved edits.

Engine descriptions come from the committed class-reference XML. Luau hovers in
the editor and standalone LSP show Luau examples from `docs/luau_examples.json`;
the website uses that same source. Signal connection examples derive their
parameter names from the XML. GDScript hovers retain their GDScript examples.

Examples follow the project's `luau/api/method_casing` and
`luau/api/property_casing`. In the native editor, marked example identifiers also
follow Text Editor > Luau > Lint naming preferences, and indentation follows Text
Editor > Luau > StyLua. Callbacks, type names, node paths, and string contents keep
their meaning. Rendering examples never renames code in your scripts.

The standalone LSP reads API casing from `project.ludot`. Clients can configure
`ludot.documentation.naming` with the `variable`, `constant`, `function`, `method`,
`property`, and `class` styles, plus `ludot.documentation.insertSpaces` and
`ludot.documentation.tabSize`, through `workspace/didChangeConfiguration`.

Some upstream examples still lack an authored Luau equivalent. Luau documentation
marks those examples as unavailable instead of displaying GDScript under a Luau
label. Add the Luau example to the shared catalog to make it available in every
consumer. Examples are local and versioned with the engine; hover requests do not
fetch documentation from the network.
