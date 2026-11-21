# Engine scripting

<p class="lead">Attach Luau classes to Godot nodes and resources. Use the scene tree, native value types, Inspector exports, and engine events.</p>

GDScript still works alongside Luau. Engine methods and properties use [configurable casing](./naming.md); this manual uses the defaults, camelCase methods and PascalCase properties.

## Engine manual

| Topic | Learn |
| --- | --- |
| [Nodes and scenes](./scenes.md) | Parenting, lifecycle, instantiation, ownership, and scene changes. |
| [Values and collections](./values.md) | Native values, tables, indexing, truthiness, and call syntax. |
| [Coordinates and transforms](./transforms.md) | Local and global space, directions, transforms, and time steps. |
| [Input actions](./input.md) | Input Map bindings, continuous input, events, and mouse capture. |
| [Physics and movement](./physics.md) | Collision bodies, shapes, velocity, gravity, and a character controller. |
| [UI, text, and themes](./ui.md) | Layout, buttons, fonts, themes, and focus. |
| [Resources and files](./resources.md) | Resource paths, shared assets, loading, and user data. |
| [Animation and tweens](./animation.md) | Authored animations, property tweens, and completion. |
| [Audio](./audio.md) | Player types, streams, buses, and playback lifetime. |
| [Rendering](./rendering.md) | Cameras, meshes, materials, environment, and viewports. |
| [Exporting](./exporting.md) | Custom templates, presets, asset filters, and desktop checks. |

## Attach a class

A script attached to a node or resource extends a native class, or another Luau script class that eventually extends one. The native class must match the object: a script that extends `Node3D` can only go on a `Node3D` or one of its subclasses.

```luau
class Spinner extends Node3D = {
	export Speed: number = 1

	function _process(delta: number)
		self:rotateY(self.Speed * delta)
	end
}
```

Attach this script to a `Node3D`. `_process` receives the time since the last frame, in seconds. Multiplying by `delta` keeps the rotation speed the same at any frame rate.

## Lifecycle

| Callback | Use |
| --- | --- |
| `__init` | Initialize a Luau instance after field defaults. |
| `_enterTree` | Respond when a node enters the scene tree. |
| `_ready` | Initialize behavior after the node and its children are ready. |
| `_process(delta)` | Update once per rendered process frame. |
| `_physicsProcess(delta)` | Update at the physics tick. |
| `_input(event)` | Receive input before ordinary unhandled-input processing. |
| `_unhandledInput(event)` | Receive events that earlier handlers did not consume. |
| `_exitTree` | Respond when the node leaves the tree. |

These names assume the default camelCase method casing. With another casing, the engine calls your spelling of the callback instead. Your own methods are never renamed.

Use `onready` fields for node lookups, since child nodes only exist once the scene is ready.

Scripts don't run their callbacks while you edit a scene in the editor. Luau editor tool scripts aren't supported.

## Nodes, resources, and values

```luau
class Builder extends Node3D = {
	function _ready()
		const child = Node3D.new()
		child.Name = "Marker"
		self:addChild(child)
		child.Position = Vector3.new(0, 2, 0)
	end
}
```

Reading or writing a property goes straight to the engine object.

Engine arrays, dictionaries, and packed arrays support `#`, iteration, and indexing **from 1**. Godot's own examples count from 0, so watch for that when porting code.

A few things to know about nodes:

- `addChild` puts a node in the tree. The node's `Owner` decides which scene saves it.
- Freeing a node also frees its children.
- During gameplay, delete nodes with `queueFree`, which waits until the end of the frame.

## Events and asynchronous work

Engine signals are typed `Signal` values. Use `connect`, `once`, or `wait`, and keep the returned `Connection` if you need to disconnect later. [Signals and connections](./signals.md) covers ownership, waiting inside handlers, and GDScript.

The [task library](./tasks.md) waits and schedules work on engine frames:

- Tasks belong to the object whose callback started them, and are dropped when it's freed.
- They pause when their node pauses.
- Module top-level code and field initializers can't wait.

## Type checking and identity

The native editor reads engine signatures from the running engine. The standalone analyzer reads the committed API manifest. Both know every native class and your casing settings.

Use an [`isA` check](../luau/operators.md#godot-class-identity) to narrow a general object, such as an `InputEvent`, to a specific class.

`private` is enforced by the checker only. It doesn't stop reflective engine calls or unchecked bytecode.

## Runtime limits

- **One VM.** All Luau code shares one VM, and threads enter it one at a time. A breakpoint can block other threads that are waiting to run Luau.
- **Hot reload.** Objects and compatible field values survive a reload, but functions that are running or paused finish with their old code. See [Hot reload and debugging](./reload.md).
- **Platforms.** Linux, Windows, and macOS are supported. Web is experimental: only the single-threaded Vesper demo has been tested, in Chromium. Mobile is unsupported.
- **Export templates** must be built from this repository.

The [class reference](../api/) includes editor and platform APIs inherited from Godot. Their presence doesn't mean those platforms are supported.
