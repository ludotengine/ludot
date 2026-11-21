`Font` is a resource used by labels, buttons, themes, and text-drawing APIs. It is not Roblox's `Font` data type. Use `FontFile` for imported font data or `FontVariation` for a configured variation, rather than constructing the abstract `Font` base.

```luau
class Caption extends Label = {
	function _ready()
		const font = ResourceLoader:load("res://fonts/Body.ttf") :: FontFile
		self:addThemeFontOverride("font", font)
		self:addThemeFontSizeOverride("font_size", 24)
		self.Text = "Hello from Ludot"
	end
}
```

Import `fonts/Body.ttf` before running this scene. Native theme item names such as `"font"` and `"font_size"` remain strings in their original spelling. For a whole interface, assign a shared `Theme` to a parent control. See [UI, text, and themes](../../engine/ui.md).
