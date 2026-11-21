# Licenses and attribution

Ludot contains independent Godot and Luau forks. The original projects and their contributors retain their copyright. Original versions, tags, and commits are recorded in [upstreams.json](https://github.com/ludotengine/ludot/blob/main/upstreams.json).

## Source code and API documentation

- [Ludot license](https://github.com/ludotengine/ludot/blob/main/LICENSE)
- [Godot engine MIT license](https://github.com/ludotengine/ludot/blob/main/engine/LICENSE.txt) and [complete dependency notices](https://github.com/ludotengine/ludot/blob/main/engine/COPYRIGHT.txt)
- [Luau MIT license](https://github.com/ludotengine/ludot/blob/main/luau/LICENSE.txt) and [Lua license](https://github.com/ludotengine/ludot/blob/main/luau/lua_LICENSE.txt)
- [Editor MCP license](https://github.com/ludotengine/ludot/blob/main/mcp/LICENSE)

The engine reference is generated from the class XML in the engine tree, which Godot's MIT notice covers. The generator rewrites member names and signatures in Ludot's default casing. Descriptions and examples keep Godot's original names and languages.

The browser playground runs the Luau compiler, VM, and analyzer built from this repository. Their license notices ship with the site under `licenses/`.

## Logo

The Ludot logo combines the Godot and Luau marks.

- **Godot Engine bot:** copyright 2017 Andrea Calabró, licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/). Ludot recolors it white, tilts and scales it, and rebuilds it from the Simple Icons single-color geometry.
- **Luau mark:** by Roblox Corporation. Ludot redraws the square in local coordinates, keeps the official gradient and proportions, and leaves out the lettering.

The source SVG keeps the full attribution comment.

Ludot is an independent project. Referencing Godot, Luau, Unity, or Roblox does not imply their endorsement.

## Site dependencies

The site is built with VitePress, Vue, Vite, and Shiki, under their own licenses. `website/package-lock.json` pins their versions. Shiki's grammars and themes keep their upstream notices.
