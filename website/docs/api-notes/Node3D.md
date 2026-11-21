Use a `Node3D` to position, rotate, and scale a 3D branch. Its local transform composes with its ancestors' transforms.

```luau
class Spinner extends Node3D = {
	export Speed: number = 1

	function _process(delta: number)
		self:rotateY(self.Speed * delta)
	end
}
```

`Speed` here is radians per second. Multiplying by `delta` converts that rate to a frame's rotation. See [Coordinates and transforms](../../engine/transforms.md).
