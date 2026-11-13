--!strict
type Enemy = { Name: string, Alive: boolean, Distance: number }
const function filter<T>(values: { T }, predicate: (T) -> boolean): { T }?
	const result: { T } = {}
	for _, value in values do
		if predicate(value) then table.insert(result, value) end
	end
	return if #result > 0 then result else nil
end
const function sort<T>(values: { T }, predicate: (T, T) -> boolean): { T }
	table.sort(values, predicate)
	return values
end
const function first<T>(values: { T }): T?
	return values[1]
end
const function isAlive(enemy: Enemy): boolean
	return enemy.Alive
end
const function byDistance(a: Enemy, b: Enemy): boolean
	return a.Distance < b.Distance
end
const enemies: { Enemy } = {
	{ Name = "far", Alive = true, Distance = 10 },
	{ Name = "dead", Alive = false, Distance = 1 },
	{ Name = "near", Alive = true, Distance = 2 },
}
const result: Enemy? = enemies
	|> filter(isAlive) ?? {}
	|> sort(byDistance)
	|> first()
assert(result ~= nil and result.Name == "near")
return result
