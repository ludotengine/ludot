# Ludot Luau

<p class="lead">Luau with native classes, imports, enums, pattern matching, and a direct connection to the engine.</p>

Ludot's language is a fork of Luau 0.743. Its parser, analyzer, compiler, and VM implement every extension directly, so there's no transpiler between your code and the compiler.

## A small example

```lua
class Counter = {
	public Value: number = 0

	function add(amount: number)
		self.Value += amount
	end
}

const counter = Counter.new()
counter:add(3)
print(counter.Value)
```

- Each instance gets its own copy of the field defaults.
- Methods receive `self` automatically.
- A `const` binding can't be reassigned, but the table or object it points to can still change. Use `local` when a variable needs to change, and `const function` for functions.

## Language map

| Topic | What to learn |
| --- | --- |
| [Classes and properties](./classes.md) | Brace classes, constructors, visibility, exports, onready initialization |
| [Modules and imports](./imports.md) | Named and namespace imports, type imports, aliases, module resolution |
| [Enums and pattern matching](./enums.md) | Nominal enum types, conversion, iteration, exhaustive matching |
| [Operators and control flow](./operators.md) | Optional access, nil coalescing, inclusive ranges, pipelines, identity checks |
| [Standard library](../api/luau.md) | Global functions and Luau libraries with signatures |
| [Engine scripting](../engine/) | Scene scripts, lifecycle, native APIs, ownership |

## Types and existing Luau

Everything from standard Luau still works: unions, intersections, generics, type packs, refinements, and type functions. Annotate function parameters. Local variables can usually rely on inference.

```lua
const function displayName(name: string?)
	return name ?? "Guest"
end

const names = { "Ada", "Lin" }
for index, name in names do
	print(index, displayName(name))
end
```

`value ?? fallback` only uses the fallback for `nil`. `value or fallback` also replaces `false`, which is usually not what you want for settings.

## Try a class

<Playground example="Classes" />

The [playground](../playground.md) runs plain Luau in the browser. Engine classes, scenes, tasks, and signals need the desktop engine.
