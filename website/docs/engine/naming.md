---
source: docs/language.md
---

# Native API and naming

## Engine naming

Project Settings > Luau > Api provides two independent settings:

| Setting | Choices | Default |
| --- | --- | --- |
| `luau/api/method_casing` | `camelCase`, `PascalCase`, `snake_case` | `camelCase` |
| `luau/api/property_casing` | `camelCase`, `PascalCase`, `snake_case` | `PascalCase` |

The property setting also controls signal names (`self.TreeExiting`) and builtin value
components. Signal methods follow the method setting, as described in Signals. Static
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
reflective scan. Luau functions convert to engine `Callable` values. Signals are
`Signal` objects, so `timer.Timeout:connect(onTimeout)` returns a `Connection`.
Arrays, dictionaries, and packed arrays support indexing (from 1), `#`, and iteration.
