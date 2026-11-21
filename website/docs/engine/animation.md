# Animation and tweens

<p class="lead">Use animation tracks for authored sequences and tweens for changes calculated during gameplay.</p>

## AnimationPlayer

Add an [AnimationPlayer](../api/engine/AnimationPlayer.md), create an animation, and add tracks for the properties you want to animate. Tracks store values at specific times. The player interpolates between compatible keys and applies the values while the animation runs.

Use `player:play("walk")` to start a named animation. Keep animation names exact. Use an [AnimationTree](../api/engine/AnimationTree.md) when transitions, blending, or a larger animation state graph should decide which clips contribute.

## Tween a property

```luau
class Slide extends Node2D = {
	function _ready()
		const tween = self:createTween()
		tween:tweenProperty(self, "position", Vector2.new(240, 80), 0.5)
	end
}
```

The property path is the native string `"position"`, even though direct Luau access uses `self.Position`. API casing changes identifiers, not reflective strings.

Create a new tween for a new sequence. A completed tween is not a reusable animation asset. Use the returned property tweener to configure delay, transition, or easing. Bind tweens to the node that owns their behavior so their lifetime follows that node.

## Coordinate animation ownership

If an animation, a tween, and `_process` all write the same property, the last writer in that frame wins. Give one system ownership of a property at a time. For a character, keep physical movement on its body and animate a visual child when appropriate.

A character's animation does not automatically move its collision body. Decide whether the motion comes from physics logic, root motion, or a visual-only track.

## Completion and events

Use the tween's completion signal or an animation player's event signals when later work must wait for completion. Signal handlers can yield through the engine scheduler; use [tasks](./tasks.md) and [signals](./signals.md) rather than blocking a thread with a busy loop.

## Reference

[Tween](../api/engine/Tween.md) · [PropertyTweener](../api/engine/PropertyTweener.md) · [AnimationPlayer](../api/engine/AnimationPlayer.md) · [AnimationTree](../api/engine/AnimationTree.md)
