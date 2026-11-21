# Ludot Luau

<p class="lead">Luau with native classes, imports, enums, pattern matching, and a direct connection to the engine.</p>

Ludot's language fork derives from Luau 0.743. Its parser, analyzer, compiler, and VM implement the extensions directly. There is no source rewriting or transpiler between your code and the compiler.

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

Each instance gets its own field defaults. Methods receive `self` implicitly. A `const` binding cannot be reassigned, but the object or table it points to can still change. Use `const function` for functions and `local` when a binding must change.

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

Unions, intersections, generics, type packs, refinements, and type functions remain available. Annotate function parameters and public contracts. Inferred local variables do not need redundant types.

```lua
const function displayName(name: string?)
	return name ?? "Guest"
end

const names = { "Ada", "Lin" }
for index, name in names do
	print(index, displayName(name))
end
```

Keep `false` distinct from `nil`. `value ?? fallback` uses the fallback only for nil, while `value or fallback` also replaces false.

## Try a class

<Playground example="Classes" />

The [playground](../playground.md) runs standalone code. Engine classes, scene lifecycle, tasks, and signals need the desktop runtime.
