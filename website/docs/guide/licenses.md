# Licenses and attribution

Ludot contains independent Godot and Luau forks. The original projects and their contributors retain their copyright. Original versions, tags, and commits are recorded in [upstreams.json](https://github.com/ludotengine/ludot/blob/main/upstreams.json).

## Source code and API documentation

- [Ludot license](https://github.com/ludotengine/ludot/blob/main/LICENSE)
- [Godot engine MIT license](https://github.com/ludotengine/ludot/blob/main/engine/LICENSE.txt) and [complete dependency notices](https://github.com/ludotengine/ludot/blob/main/engine/COPYRIGHT.txt)
- [Luau MIT license](https://github.com/ludotengine/ludot/blob/main/luau/LICENSE.txt) and [Lua license](https://github.com/ludotengine/ludot/blob/main/luau/lua_LICENSE.txt)
- [Editor MCP license](https://github.com/ludotengine/ludot/blob/main/mcp/LICENSE)

The generated engine reference reuses class XML committed in the engine tree. Godot's MIT notice covers those source documentation files. The generator changes member headings and signatures to default Ludot Luau casing; inherited descriptions and examples retain their original API names and languages.

The browser runtime compiles the committed Luau source and includes the compiler, VM, and analyzer. Its license notices are included with the published site under `licenses/`.

## Logo

The Ludot logo combines the Godot and Luau marks. The Godot Engine bot is copyright 2017 Andrea Calabró and licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). Ludot recolors it white, tilts it, scales it, and recomposes it using the Simple Icons single-color geometry.

The Luau mark is by Roblox Corporation. Ludot redraws the square in local coordinates, retains the official gradient and proportions, and omits the lettering. The source SVG preserves the complete attribution comment.

Ludot is an independent project. Referencing Godot, Luau, Unity, or Roblox does not imply their endorsement.

## Site dependencies

The site uses VitePress, Vue, Vite, and Shiki with their respective licenses. Their source packages and pinned versions are recorded in `website/package-lock.json`. Shiki's language grammars and themes retain their upstream notices.
