# Contributing

Ludot's Godot and Luau forks are committed directly in the repository. Edit them in place.

- Don't turn them into submodules.
- Don't add automatic upstream updates.

`upstreams.json` records the original versions and commits.

## Find the owning component

| Directory | Responsibility |
| --- | --- |
| `engine/modules/ludot/` | Native scripting, editor integration, signals, tasks, binding behavior |
| `luau/` | Parser, analyzer, compiler, bytecode, VM, native language extensions |
| `language/` | Shared shader subset compiler |
| `tools/ludot_lsp/` | Standalone LSP protocol and document overlays |
| `types/` | Engine API manifest and declarations |
| `website/` | VitePress site, source generation, browser runtime, deployment |
| `mcp/` | Editor MCP server and addon |

- New syntax belongs in the parser and compiler. Don't implement language features with regex preprocessing.
- GDScript and Godot's internal API names stay unchanged. Luau casing is applied in the binding layer.

## Verify a behavior change

Use the commands in [Build from source](../reference/building.md), and run the tests for the area you changed:

| Change | Tests |
| --- | --- |
| Language | Language regressions |
| Shaders | Shader regressions. Unsupported syntax must fail with a source diagnostic. |
| LSP | LSP regressions, including unsaved documents and UTF-16/UTF-8 positions |
| Native integration | An engine build and the headless engine regression |

Keep AST and compiler assertions on, even in optimized builds of the language tools.

Update `docs/status.md` when a feature becomes usable or a known limit changes. Record only what actually ran: a configured or queued CI job isn't a verified result.

## Edit these docs

Run `npm ci` in `website/`, then `npm run dev`. The landing page is at `/ludot/` and the manual at `/ludot/docs/`. The [site README](https://github.com/ludotengine/ludot/blob/main/website/README.md) covers runtime builds and GitHub Pages.

Where to edit:

| Pages | Source |
| --- | --- |
| Luau language, tasks, signals, naming, hot reload | `docs/language.md`, split into topic pages by the generator |
| Shaders, building, status, themes | `docs/*.md` |
| Other guides | `website/docs/` |
| Engine class reference | Generated from `types/engine.api` and the engine's class XML. Don't edit the output. |

## Licensing

Keep copyright notices, dependency licenses, and logo attribution intact.
