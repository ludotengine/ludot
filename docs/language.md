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
No new decorators are introduced.

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

## Engine naming

Project Settings > Luau > Api provides two independent settings:

| Setting | Choices | Default |
| --- | --- | --- |
| `luau/api/method_casing` | `camelCase`, `PascalCase`, `snake_case` | `camelCase` |
| `luau/api/property_casing` | `camelCase`, `PascalCase`, `snake_case` | `PascalCase` |

The property setting also controls signals and builtin value components. Static
methods use the method setting. Native class/type names retain their original
engine spelling, including acronym names such as `HTTPClient`, `RID`, and `AABB`.
Constructors always use `.new()`. Constants retain their existing fixed PascalCase
spelling. GDScript and engine internals keep Godot's original API names.

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
member exposes only its selected spelling. There are no legacy casing aliases,
and completions do not list alternate spellings. Changing a style can therefore
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
captures changed. A failed edit leaves the previous version running.

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
on, engine methods called from Luau get their own rows.

## Function hover help

Hover over a function in the native editor to see its signature, parameters, return
types, and documentation. Documented engine methods offer a link to the matching
Godot API page. The link stays visible below the scrolling description. Luau
binding names such as `getNode` link to the original `get_node` documentation.

Put adjacent `---` comments above Luau functions to describe them in the card.
GDScript uses its normal `##` documentation comments. Named script functions use
the current editor source, including edits that have not been saved.
