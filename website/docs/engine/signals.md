---
source: docs/language.md
---

# Signals and connections

## Signals

`Signal` is a global, modeled on Roblox's RBXScriptSignal. It is both the
constructor table and the type. The type's arguments are what the signal fires:

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

| Member | Behavior |
| --- | --- |
| `Signal.new(...names)` | Creates a signal. The optional names label its arguments for the engine. |
| `signal:connect(handler)` | Runs `handler` on every fire and returns a `Connection`. |
| `signal:once(handler)` | Runs `handler` on the next fire only and returns a `Connection`. |
| `signal:fire(...)` | Calls every connected handler with the arguments. |
| `signal:wait()` | Yields the current thread until the next fire and returns its arguments. |
| `signal:disconnectAll()` | Disconnects every handler connected through this API. |
| `connection:disconnect()` | Disconnects the handler. Disconnecting twice does nothing. |
| `connection.Connected` | Whether the handler is still connected. |

Method names follow `luau/api/method_casing` (`Fire`, `Connect` in PascalCase) and
`Connected` follows `luau/api/property_casing`. There is no `emit`. `typeof` returns
`"Signal"` and `"Connection"`. Handler parameter types are inferred from the signal,
so they can be left off. Without an annotation, `Signal.new()` fires anything, and
the `signal_declaration` warning asks for a `Signal<...>` type.

Each handler runs in its own resumable thread, so it can call `task.wait`. Handlers
run in connection order, from a snapshot taken when the fire starts, so a handler can
connect or disconnect others while it runs. The same function connected twice runs
twice. A failing handler is printed with its stack trace and the remaining handlers
still run. `wait` errors outside a yieldable thread, such as module top-level code. A
handler belongs to the object whose code connected it; when that object is freed, its
connections are dropped.

### Engine signals

Engine signals use the same type. `self.TreeExiting` is a `Signal<>`, and
`self.ChildEnteredTree` is a `Signal<Node>`, typed from the engine's argument
information. `connect` returns a `Connection`, and `fire` emits the engine signal, so
GDScript handlers run too. `disconnectAll` on an engine signal removes only the
connections made through this API, never editor or GDScript connections.

### Signal fields

On a class that extends an engine class, a field declared as `Signal<...>` is also a
script signal, named exactly as the field:

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

The Node dock lists these signals, scenes save their connections, and GDScript can
`connect("Died", ...)` and `emit_signal("HealthChanged", 1.0, 2.0)`. Firing from Luau
and emitting from GDScript reach the same handlers. Argument types come from the type
arguments and names from the string literals passed to `Signal.new`, or `arg1`,
`arg2`, and so on. The checker warns when the names don't match the type arguments or
aren't string literals.

These fields are read-only after construction, cannot be `export`ed, and are hidden
from the inspector property list. Plain Luau classes keep ordinary fields: each
instance gets its own signal, and it can be replaced.

Values cross an engine signal as engine values, like any engine call. Tables and plain
class objects travel as `LuauValue` references, so Luau handlers receive the same table
or object and GDScript handlers receive a `LuauValue` with `to_variant()`.
