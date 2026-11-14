--!strict
-- Signal misuse. tests/test_extensions.py expects a type error on each fire, connect,
-- wait and emit line, and a SignalDeclaration warning for the unannotated signal.

const HealthChanged: Signal<number, number> = Signal.new()
HealthChanged:fire("full", 75)
HealthChanged:fire(100)
HealthChanged:connect(function(previous: string, current: number) end)
const name: string = HealthChanged:wait()
HealthChanged:emit(1, 2)

class Player = {
	public Died: Signal<> = Signal.new()
}
const p = Player.new()
p.Died:fire(5)

const Untyped = Signal.new()
Untyped:fire(1)
Untyped:fire("anything")
