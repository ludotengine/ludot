# Combined language feature implementation checklist

This records implementation and local verification for the combined change.
The initial implementation is delivered in one commit. Existing pushed history
stays intact; cross-platform CI remains pending until its jobs finish.

| Feature | Parser/AST and recovery | Analysis and diagnostics | Compiler/VM and ordering | Native Godot | Editor/LSP/format/lint | Regressions/docs |
| --- | --- | --- | --- | --- | --- | --- |
| Nominal numeric/string enums | implemented | implemented | implemented | implemented | implemented | locally verified |
| Match expressions/statements/guards/exhaustiveness | implemented | implemented | implemented | implemented | implemented | locally verified |
| Optional uninterrupted property/method chains | implemented | implemented | implemented | implemented | implemented | locally verified |
| Lazy nil coalescing and single-evaluation assignment | implemented | implemented | implemented | implemented | implemented | locally verified |
| Inclusive range loops and step semantics | implemented | implemented | implemented | implemented | implemented | locally verified |
| First-argument pipelines and shared left-associative precedence | implemented | implemented | implemented | implemented | implemented | locally verified |
| Runtime and typed isA refinement | implemented | implemented | implemented | implemented | implemented | locally verified |

Cross-feature checks:

- Combined positive user fixture and intentional negative fixtures.
- Side effects, false versus nil, lazy arguments, multi-return, closure captures,
  break/continue, enum declaration order and immutable members.
- Precise diagnostics and recovery in incomplete documents.
- Shader acceptance is deliberate; unsupported constructs get source diagnostics.
- Formatting/lint compatibility and ignore directives never corrupt source.
- Native and standalone completion/hover/signatures, unsaved overlays and UTF-16.
- Existing language, shader, LSP and desktop regressions with assertions enabled.
- Separate engine build, leaving the user's running executable untouched.
- One initial implementation commit/push, then supported CI monitoring and any
  necessary follow-up fix commits until the relevant checks really succeed.


Implementation notes:

- AST lowering retains source locations and construct tags. Generated closures
  forward enclosing varargs, and compiler register lifetime tracking treats their
  captures as non-lexical uses.
- Shared enum metadata and class identity helpers serve native checking and the
  standalone API loader. Native enum namespaces use `.Enums` to avoid collisions
  with engine properties; type annotations keep `Class.Enum` spelling.
- The shader subset explicitly rejects the new syntax. Source-preserving
  formatting/lint adapters protect statements containing unsupported constructs.
- Final local verification passed on 2026-11-13: all four CTest suites, including
  17 extension regressions; 5,573 upstream unit cases and 35,373 assertions;
  the separate Linux editor build; and the full headless desktop suite.
- Desktop checks include engine execution, debugger, exported packs, editor
  formatting/indentation/Inspector behavior, all nine casing combinations, and
  extension enum/identity/scene-serialization regressions. The exact generic
  filter/coalesce/sort/first fixture also passes native imported-module checking.
- API manifest and callback generation from the final editor is deterministic.
  Cross-platform CI has not yet run for the combined commit.
