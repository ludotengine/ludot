---
source: docs/language.md
---

# Operators and control flow

## Optional access and nil coalescing

```luau
const parentName = node?:getParent()?.Name ?? "No parent"
const enabled = settings.Enabled ?? true
local value: boolean? = nil
value ??= false
```

`?.` accesses a property and `?:` calls a method. A nil receiver skips the rest of
its uninterrupted chain, including argument expressions. Each optional receiver
is evaluated once. A nonexistent member on a valid receiver remains a type error.
An ordinary access later in the chain still requires its own receiver to be valid:
`node?.Child.Name` guards `node`; `node?.Child?.Name` also guards `Child`.
Parentheses end the chain, so `(node?.Child).Name` performs an ordinary access on
the parenthesized result.

`??` evaluates the fallback only for nil. False is preserved. `??=` reads the
target once and assigns only when it is nil. Indexed receivers and keys are
calculated once. Const bindings and invalid assignment targets are rejected.
Analysis refines assigned locals and stable indexed locations; it does not assume
an effectful receiver expression denotes the same object on a later evaluation.

## Inclusive ranges

```luau
for i in 1 to 10 do
	print(i)
end
for i in 1 to 10 step 5 do
	print(i) -- 1, then 6
end
for i in 10 to 1 step -3 do
	print(i) -- 10, 7, 4, 1
end
```

Bounds and step are evaluated once, in that order. The default step is 1. A zero
step raises a runtime error before the body runs, in both the VM and JIT. Existing
numeric `for i = start, limit, step` and generic iteration forms remain available.
Range variables have ordinary numeric-for scope and closure capture behavior.

## Pipelines and precedence

A pipeline passes its input as the first argument of an ordinary function:

```luau
const result = enemies
	|> filter(isAlive)
	|> sort(byDistance)
	|> first()
```

`x |> f(a, b)` calls `f(x, a, b)`. `x |> f` calls `f(x)`. Inputs are evaluated
once, before the stage's function reference and explicit arguments. Stages may be
ordinary function references or calls, including parenthesized references and
explicit generic instantiation. Method-call stages and placeholder syntax are
rejected. Each stage consumes one input value and produces one output value;
ordinary Lua expansion of a stage's final explicit argument is preserved. Varargs
inside a lazy fallback, optional method, or pipeline stage keep their enclosing
function's values.

`|>` and `??` share the lowest precedence tier and associate left to right.
Existing arithmetic, concatenation, comparisons, `and`, and `or` bind more tightly.
Thus:

```luau
enemies |> filter(isAlive) ?? {} |> sort(byDistance)
-- ((enemies |> filter(isAlive)) ?? {}) |> sort(byDistance)

enemies |> filter(isAlive) ?? (fallback |> sort(byDistance))
-- Parentheses keep sorting inside the fallback.
```

## Godot class identity

```luau
if event:IsA("InputEventMouseMotion") then
	print(event.Relative)
end
```

Every Godot Object subclass supports class identity checks, including nodes,
resources, input events, and extension classes registered in ClassDB. Both `isA`
and `IsA` are accepted. Completion shows only the spelling selected by method
casing, including `is_a` in snake_case projects. This is a deliberate compatibility
exception to the single-spelling rule for ordinary engine methods.

A valid literal name narrows the receiver using inheritance information. Invalid
literal class names are diagnosed, and completion offers known class names inside
the string. Dynamic strings return booleans without selecting a narrowed class.
Negative branches retain alternatives that could still match at runtime.
Imported Godot-backed script classes and script classes visible to the current
analysis context use their declared class names, not import aliases. Runtime
checks follow native class identity, the attached script inheritance chain, and
Luau object class ancestry. Pure Luau classes without a Godot Object base do not
receive this engine method automatically.

## Tooling for the extended syntax

The editor and standalone LSP share parser and analysis support for these
constructs. Match block/arm entry supports indentation and auto-end completion.
StyLua preserves statements containing syntax its parser does not understand,
byte for byte, while formatting surrounding statements. Selene receives a
line-preserving compatibility view; the native checker validates the protected
constructs. Existing StyLua ignore comments remain effective.

Use `-- selene: allow(match_exhaustiveness)` for the next statement, or
`--# selene: allow(match_exhaustiveness)` at the start of a file, to suppress the
native editor or LSP warning. `signal_declaration` works the same way. Naming rules remain separate from engine casing settings.
The shader subset rejects these new constructs with source diagnostics; its
existing numeric-for syntax remains supported.
