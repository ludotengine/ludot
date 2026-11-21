--!strict
-- Signal misuse. tests/test_extensions.py expects a type error on each fire, connect,
-- wait and emit line, and a SignalDeclaration warning for the untyped parameter.

const HealthChanged = signal with (previous: number, current: number)
HealthChanged:fire("full", 75)
HealthChanged:fire(100)
HealthChanged:connect(function(previous: string, current: number) end)
const name: string = HealthChanged:wait()
HealthChanged:emit(1, 2)

class Player = {
	public Died = signal with ()
}
const p = Player.new()
p.Died:fire(5)

const Untyped = signal with (value)
Untyped:fire(1)
Untyped:fire("anything")
