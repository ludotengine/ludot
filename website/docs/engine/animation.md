# Animation and tweens

<p class="lead">Use animation tracks for authored sequences and tweens for changes calculated during gameplay.</p>

## AnimationPlayer

Add an [AnimationPlayer](../api/engine/AnimationPlayer.md), create an animation, and add a track for each property you want to animate. A track stores values at points in time, and the player blends between them while the animation runs.

Start an animation by name with `player:play("walk")`. Names must match exactly.

For transitions and blending between many clips, use an [AnimationTree](../api/engine/AnimationTree.md).

## Tween a property

```luau
class Slide extends Node2D = {
	function _ready()
		const tween = self:createTween()
		tween:tweenProperty(self, "position", Vector2.new(240, 80), 0.5)
	end
}
```

The property is named with Godot's string `"position"`, even though Luau code reads it as `self.Position`. Casing settings change identifiers, not strings.

- `tweenProperty` returns a tweener. Use it to set a delay, transition, or easing.
- A tween runs once. Create a new one for each new animation.
- Create tweens from the node they animate, so they stop when that node is freed.

## Coordinate animation ownership

If an animation, a tween, and `_process` all set the same property, whichever runs last in the frame wins. Let one system control a property at a time.

For characters, move the physics body in code and animate a visual child node. An animation doesn't move the collision body by itself, so decide whether motion comes from physics code, root motion, or a visual-only track.

## Completion and events

To run code after an animation, use the tween's `Finished` signal or the animation player's signals:

```luau
class FadeOut extends Node2D = {
	function _ready()
		const tween = self:createTween()
		tween:tweenProperty(self, "modulate:a", 0, 1)
		tween.Finished:wait()
		self:queueFree()
	end
}
```

`wait` pauses only the current callback. Don't use a loop to wait. See [tasks](./tasks.md) and [signals](./signals.md).

## Reference

[Tween](../api/engine/Tween.md) · [PropertyTweener](../api/engine/PropertyTweener.md) · [AnimationPlayer](../api/engine/AnimationPlayer.md) · [AnimationTree](../api/engine/AnimationTree.md)
