# Playground

<Playground />

## Editor controls

| Action | Control |
| --- | --- |
| Complete a name or member | Ctrl+Space, or type after `.` or `:` |
| Inspect a type or function signature | Hover the name |
| Run | Ctrl+Enter or Command+Enter |
| Indent | Tab |
| Leave the editor | Escape, then Tab |

Errors appear inline. Reset restores the selected example; Stop terminates execution.

## Environments

**Standalone Luau** runs this fork's compiler and VM. It supports Ludot classes, enums, matching, ranges, pipelines, and standard libraries.

**Engine API · check only** adds native types, methods, signals, tasks, and enums to completion and checking. It does not execute a scene. Both modes lack project file imports.

Code stays in the browser and is not saved between visits. Runs stop after 10 seconds; WebAssembly memory is limited to 256 MiB, source to 50,000 characters, and output to 16,000 characters.
