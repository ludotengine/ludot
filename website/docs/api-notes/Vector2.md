`Vector2` represents a point or direction in 2D. Canvas coordinates increase rightward on X and downward on Y.

```luau
const point = Vector2.new(3, 4)
print(point:length())
print(point:normalized())
```

The length is 5. The normalized result has length 1. A node's `Position` is in parent space; use `toGlobal` or `toLocal` when changing spaces. See [Coordinates and transforms](../../engine/transforms.md).
