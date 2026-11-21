# Values and collections

<p class="lead">Luau values and engine values share a script, but they have different construction, indexing, and lifetime rules.</p>

## Primitive values

Luau has `nil`, booleans, numbers, strings, functions, threads, tables, and userdata. Integers and floating-point engine arguments both use Luau `number`. Engine `String`, `StringName`, and `NodePath` arguments accept Luau strings through the binding.

Only `nil` and `false` are falsey. Zero, an empty string, an empty collection, and a zero vector are all truthy. Do not carry GDScript's truthiness rules into Luau.

Use `value ?? fallback` when false is a valid value. `value or fallback` replaces false too.

## Native value types

Construct native values with `.new()`:

```luau
const offset = Vector2.new(24, 16)
const direction = Vector3.new(1, 0, -1)
const tint = Color.new(0.3, 0.6, 1.0, 1.0)
print(offset.X, direction.Z, tint.A)
```

Engine value properties use the project property casing. With defaults, vector components are `X`, `Y`, and `Z`. Methods use camelCase, such as `direction:normalized()`.

Value properties crossing the engine boundary are values, not live views into an object's property. To change a node position, assign the result back to `node.Position`. Prefer `node.Position = node.Position + offset` over assuming a nested component write changes the node.

The VM's lower-case `vector` library is separate from engine `Vector2` and `Vector3`. The standalone playground has `vector`; engine mode can check native vector code without executing an engine.

## Tables and native collections

A Luau table is suitable for ordinary gameplay data:

```luau
const scores = { 10, 25, 40 }
for index, score in scores do
	print(index, score)
end
```

Engine [Array](../api/engine/Array.md), [Dictionary](../api/engine/Dictionary.md), and packed arrays are native collection values. Ludot supports indexing, `#`, and iteration on them. Sequential collection indexes start at **1** in the Luau binding, including collections returned by engine methods.

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

Engine methods that take integer indexes are still ordinary method calls. Their argument semantics come from the method reference; do not blindly add or subtract one for every numeric argument.

## Objects and resources

Nodes and resources expose reflected methods and properties. A native object reference identifies the same engine object across property reads and calls. Nodes follow scene ownership and explicit deletion. Ref-counted resources have a different lifetime and can be shared among many nodes.

Tables and plain Luau class objects that cross into engine code travel as `LuauValue` references. They do not automatically turn into independent copies of every nested field. See [signals](./signals.md) for values crossing between Luau and GDScript.

## Call syntax and strings

Call instance methods with a colon, such as `node:getName()`. Call static methods and constructors with a dot, such as `Vector3.new(1, 2, 3)` or `FileAccess.open(path, mode)`.

Identifier casing does not rewrite strings. Native reflective paths such as `"position"`, shader uniform names, node paths, and action names keep the spelling their API expects. A tween targets `"position"`, even though identifier access is `node.Position`.

## Reference

[Vector2](../api/engine/Vector2.md) · [Vector3](../api/engine/Vector3.md) · [Color](../api/engine/Color.md) · [Array](../api/engine/Array.md) · [Dictionary](../api/engine/Dictionary.md)
