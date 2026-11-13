# Ludot language

Ludot derives from Luau 0.743. Its native parser accepts the examples below.
Luau's unions, intersections, generics, type packs, refinements, and type functions
remain available. `const` retains Luau's immutable binding semantics.

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

Brace classes are extensible. Methods receive `self` implicitly. An optional
`__init` method runs after field defaults. Defaults run per instance, so table
initializers do not share a table between instances. Constructor arguments use
the `__init` signature. Classes use the fork's native class bytecode and VM objects.
The original experimental `class ... end` syntax remains available internally.

### Onready properties

`onready` delays a property's initializer until the node is ready, like
GDScript's `@onready`. The initializer can use `self`, so it can look up nodes
that do not exist yet when the instance is constructed:

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

`getNode` accepts Godot node paths, so `"Arm/Hand/Muzzle"` reaches a
great-grandchild. Onready initializers run in declaration order immediately
before `_ready`, so a later initializer can read an earlier one and `_ready`
sees every value. Base class initializers run before those of a derived class.
They also run when the class defines no `_ready` method. Until then the
properties are `nil`.

An onready property without a type annotation takes its initializer's type. A
`::` assertion, such as `:: Node3D` above, narrows `getNode`'s `Node` result.
When the editor knows the attached scene, `getNode` with a literal path already
returns the node's type. An annotation, such as `onready public Head: Node3D =
...`, must match the initializer's type.

`onready` comes before `public` or `private` and requires an initializer. It is
only allowed in brace classes. Exported properties cannot be onready, since the
initializer would replace the value set in the inspector. Hot reload keeps the
current values of onready properties; their initializers run again only when
the node is next ready.

Construct native nodes, resources, value types, and Luau classes with `.new()`:

```lua
const node = Node3D.new()
const position = Vector3.new(1, 2, 3)
const player = Player.new()
```

The checker and completion understand `.new()` and preserve its instance type and
initializer arguments. `new` is reserved as a class member. Direct calls such as
`Vector3(1, 2, 3)` remain compatible with older scripts.

Normal Luau node scripts do not instantiate or run lifecycle callbacks while a
scene is being edited. The editor uses placeholders for exported properties;
scripts execute when the project runs. No `Engine:isEditorHint()` guard is needed
in `_ready`, `_process`, or input callbacks. Editor tool scripts are not currently
supported.

`export` properties appear in the engine inspector. `private` properties are hidden
from the inspector, and the checker rejects accesses outside the declaring class.
Privacy is a type-checking rule, not a sandbox against reflective engine access or
unchecked bytecode. Editor range hints are permitted only on exported properties.
No decorators are introduced; `onready` is a property keyword.

An attached engine script extends a native engine class, such as `Node`,
`CharacterBody2D`, or `Resource`, or another Luau script class imported from its
module. Derived scripts inherit fields, defaults, and methods, and reloading a base
script reloads its live derived instances. General Luau modules can return tables,
functions, or classes.

## Imports

```lua
import add from "@helpers/mathutils"
import add, subtract from "@helpers/mathutils"
import (add as plus), subtract from "@helpers/mathutils"
import "@pkgs/something" as something
import type Person from "@helpers/personutils"
import type (Person as Guy), Female from "@helpers/personutils"
import type "@helpers/personutils" as utils
import "@helpers/personutils" as personutils
import "@helpers/setup"
```

Named values lower to immutable local bindings. Each import statement loads the
module once. Namespace value imports also expose the module's exported types.
Type imports participate in checking and emit no runtime module load. A type-only
namespace cannot be read as a value. A module exports types with `export type`
and values through a returned table or Luau's value export syntax.

Define aliases in `.luaurc` at the project root:

```json
{"languageMode": "strict", "aliases": {"helpers": "./src/helpers", "pkgs": "./packages"}}
```

The native runtime also accepts a `luau/modules/aliases` dictionary in project
settings. Both `import` and legacy `require()` use these aliases. The native runtime
resolves `.luau` files and directory `init.luau` modules. The engine and the CLI tools
both read `.luaurc` files in the importing file's directory and its ancestors, and
nearer files override outer ones. The engine applies the project setting first.

## Enums

Enums declare immutable numeric or string values and a distinct nominal type.
Numeric members start at 1. An explicit integer resets the sequence for subsequent
implicit members. Integer values must fit exactly in Luau's number representation.
String enums require an explicit string for every member. Mixed numeric/string
enums are errors.

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

`Action.from(value)` checks membership and returns an enum value or nil. Assigning
an arbitrary number/string to an enum, or assigning one enum to another enum, is a
type error. Enum values remain ordinary numbers/strings at runtime, including when
passed to Godot. Equality with the underlying number/string is permitted. Member
aliases with equal underlying values compare equal and represent the same case
for matching. The name `from` is reserved for checked conversion.

Use `export enum` with the existing module import forms. Enum types can annotate
class fields, including exported Inspector fields. Numeric and string exports use
dropdowns with member names; scene files store the underlying values. String
values may contain commas or colons. Inspector dropdowns keep the names and values
separate rather than parsing the display text.

Direct iteration yields members in declaration order, including aliases. It does
not expose the `from` method:

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
print(Enum.MouseMode.Captured) -- Enum.MouseMode.Captured
print(Enum.MouseMode.Captured.Name) -- Captured
print(Enum.MouseMode.Captured.Value) -- 2
print(Enum.MouseMode.Captured.EnumType) -- Enum.MouseMode

print(Enum.MouseMode:fromName("Captured")) -- Enum.MouseMode.Captured, or nil
print(Enum.MouseMode:fromValue(2)) -- Enum.MouseMode.Captured, or nil
print(#Enum.MouseMode:getEnumItems()) -- 6
print(#Enum:getEnums()) -- every engine enum

for name, item in Enum.MouseMode do
	print(name, item.Value)
end
```

`typeof` returns `"EnumItem"` and `"Enum"`. Each item is a single shared value, so
`==` compares identity, and `Enum.MouseMode.Captured.EnumType.Captured` is the same
item. Indexing a missing enum or item is an error. User `enum` declarations are not
part of `Enum`.

Item names drop the words shared by every constant of the enum and use PascalCase:
`MOUSE_MODE_CONFINED_HIDDEN` becomes `ConfinedHidden`. A name that would start with a
digit keeps the last shared word, so `KEY_0` is `Enum.Key.Key0`. An enum keeps its
Godot name when the name is unique, such as `Enum.Key`, `Enum.MouseButton`, and
`Enum.LayoutPreset`. Same-named enums with the same items are one enum:
`Input.MouseMode` and `DisplayServer.MouseMode` are both `Enum.MouseMode`, and
`Control`, `Input`, and `DisplayServer` share `Enum.CursorShape`. Different enums with
the same name take their class as a prefix, such as `Enum.Light3DParam`,
`Enum.NodeProcessMode`, and `Enum.VariantType`. Editor-only enums are left out, so
editor and exported builds see the same names.

Engine methods and properties that take an enum accept an `EnumItem` or its number.
Those that return a non-bitfield enum return the `EnumItem`, such as
`Input:getMouseMode()` and `node.ProcessMode`. A value with no matching item, such as a
key code with modifier bits, stays a number. Bitfield enums, such as
`Enum.MouseButtonMask`, return numbers, because their values combine. Use `.Value`
with `bit32` to combine items.

Annotate engine enum types as `Enum.<Name>`, such as `Enum.MouseMode`. `EnumItem`
and `Enum` are the general types. Engine methods, properties, `fromName`, and
`getEnumItems` are typed with the specific enum, and matches over engine enums check
exhaustiveness. An `export` field typed `Enum.<Name>` shows a dropdown and stores the
number in scenes.

Class enum constants keep Godot's original names, such as
`Input.MOUSE_MODE_CAPTURED`. Non-bitfield ones are the matching `EnumItem`, so
`Input.MOUSE_MODE_CAPTURED == Enum.MouseMode.Captured`. Bitfield constants, such as
`Control.SIZE_EXPAND_FILL`, and builtin value type constants, such as
`Vector3.AXIS_Z`, stay numbers. The class enum namespaces (`Input.Enums.MouseMode`),
the global enum tables (`Key.KEY_W`, `MouseButton`, `Variant.Type`), and the
`Input.MouseMode` style annotations are replaced by `Enum`. Method names follow the
project method casing and `Name`, `Value`, and `EnumType` follow the property
casing. Enum and item names do not change with casing. Native bitfield
parameters/properties continue to accept numeric masks.

## Tasks

The `task` library schedules threads, modeled on Roblox's:

| Function | Behavior |
| --- | --- |
| `task.spawn(fnOrThread, ...)` | Runs it now in a new thread, or resumes the given thread, and returns the thread. |
| `task.defer(fnOrThread, ...)` | Same, but runs it after the current engine callback returns, later in the same frame. |
| `task.delay(seconds, fnOrThread, ...)` | Resumes it after the delay and returns the thread. |
| `task.wait(seconds?)` | Yields the current thread and returns the time that actually passed. With no argument, waits one frame. |
| `task.cancel(thread)` | Stops a scheduled or suspended thread. It cannot cancel a running thread. |

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

Engine callbacks, such as `_ready`, `_process`, and signal handlers, start in a
resumable thread, so they can call `task.wait` directly. When a callback yields, the
engine gets `nil` back and the rest of the callback runs when the thread resumes.
Module top-level code and field initializers cannot yield.

Each VM has one scheduler, stepped at the start of every SceneTree process frame,
before `_process`. Waits and delays use game time: they follow `Engine.time_scale`
and stop like a Timer while their node cannot process, such as during a pause. A
node with `PROCESS_MODE_ALWAYS` keeps waiting through a pause. Threads with no node
stop during a pause. Deferred threads that defer again run in the next round, for at
most 64 rounds a frame.

A thread belongs to the object whose callback started it. Threads it spawns, and
signal handlers connected from its code, belong to the same object. When the object
is freed, its scheduled threads are dropped, never resumed with a freed `self`. An
error in a resumed thread is printed with its stack trace, and the debugger breaks on
it when one is attached.

Starting callbacks in threads costs little. Finished threads are reused, so a plain
callback allocates nothing. On Linux, a GDScript call to a Luau method measured
0.74 µs, against 0.69 µs with the previous `pcall` path.

## Match expressions and statements

A match evaluates its subject once and tests arms in source order. Only the first
matching arm runs. There is no fallthrough.

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

Expression arms contain one expression and produce one value. Their types form a
union. Value cases accept literals and enum members. Guards use `case if` and do
not establish exhaustive coverage. Expressions require exhaustive coverage or
`otherwise`. The diagnostic lists missing enum members. Statements report a
`match_exhaustiveness` warning when cases are missing. Duplicate values, invalid
case domains, and arms after `otherwise` are diagnosed. Equal-valued aliases count
as the same case. A guard that always evaluates true makes subsequent arms
unreachable, but still does not replace explicit exhaustive coverage.

Match statements keep ordinary `return`, `break`, and `continue` behavior in the
enclosing function/loop. Match expressions have their own expression boundary.
`case` and `otherwise` mark arms at statement boundaries inside a match; they are
contextual names elsewhere. An ordinary function named `match` can still be
called with parentheses.

## Optional access and nil coalescing

```lua
const parentName = node?:getParent()?.Name ?? "No parent"
const enabled = settings.Enabled ?? true
local value: boolean? = nil
value ??= false
```

`?.` accesses a property and `?:` calls a method. A nil receiver skips the rest of
its uninterrupted chain, including argument expressions. Each optional receiver
is evaluated once. A nonexistent member on a valid receiver remains a type error.
An ordinary access later in the chain still requires its own receiver to be valid:
`node?.Child.Name` guards `node`; `node?.Child?.Name` also guards `Child`.
Parentheses end the chain, so `(node?.Child).Name` performs an ordinary access on
the parenthesized result.

`??` evaluates the fallback only for nil. False is preserved. `??=` reads the
target once and assigns only when it is nil. Indexed receivers and keys are
calculated once. Const bindings and invalid assignment targets are rejected.
Analysis refines assigned locals and stable indexed locations; it does not assume
an effectful receiver expression denotes the same object on a later evaluation.

## Inclusive ranges

```lua
for i in 1 to 10 do
	print(i)
end
for i in 1 to 10 step 5 do
	print(i) -- 1, then 6
end
for i in 10 to 1 step -3 do
	print(i) -- 10, 7, 4, 1
end
```

Bounds and step are evaluated once, in that order. The default step is 1. A zero
step raises a runtime error before the body runs, in both the VM and JIT. Existing
numeric `for i = start, limit, step` and generic iteration forms remain available.
Range variables have ordinary numeric-for scope and closure capture behavior.

## Pipelines and precedence

A pipeline passes its input as the first argument of an ordinary function:

```lua
const result = enemies
	|> filter(isAlive)
	|> sort(byDistance)
	|> first()
```

`x |> f(a, b)` calls `f(x, a, b)`. `x |> f` calls `f(x)`. Inputs are evaluated
once, before the stage's function reference and explicit arguments. Stages may be
ordinary function references or calls, including parenthesized references and
explicit generic instantiation. Method-call stages and placeholder syntax are
rejected. Each stage consumes one input value and produces one output value;
ordinary Lua expansion of a stage's final explicit argument is preserved. Varargs
inside a lazy fallback, optional method, or pipeline stage keep their enclosing
function's values.

`|>` and `??` share the lowest precedence tier and associate left to right.
Existing arithmetic, concatenation, comparisons, `and`, and `or` bind more tightly.
Thus:

```lua
enemies |> filter(isAlive) ?? {} |> sort(byDistance)
-- ((enemies |> filter(isAlive)) ?? {}) |> sort(byDistance)

enemies |> filter(isAlive) ?? (fallback |> sort(byDistance))
-- Parentheses keep sorting inside the fallback.
```

## Godot class identity

```lua
if event:IsA("InputEventMouseMotion") then
	print(event.Relative)
end
```

Every Godot Object subclass supports class identity checks, including nodes,
resources, input events, and extension classes registered in ClassDB. Both `isA`
and `IsA` are accepted. Completion shows only the spelling selected by method
casing, including `is_a` in snake_case projects. This is a deliberate compatibility
exception to the single-spelling rule for ordinary engine methods.

A valid literal name narrows the receiver using inheritance information. Invalid
literal class names are diagnosed, and completion offers known class names inside
the string. Dynamic strings return booleans without selecting a narrowed class.
Negative branches retain alternatives that could still match at runtime.
Imported Godot-backed script classes and script classes visible to the current
analysis context use their declared class names, not import aliases. Runtime
checks follow native class identity, the attached script inheritance chain, and
Luau object class ancestry. Pure Luau classes without a Godot Object base do not
receive this engine method automatically.

## Tooling for the extended syntax

The editor and standalone LSP share parser and analysis support for these
constructs. Match block/arm entry supports indentation and auto-end completion.
StyLua preserves statements containing syntax its parser does not understand,
byte for byte, while formatting surrounding statements. Selene receives a
line-preserving compatibility view; the native checker validates the protected
constructs. Existing StyLua ignore comments remain effective.

Use `-- selene: allow(match_exhaustiveness)` for the next statement, or
`--# selene: allow(match_exhaustiveness)` at the start of a file, to suppress the
native editor or LSP warning. Naming rules remain separate from engine casing settings.
The shader subset rejects these new constructs with source diagnostics; its
existing numeric-for syntax remains supported.

## Engine naming

Project Settings > Luau > Api provides two independent settings:

| Setting | Choices | Default |
| --- | --- | --- |
| `luau/api/method_casing` | `camelCase`, `PascalCase`, `snake_case` | `camelCase` |
| `luau/api/property_casing` | `camelCase`, `PascalCase`, `snake_case` | `PascalCase` |

The property setting also controls signals and builtin value components. Static
methods use the method setting. Native class/type names retain their original
engine spelling, including acronym names such as `HTTPClient`, `RID`, and `AABB`.
Constructors always use `.new()`. Native enum constants use their original Godot spelling and retain existing
PascalCase aliases. Other builtin constants keep their fixed PascalCase spelling. GDScript and engine internals keep Godot's original API names.

Examples for each selected style:

| Godot member | camelCase | PascalCase | snake_case |
| --- | --- | --- | --- |
| `move_and_slide` | `moveAndSlide` | `MoveAndSlide` | `move_and_slide` |
| `get_instance_id` | `getInstanceId` | `GetInstanceId` | `get_instance_id` |
| `velocity` | `velocity` | `Velocity` | `velocity` |
| `tree_entered` | `treeEntered` | `TreeEntered` | `tree_entered` |
| Vector3 `x`, `y`, `z` | `x`, `y`, `z` | `X`, `Y`, `Z` | `x`, `y`, `z` |
| `_physics_process` | `_physicsProcess` | `_PhysicsProcess` | `_physics_process` |

Naming splits ASCII identifiers at underscores, lowercase-to-uppercase boundaries,
and acronym boundaries such as `HTTPResponse` into `HTTP` + `Response`. Words are
lowercased before applying the chosen style: `get_rid` becomes `getRid`, `GetRid`,
or `get_rid`, not `getRID`. Digits stay in their word. Leading underscores are
preserved. Thus `_ready` stays `_ready` with camelCase/snake_case, and becomes
`_Ready` with PascalCase. This conversion is shared by the native binding and the
standalone analyzer, using raw reflected member names rather than converting
already-renamed completions.

### Compatibility and setting changes

Existing projects without these settings keep the defaults above. Each engine
method/property exposes only its selected spelling, except the documented
`isA`/`IsA` compatibility pair. Completions do not list alternate spellings. Changing a style can therefore
require source edits. Existing source is never automatically renamed.

The editor and game take an immutable naming snapshot when their language runtime
starts. The settings are marked as requiring an editor restart; restart running
games as well. Reloading a script or changing ProjectSettings in a running process
does not switch its API midway through execution. The standalone LSP reads the
nearest ancestor `project.godot`, takes a per-project snapshot, and requires an LSP
restart after casing changes. Different projects can use different styles in one
LSP session. Invalid manually configured style names use the corresponding default
and report a warning.

Custom user-declared class, function, method, property, parameter, and table-key
names remain exactly as written. Naming lint preferences are independent and do
not configure the engine API. Native virtual callbacks are the engine boundary:
Godot dispatches its original callback name to the selected Luau spelling. An exact
user declaration takes precedence when both names exist. Only actual native
callbacks receive this bridge; arbitrary user methods have no casing aliases.
Builtin script templates and generated native callback stubs use the selected
engine style, while custom handler names are preserved.

Names passed as string arguments to Godot's reflective APIs keep Godot's original
semantics. For example, `EmitSignal("ready")` uses the original signal name even
when the signal property is `Ready`. User method/property names passed as strings
remain exact too. The selected member styles affect identifier access, not string
contents or resource/node paths.

The native binding reflects methods, properties, signals, static methods, and
constants. Property reads and writes go straight to the engine object, so
`self.Name = "x"` is visible to `self:getName()` on the next line. Member lookups use
a per-class cache built from ClassDB. Scripted and dynamic properties fall back to a
reflective scan. Luau functions convert to engine `Callable` values, so
`timer.Timeout:connect(onTimeout)` works. Arrays, dictionaries, and packed arrays
support indexing (from 1), `#`, and iteration.

## Hot reload

Saving a script or module reloads it in place. Live instances keep their identity, so
references held elsewhere stay valid. Fields keep their values. When a field's
declared type changes, the value is kept if it still fits the new type, converted when
a lossless conversion exists (between strings and numbers, or between engine value
types such as `Vector2` and `Vector2i`), and otherwise reset to the new default with a
warning. Scripts that import a changed module, and scripts that extend a changed
class, reload with it.

Functions created by the previous version keep working after a reload and run the
new code. That includes callbacks connected to signals and functions stored in
fields or tables. Functions match by name and position within their enclosing
function. A function keeps its old code while it is running, or if the variables it
captures changed. A failed edit leaves the previous version running. A thread
suspended in `task.wait` is still running, so it finishes with the code it started
with. Functions it calls later, and new calls, use the new code.

## Debugging and profiling

Breakpoints, stepping, stack inspection, and expression evaluation work on the main
thread, in coroutines, and on worker threads. Expressions are Luau and can read the
frame's locals, upvalues, and `self`. Constant locals stay inspectable because scripts
compile without optimization while a debugger is attached.

All Luau code runs on one VM, guarded by a lock. Calls from different threads run one
at a time, so while one thread is stopped at a breakpoint, other threads that call
into Luau wait for it.

The editor's scripts profiler shows every Luau function, whether the engine called it
or another Luau function did, including `pcall` targets, metamethods, and coroutine
bodies. Coroutine time excludes the time spent suspended. With native call profiling
on, engine methods called from Luau get their own rows. Code resumed by the task
scheduler is profiled the same way, and time spent waiting is excluded.

The debugger's globals include `[suspended tasks]`, which lists every scheduled
thread with its kind (`wait`, `delay`, or `defer`), where it is suspended, its owner,
and for waits and delays the duration and elapsed time.

## Function hover help

Hover over a function in the native editor to see its signature, parameters, return
types, and documentation. Documented engine methods offer a link to the matching
Godot API page. The link stays visible below the scrolling description. Luau
binding names such as `getNode` link to the original `get_node` documentation.

Put adjacent `---` comments above Luau functions to describe them in the card.
GDScript uses its normal `##` documentation comments. Named script functions use
the current editor source, including edits that have not been saved.
