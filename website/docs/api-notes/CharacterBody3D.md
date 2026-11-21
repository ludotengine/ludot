Set `Velocity` in units per second, then call `moveAndSlide` from `_physicsProcess`. The movement call uses the physics time step internally. Do not multiply `Velocity` by delta again before calling it.

The [character movement guide](../../engine/physics.md) includes a complete Luau controller, collision-shape setup, gravity, and jumping. A visible mesh does not create a collider.
