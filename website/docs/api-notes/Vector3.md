Use `Vector3` for native 3D positions, directions, and velocity. Component names follow property casing; the default spelling is `X`, `Y`, and `Z`.

```luau
const origin = Vector3.new(0, 0, 0)
const target = Vector3.new(3, 0, 4)
print(origin:distanceTo(target))
print(origin:directionTo(target))
```

`distanceTo` returns 5 for this example. `directionTo` returns the normalized displacement. Engine value methods use a colon. Construct values with `.new`, using a dot.

A zero vector is truthy in Luau. Compare its length or use `isZeroApprox` when the algorithm needs to distinguish it from other vectors. See [Coordinates and transforms](../../engine/transforms.md).
