# Your first Ludot project

<p class="lead">Create a scene and attach a Luau script that updates a label.</p>

The engine is experimental. Keep a backup of projects you migrate, and expect API changes between revisions. Ludot supports Linux, Windows, and macOS. You need a Ludot editor build; an ordinary Godot editor does not include the Luau runtime.

## Get the editor

Use [Install or update](./installation.md) for a desktop release, or follow [Build from source](../reference/building.md#editor). Desktop CI may also have editor artifacts for a recent run, but artifacts expire and an upload can fail independently of the build. Check the run's artifact list rather than assuming a download exists.

Start the built editor and create a new project in an empty directory. Choose a renderer suitable for your desktop. The steps below only need a 2D scene.

## Create the scene

1. Create a **User Interface** root node and name it `Main`.
2. Add a `Label` child named `Message`.
3. Give the label enough room to display a short line. Move it away from the top-left edge if you want a margin.
4. Save the scene as `Main.tscn`.
5. Attach a new Luau script to `Main`. Save it as `Main.luau`.

Replace the script with this code. It uses the default API casing, camelCase methods and PascalCase properties.

```luau
class Main extends Control = {
	onready private Message = self:getNode("Message") :: Label

	function _ready()
		self.Message.Text = "Hello from Luau"
	end
}
```

`onready` evaluates the node lookup immediately before `_ready`. The label exists by then. The `:: Label` assertion tells the checker which kind of node to expect. The native editor can also infer literal node paths when it knows the attached scene.

## Run the scene

Run the current scene with F6, or run the project with F5 and choose `Main.tscn` as its main scene. The label should display `Hello from Luau`. Check the debugger panel if the script reports an error.

This example needs the desktop engine. It cannot run in the browser playground because it uses `Control`, `Label`, and a scene tree.

## Add an exported setting

Make the greeting editable in the Inspector:

```luau
class Main extends Control = {
	export Greeting: string = "Hello from Luau"
	onready private Message = self:getNode("Message") :: Label

	function _ready()
		self.Message.Text = self.Greeting
	end
}
```

Select `Main`, change `Greeting` in the Inspector, and run again. The scene stores that exported value. Ordinary Luau scripts do not run their lifecycle callbacks while you edit the scene.

## Share code with a module

Create `helpers/Greeting.luau` in the project:

```luau
const function greet(name: string)
	return "Hello, " .. name
end

return { greet = greet }
```

Import the function at the top of `Main.luau`, before the class:

```luau
import greet from "./helpers/Greeting"

class Main extends Control = {
	onready private Message = self:getNode("Message") :: Label

	function _ready()
		self.Message.Text = greet("Ludot")
	end
}
```

Imports resolve relative to the importing file. For project-wide aliases, see [Modules and imports](../luau/imports.md).

## Fix common setup errors

| Symptom | Check |
| --- | --- |
| Luau is missing from the script language list | Open the project with a Ludot editor build. |
| The label lookup fails | `Message` must be a direct child of the node with the script, with the exact name. |
| A member name is unknown | Check [API naming](../engine/naming.md). Casing changes require an editor restart. |
| A different scene runs | Run the current scene or change the project's main scene. |
| A game export cannot load Luau | Use export templates built from the Ludot tree. Official Godot templates do not contain the module. |

## Continue

Read [Classes and properties](../luau/classes.md) for initialization and inheritance, [Engine scripting](../engine/) for lifecycle and ownership, or [Signals](../engine/signals.md) to connect UI events.
