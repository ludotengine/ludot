# Exporting a desktop game

<p class="lead">Use templates built from the Ludot source tree. Official Godot templates do not include Ludot's Luau module.</p>

## Build templates

An export template is the game executable your project ships with. It's a separate build from the editor. Follow [Build from source](../reference/building.md#export-templates) to build debug and release templates.

Linux, Windows, and macOS are supported. Web export is experimental; the [build guide](../reference/building.md) covers the single-threaded template used by the Vesper demo. Mobile is unsupported.

## Create a preset

1. Open Project > Export in the Ludot editor.
2. Add a preset for the target desktop platform.
3. Select the matching custom debug and release template paths.
4. Configure the export destination and resource filters.
5. Set the project's main scene and export the game.

Use templates from the same Ludot version as your editor. A `.pck` file only holds your project's content; it needs a matching executable to run.

## Include dynamic resources

The exporter finds files that scenes reference directly. It can't follow paths your code builds at runtime, like `"res://levels/" .. name .. ".tscn"`. Add those with an export filter.

Also make sure the export includes every Luau module and `.lushader` file the project uses, including imported libraries.

In an exported game, `res://` is read-only. Write save data and settings to `user://`.

## Check the exported build

A game that works in the editor can still be missing files when exported. Run the exported build from a folder outside your project and check:

- the main scene loads
- scripts load and shaders compile
- input and audio work
- assets loaded at runtime are found
- saving works under a normal user account

## Reference

[Build instructions](../reference/building.md) · [Resources and files](./resources.md) · [Implementation status](../reference/status.md)
