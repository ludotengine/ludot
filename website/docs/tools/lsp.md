# Standalone language server

<p class="lead">Use Ludot checking and navigation in an editor that supports the Language Server Protocol.</p>

## Start the server

Build the [language tools](../reference/building.md#language-tools), then configure your editor to launch this command over stdio from the repository:

```bash
python tools/ludot-lsp
```

- Register the server for `.luau` and `.lushader` files.
- If your editor config lives outside the repository, use an absolute path to the script.
- Don't print anything else to stdout; it's reserved for the protocol.

To install the server without a checkout, see [Standalone LSP packaging](../reference/building.md#standalone-lsp).

### Options

| Option | Purpose |
| --- | --- |
| `--analyzer <path>` | Use a specific analyzer binary |
| `--shader-compiler <path>` | Use a specific shader compiler binary |
| `--definitions <path>` | Add a Luau declaration file on top of the engine types |

## Capabilities

- Diagnostics
- Typed completion, including module paths in `import`, `require`, and `.luaurc` aliases
- Hover and signature help
- Go to definition, find references, and rename

To turn off module path completion, set `ludot.requireAutocomplete` to `false` in your editor's settings.

One analyzer process runs for the whole session and caches types and module dependencies. Editing a module, even without saving, updates diagnostics in the files that import it.

## Engine types and casing

Engine types come from `types/engine.api`. Regenerate it after changing the native API; the [build guide](../reference/building.md#standalone-lsp) has the command.

Project settings come from two places:

- **Casing** comes from the nearest `project.godot`. Restart the server after changing it.
- **Aliases and language mode** come from `.luaurc` files.

The standalone server can't see your scenes, so `getNode` returns a general `Node`. Add a `::` assertion when you need a specific type.

## Protocol details

Only relevant if you're writing a custom client:

- The server uses **full** document sync. Sending incremental edits will corrupt its copy of the document.
- Positions are in UTF-16 code units, as the LSP spec requires. The server converts them to Luau's UTF-8 byte columns.
- Unsaved buffers take priority over files on disk.
- Requests can be cancelled.

## Troubleshooting

| Problem | Action |
| --- | --- |
| Server exits immediately | Run the command from a terminal and check that Python and built tools exist. |
| Native class names are unknown | Check the engine manifest location and build/package contents. |
| Imports do not resolve | Check alias paths relative to the `.luaurc` file and the importing module. |
| Completion uses an old casing style | Restart the language server after changing project settings. |
| A shader type error is absent | The standalone compiler checks the Luau subset; final GPU type validation happens in the engine. |
