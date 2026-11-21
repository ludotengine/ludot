---
source: docs/language.md
---

# Modules and imports

## Imports

```luau
import add from "@helpers/mathutils"
import add, subtract from "@helpers/mathutils"
import (add as plus), subtract from "@helpers/mathutils"
import "@pkgs/something" as something
import type Person from "@helpers/personutils"
import type (Person as Guy), Female from "@helpers/personutils"
import type "@helpers/personutils" as utils
import "@helpers/personutils" as personutils
import "@helpers/setup"
```

| Form | What you get |
| --- | --- |
| `import a, b from "path"` | Named values, as immutable locals |
| `import (a as b) from "path"` | A named value under a different local name |
| `import "path" as name` | The whole module, including its exported types |
| `import type A from "path"` | Types only, with no runtime load |
| `import type "path" as name` | A type-only namespace, which can't be read as a value |
| `import "path"` | Runs the module for its side effects |

Each import statement loads its module once.

A module exports types with `export type`. It exports values by returning a table or
with Luau's value export syntax.

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
