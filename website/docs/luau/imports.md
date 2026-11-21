---
source: docs/language.md
---

# Modules and imports

## Imports

Every import starts with `import` and a module path. What comes after the path
decides what you get.

```luau
import "@helpers/setup"
import "@helpers/mathutils" as math
import "@helpers/mathutils".{ add, subtract }
import "@helpers/mathutils".{ add as plus, subtract }
import "@helpers/personutils".{ type Person, createPerson }
import "@helpers/personutils" as people.{ type Person }
import type "@helpers/personutils" as utils
```

| Form | What you get |
| --- | --- |
| `import "path"` | Runs the module for its side effects |
| `import "path" as name` | The whole module, including its exported types |
| `import "path".{ a, b }` | Named values, as immutable locals |
| `import "path".{ a as b }` | A named value under a different local name |
| `import "path".{ type A }` | A named type |
| `import "path" as name.{ a }` | The whole module and named items in one statement |
| `import type "path" as name` | A type-only namespace, which can't be read as a value |

Each import statement loads its module once.

A module exports types with `export type`. It exports values by returning a table or
with Luau's value export syntax.

### Named items

The braces list the exports you want, much like `use module::{a, b}` in Rust. The
path comes first, so the editor can complete item names from the module's exports.

- `as` renames the name to its left, whether that's an item or the module.
- Mark types with `type`, as in TypeScript's `import { type A }`. Values and types
  can share one list. If every item is a type and there's no namespace, the module
  isn't loaded at runtime.
- A long list can span several lines and end with a trailing comma:

```luau
import "@pkgs/ui".{
	Button,
	type ButtonProps,
	Label as Text,
}
```

`type` still works as an ordinary name. `.{ type }` and `.{ type as kind }` import a
value called `type`. `type` followed by another name marks a type.

### Namespaces

`as name` binds the whole module to one local, like `import module as name` in
Python. Exported types are available as `name.Type`.

`import type "path" as name` binds a namespace for types only. The module isn't
loaded at runtime, and reading `name` as a value is an error. A type-only import
can't take an item list. Mark the items with `type` instead.

### Aliases

Define aliases in a `.luaurc` file at the project root:

```json
{
	"languageMode": "strict",
	"aliases": {
		"helpers": "./src/helpers",
		"pkgs": "./packages"
	}
}
```

The engine also reads a `luau/modules/aliases` dictionary from Project Settings.
Both `import` and the older `require()` use these aliases.

### Module resolution

- A path resolves to a `.luau` file, or to `init.luau` inside a directory.
- The engine and the CLI tools read every `.luaurc` from the importing file's
  directory up through its ancestors. Nearer files override outer ones.
- The engine applies the Project Settings aliases first.
