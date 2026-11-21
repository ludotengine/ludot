# Support and limits

The engine is experimental. Breaking changes are expected.

## Platforms

| Platform | Status |
| --- | --- |
| Linux x86_64 | Desktop release target |
| macOS Apple Silicon | Desktop release target |
| Windows x86_64 | Desktop release target |
| Web | Experimental; single-threaded Vesper tested in Chromium |
| Mobile | Unsupported |

## Scripting

- Luau and GDScript run alongside each other. Luau editor tool scripts are unsupported.
- Luau shares one VM. Calls enter it one at a time; a breakpoint can block other threads waiting for Luau.
- Hot reload preserves compatible values. Running and suspended functions can finish with their old code.
- Scene-based node inference requires the native editor's active scene. The standalone LSP uses general native return types.
- The shader CLI checks the Luau subset. Final GPU type validation happens in the engine.

## Browser tools

The playground runs standalone Luau. Engine mode provides completion and checking, without running a scene or resolving project imports. The live Vesper demo runs a separate experimental engine build.

[Verification record](../reference/status.md) · [Report an issue](https://github.com/ludotengine/ludot/issues)
