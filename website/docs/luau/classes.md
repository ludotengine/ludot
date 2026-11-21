---
source: docs/language.md
---

# Classes and properties

## Classes

```luau
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

```luau
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

```luau
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
