---
source: docs/language.md
---

# Enums and pattern matching

## Enums

An enum declares a fixed set of named values and gives them their own type.

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

### Values

- Numeric members start at 1 and count up.
- An explicit integer, like `Leave = 10`, restarts the count for the members after
  it. Integers must fit exactly in a Luau number.
- In a string enum, every member needs an explicit string.
- An enum can't mix numbers and strings.

### Type safety

An enum is a distinct type. These are type errors:

- assigning a plain number or string to an enum variable
- assigning one enum type to another

To turn an unchecked value into an enum, use `Action.from(value)`. It returns the
matching member or `nil`. Because of this, `from` is reserved and can't be a member
name.

At runtime, members are still plain numbers or strings, including when they're
passed to Godot. Comparing a member with its underlying value using `==` is allowed.
Two members with the same value are aliases. They compare equal and count as the
same case in a `match`.

### Exporting and the Inspector

- Use `export enum` to share an enum, then import it like any other module export.
- Enum types can annotate class fields, including `export` fields.
- Exported enum fields show a dropdown of member names. Scenes store the underlying
  values.
- String values can contain commas or colons. The dropdown keeps names and values
  separate instead of parsing display text.

### Iteration

Looping over an enum yields its members in declaration order, aliases included. The
`from` function isn't part of the loop:

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

print(Enum.MouseMode.Captured)          -- Enum.MouseMode.Captured
print(Enum.MouseMode.Captured.Name)     -- Captured
print(Enum.MouseMode.Captured.Value)    -- 2
print(Enum.MouseMode.Captured.EnumType) -- Enum.MouseMode

print(Enum.MouseMode:fromName("Captured")) -- Enum.MouseMode.Captured, or nil
print(Enum.MouseMode:fromValue(2))         -- Enum.MouseMode.Captured, or nil
print(#Enum.MouseMode:getEnumItems())      -- 6
print(#Enum:getEnums())                    -- every engine enum

for name, item in Enum.MouseMode do
	print(name, item.Value)
end
```

- `typeof` returns `"EnumItem"` or `"Enum"`.
- Each item is a single shared value, so `==` compares identity.
  `Enum.MouseMode.Captured.EnumType.Captured` is the same item.
- Indexing a missing enum or item is an error.
- Your own `enum` declarations are separate and don't appear under `Enum`.

#### How names are built

Item names drop the words every constant in the enum shares, then use PascalCase.
`MOUSE_MODE_CONFINED_HIDDEN` becomes `ConfinedHidden`. A name that would start with
a digit keeps the last shared word, so `KEY_0` becomes `Enum.Key.Key0`.

Enum names follow these rules:

| Case | Result | Examples |
| --- | --- | --- |
| The Godot name is unique | Kept as is | `Enum.Key`, `Enum.MouseButton`, `Enum.LayoutPreset` |
| Same name, same items | Merged into one enum | `Input.MouseMode` and `DisplayServer.MouseMode` are both `Enum.MouseMode` |
| Same name, different items | Prefixed with the class | `Enum.Light3DParam`, `Enum.NodeProcessMode`, `Enum.VariantType` |

`Control`, `Input`, and `DisplayServer` all share `Enum.CursorShape`. Editor-only
enums are left out, so the editor and exported games see the same names.

#### Passing and returning enums

- Engine methods and properties that take an enum accept an `EnumItem` or its number.
- Those that return an ordinary enum return the `EnumItem`, for example
  `Input:getMouseMode()` and `node.ProcessMode`.
- A value with no matching item, such as a key code with modifier bits, stays a
  number.
- Bitfield enums such as `Enum.MouseButtonMask` return numbers, because their values
  combine. Use `.Value` with `bit32` to combine items. Bitfield parameters and
  properties still accept numeric masks.

#### Types

Annotate engine enums as `Enum.<Name>`, such as `Enum.MouseMode`. `EnumItem` and
`Enum` are the general types.

Engine methods, properties, `fromName`, and `getEnumItems` are typed with the
specific enum, and a `match` over an engine enum checks that every case is covered.
An `export` field typed `Enum.<Name>` shows a dropdown and stores the number in the
scene.

#### Class constants

Class constants keep Godot's names, such as `Input.MOUSE_MODE_CAPTURED`.

- Non-bitfield constants are the matching `EnumItem`, so
  `Input.MOUSE_MODE_CAPTURED == Enum.MouseMode.Captured`.
- Bitfield constants, such as `Control.SIZE_EXPAND_FILL`, stay numbers.
- Value type constants, such as `Vector3.AXIS_Z`, stay numbers.

#### Casing

Method names (`fromName`, `getEnumItems`) follow the project's method casing.
`Name`, `Value`, and `EnumType` follow the property casing. Enum and item names
never change with casing.

#### Replaced forms

`Enum` replaces these older forms, which no longer exist:

- class enum namespaces, such as `Input.Enums.MouseMode`
- global enum tables, such as `Key.KEY_W`, `MouseButton`, and `Variant.Type`
- annotations in the `Input.MouseMode` style

## Match expressions and statements

A `match` compares one value against a list of cases, top to bottom. Only the first
matching case runs, and there is no fallthrough.

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

The value after `match` is evaluated once.

### Cases

| Case | Matches when |
| --- | --- |
| `case 0`, `case "idle"`, `case Action.Join` | The value equals this literal or enum member |
| `case if condition` | The condition is true (a guard) |
| `otherwise` | Nothing above matched |

### Expressions and statements

**Match expressions** produce a value. Each case holds one expression, and the
result type is the union of all of them. An expression must cover every case or end
with `otherwise`. If it doesn't, the error lists the missing enum members.

**Match statements** run code. Missing cases produce a `match_exhaustiveness`
warning instead of an error. `return`, `break`, and `continue` inside a statement
act on the enclosing function or loop as usual. A match expression is its own
boundary.

### Checks

The checker reports:

- duplicate values
- values that can't match the subject's type
- cases written after `otherwise`

Aliases with the same value count as one case. Guards never count toward complete
coverage, even a guard that is always true. Such a guard does make every later case
unreachable.

### Reserved words

`case` and `otherwise` are only keywords at the start of a case inside a match.
Elsewhere they are ordinary names. A function named `match` can still be called with
parentheses.
