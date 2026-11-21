---
source: docs/language.md
---

# Enums and pattern matching

## Enums

Enums declare immutable numeric or string values and a distinct nominal type.
Numeric members start at 1. An explicit integer resets the sequence for subsequent
implicit members. Integer values must fit exactly in Luau's number representation.
String enums require an explicit string for every member. Mixed numeric/string
enums are errors.

```luau
enum Action = {
	Join,
	Chat,
	Leave = 10,
}
enum Message = {
	Join = "join",
	Chat = "chat",
}
const action: Action = Action.Chat
const checked: Action? = Action.from(2)
```

`Action.from(value)` checks membership and returns an enum value or nil. Assigning
an arbitrary number/string to an enum, or assigning one enum to another enum, is a
type error. Enum values remain ordinary numbers/strings at runtime, including when
passed to Godot. Equality with the underlying number/string is permitted. Member
aliases with equal underlying values compare equal and represent the same case
for matching. The name `from` is reserved for checked conversion.

Use `export enum` with the existing module import forms. Enum types can annotate
class fields, including exported Inspector fields. Numeric and string exports use
dropdowns with member names; scene files store the underlying values. String
values may contain commas or colons. Inspector dropdowns keep the names and values
separate rather than parsing the display text.

Direct iteration yields members in declaration order, including aliases. It does
not expose the `from` method:

```luau
for name, value in Action do
	print(name, value)
end
```

### Engine enums

Every engine enum lives under one global, `Enum`, modeled on Roblox's. Each enum is
an `Enum` object, and each value is an `EnumItem`:

```luau
Input:setMouseMode(Enum.MouseMode.Captured)
print(Enum.MouseMode.Captured) -- Enum.MouseMode.Captured
print(Enum.MouseMode.Captured.Name) -- Captured
print(Enum.MouseMode.Captured.Value) -- 2
print(Enum.MouseMode.Captured.EnumType) -- Enum.MouseMode

print(Enum.MouseMode:fromName("Captured")) -- Enum.MouseMode.Captured, or nil
print(Enum.MouseMode:fromValue(2)) -- Enum.MouseMode.Captured, or nil
print(#Enum.MouseMode:getEnumItems()) -- 6
print(#Enum:getEnums()) -- every engine enum

for name, item in Enum.MouseMode do
	print(name, item.Value)
end
```

`typeof` returns `"EnumItem"` and `"Enum"`. Each item is a single shared value, so
`==` compares identity, and `Enum.MouseMode.Captured.EnumType.Captured` is the same
item. Indexing a missing enum or item is an error. User `enum` declarations are not
part of `Enum`.

Item names drop the words shared by every constant of the enum and use PascalCase:
`MOUSE_MODE_CONFINED_HIDDEN` becomes `ConfinedHidden`. A name that would start with a
digit keeps the last shared word, so `KEY_0` is `Enum.Key.Key0`. An enum keeps its
Godot name when the name is unique, such as `Enum.Key`, `Enum.MouseButton`, and
`Enum.LayoutPreset`. Same-named enums with the same items are one enum:
`Input.MouseMode` and `DisplayServer.MouseMode` are both `Enum.MouseMode`, and
`Control`, `Input`, and `DisplayServer` share `Enum.CursorShape`. Different enums with
the same name take their class as a prefix, such as `Enum.Light3DParam`,
`Enum.NodeProcessMode`, and `Enum.VariantType`. Editor-only enums are left out, so
editor and exported builds see the same names.

Engine methods and properties that take an enum accept an `EnumItem` or its number.
Those that return a non-bitfield enum return the `EnumItem`, such as
`Input:getMouseMode()` and `node.ProcessMode`. A value with no matching item, such as a
key code with modifier bits, stays a number. Bitfield enums, such as
`Enum.MouseButtonMask`, return numbers, because their values combine. Use `.Value`
with `bit32` to combine items.

Annotate engine enum types as `Enum.<Name>`, such as `Enum.MouseMode`. `EnumItem`
and `Enum` are the general types. Engine methods, properties, `fromName`, and
`getEnumItems` are typed with the specific enum, and matches over engine enums check
exhaustiveness. An `export` field typed `Enum.<Name>` shows a dropdown and stores the
number in scenes.

Class enum constants keep Godot's original names, such as
`Input.MOUSE_MODE_CAPTURED`. Non-bitfield ones are the matching `EnumItem`, so
`Input.MOUSE_MODE_CAPTURED == Enum.MouseMode.Captured`. Bitfield constants, such as
`Control.SIZE_EXPAND_FILL`, and builtin value type constants, such as
`Vector3.AXIS_Z`, stay numbers. The class enum namespaces (`Input.Enums.MouseMode`),
the global enum tables (`Key.KEY_W`, `MouseButton`, `Variant.Type`), and the
`Input.MouseMode` style annotations are replaced by `Enum`. Method names follow the
project method casing and `Name`, `Value`, and `EnumType` follow the property
casing. Enum and item names do not change with casing. Native bitfield
parameters/properties continue to accept numeric masks.

## Match expressions and statements

A match evaluates its subject once and tests arms in source order. Only the first
matching arm runs. There is no fallthrough.

```luau
const text = match action with
	case Action.Join then "joined"
	case Action.Chat then "chatting"
	case Action.Leave then "left"
end

match health with
	case 0 then
		print("Dead")
	case if health < 25 then
		print("Critical health")
	otherwise
		print("Alive")
end
```

Expression arms contain one expression and produce one value. Their types form a
union. Value cases accept literals and enum members. Guards use `case if` and do
not establish exhaustive coverage. Expressions require exhaustive coverage or
`otherwise`. The diagnostic lists missing enum members. Statements report a
`match_exhaustiveness` warning when cases are missing. Duplicate values, invalid
case domains, and arms after `otherwise` are diagnosed. Equal-valued aliases count
as the same case. A guard that always evaluates true makes subsequent arms
unreachable, but still does not replace explicit exhaustive coverage.

Match statements keep ordinary `return`, `break`, and `continue` behavior in the
enclosing function/loop. Match expressions have their own expression boundary.
`case` and `otherwise` mark arms at statement boundaries inside a match; they are
contextual names elsewhere. An ordinary function named `match` can still be
called with parentheses.
