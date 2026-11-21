# Coordinates and transforms

<p class="lead">Use vectors for positions and directions, and transforms to convert between coordinate spaces.</p>

## 2D coordinates

In 2D, X points right and **Y points down**.

- [`Position`](../api/engine/Node2D.md#property-position) is relative to the parent node.
- `GlobalPosition` is relative to the world.

```luau
class Marker extends Node2D = {
	function _ready()
		self.Position = Vector2.new(100, 60)
		const worldPoint = self:toGlobal(Vector2.new(10, 0))
		const localPoint = self:toLocal(worldPoint)
		print(worldPoint, localPoint)
	end
}
```

Children move, rotate, and scale with their parent. Use local positions to arrange parts of one object, and global positions to compare objects in different parts of the tree.

## 3D coordinates

3D uses Godot's right-handed axes: X is right, Y is up, and cameras look toward −Z. Imported models may face a different way, so check which way a model's front points before moving or aiming it.

- `Position`, `Rotation`, and `Scale` on a [Node3D](../api/engine/Node3D.md) are relative to the parent.
- `GlobalPosition` and `GlobalTransform` are relative to the world.
- Rotations are in **radians**. Use `math.rad(90)` to convert from degrees.

## Direction and distance

```luau
const start = Vector3.new(0, 0, 0)
const target = Vector3.new(3, 0, 4)
const distance = start:distanceTo(target)
const direction = start:directionTo(target)
print(distance, direction)
```

A position is a point. A direction only says which way, and is usually normalized to length 1. Direction × speed × `delta` gives how far to move this frame.

Tips:

- For range checks that run often, compare `distanceSquaredTo` against the range squared. It skips a square root.
- Floating-point math is rarely exact. Use `isEqualApprox` instead of `==` for computed vectors.

## Basis and transforms

- A [Basis](../api/engine/Basis.md) holds rotation and scale.
- A [Transform3D](../api/engine/Transform3D.md) is a basis plus a position (its origin). In 2D, use [Transform2D](../api/engine/Transform2D.md).

Transform a point with the whole transform, which moves it. Transform a direction with only the basis, which doesn't.

To convert between a node's local space and world space, use `toLocal` and `toGlobal`. Adding the parent's position by hand ignores its rotation and scale.

## Frame-independent motion

```luau
class Drift extends Node3D = {
	export Speed: number = 2

	function _process(delta: number)
		self.Position = self.Position + Vector3.new(1, 0, 0) * self.Speed * delta
	end
}
```

Setting the position directly is fine for things that don't collide. Characters and other physics bodies should move through the physics API instead. See [Physics and character movement](./physics.md).
