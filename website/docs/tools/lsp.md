# Standalone language server

<p class="lead">Use Ludot checking and navigation in an editor that supports the Language Server Protocol.</p>

## Start the server

Build the [language tools](../reference/building.md#language-tools), then configure your editor to launch this command over stdio from the repository:

```bash
python tools/ludot-lsp
```

For a configuration stored outside the checkout, use an absolute path to the script. Keep stdout reserved for the LSP protocol. Assign the server to `.luau` and `.lushader` files.

The server accepts `--analyzer`, `--shader-compiler`, and `--definitions`. Use the first two to select explicit tool binaries and the third to add a Luau declaration file. For installation without a development checkout, see [Standalone LSP packaging](../reference/building.md#standalone-lsp).

## Capabilities

The server supports diagnostics, typed completion, hover, signature help, go-to-definition, references, and rename. Module completion understands `import`, `require`, and `.luaurc` aliases. Set `ludot.requireAutocomplete` to `false` in the client's configuration to turn module path completion off.

One analyzer process stays alive for the session. It caches type information and module dependencies. An unsaved imported file can therefore update diagnostics in files that depend on it.

## Engine types and casing

The server loads `types/engine.api` into the checker. Regenerate that file after native API changes using the [build guide](../reference/building.md#standalone-lsp). `--definitions` supplements it rather than replacing engine signatures.

The nearest ancestor `project.godot` supplies API casing settings. Restart the LSP after changing method or property casing. `.luaurc` files supply module aliases and language mode. The native editor's scene-based node inference is not available in a standalone session, so a dynamic or scene-specific lookup may need a type assertion.

## Protocol details

The server uses full-document synchronization. LSP positions count UTF-16 code units; the backend converts them to Luau's UTF-8 byte columns. Unsaved buffers take priority over disk files. Requests support cancellation.

These details matter when configuring a custom client. Sending incremental edits to a server that advertises full synchronization can produce incorrect document contents.

## Troubleshooting

| Problem | Action |
| --- | --- |
| Server exits immediately | Run the command from a terminal and check that Python and built tools exist. |
| Native class names are unknown | Check the engine manifest location and build/package contents. |
| Imports do not resolve | Check alias paths relative to the `.luaurc` file and the importing module. |
| Completion uses an old casing style | Restart the language server after changing project settings. |
| A shader type error is absent | The standalone compiler checks the Luau subset; final GPU type validation happens in the engine. |
