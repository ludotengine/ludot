--!strict
-- Every Signal usage case. tools/test_signals_engine.py runs it and checks the log;
-- tests/test_extensions.py checks it with the strict analyzer.

const log: { string } = {}
const function note(entry: string)
	table.insert(log, entry)
end

-- 1. A module-level signal with no arguments.
const Saved = signal with ()
Saved:connect(function()
	note("saved")
end)
Saved:fire()

-- 2. Typed arguments, fired and received.
const HealthChanged = signal with (previous: number, current: number)
HealthChanged:connect(function(previous: number, current: number)
	note(`health {previous}->{current}`)
end)
HealthChanged:fire(100, 75)

-- 3. Handler parameter types are inferred from the signal, so they can be left off.
HealthChanged:connect(function(previous, current)
	note(`inferred {previous + current}`)
end)
HealthChanged:fire(10, 5)
HealthChanged:disconnectAll()

-- 4. connect returns a Connection that can be disconnected.
const Tick = signal with (frame: number)
const connection: Connection = Tick:connect(function(frame: number)
	note(`tick {frame}`)
end)
Tick:fire(1)
note(`connected {connection.Connected}`)
connection:disconnect()
note(`connected {connection.Connected}`)
Tick:fire(2)
-- Disconnecting twice is harmless.
connection:disconnect()

-- 5. A handler can disconnect itself.
local selfRemoving: Connection
selfRemoving = Tick:connect(function(frame: number)
	note(`self removing {frame}`)
	selfRemoving:disconnect()
end)
Tick:fire(3)
Tick:fire(4)

-- 6. once runs a single time.
Tick:once(function(frame: number)
	note(`once {frame}`)
end)
Tick:fire(5)
Tick:fire(6)

-- 7. wait yields until the next fire and returns its arguments.
const Loaded = signal with (name: string, size: number)
const waiter = coroutine.create(function()
	const name, size = Loaded:wait()
	note(`waited {name} {size}`)
end)
coroutine.resume(waiter)
Loaded:fire("level1", 42)

-- 8. Any number of untyped arguments.
const Debug = signal with (...any)
Debug:connect(function(...)
	note(`debug {select("#", ...)}`)
end)
Debug:fire(1, "two", true)

-- 9. Signals as parameters and return values.
const function forward<T...>(from: Signal<T...>, to: Signal<T...>): Connection
	return from:connect(function(...: T...)
		to:fire(...)
	end)
end
const Source = signal with (message: string)
const Target = signal with (message: string)
Target:connect(function(message: string)
	note(`forwarded {message}`)
end)
forward(Source, Target)
Source:fire("hi")

-- 10. The same handler connected twice runs twice, in connection order.
const function count()
	note("count")
end
Saved:connect(count)
Saved:connect(count)
Saved:fire()

-- 11. An erroring handler doesn't stop the others.
const Risky = signal with ()
Risky:connect(function()
	error("boom")
end)
Risky:connect(function()
	note("after error")
end)
Risky:fire()

-- 12. Signals owned by a class. Each instance gets its own signal.
class Player = {
	public Name: string = ""
	public Health: number = 100
	public Died = signal with ()
	public Damaged = signal with (previous: number, current: number)

	function damage(amount: number)
		const previous = self.Health
		self.Health = math.max(self.Health - amount, 0)
		self.Damaged:fire(previous, self.Health)
		if self.Health == 0 then
			self.Died:fire()
		end
	end
}

const alice = Player.new()
alice.Name = "alice"
const bob = Player.new()
bob.Name = "bob"
alice.Died:connect(function()
	note("alice died")
end)
bob.Damaged:connect(function(previous: number, current: number)
	note(`bob {previous}->{current}`)
end)
alice:damage(100)
bob:damage(30)
note(`separate {alice.Died ~= bob.Died}`)

print(table.concat(log, "\n"))
