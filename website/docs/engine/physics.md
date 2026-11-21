# Physics and character movement

<p class="lead">Choose the physics body that owns the motion you need, then update it on physics ticks.</p>

## Choose a body

| Type | Use |
| --- | --- |
| [StaticBody3D](../api/engine/StaticBody3D.md) | Fixed world geometry. |
| [RigidBody3D](../api/engine/RigidBody3D.md) | Objects whose motion the physics simulation controls. |
| [CharacterBody3D](../api/engine/CharacterBody3D.md) | A character whose velocity and movement logic your script controls. |
| [Area3D](../api/engine/Area3D.md) | Detect entry and overlap without acting as a solid body. |

Each has a 2D counterpart. Add a collision shape with dimensions that match the intended collision volume. A visible mesh alone does not create a collider.

## Move a 3D character

Create a `CharacterBody3D` with a `CollisionShape3D` child. Give the shape a capsule, add ground with a static body and collision shape, and set up the input actions in [Input actions](./input.md).

```luau
class Player extends CharacterBody3D = {
	export Speed: number = 5
	export JumpSpeed: number = 5
	export Gravity: number = 9.8

	function _physicsProcess(delta: number)
		const axis = Input:getVector("move_left", "move_right", "move_forward", "move_back")
		local vertical = self.Velocity.Y
		if self:isOnFloor() then
			if Input:isActionJustPressed("jump") then
				vertical = self.JumpSpeed
			end
		else
			vertical -= self.Gravity * delta
		end
		self.Velocity = Vector3.new(axis.X * self.Speed, vertical, axis.Y * self.Speed)
		self:moveAndSlide()
	end
}
```

`Velocity` is in units per second. `moveAndSlide` uses the physics step internally; do not multiply the velocity passed through this property by delta. Gravity changes velocity over time, so its acceleration does multiply by delta.

## Collision information

After movement, `isOnFloor`, `isOnWall`, and `isOnCeiling` describe the last slide result. Iterate slide collisions when you need contact details. `moveAndCollide` instead accepts a motion vector and returns the collision information for that movement.

Layers label where an object belongs. Masks select which layers it tests against. Two overlapping visuals may not collide because the masks exclude one another, because the shape is disabled, or because their collision volumes do not meet.

## Rigid bodies and detection

Apply forces or impulses to a rigid body. Repeatedly assigning its transform bypasses the simulation's motion and can produce unstable contacts. Use an area for triggers, then connect its typed body-entry or area-entry signal.

Connect handlers in `_ready`, and use [signals](./signals.md) for connection ownership and lifetime rules. Read the relevant body reference for callbacks and settings; the physics simulation remains the engine's native implementation.
