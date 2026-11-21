`Position`, `Rotation`, and `Scale` are local to the parent. Rotation uses radians. `GlobalPosition` includes the parent transform.

```luau
class Marker extends Node2D = {
	function _ready()
		self.Position = Vector2.new(100, 60)
		print(self.GlobalPosition)
	end
}
```

Use `toGlobal` and `toLocal` for point conversion. Directly assigning position is suitable for visual motion; collision-driven motion belongs on a physics body. See [Coordinates and transforms](../../engine/transforms.md).
