export const examples = {
	'Enums & matching': `enum State = {
	Idle,
	Running,
	Finished,
}

const state: State = State.Running
const label = match state with
	case State.Idle then "Waiting to start"
	case State.Running then "Game in progress"
	case State.Finished then "All done"
end

print(label)
for name, value in State do
	print(name, value)
end`,
	Classes: `class Counter = {
	public Value: number = 0

	function add(amount: number)
		self.Value += amount
	end
}

const counter = Counter.new()
counter:add(5)
counter:add(3)
print("Count:", counter.Value)`,
	'Ranges & pipelines': `const function double(value: number)
	return value * 2
end

const function label(value: number)
	return "Score: " .. tostring(value)
end

for score in 1 to 3 do
	const text = score |> double() |> label()
	print(text)
end`,
	'Optional values': `const settings: { Sound: boolean?, Title: string? } = {
	Sound = false,
}

print("Sound:", settings.Sound ?? true)
print("Title:", settings.Title ?? "My game")

const player: { Name: string }? = nil
print(player?.Name ?? "No player yet")`,
}
