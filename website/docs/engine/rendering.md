# Rendering, cameras, and materials

<p class="lead">A visible scene needs drawable objects, a camera or viewport, and materials suited to the chosen renderer.</p>

## Choose a renderer

You pick a renderer when you create a project. Which renderers are available, and which effects they support, depends on the graphics hardware. If you use a renderer-specific effect, test it on every platform you ship to.

Desktop platforms are supported. Web is experimental, and mobile is unsupported.

## 2D rendering

[CanvasItem](../api/engine/CanvasItem.md) is the common base for drawable 2D nodes. Use [Sprite2D](../api/engine/Sprite2D.md) for a texture, [Polygon2D](../api/engine/Polygon2D.md) for polygon geometry, and [Camera2D](../api/engine/Camera2D.md) to control the visible region.

Children move, rotate, and scale with their parent. Tree order, canvas layers, and Z index decide what draws in front.

[Control](../api/engine/Control.md) nodes are for UI and have their own layout rules. See [UI, text, and themes](./ui.md).

## 3D rendering

- A [MeshInstance3D](../api/engine/MeshInstance3D.md) draws a mesh.
- A [Camera3D](../api/engine/Camera3D.md) sets the view. It looks along its local −Z axis.
- Materials and lights decide how meshes look.

A mesh doesn't collide with anything; physics needs a separate collision shape.

If nothing shows up, check the active camera, where the object is relative to it, whether the object is visible, and its material and lighting. [Coordinates and transforms](./transforms.md) explains local and global positions.

## Materials and shaders

Use [StandardMaterial3D](../api/engine/StandardMaterial3D.md) for common physically based materials. Use a [ShaderMaterial](../api/engine/ShaderMaterial.md) when custom GPU code should control the result.

Ludot shaders are `.lushader` files written in a typed subset of Luau. The [shader language](../reference/shaders.md) page covers uniforms, varyings, helpers, and imports.

To use a shader, assign it to a `ShaderMaterial`, then assign that material to a node or mesh surface. Uniform names keep the spelling you declared.

## Environment and viewports

[WorldEnvironment](../api/engine/WorldEnvironment.md) sets the sky, background, fog, tone mapping, and similar scene-wide settings.

A [SubViewport](../api/engine/SubViewport.md) renders a second view, such as a minimap or a security camera screen. It doesn't show up on screen by itself. Display its texture with a `TextureRect` or a material.

## Reference

[Viewport](../api/engine/Viewport.md) · [Camera3D](../api/engine/Camera3D.md) · [MeshInstance3D](../api/engine/MeshInstance3D.md) · [ShaderMaterial](../api/engine/ShaderMaterial.md)
