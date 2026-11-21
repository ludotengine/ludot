---
source: docs/language.md
---

# Signals and connections

## Signals

`Signal` is a global modeled on Roblox's RBXScriptSignal. It's both the constructor
and the type. The type arguments list what the signal fires with:

```luau
const HealthChanged: Signal<number, number> = Signal.new()

const connection: Connection = HealthChanged:connect(function(previous, current)
	print(previous, current)
end)

HealthChanged:fire(100, 75)
print(connection.Connected) --> true
connection:disconnect()

const Saved: Signal<> = Signal.new()
const Debug: Signal<...any> = Signal.new()
```

| Member | What it does |
| --- | --- |
| `Signal.new(...names)` | Creates a signal. The optional names label its arguments for the engine. |
| `signal:connect(handler)` | Runs `handler` on every fire. Returns a `Connection`. |
| `signal:once(handler)` | Runs `handler` on the next fire only. Returns a `Connection`. |
| `signal:fire(...)` | Calls every connected handler with the arguments. |
| `signal:wait()` | Pauses the current thread until the next fire, then returns its arguments. |
| `signal:disconnectAll()` | Disconnects every handler connected through this API. |
| `connection:disconnect()` | Disconnects the handler. Calling it twice does nothing. |
| `connection.Connected` | Whether the handler is still connected. |

### Names and types

- Methods follow `luau/api/method_casing`, so they become `Fire` and `Connect` in
  PascalCase. `Connected` follows `luau/api/property_casing`.
- The method is `fire`. There is no `emit`.
- `typeof` returns `"Signal"` or `"Connection"`.
- Handler parameter types come from the signal, so you can leave them off.
- A `Signal.new()` without a type annotation accepts anything. The
  `signal_declaration` warning asks you to add a `Signal<...>` type.

### How handlers run

- Each handler runs in its own thread, so it can call `task.wait`.
- Handlers run in connection order. The list is copied when a fire starts, so a
  handler can connect or disconnect others safely.
- Connecting the same function twice makes it run twice.
- If a handler errors, the error prints with its stack trace and the remaining
  handlers still run.
- `wait` errors outside a thread that can pause, such as module top-level code.
- A handler belongs to the object whose code connected it. When that object is
  freed, its connections are dropped.

### Engine signals

Engine signals use the same type. `self.TreeExiting` is a `Signal<>`, and
`self.ChildEnteredTree` is a `Signal<Node>`, typed from the engine's argument
information.

- `connect` returns a `Connection`.
- `fire` emits the engine signal, so GDScript handlers run too.
- `disconnectAll` removes only connections made through this API. It never touches
  editor or GDScript connections.

### Signal fields

In a class that extends an engine class, a field typed `Signal<...>` also becomes a
script signal with the same name:

```luau
class Player extends CharacterBody3D = {
	public Died: Signal<> = Signal.new()
	public HealthChanged: Signal<number, number> = Signal.new("previous", "current")

	function damage(amount: number)
		const previous = self.Health
		self.Health -= amount
		self.HealthChanged:fire(previous, self.Health)
	end
}
```

These signals work everywhere a GDScript signal does:

- The Node dock lists them, and scenes save their connections.
- GDScript can call `connect("Died", ...)` and `emit_signal("HealthChanged", 1.0, 2.0)`.
- Firing from Luau and emitting from GDScript reach the same handlers.

Argument types come from the type arguments. Argument names come from the strings
passed to `Signal.new`, or default to `arg1`, `arg2`, and so on. The checker warns
when the names don't match the type arguments or aren't string literals.

Signal fields are read-only after construction, can't be `export`ed, and don't
appear in the Inspector.

In plain Luau classes that don't extend an engine class, a signal field is an
ordinary field. Each instance gets its own signal, and it can be replaced.

### Passing values

Values passed through an engine signal convert like any other engine call. Tables
and plain class objects travel as `LuauValue` references. Luau handlers receive the
same table or object. GDScript handlers receive a `LuauValue` and can call
`to_variant()` on it.
