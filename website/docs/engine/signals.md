---
source: docs/language.md
---

# Signals and connections

## Signals

Signals are created with `signal with (...)`, modeled on Roblox's RBXScriptSignal.
The parameter list names and types the arguments the signal fires with, and its
type is `Signal<T...>`:

```luau
const HealthChanged = signal with (previous: number, current: number)

const connection: Connection = HealthChanged:connect(function(previous, current)
	print(previous, current)
end)

HealthChanged:fire(100, 75)
print(connection.Connected) --> true
connection:disconnect()

const Saved = signal with ()
const Debug = signal with (...any)
```

| Form / Member | What it does |
| --- | --- |
| `signal with (param: Type, ...)` | Creates a signal. Parameter names also label its arguments for the engine. |
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
- `Signal` is a type only (`Signal<number, number>`, `Signal<>`, `Signal<...any>`).
  `typeof` returns `"Signal"` or `"Connection"`.
- Handler parameter types come from the signal, so you can leave them off.
- A signal parameter without a type annotation, such as `signal with (value)`,
  defaults to `any` and triggers the `signal_declaration` warning.

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

In a class that extends an engine class, a `public` field initialized with
`signal with (...)` also becomes a script signal with the same name:

```luau
class Player extends CharacterBody3D = {
	public Died = signal with ()
	public HealthChanged = signal with (previous: number, current: number)
	private InternalTick = signal with (dt: number)

	function damage(amount: number)
		const previous = self.Health
		self.Health -= amount
		self.HealthChanged:fire(previous, self.Health)
	end
}
```

These signals work everywhere a GDScript signal does:

- The Node dock lists them with their parameter names and types, and scenes save
  their connections.
- GDScript can call `connect("Died", ...)` and `emit_signal("HealthChanged", 1.0, 2.0)`.
- Firing from Luau and emitting from GDScript reach the same handlers.

`private` signal fields stay internal to the declaring class: they are not
registered as engine signals and cannot be accessed from other classes.

Signal fields are read-only, cannot be `export`ed or `onready`, and do not appear
in the Inspector.

### Passing values

Values passed through an engine signal convert like any other engine call. Tables
and plain class objects travel as `LuauValue` references. Luau handlers receive the
same table or object. GDScript handlers receive a `LuauValue` and can call
`to_variant()` on it.
