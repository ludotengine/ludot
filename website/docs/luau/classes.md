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

```luau
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
