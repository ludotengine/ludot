Create a tween through a node or scene tree. A node-bound tween follows that node's lifetime.

```luau
class Fade extends Sprite2D = {
	function _ready()
		const tween = self:createTween()
		tween:tweenProperty(self, "modulate:a", 0.0, 0.5)
	end
}
```

The property path `"modulate:a"` uses native property spelling. See [Animation and tweens](../../engine/animation.md).
