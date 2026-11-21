# UI, text, and themes

<p class="lead">Build interfaces with Control nodes, containers, themes, and signals.</p>

## Layout a menu

Create a `Control` root and set its layout to fill the viewport. Add a `MarginContainer`, a `VBoxContainer` inside it, and `Label` and `Button` children inside the vertical container.

Containers calculate their children's layout. Use size flags, minimum sizes, spacing, and margins to guide that layout. Avoid setting child positions every frame inside a container. For freely placed controls, anchors define a fraction of the parent's rectangle and offsets add pixel distances.

## Connect a button

```luau
class Menu extends Control = {
	onready private Start = self:getNode("Margin/Column/Start") :: Button
	onready private Status = self:getNode("Margin/Column/Status") :: Label

	function _ready()
		self.Start.Pressed:connect(function()
			self.Status.Text = "Starting"
		end)
	end
}
```

Name the scene nodes to match these paths. The button's `Pressed` property is a `Signal<>`. A handler connected by this script belongs to the script's object and is disconnected when that owner is freed.

## Fonts and text

A [Font](../api/engine/Font.md) is a resource used to draw text. Use [FontFile](../api/engine/FontFile.md) for an imported font or [FontVariation](../api/engine/FontVariation.md) to configure a variation. Font resources can be shared by many labels and buttons.

Set fonts through a theme when they should apply to a whole interface. Use a node's theme override when only that control should differ. Font size is a separate theme value. `Label.Text` displays ordinary text; [RichTextLabel](../api/engine/RichTextLabel.md) supports formatted content and has different parsing and layout options.

## Apply a theme

Assign a [Theme](../api/engine/Theme.md) resource to a parent control. Descendants inherit theme values unless a closer theme or override replaces them. Themes hold fonts, font sizes, colors, constants, icons, and style boxes.

Use the Inspector to set up the initial theme and compare controls at different window sizes. Editor color themes in [Editor themes](../reference/themes.md) affect the Ludot editor, not the controls in your game.

## Focus and mouse input

Buttons should be reachable by keyboard and controller focus, not just mouse clicks. Configure focus neighbors for custom layouts. A decorative control can intercept mouse input if its mouse-filter setting allows it; adjust that setting when input should pass through.

Use `_unhandledInput` for gameplay actions that the UI should consume first. See [Input actions](./input.md).

## Reference

[Control](../api/engine/Control.md) · [Container](../api/engine/Container.md) · [Label](../api/engine/Label.md) · [Button](../api/engine/Button.md) · [Theme](../api/engine/Theme.md) · [Font](../api/engine/Font.md)
