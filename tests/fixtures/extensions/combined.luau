--!strict
enum Msg = {
	Join,
	Chat,
	Leave = 10,
	Other,
}
enum Text = { Join = "join", Chat = "chat" }
const order = {}
for name, value in Msg do
	table.insert(order, name .. ":" .. value)
end
assert(table.concat(order, ",") == "Join:1,Chat:2,Leave:10,Other:11")
assert(Msg.Join == 1 and Msg.Other == 11 and Text.Join == "join")
assert(Msg.from(10) == Msg.Leave and Msg.from(99) == nil)
assert(not pcall(function() Msg.Join = 7 end))
local calls = 0
const function subject(): Msg
	calls += 1
	return Msg.Chat
end
const who = match subject() with
	case Msg.Join then "join"
	case Msg.Chat then "chat"
	case Msg.Leave then "leave"
	case Msg.Other then "other"
end
assert(who == "chat" and calls == 1)
local health = 20
match health with
	case 0 then error("dead")
	case if health < 25 then health = 50
	case if health < 75 then health = 1
	otherwise error("bad")
end
assert(health == 50)
const function parent(node: { Name: string }?): string
	return node?.Name ?? "missing"
end
assert(parent(nil) == "missing" and parent({ Name = "parent" }) == "parent")
assert((false ?? true) == false)
local enabled: boolean? = nil
enabled ??= false
const concrete: boolean = enabled
assert(concrete == false)
const function add(value: number, amount: number): number
	return value + amount
end
const result: number = 1 |> add(2) ?? 0 |> add(4)
assert(result == 7)
local sum = 0
for i in 1 to 10 step 5 do
	sum += i
end
assert(sum == 7)
print("COMBINED_FEATURES_PASSED")
