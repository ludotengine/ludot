# Exporting a desktop game

<p class="lead">Use templates built from the Ludot source tree. Official Godot templates do not include Ludot's Luau module.</p>

## Build templates

Follow [Build from source](../reference/building.md#export-templates) for the platform dependencies and template commands. Build both debug and release templates when your workflow needs both. An editor binary and an export template serve different purposes.

Ludot currently supports Linux, Windows, and macOS. Mobile is unsupported. Web export is experimental; see the [build guide](../reference/building.md) for the single-threaded template used by the Vesper demo.

## Create a preset

1. Open Project > Export in the Ludot editor.
2. Add a preset for the target desktop platform.
3. Select the matching custom debug and release template paths.
4. Configure the export destination and resource filters.
5. Set the project's main scene and export the game.

Choose a template from the same Ludot revision as the editor when possible. A `.pck` contains project content; it still needs a compatible executable to run.

## Include dynamic resources

Scenes referenced directly by other scenes are easier for export dependency discovery to follow. Assets loaded by paths constructed at runtime may need explicit export filters. Include Luau modules and `.lushader` dependencies used by the project, including imported libraries.

`res://` can refer to packed resources in an export. Use `user://` for data the game writes, rather than trying to modify the application bundle or packed project.

## Check the exported build

Launch the exported game from a directory outside the development checkout. Check its main scene, script loading, shader compilation, input, audio, and any dynamically loaded assets. Verify writable data paths under a normal user account.

A game working in the editor does not prove its export includes every dependency. The repository's desktop regressions include exported-pack checks, but your project's own resource and platform behavior still needs verification.

## Reference

[Build instructions](../reference/building.md) · [Resources and files](./resources.md) · [Implementation status](../reference/status.md)
