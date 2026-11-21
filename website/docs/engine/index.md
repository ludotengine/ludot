# Engine scripting

<p class="lead">Attach Luau classes to Godot nodes and resources. Use the scene tree, native value types, Inspector exports, and engine events.</p>

GDScript remains available. The Luau binding uses [configurable API naming](./naming.md).

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

An attached script extends a native class or an imported Luau script class that ultimately extends one. The native base must fit the object carrying the script.

```luau
class Spinner extends Node3D = {
	export Speed: number = 1

	function _process(delta: number)
		self:rotateY(self.Speed * delta)
	end
}
```

Attach this script to a `Node3D`. `_process` receives elapsed frame time in seconds. Multiplying by `delta` keeps the rotation rate independent of rendering frame rate.

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

These names use default camelCase method settings. Engine dispatch maps the native callbacks to the selected Luau spelling. User methods are not renamed. Use `onready` fields for node lookups that need the scene to exist.

Normal scripts do not run lifecycle callbacks while the editor is editing a scene. Luau editor tool scripts are not supported.

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

Native object properties read and write the engine object directly. Engine arrays, dictionaries, and packed arrays support indexing from 1, length with `#`, and direct iteration. This differs from Godot's usual zero-based script examples.

Use scene ownership deliberately. `addChild` adds a node to the tree, while the node's `Owner` controls saved scene ownership. Freeing a node also frees its children. Use `queueFree` to request deferred deletion during normal gameplay.

## Events and asynchronous work

Engine signals are typed `Signal` values. Use `connect`, `once`, or `wait` and retain the returned `Connection` if you need to disconnect it. See [Signals and connections](./signals.md) for ownership, yielding handlers, and GDScript interoperability.

The [task scheduler](./tasks.md) resumes work on engine process frames. A callback's tasks belong to its object, respect processing and pause state, and are dropped when that object is freed. Module top-level code and field initializers cannot yield.

## Type checking and identity

The native editor derives API signatures from live reflection. The standalone analyzer reads the committed API manifest. Both understand native classes and selected API casing. Use [identity checks](../luau/operators.md#godot-class-identity) to refine a general object to its engine type.

Privacy is enforced by the checker. It is not an isolation boundary against unchecked bytecode or reflective native APIs.

## Runtime limits

All Luau code shares one VM. Engine threads enter it one at a time, and a breakpoint can block other threads waiting to execute Luau. Hot reload preserves objects and compatible field values, but already running or suspended functions can finish with old code. Read [Hot reload and debugging](./reload.md) before relying on live changes.

Desktop exports are the primary targets. Mobile is unsupported. Web support is experimental; the single-threaded Vesper build has been checked in Chromium, while other projects and browsers remain unverified. Export templates must come from this repository. The [class reference](../api/) also includes inherited editor and platform APIs; their presence does not remove these limits.
