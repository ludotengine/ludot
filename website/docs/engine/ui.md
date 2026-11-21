# UI, text, and themes

<p class="lead">Build interfaces with Control nodes, containers, themes, and signals.</p>

## Layout a menu

Build this scene, and set the root's layout to **Full Rect**:

```
Menu (Control)
└── Margin (MarginContainer)
    └── Column (VBoxContainer)
        ├── Status (Label)
        └── Start (Button)
```

Containers position their children for you. Control the result with size flags, minimum sizes, spacing, and margins. Don't set positions on a container's children; the container will overwrite them.

Outside containers, position controls with anchors and offsets. Anchors are fractions of the parent's size, and offsets are pixel distances from them.

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

`Pressed` is a `Signal<>`. The handler belongs to the menu, so it's disconnected automatically when the menu is freed.

## Fonts and text

- [FontFile](../api/engine/FontFile.md) is an imported font file.
- [FontVariation](../api/engine/FontVariation.md) adjusts an existing font, such as its weight or spacing.

Set a font in a theme to use it everywhere, or with a theme override on a single control. Font size is a separate setting.

`Label` shows plain text. [RichTextLabel](../api/engine/RichTextLabel.md) supports bold, colors, images, and other formatting.

## Apply a theme

A [Theme](../api/engine/Theme.md) holds fonts, font sizes, colors, icons, and style boxes. Assign it to a control, and every control below it uses the theme unless a closer theme or override replaces a value.

[Editor themes](../reference/themes.md) change the Ludot editor's colors, not your game's UI.

## Focus and mouse input

Make buttons reachable with a keyboard or controller, not just a mouse. For custom layouts, set each control's focus neighbors.

If clicks don't reach a button, a control on top of it may be catching them. Set that control's **Mouse Filter** to **Ignore**.

Handle gameplay input in `_unhandledInput`, so the UI gets the first chance at each event. See [Input actions](./input.md).

## Reference

[Control](../api/engine/Control.md) · [Container](../api/engine/Container.md) · [Label](../api/engine/Label.md) · [Button](../api/engine/Button.md) · [Theme](../api/engine/Theme.md) · [Font](../api/engine/Font.md)
