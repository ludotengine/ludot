# Luau shaders

The `.lushader` extension identifies a restricted, statically compiled shader dialect.

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

## Declarations

`canvas_item` and `spatial` accept `vertex`, `fragment`, and `light` entry points.
`particles` accepts `start` and `process`. Supported type names include `number` or
`float`, `boolean` or `bool`, `int`, `uint`, vector types, matrix types, and sampler
types.

Top-level declarations:

- `export Name: type = value` declares a uniform. The value is optional. Number and
  `int` uniforms accept the same `in <min> to <max> step <value>` range hint as script
  exports. The hint becomes `hint_range` and its bounds must be numeric literals.
- `local Name: type` without a value declares a varying. Particle shaders have no
  varyings.
- `local Name: type = value` or `const Name: type = value` declares a constant.
- `function name(a: type): type` or `local function` declares a helper. Parameters
  need types. A helper with a return type must end with `return`. Helpers must
  appear before their callers and cannot recurse.
- `renderMode("mode", ...)` sets render modes. Modes are lowercase string literals,
  and the engine validates them for the shader type.
- A varying can request interpolation with `flat<type>` or `smooth<type>`, for example
  `local Normal: flat<vec3>`. Integer and boolean varyings are always `flat`.

## Includes

A file that starts with `shader include` is a library. It holds uniforms, varyings,
constants, and helper functions, but no entry points or render modes. Other shaders
import it before their other declarations:

```lua
shader canvas_item
import tinted from "./shaders/tint_common"
import "@shaders/noise"
```

`import "path"` includes the library. `import a, b from "path"` also checks that the
library declares `a` and `b`. All of the library's declarations are visible either
way. Paths start with `./`, `../`, or an `@alias`, and the `.lushader` extension is
optional. Aliases come from the same project setting and `.luaurc` files as Luau
modules. Libraries can import other libraries. Each one is included once, and cyclic
imports are errors.

Editing a library, on disk or in the editor, recompiles every loaded shader that
includes it. Errors inside a library are reported in the library and at the importing
line.

## Statements and expressions

Function bodies support typed locals, assignment, compound assignment (`+=`, `-=`,
`*=`, `/=`, `%=`, `^=`), `if`/`elseif`/`else`, `while`, numeric `for`, `break`,
`continue`, `return`, and `discard()` in canvas and spatial shaders. A numeric `for`
uses Luau's inclusive bounds. Its step must be a numeric literal, and the loop
variable is `int` when the start and step are integers. Annotate the loop variable to
choose another type.

Expressions support arithmetic, comparisons, `and`, `or`, `not`, swizzles, indexing,
`if ... then ... else` expressions, `%` (as `mod` for floating-point values), `^` (as
`pow`), and `//` (as `floor(a / b)` for floating-point values). An integral literal
becomes an `int` or `uint` when the other operand, the assignment target, or the
declared type is an integer type. Otherwise literals are floats.

Standard shader functions include `mix`, `clamp`, `normalize`, `texture`, and the
functions listed in `language/src/shader.cpp`. Shader built-ins use explicit aliases
such as `Color`, `Texture`, `Vertex`, `Albedo`, `Velocity`, and `Time`. `UV` keeps its
spelling.

## Diagnostics

Tables, strings outside `renderMode` and imports, dynamic calls, classes, coroutines,
`repeat` loops, generic `for` loops, multiple returns, and recursion produce
compile-time errors with source positions.

The native engine validates GPU types after lowering, and maps the engine's
diagnostics back to `.lushader` lines. The standalone `ludot-shader` tool checks the
Luau subset and emits shader source. It does not validate GPU types itself. It resolves
imports relative to the file, or to the path given with `--origin`, which the LSP uses
to check unsaved buffers.

## Editing and hot reload

A native `LuauShader` resource preserves the shader RID and uniform values when it
compiles a valid edit. A failed edit keeps the previous shader code. The editor opens
`.lushader` files in a dedicated code editor with delayed validation and shader word
completion.
