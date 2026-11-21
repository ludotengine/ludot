# Resources and files

<p class="lead">Load scenes, materials, textures, and other resources through the engine. Use user storage for writable game data.</p>

## Paths

| Prefix | Points to | Writable |
| --- | --- | --- |
| `res://` | The project folder | Only in the editor |
| `user://` | A per-user data folder chosen by the platform | Yes |

In an exported game, project files live inside a pack, not as loose files. Load them through the engine's resource APIs rather than the operating system.

Luau `import` is only for Luau modules. Use `ResourceLoader` for textures, scenes, and other assets. See [Modules and imports](../luau/imports.md).

## Load a resource

```luau
class Icon extends Sprite2D = {
	function _ready()
		self.Texture = ResourceLoader:load("res://icon.svg") :: Texture2D
	end
}
```

`ResourceLoader:load` returns a general `Resource`, so cast it to the type you expect. The file must exist and be imported. If the path comes from a setting or player input, check that the result isn't `nil` before using it.

### Shared resources

Loaded resources are cached and shared. Changing a material changes it on every object that uses it. To change one object only, `duplicate()` the resource first, or enable **Local to Scene** on it.

## Save game data

Use [FileAccess](../api/engine/FileAccess.md) to read and write files, and [JSON](../api/engine/JSON.md) to convert data to text:

1. Open a `user://` path for reading or writing.
2. Check that it opened.
3. Read or write the data.
4. Close the file.

Treat save files as untrusted input. An older version of your game, or the player, may have written something unexpected. Check fields, ranges, and a version number before using the data.

To save engine resources themselves, use [ResourceSaver](../api/engine/ResourceSaver.md), which keeps their format and references.

## Background loading

1. Call `ResourceLoader:loadThreadedRequest(path)` once.
2. Check `loadThreadedGetStatus(path)` each frame.
3. When it's loaded, get the resource with `loadThreadedGet(path)`.

Only the loading happens in the background. Your Luau code still runs on the one shared VM.

## Export behavior

The editor can see files that an export leaves out, especially assets loaded from paths built at runtime. Check your export filters and test the exported game. See [Exporting](./exporting.md).

## Reference

[Resource](../api/engine/Resource.md) · [ResourceLoader](../api/engine/ResourceLoader.md) · [ResourceSaver](../api/engine/ResourceSaver.md) · [FileAccess](../api/engine/FileAccess.md) · [PackedScene](../api/engine/PackedScene.md)
