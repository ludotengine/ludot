---
source: luau/Analysis/src/EmbeddedBuiltinDefinitions.cpp
---

# Luau standard library

The language keeps Luau's standard libraries. These declarations come directly from the committed analyzer and describe argument and return types. `T?` accepts nil, `...` is variadic, and generic type packs preserve multiple return values.

Use direct table iteration, `for key, value in values do`, for ordinary tables. The browser playground exposes VM libraries; engine-specific `task`, `Signal`, `Connection`, and `Enum` are documented separately.

## Common operations

| Task | API |
| --- | --- |
| Print a value | `print(value)` |
| Convert text to a number | `tonumber(text)` returns a number or nil |
| Catch an error | `pcall(callback, ...)` returns success followed by results or an error |
| Clamp a value | `math.clamp(value, minimum, maximum)` |
| Build a sequence | `table.insert(values, value)` |
| Join text | `table.concat(values, separator)` |
| Format text | `string.format(format, ...)` |
| Work with UTF-8 | `utf8.len(text)`, `utf8.codes(text)` |

The Luau `vector` library is distinct from Godot's `Vector2` and `Vector3` types. The `os` library is sandboxed and does not provide process execution or file access. Coroutines alone do not schedule engine frames; use [tasks](../engine/tasks.md) in the engine.

## Global functions

```luau
@checked declare function require(target: any): any

@checked declare function getfenv(target: any): { [string]: any }

declare _G: any
declare _VERSION: string

declare function gcinfo(): number

declare function print<T...>(...: T...)

declare function type<T>(value: T): string
declare function typeof<T>(value: T): string

-- `assert` has a magic function attached that will give more detailed type information
declare function assert<T>(value: T, errorMessage: string?): T
declare function error<T>(message: T, level: number?): never

declare function tostring<T>(value: T): string
declare function tonumber<T>(value: T, radix: number?): number?

declare function rawequal<T1, T2>(a: T1, b: T2): boolean
declare function rawget<K, V>(tab: {[K]: V}, k: K): V?
declare function rawset<K, V>(tab: {[K]: V}, k: K, v: V): {[K]: V}
declare function rawlen<K, V>(obj: {[K]: V} | string): number

declare function setfenv<T..., R...>(target: number | (T...) -> R..., env: {[string]: any}): ((T...) -> R...)?

declare function ipairs<V>(tab: {V}): (({V}, number) -> (number?, V), {V}, number)

declare function pcall<A..., R...>(f: (A...) -> R..., ...: A...): (boolean, R...)

-- FIXME: The actual type of `xpcall` is:
-- <E, A..., R1..., R2...>(f: (A...) -> R1..., err: (E) -> R2..., A...) -> (true, R1...) | (false, R2...)
-- Since we can't represent the return value, we use (boolean, R1...).
declare function xpcall<E, A..., R1..., R2...>(f: (A...) -> R1..., err: (E) -> R2..., ...: A...): (boolean, R1...)

-- `select` has a magic function attached to provide more detailed type information
declare function select<A...>(i: string | number, ...: A...): ...any

@checked declare function newproxy(mt: boolean?): any

-- Cannot use `typeof` here because it will produce a polytype when we expect a monotype.
declare function unpack<V>(tab: {V}, i: number?, j: number?): ...V
```

## bit32

```luau
declare bit32: {
	band: @checked (...number) -> number,
	bor: @checked (...number) -> number,
	bxor: @checked (...number) -> number,
	btest: @checked (number, ...number) -> boolean,
	rrotate: @checked (x: number, disp: number) -> number,
	lrotate: @checked (x: number, disp: number) -> number,
	lshift: @checked (x: number, disp: number) -> number,
	arshift: @checked (x: number, disp: number) -> number,
	rshift: @checked (x: number, disp: number) -> number,
	bnot: @checked (x: number) -> number,
	extract: @checked (n: number, field: number, width: number?) -> number,
	replace: @checked (n: number, v: number, field: number, width: number?) -> number,
	countlz: @checked (n: number) -> number,
	countrz: @checked (n: number) -> number,
	byteswap: @checked (n: number) -> number,
}
```

## math

```luau
declare math: {
	frexp: @checked (n: number) -> (number, number),
	ldexp: @checked (s: number, e: number) -> number,
	fmod: @checked (x: number, y: number) -> number,
	modf: @checked (n: number) -> (number, number),
	pow: @checked (x: number, y: number) -> number,
	exp: @checked (n: number) -> number,

	ceil: @checked (n: number) -> number,
	floor: @checked (n: number) -> number,
	abs: @checked (n: number) -> number,
	sqrt: @checked (n: number) -> number,

	log: @checked (n: number, base: number?) -> number,
	log10: @checked (n: number) -> number,

	rad: @checked (n: number) -> number,
	deg: @checked (n: number) -> number,

	sin: @checked (n: number) -> number,
	cos: @checked (n: number) -> number,
	tan: @checked (n: number) -> number,
	sinh: @checked (n: number) -> number,
	cosh: @checked (n: number) -> number,
	tanh: @checked (n: number) -> number,
	atan: @checked (n: number) -> number,
	acos: @checked (n: number) -> number,
	asin: @checked (n: number) -> number,
	atan2: @checked (y: number, x: number) -> number,

	min: @checked (number, ...number) -> number,
	max: @checked (number, ...number) -> number,

	pi: number,
	huge: number,
	nan: number,
	e: number,
	phi: number,
	sqrt2: number,
	tau: number,

	randomseed: @checked (seed: number) -> (),
	random: @checked (number?, number?) -> number,

	sign: @checked (n: number) -> number,
	clamp: @checked (n: number, min: number, max: number) -> number,
	noise: @checked (x: number, y: number?, z: number?) -> number,
	round: @checked (n: number) -> number,
	map: @checked (x: number, inmin: number, inmax: number, outmin: number, outmax: number) -> number,
	lerp: @checked (a: number, b: number, t: number) -> number,

	isnan: @checked (x: number) -> boolean,
	isinf: @checked (x: number) -> boolean,
	isfinite: @checked (x: number) -> boolean,
}
```

## os

```luau
type DateTypeArg = {
	year: number,
	month: number,
	day: number,
	hour: number?,
	min: number?,
	sec: number?,
	isdst: boolean?,
}

type DateTypeResult = {
	year: number,
	month: number,
	wday: number,
	yday: number,
	day: number,
	hour: number,
	min: number,
	sec: number,
	isdst: boolean,
}

declare os: {
	time: (time: DateTypeArg?) -> number,
	date: ((formatString: "*t" | "!*t", time: number?) -> DateTypeResult) & ((formatString: string?, time: number?) -> string),
	difftime: (t2: DateTypeResult | number, t1: DateTypeResult | number) -> number,
	clock: () -> number,
}
```

## coroutine

```luau
declare coroutine: {
	create: <A..., R...>(f: (A...) -> R...) -> thread,
	resume: <A..., R...>(co: thread, A...) -> (boolean, R...),
	running: () -> thread,
	status: @checked (co: thread) -> "dead" | "running" | "normal" | "suspended",
	wrap: <A..., R...>(f: (A...) -> R...) -> ((A...) -> R...),
	yield: <A..., R...>(A...) -> R...,
	isyieldable: () -> boolean,
	close: @checked (co: thread) -> (boolean, any)
}
```

## table

```luau
declare table: {
	concat: <V>(t: {V}, sep: string?, i: number?, j: number?) -> string,
	insert: (<V>(t: {V}, value: V) -> ()) & (<V>(t: {V}, pos: number, value: V) -> ()),
	maxn: <V>(t: {V}) -> number,
	remove: <V>(t: {V}, number?) -> V?,
	sort: <V>(t: {V}, comp: ((V, V) -> boolean)?) -> (),
	create: <V>(count: number, value: V?) -> {V},
	find: <V>(haystack: {V}, needle: V, init: number?) -> number?,

	unpack: <V>(list: {V}, i: number?, j: number?) -> ...V,
	pack: <V>(...V) -> { n: number, [number]: V },

	getn: <V>(t: {V}) -> number,
	foreach: <K, V>(t: {[K]: V}, f: (K, V) -> ()) -> (),
	foreachi: <V>({V}, (number, V) -> ()) -> (),

	move: <V>(src: {V}, a: number, b: number, t: number, dst: {V}?) -> {V},

	clear: (table: {}) -> (),
	isfrozen: (t: {}) -> boolean,
}
```

## debug

```luau
declare debug: {
	info: ((thread: thread, level: number, options: string) -> ...any) & ((level: number, options: string) -> ...any) & (<A..., R1...>(func: (A...) -> R1..., options: string) -> ...any),
	traceback: ((message: string?, level: number?) -> string) & ((thread: thread, message: string?, level: number?) -> string),
}
```

## utf8

```luau
declare utf8: {
	char: @checked (...number) -> string,
	charpattern: string,
	codes: @checked (str: string) -> ((string, number) -> (number, number), string, number),
	codepoint: @checked (str: string, i: number?, j: number?) -> ...number,
	len: @checked (s: string, i: number?, j: number?) -> (number?, number?),
	offset: @checked (s: string, n: number?, i: number?) -> number,
}
```

## buffer

```luau
--- Buffer API
declare buffer: {
	create: @checked (size: number) -> buffer,
	fromstring: @checked (str: string) -> buffer,
	tostring: @checked (b: buffer) -> string,
	len: @checked (b: buffer) -> number,
	copy: @checked (target: buffer, targetOffset: number, source: buffer, sourceOffset: number?, count: number?) -> (),
	fill: @checked (b: buffer, offset: number, value: number, count: number?) -> (),
	readi8: @checked (b: buffer, offset: number) -> number,
	readu8: @checked (b: buffer, offset: number) -> number,
	readi16: @checked (b: buffer, offset: number) -> number,
	readu16: @checked (b: buffer, offset: number) -> number,
	readi32: @checked (b: buffer, offset: number) -> number,
	readu32: @checked (b: buffer, offset: number) -> number,
	readf32: @checked (b: buffer, offset: number) -> number,
	readf64: @checked (b: buffer, offset: number) -> number,
	writei8: @checked (b: buffer, offset: number, value: number) -> (),
	writeu8: @checked (b: buffer, offset: number, value: number) -> (),
	writei16: @checked (b: buffer, offset: number, value: number) -> (),
	writeu16: @checked (b: buffer, offset: number, value: number) -> (),
	writei32: @checked (b: buffer, offset: number, value: number) -> (),
	writeu32: @checked (b: buffer, offset: number, value: number) -> (),
	writef32: @checked (b: buffer, offset: number, value: number) -> (),
	writef64: @checked (b: buffer, offset: number, value: number) -> (),
	readstring: @checked (b: buffer, offset: number, count: number) -> string,
	writestring: @checked (b: buffer, offset: number, value: string, count: number?) -> (),
	readbits: @checked (b: buffer, bitOffset: number, bitCount: number) -> number,
	writebits: @checked (b: buffer, bitOffset: number, bitCount: number, value: number) -> ()
}
```

## vector

```luau
-- While vector would have been better represented as a built-in primitive type, type solver extern type handling covers most of the properties
declare extern type vector with
	read x: number
	read y: number
	read z: number
end

declare vector: {
	create: @checked (x: number, y: number, z: number?) -> vector,
	magnitude: @checked (vec: vector) -> number,
	normalize: @checked (vec: vector) -> vector,
	cross: @checked (vec1: vector, vec2: vector) -> vector,
	dot: @checked (vec1: vector, vec2: vector) -> number,
	angle: @checked (vec1: vector, vec2: vector, axis: vector?) -> number,
	floor: @checked (vec: vector) -> vector,
	ceil: @checked (vec: vector) -> vector,
	abs: @checked (vec: vector) -> vector,
	sign: @checked (vec: vector) -> vector,
	clamp: @checked (vec: vector, min: vector, max: vector) -> vector,
	max: @checked (vector, ...vector) -> vector,
	min: @checked (vector, ...vector) -> vector,
	lerp: @checked (vec1: vector, vec2: vector, t: number) -> vector,

	zero: vector,
	one: vector,
}
```

## string

The analyzer registers string overloads in `luau/Analysis/src/BuiltinDefinitions.cpp`; the VM implementation is `luau/VM/src/lstrlib.cpp`.

| Function | Purpose |
| --- | --- |
| `string.byte(text, start?, finish?)` | Read byte values. |
| `string.char(...)` | Build text from byte values. |
| `string.find(text, pattern, start?, plain?)` | Find a match; returns positions and captures, or nil. |
| `string.match(text, pattern, start?)` | Return the pattern captures. |
| `string.gmatch(text, pattern)` | Iterate over matches. |
| `string.gsub(text, pattern, replacement, count?)` | Replace matches; returns text and replacement count. |
| `string.format(format, ...)` | Format values into text. |
| `string.len(text)` | Count bytes, not Unicode characters. |
| `string.sub(text, start, finish?)` | Slice using byte positions. |
| `string.lower(text)` / `string.upper(text)` | Change ASCII letter casing. |
| `string.rep(text, count, separator?)` | Repeat text. |
| `string.reverse(text)` | Reverse bytes. |
| `string.split(text, separator?)` | Split text into an array. |

Positions are 1-based; negative positions count from the end. Pattern functions use Lua patterns, not regular expressions. Use `utf8` for Unicode code points.

## Runtime availability

The engine installs its own sandbox and module loader. A declaration in the analyzer does not grant filesystem or operating system access. Internal feature-flag variants, experimental integer APIs, and type-function implementation helpers are not presented as stable library contracts. The exact definitions remain in the linked source.

## Try it

<Playground example="Optional values" />
