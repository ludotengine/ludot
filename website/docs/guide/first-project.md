# Your first Ludot project

<p class="lead">Create a scene and attach a Luau script that updates a label.</p>

::: warning Experimental
Ludot is experimental, and the API can change between releases. Back up any project you move to it.
:::

You need the Ludot editor. A regular Godot editor doesn't include the Luau runtime.

## Get the editor

[Install a desktop release](./installation.md), or [build it from source](../reference/building.md#editor).

Open the editor and create a new project in an empty folder. Any renderer works; this guide only uses a 2D scene.

## Create the scene

1. Create a **User Interface** root node and name it `Main`.
2. Add a `Label` child named `Message`.
3. Make the label wide enough for a short line of text.
4. Save the scene as `Main.tscn`.
5. Attach a new Luau script to `Main`. Save it as `Main.luau`.

Replace the script's contents with this:

```luau
class Main extends Control = {
	onready private Message = self:getNode("Message") :: Label

	function _ready()
		self.Message.Text = "Hello from Luau"
	end
}
```

- `onready` runs the lookup just before `_ready`, when the label exists.
- `:: Label` tells the checker what kind of node to expect. The editor can often work this out from the scene by itself.
- The code uses the default casing: camelCase methods (`getNode`) and PascalCase properties (`Text`).

## Run the scene

Press F6 to run the current scene. Or press F5 to run the project and pick `Main.tscn` as the main scene.

The label should say `Hello from Luau`. If something goes wrong, check the Debugger panel.

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

Select `Main`, change `Greeting` in the Inspector, and run again. The new value is saved with the scene.

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

`./` paths are relative to the importing file. To set up project-wide shortcuts like `@helpers`, see [Modules and imports](../luau/imports.md).

## Fix common setup errors

| Symptom | Check |
| --- | --- |
| Luau is missing from the script language list | Open the project with a Ludot editor build. |
| The label lookup fails | `Message` must be a direct child of the node with the script, with the exact name. |
| A member name is unknown | Check [API naming](../engine/naming.md). Casing changes require an editor restart. |
| A different scene runs | Run the current scene or change the project's main scene. |
| A game export cannot load Luau | Use export templates built from the Ludot tree. Official Godot templates do not contain the module. |

## Continue

- [Classes and properties](../luau/classes.md): fields, constructors, and inheritance
- [Engine scripting](../engine/): callbacks, nodes, and the scene tree
- [Signals](../engine/signals.md): reacting to buttons and other events
