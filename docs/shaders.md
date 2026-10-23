# Luau shaders

The `.lushader` extension identifies a restricted, statically compiled shader dialect.

```lua
shader canvas_item
export Tint: vec4 = vec4(1, 1, 1, 1)

function fragment()
    Color = texture(Texture, UV) * Tint
end
```

`canvas_item` and `spatial` accept `vertex`, `fragment`, and `light` entry points.
`particles` accepts `start` and `process`. Uniforms use `export Name: type`, with an
optional initializer. Supported type names include `number` or `float`, `boolean`
or `bool`, `int`, `uint`, vector types, matrix types, and sampler types.

Typed locals, assignments, arithmetic, swizzles, conditionals, and early returns
compile to Godot shader source. Standard shader functions include `mix`, `clamp`,
`normalize`, `texture`, and the functions listed in `language/src/shader.cpp`.
Shader built-ins use explicit aliases such as `Color`, `Texture`, `Vertex`,
`Albedo`, `Velocity`, and `Time`. `UV` keeps its spelling.

Tables, dynamic calls, imports, classes, coroutine operations, and unsupported
control flow produce compile-time errors. Loops, custom helper functions,
varyings, render modes, and shader includes are not implemented yet. The native
engine validates GPU types after lowering. The standalone tool checks the Luau
subset and emits shader source; it does not independently validate GPU types.

A native `LuauShader` resource preserves the shader RID and uniform values when
compiling valid edits. Failed edits retain the previous shader code. Source is
editable through the resource inspector and external editors. A dedicated native
shader code editor remains unfinished.
