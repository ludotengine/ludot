Read the typed `Pressed` signal and connect a handler. The connection belongs to the object whose callback connected it.

```luau
class ActionButton extends Button = {
	function _ready()
		self.Pressed:connect(function()
			print("Pressed:", self.Text)
		end)
	end
}
```

Use keyboard focus and controller navigation as well as pointer input. See [UI, text, and themes](../../engine/ui.md) and [Signals](../../engine/signals.md).
