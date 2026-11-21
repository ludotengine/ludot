# Input actions

<p class="lead">Bind named actions in the Input Map, then use those actions from Luau.</p>

## Configure actions

Open Project Settings > Input Map and add these actions:

- `move_left`, `move_right`, `move_forward`, `move_back`
- `jump`, if your character jumps

Bind keys or gamepad controls to each one. An action can have several bindings. Your code only checks the action name, so keyboard and controller share the same code.

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

`getVector` combines four actions into one direction, applies a dead zone, and keeps the length at 1 or less. For a physics character, read input in `_physicsProcess` and set its velocity instead of its position.

| Method | True when |
| --- | --- |
| `isActionPressed` | The action is held down |
| `isActionJustPressed` | The action was pressed this frame |
| `isActionJustReleased` | The action was released this frame |

"This frame" means the current physics tick when called from `_physicsProcess`.

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

- `_unhandledInput` runs after UI controls have had a chance to use the event. Use it for gameplay.
- `_input` runs before the UI. Use it only when you need the event first.

`setInputAsHandled` stops the event from reaching anything else.

## Mouse capture

For a mouse-look camera, capture the mouse, and release it when a menu opens:

```luau
Input:setMouseMode(Enum.MouseMode.Captured) -- hide and lock the cursor
Input:setMouseMode(Enum.MouseMode.Visible)  -- give it back
```

Mouse movement arrives as an [InputEventMouseMotion](../api/engine/InputEventMouseMotion.md). Check for it with `isA` before reading its properties:

```luau
class MouseLook extends Node3D = {
	function _unhandledInput(event: InputEvent)
		if event:isA("InputEventMouseMotion") then
			print(event.Relative)
		end
	end
}
```

## Reference

[Input](../api/engine/Input.md) · [InputMap](../api/engine/InputMap.md) · [InputEvent](../api/engine/InputEvent.md) · [Control](../api/engine/Control.md)
