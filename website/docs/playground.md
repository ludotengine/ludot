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

Errors show up inline. **Reset** restores the selected example, and **Stop** ends a running program.

## Environments

| Mode | What it does |
| --- | --- |
| **Standalone Luau** | Runs your code with Ludot's compiler and VM. Classes, enums, `match`, ranges, pipelines, and the standard library all work. |
| **Engine API · check only** | Adds engine types, methods, signals, tasks, and enums to completion and type checking. It can't run scenes. |

Neither mode can import other files.

## Limits

Your code stays in the browser and isn't saved between visits.

| Limit | Value |
| --- | --- |
| Run time | 10 seconds |
| Memory | 256 MiB |
| Source size | 50,000 characters |
| Output | 16,000 characters |
