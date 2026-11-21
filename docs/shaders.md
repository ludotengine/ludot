# Luau shaders

A `.lushader` file is a shader written in a typed subset of Luau. It compiles ahead
of time to Godot shader code, so it only accepts what a GPU can run: no tables,
strings, or dynamic calls.

```lua
shader spatial
renderMode("unshaded", "cull_disabled")

export Cutoff: number = 0.5 in 0 to 1 step 0.05
export Steps: int = 3 in 1 to 8

local Height: number          -- varying, written in vertex() and read in fragment()
local Scale: number = 2       -- constant

local function wave(x: number): number
    return sin(x * Scale)
end

function vertex()
    Height = wave(Vertex.y + Time)
end

function fragment()
    local total: number = 0
    for i = 1, Steps do
        if i % 2 == 0 then continue end
        total += Height / float(i)
    end
    if total < Cutoff then
        discard()
    end
    Albedo = vec3(total)
end
```

## Shader types

The first line names the shader type, which decides the entry points you can define:

| Type | Entry points |
| --- | --- |
| `shader canvas_item` | `vertex`, `fragment`, `light` |
| `shader spatial` | `vertex`, `fragment`, `light` |
| `shader particles` | `start`, `process` |

## Types

| Kind | Names |
| --- | --- |
| Scalars | `number` or `float`, `boolean` or `bool`, `int`, `uint` |
| Vectors and matrices | `vec2`, `vec3`, `vec4`, `mat4`, and the other GPU vector and matrix types |
| Textures | sampler types |

## Declarations

| Declaration | Meaning |
| --- | --- |
| `export Name: type = value` | Uniform, set from the Inspector or code. The value is optional. |
| `local Name: type` | Varying, written in `vertex()` and read in `fragment()` |
| `local Name: type = value` | Constant |
| `const Name: type = value` | Constant |
| `function name(a: type): type` | Helper function. `local function` works too. |
| `renderMode("mode", ...)` | Render modes |

### Uniforms

Number and `int` uniforms accept the same range hint as script exports:
`in <min> to <max> step <value>`. It becomes `hint_range`, and the bounds must be
numeric literals.

These special uniforms bind to the current frame's buffers and take no value:

| Type | Buffer |
| --- | --- |
| `export Name: DepthTexture` | Depth |
| `export Name: ScreenTexture` | Color |
| `export Name: NormalRoughnessTexture` | Normal and roughness |

Each one becomes a `sampler2D`.

### Varyings

Request an interpolation mode with `flat<type>` or `smooth<type>`, for example
`local Normal: flat<vec3>`. Integer and boolean varyings are always `flat`.

Particle shaders have no varyings.

### Helpers

- Parameters need types.
- A helper with a return type must end with `return`.
- Helpers must be declared before the code that calls them.
- Helpers can't call themselves.

### Render modes

Modes are lowercase string literals. The engine checks that they are valid for the
shader type.

## Includes

A file that starts with `shader include` is a library. It can hold uniforms,
varyings, constants, and helpers, but no entry points or render modes.

Import libraries before any other declaration:

```lua
shader canvas_item
import tinted from "./shaders/tint_common"
import "@shaders/noise"
```

- `import "path"` includes the library.
- `import a, b from "path"` also checks that the library declares `a` and `b`.

Either way, every declaration in the library becomes visible.

**Paths** start with `./`, `../`, or an `@alias`. The `.lushader` extension is
optional. Aliases come from the same Project Settings entry and `.luaurc` files as
Luau modules.

**Nesting:** libraries can import other libraries. Each library is included once,
and import cycles are errors.

Editing a library, on disk or in the editor, recompiles every loaded shader that
includes it. An error inside a library is reported in the library and on the
importing line.

## Statements

Function bodies support:

- typed locals and assignment
- compound assignment: `+=`, `-=`, `*=`, `/=`, `%=`, `^=`
- `if` / `elseif` / `else`
- `while` and numeric `for`
- `break`, `continue`, and `return`
- `discard()`, in canvas and spatial shaders

A numeric `for` includes both bounds, like Luau. Its step must be a numeric literal.
The loop variable is an `int` when the start and step are integers; annotate it to
choose another type.

## Expressions

Expressions support arithmetic, comparisons, `and`, `or`, `not`, swizzles, indexing,
and `if ... then ... else` expressions.

Some operators map to GPU functions:

| Luau | Becomes |
| --- | --- |
| `a % b` (floats) | `mod(a, b)` |
| `a ^ b` | `pow(a, b)` |
| `a // b` (floats) | `floor(a / b)` |

A whole-number literal becomes an `int` or `uint` when the other operand, the
assignment target, or the declared type is an integer type. Otherwise it's a float.

## Built-ins

Standard shader functions include `mix`, `clamp`, `normalize`, `texture`, and the
rest of the functions listed in `language/src/shader.cpp`.

Built-in variables use PascalCase names such as `Color`, `Texture`, `Vertex`,
`Albedo`, `Velocity`, and `Time`. `UV` keeps its spelling.

To reconstruct world positions from depth, use the camera and screen built-ins:
`CameraPosition`, `CameraDirection`, `NodePosition`, `View`, `FragCoord`,
`ViewportSize`, `InverseViewMatrix`, and `InverseProjectionMatrix`.

## Errors

These produce compile-time errors with source positions:

- tables
- strings, except in `renderMode` and imports
- dynamic calls
- classes and coroutines
- `repeat` loops and generic `for` loops
- multiple return values
- recursion

### Where checking happens

The **engine** validates GPU types after compiling, and maps its errors back to
`.lushader` lines.

The standalone **`ludot-shader`** tool checks the Luau subset and emits shader code,
but doesn't validate GPU types. It resolves imports relative to the file, or to the
path given with `--origin`. The LSP uses `--origin` to check unsaved buffers.

## Editing and hot reload

The editor opens `.lushader` files in a dedicated code editor with delayed
validation and shader word completion.

When you save a valid edit, the `LuauShader` resource keeps its shader RID and
uniform values. A failed edit keeps the previous shader code running.
