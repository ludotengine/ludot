# Contributing

Ludot commits its Godot and Luau forks directly. Edit these source trees in place. Do not turn them into submodules or add automatic upstream updates. `upstreams.json` records the original versions and commits.

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

Language syntax belongs in the parser and compiler. Do not implement new language behavior with regular-expression preprocessing. GDScript and internal Godot API names stay compatible; Luau casing belongs in the binding layer.

## Verify a behavior change

Use the commands in [Build from source](../reference/building.md). Run the relevant language, shader, and LSP regressions. Native integration changes also need an engine build and the headless engine regression. Keep AST and compiler assertions enabled in optimized language tools.

LSP changes must cover unsaved document overlays and UTF-16/UTF-8 positions. Shader changes must reject unsupported AST nodes with source diagnostics. The engine performs final generated-shader type checking separately.

Update `docs/status.md` when a feature becomes usable or a known limit changes. Record what actually ran. A configured or queued CI job is not a verified result.

## Edit these docs

Run `npm ci` in `website/`, then `npm run dev`. The landing page is at `/ludot/` and the manual at `/ludot/docs/` with the default base. See [the site README](https://github.com/ludotengine/ludot/blob/main/website/README.md) for runtime builds and GitHub Pages setup.

Edit existing language, shader, build, status, and theme manuals in `docs/`. The generator publishes those sources into the site and splits the language guide into topic pages. Edit new site-only guides in `website/docs/`. Engine class pages come from `types/engine.api` and class XML, so do not edit generated pages.

Preserve copyright notices, dependency licenses, and logo attribution.
