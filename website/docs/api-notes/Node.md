Attach a class extending `Node` to own non-visual scene behavior. Use a more specific base, such as `Node2D` or `Control`, when the attached object has that native type.

```luau
class SceneController extends Node = {
	function _ready()
		print("Ready:", self.Name)
	end
}
```

`onready` fields initialize just before `_ready`. Children become ready before their parent. `getNode("Child/Grandchild")` resolves relative to the receiver, using exact scene node names. See [Nodes and scenes](../../engine/scenes.md).
