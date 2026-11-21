# Nodes and scenes

<p class="lead">A scene is a saved tree of nodes. Compose a game from scenes, then attach Luau scripts to the nodes that own behavior.</p>

## Build a scene tree

A node has one parent and any number of children, and siblings must have different names. A node path like `"HUD/Score"` starts from the node you call `getNode` on. It's not a file path.

As an example, build this scene and save it as `World.tscn`:

```
World (Node2D)
├── Player (CharacterBody2D)
└── HUD (CanvasLayer)
    └── Score (Label)
```

```luau
class World extends Node2D = {
	onready private Score = self:getNode("HUD/Score") :: Label

	function _ready()
		self.Score.Text = "Score: 0"
	end
}
```

Attach the script to `World`. `onready` waits until the scene's children exist before looking them up. Node names are case-sensitive.

## Lifecycle order

- `_enterTree` runs on parents first, then children.
- `_ready` runs the other way: children first, then the parent. So when a parent's `_ready` runs, all its children are ready.
- `onready` fields are set just before their own node's `_ready`.

Use `_process(delta)` for visuals and `_physicsProcess(delta)` for physics movement. `delta` is in seconds.

## Instantiate a saved scene

A `.tscn` scene loads as a [PackedScene](../api/engine/PackedScene.md). Instantiate it to get a node, then add that node to the tree:

```luau
class Spawner extends Node2D = {
	function _ready()
		const scene = ResourceLoader:load("res://Enemy.tscn") :: PackedScene
		const enemy = scene:instantiate() :: Node2D
		enemy.Position = Vector2.new(120, 80)
		self:addChild(enemy)
	end
}
```

This assumes an `Enemy.tscn` with a `Node2D` root.

Loading and instantiating are separate steps. Load the scene once, then call `instantiate` for each copy. All copies share the scene's resources, such as materials, unless you duplicate them.

## Ownership and deletion

`addChild` sets a node's parent. `Owner` decides which scene saves the node. Nodes you create during gameplay only need a parent. Code that builds a scene to save it must also set `Owner`.

Delete nodes with `queueFree`, which frees them at the end of the frame, along with their children. Holding a reference to a node doesn't keep it alive. Once it's freed, the reference can't be used.

## Groups and scene changes

### Groups

Groups tag nodes no matter where they are in the tree:

```luau
class Enemy extends Node2D = {
	function _ready()
		self:addToGroup("enemies")
	end
}

class EnemyCounter extends Node = {
	function _ready()
		const enemies = self:getTree():getNodesInGroup("enemies")
		print(enemies:size(), "enemies left")
	end
}
```

### Changing scenes

`self:getTree():changeSceneToFile("res://NextLevel.tscn")` replaces the current scene. It returns an error code, so check it when the path comes from a setting or player input.

Everything in the old scene is freed. Keep data that must survive, like the score, in an autoload (Project Settings > Globals).

## Reference

[Node](../api/engine/Node.md) · [SceneTree](../api/engine/SceneTree.md) · [PackedScene](../api/engine/PackedScene.md) · [Resources](./resources.md)
