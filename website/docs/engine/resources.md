# Resources and files

<p class="lead">Load scenes, materials, textures, and other resources through the engine. Use user storage for writable game data.</p>

## Paths

`res://` starts at the project root. `user://` starts at the platform-specific writable user-data directory. Exported project resources may live inside a pack rather than as loose operating-system files. Use engine resource APIs to load them.

Module imports resolve Luau modules and have their own alias rules. They are not a replacement for loading a texture or packed scene. See [Modules and imports](../luau/imports.md).

## Load a resource

```luau
class Icon extends Sprite2D = {
	function _ready()
		self.Texture = ResourceLoader:load("res://icon.svg") :: Texture2D
	end
}
```

`ResourceLoader:load` returns a general resource, so narrow it to the type the code expects. The asset must exist and be imported. A dynamic path may fail to load; validate external paths and handle failure before using the result.

Resources are often cached and shared. Changing a loaded material can affect every object using that same material. Duplicate a resource when the change should belong to one instance, or configure scene-local behavior deliberately.

## Save game data

Use [FileAccess](../api/engine/FileAccess.md) for files and [JSON](../api/engine/JSON.md) for JSON text. Open a `user://` path with the desired mode, check whether opening succeeded, write or read the data, and close the file.

Serialized data is external input. Validate its fields, ranges, and schema version before applying it to gameplay. Do not treat a saved table as trusted merely because an earlier version of your game wrote it. For engine resources, [ResourceSaver](../api/engine/ResourceSaver.md) preserves resource formats and references.

## Background loading

`ResourceLoader:loadThreadedRequest` starts a background load. Poll its status and call `loadThreadedGet` when ready. Do not repeatedly start the same request every frame. A background load moves resource-loading work; it does not make arbitrary Luau callbacks execute concurrently in separate VMs.

## Export behavior

An editor project can see files that an exported pack omits. Check export filters and referenced resources, especially for assets loaded from constructed paths. Test the exported game with the custom Ludot templates described in [Exporting](./exporting.md).

## Reference

[Resource](../api/engine/Resource.md) · [ResourceLoader](../api/engine/ResourceLoader.md) · [ResourceSaver](../api/engine/ResourceSaver.md) · [FileAccess](../api/engine/FileAccess.md) · [PackedScene](../api/engine/PackedScene.md)
