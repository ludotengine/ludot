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

`export` properties appear in the engine inspector. `private` properties are hidden
from the inspector, and the checker rejects accesses outside the declaring class.
Privacy is a type-checking rule, not a sandbox against reflective engine access or
unchecked bytecode. Editor range hints are permitted only on exported properties.
No new decorators are introduced.

An attached engine script currently needs a direct native engine base, such as
`Node`, `CharacterBody2D`, or `Resource`. General Luau modules can return tables,
functions, or classes. Script-to-script inheritance in attached resources remains
unfinished.

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
resolves `.luau` files and directory `init.luau` modules. CLI tools retain upstream
relative alias resolution through ancestor `.luaurc` files. Nested alias configuration
parity with the engine is still pending.

## Engine naming

Native class and value type names retain PascalCase. Properties use PascalCase,
methods use camelCase, and methods beginning with `_` retain that prefix. For
example, `move_and_slide` becomes `moveAndSlide`, `velocity` becomes `Velocity`,
and `_physics_process` becomes `_physicsProcess`. `_ready` keeps its spelling.
GDScript and engine internals keep the original Godot names.

The current native binding reflects methods and properties. Complete support for
static methods, constants, signal callbacks, typed collection conversion, and
reference-counted object lifetime needs further work. Native property synchronization
currently occurs at script callback boundaries. It is not yet a direct property proxy.
