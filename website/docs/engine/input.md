# Input actions

<p class="lead">Bind named actions in the Input Map, then use those actions from Luau.</p>

## Configure actions

Open Project Settings > Input Map. Add `move_left`, `move_right`, `move_forward`, and `move_back`. Assign keyboard keys or gamepad controls to each action. Add `jump` if your character needs it.

An action can have several bindings. Gameplay code depends on the action name rather than a physical key, so keyboard and controller input use the same movement code.

## Poll continuous input

```luau
class Mover extends Node2D = {
	export Speed: number = 180

	function _process(delta: number)
		const axis = Input:getVector("move_left", "move_right", "move_forward", "move_back")
		self.Position = self.Position + axis * self.Speed * delta
	end
}
```

`getVector` combines four actions, applies its dead zone, and limits vector length to 1. For a physics character, read this input in `_physicsProcess` and assign its velocity instead of directly changing position.

`isActionPressed` stays true while an action is held. `isActionJustPressed` and `isActionJustReleased` detect transitions in the current frame or physics tick. Poll once in the update loop that owns the behavior.

## Handle individual events

```luau
class MenuInput extends Node = {
	function _unhandledInput(event: InputEvent)
		if event:isActionPressed("ui_cancel") then
			print("Cancel requested")
			self:getViewport():setInputAsHandled()
		end
	end
}
```

Use `_unhandledInput` for gameplay shortcuts that UI controls should be able to consume first. Use `_input` when the earlier event phase is intentional. Marking the event handled prevents later handlers from acting on it.

## Mouse capture

`Input:setMouseMode(Enum.MouseMode.Captured)` captures the mouse for a look controller. Restore `Enum.MouseMode.Visible` when opening a menu or releasing the pointer. Mouse motion is an [InputEventMouseMotion](../api/engine/InputEventMouseMotion.md); use an `isA` check to narrow a general input event before reading motion-specific properties.

The browser playground does not receive engine input events. Use engine mode for completion and type checking, then run the scene in the desktop engine.

## Reference

[Input](../api/engine/Input.md) · [InputMap](../api/engine/InputMap.md) · [InputEvent](../api/engine/InputEvent.md) · [Control](../api/engine/Control.md)
