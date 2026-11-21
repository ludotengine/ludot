---
source: docs/language.md
---

# Native API and naming

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
- **Standalone LSP:** it reads the nearest `project.godot` and needs a restart after
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
