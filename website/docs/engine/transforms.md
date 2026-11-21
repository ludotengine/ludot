# Coordinates and transforms

<p class="lead">Use vectors for positions and directions, and transforms to convert between coordinate spaces.</p>

## 2D coordinates

The 2D X axis points right and Y points down. [Node2D.Position](../api/engine/Node2D.md#property-position) is relative to its parent. `GlobalPosition` is in world coordinates after ancestor transforms apply.

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

Parent rotation and scale affect children. Use local coordinates to arrange a scene internally and global coordinates to compare objects in different branches.

## 3D coordinates

Ludot uses the inherited right-handed 3D coordinate system: X is right, Y is up, and a camera looks along negative Z. A model may have its own visual forward direction. Keep that distinction explicit when moving a character or aiming a mesh.

`Position`, `Rotation`, and `Scale` are local properties of [Node3D](../api/engine/Node3D.md). Rotation values use radians. `GlobalPosition` and `GlobalTransform` include ancestor transforms.

## Direction and distance

```luau
const start = Vector3.new(0, 0, 0)
const target = Vector3.new(3, 0, 4)
const distance = start:distanceTo(target)
const direction = start:directionTo(target)
print(distance, direction)
```

A position is a point. A direction is a displacement without an origin. Normalize a direction when only its orientation matters. Multiplying a unit direction by a speed and a time step gives displacement.

For repeated range tests, compare `distanceSquaredTo` with the square of the range to avoid a square root. Use approximate comparisons for floating-point results when exact equality is not part of the algorithm.

## Basis and transforms

[Basis](../api/engine/Basis.md) describes 3D orientation, scale, and shear. [Transform3D](../api/engine/Transform3D.md) combines a basis and origin. Applying a transform to a position includes translation; applying only its basis to a direction does not.

Use `Node3D:toLocal` and `toGlobal` when converting node-relative points. Avoid adding parent position by hand, which misses rotation and scale. The 2D counterpart is [Transform2D](../api/engine/Transform2D.md).

## Frame-independent motion

```luau
class Drift extends Node3D = {
	export Speed: number = 2

	function _process(delta: number)
		self.Position = self.Position + Vector3.new(1, 0, 0) * self.Speed * delta
	end
}
```

This direct transform update is appropriate for a visual object. A collision-controlled character should move through the physics API instead. See [Physics and character movement](./physics.md).
