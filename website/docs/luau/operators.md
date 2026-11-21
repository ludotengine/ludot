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

| Operator | Meaning |
| --- | --- |
| `a?.b` | Read `b`, or skip if `a` is nil |
| `a?:m()` | Call `m`, or skip if `a` is nil |
| `a ?? b` | `a`, unless it's nil, then `b` |
| `a ??= b` | Assign `b` only if `a` is nil |

### Optional chains

When the receiver is nil, the rest of the chain is skipped, including any argument
expressions, and the result is nil. Each optional receiver is evaluated once.

Only receivers marked with `?` are guarded:

- `node?.Child.Name` guards `node`, but `Child` must exist.
- `node?.Child?.Name` guards both.
- Parentheses end a chain. `(node?.Child).Name` reads `Name` from the result
  normally.

Reading a member that doesn't exist on a valid receiver is still a type error.

### Nil coalescing

`??` only uses the fallback for `nil`. Unlike `or`, it keeps `false`:

```luau
const settings = { Sound = false }
print(settings.Sound ?? true) --> false
print(settings.Sound or true) --> true
```

The fallback is only evaluated when it's needed.

`??=` reads its target once and assigns only when the target is nil. Indexed targets
compute their table and key once. Assigning to a `const` or to something that isn't
assignable is an error.

The checker narrows assigned locals and stable indexed locations after `??=`. It
doesn't assume that an expression with side effects returns the same object twice.

## Inclusive ranges

```luau
for i in 1 to 10 do
	print(i) -- 1, 2, ..., 10
end

for i in 1 to 10 step 5 do
	print(i) -- 1, then 6
end

for i in 10 to 1 step -3 do
	print(i) -- 10, 7, 4, 1
end
```

- Both ends are included.
- The start, end, and step are evaluated once, in that order. The default step is 1.
- A step of zero raises a runtime error before the loop body runs.
- The loop variable has the same scope and closure behavior as a numeric `for`.

The classic `for i = start, limit, step` and generic `for` loops still work.

## Pipelines and precedence

The pipeline operator `|>` is a convenience. It doesn't add anything you couldn't
write before. It changes the order you write function calls in, so a chain of steps
reads in the order it runs.

```luau
x |> f(a, b)   -- same as f(x, a, b)
x |> f()       -- same as f(x)
x |> f         -- same as f(x)
```

The value on the left becomes the first argument of the function on the right.

### What it replaces

Say you want the closest living enemy. With three small functions, `filter`, `sort`,
and `first`, you'd usually write one of these:

```luau
-- Nested calls: read from the inside out.
const target = first(sort(filter(enemies, isAlive), byDistance))

-- Temporary variables: clear, but every step needs a name.
const alive = filter(enemies, isAlive)
const sorted = sort(alive, byDistance)
const target = first(sorted)
```

The pipeline version makes the same calls in the same order:

```luau
const target = enemies
	|> filter(isAlive)
	|> sort(byDistance)
	|> first()
```

It reads top to bottom: take the enemies, keep the living ones, sort them by
distance, take the first. You can add, remove, or reorder a step by editing one line.

<details>
<summary>The helper functions used above</summary>

```luau
type Enemy = { Name: string, Alive: boolean, Distance: number }

const function filter<T>(values: { T }, keep: (T) -> boolean): { T }
	const result: { T } = {}
	for _, value in values do
		if keep(value) then
			table.insert(result, value)
		end
	end
	return result
end

const function sort<T>(values: { T }, before: (T, T) -> boolean): { T }
	table.sort(values, before)
	return values
end

const function first<T>(values: { T }): T?
	return values[1]
end

const function isAlive(enemy: Enemy)
	return enemy.Alive
end

const function byDistance(a: Enemy, b: Enemy)
	return a.Distance < b.Distance
end
```

</details>

### Another example: damage

Pipelines work for any value, not just lists. Here a hit's damage goes through
armor, a critical bonus, rounding, and a cap:

```luau
-- Without a pipeline
const damage = math.clamp(math.floor(applyCritical(applyArmor(40, 25), true)), 0, 999)

-- With a pipeline
const damage = 40
	|> applyArmor(25)
	|> applyCritical(true)
	|> math.floor()
	|> math.clamp(0, 999)
```

In the nested version, the `25` and `true` are far from the functions they belong
to, and the closing parentheses pile up. The pipeline keeps each argument next to
its step.

### When to use one

A pipeline helps when:

- you apply three or more steps to one value
- the steps are plain functions that take that value first
- you expect to add or reorder steps later

### When to skip it

- **One or two calls.** `first(alive)` is already clear.
- **The value has methods.** Use `:` instead. `name:lower():sub(1, 12)` already
  reads left to right. Pipelines are for free functions, which can't chain that way.
  Method calls aren't allowed as stages, so `x |> obj:method()` is a compile error.
- **The value isn't the first argument.** There's no placeholder syntax for putting
  it anywhere else, so write a normal call.

### Evaluation rules

- The input is evaluated once, before the stage's function and its other arguments.
- A stage can be a function name, a call, a parenthesized function reference, or a
  call with explicit generic arguments.
- Each stage takes one value and returns one value. If a function returns several
  values, the next stage only gets the first.
- If a stage's last explicit argument is a call or `...`, it expands normally, as in
  any Lua call.
- `...` inside a pipeline stage, a `??` fallback, or an optional method call refers
  to the enclosing function's varargs.

### Precedence

`|>` and `??` share the lowest precedence and group left to right. Arithmetic,
`..`, comparisons, `and`, and `or` all bind more tightly. That lets you put a
fallback in the middle of a chain:

```luau
enemies |> filter(isAlive) ?? {} |> sort(byDistance)
-- ((enemies |> filter(isAlive)) ?? {}) |> sort(byDistance)
```

Use parentheses to keep later steps inside the fallback:

```luau
enemies |> filter(isAlive) ?? (fallback |> sort(byDistance))
```

## Godot class identity

```luau
if event:IsA("InputEventMouseMotion") then
	print(event.Relative)
end
```

`isA` checks whether an object is an instance of a class or inherits from it. Every
Godot object supports it: nodes, resources, input events, and extension classes
registered in ClassDB.

### Type narrowing

With a literal class name, the checker narrows the object's type inside the `if`, so
`event.Relative` above is known to exist.

- Misspelled class names are reported, and completion suggests class names inside
  the string.
- A string built at runtime still returns a boolean, but doesn't narrow the type.
- In the `else` branch, the checker keeps every type that could still match.

Imported script classes are checked by their declared class name, not their import
alias.

### Runtime behavior

At runtime, `isA` checks the native class, the attached script's inheritance chain,
and Luau class ancestry. Pure Luau classes that don't extend a Godot class don't get
`isA` automatically.

### Spelling

Both `isA` and `IsA` work in every project. Completion only shows the spelling that
matches your method casing, which is `is_a` in snake_case projects. This is the one
exception to the rule that each engine method has a single spelling.

## Tooling for the extended syntax

The native editor and the standalone LSP share the same parser and analyzer, so both
understand everything on this page. Typing a `match` block indents its cases and
completes the closing `end`.

### Formatting and linting

- **StyLua** formats the statements it understands and leaves statements with Ludot
  syntax byte for byte. StyLua ignore comments still work.
- **Selene** gets a copy of the file with Ludot syntax hidden and line numbers kept.
  The native checker validates the hidden parts.

Naming lint rules are separate from the engine casing settings.

### Suppressing warnings

To silence `match_exhaustiveness` or `signal_declaration`:

```luau
-- selene: allow(match_exhaustiveness)    -- for the next statement
--# selene: allow(match_exhaustiveness)   -- for the whole file, at the top
```

### Shaders

The shader subset rejects these extensions with a source diagnostic. Its numeric
`for` loop still works.
