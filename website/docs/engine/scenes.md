# Nodes and scenes

<p class="lead">A scene is a saved tree of nodes. Compose a game from scenes, then attach Luau scripts to the nodes that own behavior.</p>

## Build a scene tree

A node has one parent and any number of children. Names must be unique among siblings. A path such as `"HUD/Score"` starts at the receiver of `getNode`; it is not a filesystem path.

For a small game, create a `Node2D` root named `World`, a `CharacterBody2D` child named `Player`, and a `CanvasLayer` child named `HUD`. Add a `Label` named `Score` under `HUD`. Save the root as `World.tscn`.

```luau
class World extends Node2D = {
	onready private Score = self:getNode("HUD/Score") :: Label

	function _ready()
		self.Score.Text = "Score: 0"
	end
}
```

Attach the script to `World`. `onready` waits until the node is ready, so the lookup sees the scene's children. Use exact node names, including capitalization.

## Lifecycle order

When a branch enters the tree, parents receive `_enterTree` before their children. Readiness goes in the opposite direction: children receive `_ready` before their parent. An `onready` initializer runs immediately before its object's `_ready`.

Use `_process(delta)` for visual updates. Use `_physicsProcess(delta)` for physics movement. Both receive seconds. The default names here assume camelCase method casing. [API naming](./naming.md) explains other styles.

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

Create `Enemy.tscn` first, with a `Node2D` root. Loading a resource and creating its instance are separate operations. Multiple instances share scene resources unless you duplicate or localize those resources.

## Ownership and deletion

`addChild` establishes parenting. `Owner` establishes which scene saves a node. Nodes created during gameplay usually need parenting only. Tools that construct a scene for saving must also set ownership to an ancestor in that scene.

Call `queueFree` for normal deferred deletion. Freeing a node also frees its children. References to freed native objects are not usable; keep object lifetimes clear rather than assuming a stored reference keeps a node alive.

## Groups and scene changes

Groups label nodes independently of their parent. `addToGroup("enemies")` adds a label; `self:getTree():getNodesInGroup("enemies")` returns the current members. Group names are strings and keep their spelling.

Use `self:getTree():changeSceneToFile("res://NextLevel.tscn")` to replace the current scene. Check the returned error value when the path comes from configuration or user input. Persist data that must survive a scene change outside the outgoing scene, such as in an autoload configured in Project Settings.

## Reference

[Node](../api/engine/Node.md) · [SceneTree](../api/engine/SceneTree.md) · [PackedScene](../api/engine/PackedScene.md) · [Resources](./resources.md)
