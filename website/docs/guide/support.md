# Support and limits

Ludot is experimental. Expect breaking changes.

## Platforms

| Platform | Status |
| --- | --- |
| Linux x86_64 | Desktop release target |
| macOS Apple Silicon | Desktop release target |
| Windows x86_64 | Desktop release target |
| Web | Experimental; single-threaded Vesper tested in Chromium |
| Mobile | Unsupported |

## Scripting

- Luau and GDScript can be used in the same project. Luau editor tool scripts aren't supported.
- All Luau code shares one VM, and calls enter it one at a time. A breakpoint can block other threads waiting for Luau.
- Hot reload keeps compatible field values. Functions that are running or paused finish with their old code.
- Node types inferred from the scene need the native editor with that scene open. The standalone LSP falls back to general types such as `Node`.
- The standalone shader tool checks the Luau subset only. GPU type checking happens in the engine.

## Browser tools

- The playground runs plain Luau.
- Its engine mode adds completion and type checking for engine APIs, but can't run scenes or resolve project imports.
- The Vesper demo runs on a separate, experimental web build of the engine.

[Verification record](../reference/status.md) · [Report an issue](https://github.com/ludotengine/ludot/issues)
