# Physics and character movement

<p class="lead">Choose the physics body that owns the motion you need, then update it on physics ticks.</p>

## Choose a body

| Type | Use |
| --- | --- |
| [StaticBody3D](../api/engine/StaticBody3D.md) | Fixed world geometry. |
| [RigidBody3D](../api/engine/RigidBody3D.md) | Objects whose motion the physics simulation controls. |
| [CharacterBody3D](../api/engine/CharacterBody3D.md) | A character whose velocity and movement logic your script controls. |
| [Area3D](../api/engine/Area3D.md) | Detect entry and overlap without acting as a solid body. |

Each has a 2D version, such as `CharacterBody2D`.

Every body needs a collision shape child. A visible mesh doesn't collide with anything by itself.

## Move a 3D character

Set up the scene:

1. Add a `CharacterBody3D` with a `CollisionShape3D` child, and give the shape a capsule.
2. Add a floor: a `StaticBody3D` with its own collision shape.
3. Create the input actions from [Input actions](./input.md).

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

`Velocity` is in units per second, and `moveAndSlide` applies the time step for you. So:

- **Don't** multiply `Velocity` by `delta`.
- **Do** multiply gravity by `delta`, because gravity changes the velocity over time.

## Collision information

After `moveAndSlide`, `isOnFloor`, `isOnWall`, and `isOnCeiling` report what the last move touched. Loop over the slide collisions for details about each contact.

`moveAndCollide` is the lower-level option. It takes a motion vector, stops at the first hit, and returns information about that collision.

### Layers and masks

- A body's **layers** are the groups it belongs to.
- Its **mask** is the set of layers it collides with.

If two objects pass through each other, check their layers and masks, whether a shape is disabled, and whether the collision shapes actually overlap.

## Rigid bodies and detection

Move a `RigidBody3D` with forces and impulses. Setting its position every frame fights the simulation and causes jittery collisions.

Use an `Area3D` for triggers, such as pickups or a goal zone. Connect its `BodyEntered` signal in `_ready`:

```luau
class Pickup extends Area3D = {
	function _ready()
		self.BodyEntered:connect(function(body)
			print(body.Name, "picked this up")
			self:queueFree()
		end)
	end
}
```

See [signals](./signals.md) for how connections are cleaned up.
