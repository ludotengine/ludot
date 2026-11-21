# Rendering, cameras, and materials

<p class="lead">A visible scene needs drawable objects, a camera or viewport, and materials suited to the chosen renderer.</p>

## Choose a renderer

Select a renderer when creating the project. Renderer availability and features depend on the desktop graphics backend. A scene that relies on a renderer-specific effect needs testing with that renderer on each target platform.

Ludot's native integration supports desktop targets. Mobile is unsupported. A single-threaded web target is experimental and currently verified only for the Vesper demo in Chromium.

## 2D rendering

[CanvasItem](../api/engine/CanvasItem.md) is the common base for drawable 2D nodes. Use [Sprite2D](../api/engine/Sprite2D.md) for a texture, [Polygon2D](../api/engine/Polygon2D.md) for polygon geometry, and [Camera2D](../api/engine/Camera2D.md) to control the visible region.

Parent transforms affect children. Draw order, canvas layers, and Z settings decide which item appears in front. [Control](../api/engine/Control.md) nodes belong to the UI layout system and have their own sizing rules.

## 3D rendering

A [MeshInstance3D](../api/engine/MeshInstance3D.md) draws a mesh. A [Camera3D](../api/engine/Camera3D.md) determines the view. Materials and lighting determine how the mesh appears. A mesh resource is not a collision shape.

If nothing appears, check the active camera, the object's position relative to it, visibility, material settings, and lighting. A camera looks along its local negative Z axis. [Coordinates and transforms](./transforms.md) explains local and global space.

## Materials and shaders

Use [StandardMaterial3D](../api/engine/StandardMaterial3D.md) for common physically based materials. Use a [ShaderMaterial](../api/engine/ShaderMaterial.md) when custom GPU code should control the result.

Ludot's `.lushader` compiler accepts a typed Luau subset, with separate canvas, spatial, and particle entry points. It does not accept arbitrary gameplay code. See the complete [Shader language](../reference/shaders.md) guide for uniforms, varyings, helpers, imports, and source diagnostics.

Uniform names keep their declared spelling. Assign a shader resource to a shader material, then assign that material to the appropriate drawable node or surface.

## Environment and viewports

[WorldEnvironment](../api/engine/WorldEnvironment.md) configures environment resources such as background and tone mapping. [SubViewport](../api/engine/SubViewport.md) renders another view, for example a minimap or a texture shown in the main scene.

A viewport has its own size and update behavior. Rendering into it does not automatically place its result on screen; display its viewport texture with a suitable material or control.

## Reference

[Viewport](../api/engine/Viewport.md) · [Camera3D](../api/engine/Camera3D.md) · [MeshInstance3D](../api/engine/MeshInstance3D.md) · [ShaderMaterial](../api/engine/ShaderMaterial.md)
