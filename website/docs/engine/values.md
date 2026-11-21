# Values and collections

<p class="lead">Luau values and engine values share a script, but they have different construction, indexing, and lifetime rules.</p>

## Primitive values

Luau has `nil`, booleans, numbers, strings, functions, threads, tables, and userdata.

- Engine `int` and `float` are both Luau `number`.
- Engine `String`, `StringName`, and `NodePath` all accept Luau strings.

### Truthiness

Only `nil` and `false` count as false. Unlike GDScript, `0`, `""`, empty collections, and zero vectors are all **true** in Luau.

When `false` is a valid value, use `value ?? fallback`. `value or fallback` also replaces `false`.

## Native value types

Construct native values with `.new()`:

```luau
const offset = Vector2.new(24, 16)
const direction = Vector3.new(1, 0, -1)
const tint = Color.new(0.3, 0.6, 1.0, 1.0)
print(offset.X, direction.Z, tint.A)
```

With the default casing, components are `X`, `Y`, and `Z`, and methods are camelCase, like `direction:normalized()`.

### Values are copies

Reading `node.Position` gives you a copy of the value, not a live link to the node. To move the node, assign a new value back:

```luau
class Nudge extends Node2D = {
	function _ready()
		self.Position = self.Position + Vector2.new(10, 0)
	end
}
```

### `vector` vs `Vector3`

Luau's lowercase `vector` library is a different type from the engine's `Vector2` and `Vector3`. Use the engine types for anything you pass to the engine.

## Tables and native collections

A Luau table is suitable for ordinary gameplay data:

```luau
const scores = { 10, 25, 40 }
for index, score in scores do
	print(index, score)
end
```

Engine [Array](../api/engine/Array.md), [Dictionary](../api/engine/Dictionary.md), and packed arrays support indexing, `#`, and iteration, like tables. Indexes start at **1**, including in arrays returned by engine methods.

```luau
class Inspector extends Node = {
	function _ready()
		for index = 0, self:getChildCount() - 1 do
			const child = self:getChild(index)
			print(index, child.Name)
		end
	end
}
```

Methods that take an index, like `getChild`, still use Godot's numbering, which starts at 0. Only indexing with `[]` starts at 1. Check the method's reference instead of adding or subtracting 1 everywhere.

## Objects and resources

A variable holding a node or resource refers to the engine object itself, not a copy.

- **Nodes** live until they're freed, usually with `queueFree`.
- **Resources** are reference counted. They're freed when nothing uses them, and many nodes can share one.

Tables and Luau class objects passed to the engine travel as `LuauValue` references, not copies. See [signals](./signals.md) for how they reach GDScript.

## Call syntax and strings

- Instance methods use a colon: `node:getName()`
- Static methods and constructors use a dot: `Vector3.new(1, 2, 3)`, `FileAccess.open(path, mode)`

Casing settings don't change strings. Property paths, shader uniforms, node paths, and action names keep their original spelling. A tween animates `"position"`, even though code reads `node.Position`.

## Reference

[Vector2](../api/engine/Vector2.md) · [Vector3](../api/engine/Vector3.md) · [Color](../api/engine/Color.md) · [Array](../api/engine/Array.md) · [Dictionary](../api/engine/Dictionary.md)
