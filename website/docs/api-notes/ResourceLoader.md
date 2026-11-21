`ResourceLoader` is an engine singleton. Use a colon for its instance methods:

```luau
const scene = ResourceLoader:load("res://Enemy.tscn") :: PackedScene
const instance = scene:instantiate()
print(instance.Name)
```

This example requires `Enemy.tscn`. Add the instantiated node to the scene tree to give it normal scene behavior. Loading can fail for an invalid or unavailable path; handle external paths before using the result. See [Resources and files](../../engine/resources.md).
