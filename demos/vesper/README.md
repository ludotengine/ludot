# Vesper

A small, procedural space playground for Ludot. Seven orbiting worlds, a glowing
sun, Earth and its moon, a low-poly asteroid belt, a ringed Saturn, and an icy Pluto.
All gameplay and scene construction use **Luau**. Every custom surface effect uses
**lushader**. There are no GDScript files, downloaded planet textures, or external
models in this project.

The palette and miniature planetary feel are inspired by Outer Wilds. This is
original geometry and shader code, not a reproduction of that game's assets.
Orbits, distances, sizes, and time are deliberately stylized. This is not an N-body
or physically scaled astronomy simulation.

## Run

Use the Ludot fork from this monorepo. Stock Godot cannot execute these scripts.

```sh
ludot --editor --path demos/vesper --import
ludot --path demos/vesper
```

When running from this project directory, use `ludot --path .` instead. Open
`project.godot` with Ludot to edit it. Geometry is built at runtime, so press Play
to see the complete system.

## Controls

| Input | Action |
| --- | --- |
| WASD | Fly in camera space |
| Hold right mouse | Capture the cursor and look around |
| Space / Ctrl | Move up / down |
| Shift | Boost |
| 1 / 2 / 3 | Jump near Earth / Mars / Pluto |
| 0 | Return to the system overview |
| P | Pause or resume planetary motion |
| O | Show or hide orbit lines |
| C | Show or hide actual source code beside the world |
| R | Start or stop the cinematic tour |
| Esc | Release the mouse cursor |

## Code

- `scripts/SolarSystem.luau`: geometry, native material resources, HUD, star field,
  asteroid field, and the live source panel.
- `scripts/OrbitBody.luau`: circular orbits and axial rotation.
- `scripts/Flight.luau`: mouse/keyboard free flight and the repeatable camera tour.
- `scripts/System.luau`: planet palette and orbit data.
- `scripts/Clock.luau`: shared pause state.
- `shaders/Planet.lushader`: procedural continents/bands, ice, faceted normals, and
  sun-facing illumination.
- `shaders/Sun.lushader` and `Corona.lushader`: animated solar surface and glow.
- `shaders/Atmosphere.lushader`: additive atmospheric rim.
- `shaders/Ink.lushader`: orbit paths, Saturn's rings, and stars.

Shader resources load by their `.lushader` paths and share compiled shaders across
materials. The code panel reads the real files rather than showing placeholder text.
The demo uses the compatibility renderer so its corona also works without HDR glow.

## Reproduce the video

From the monorepo root:

```sh
ludot --path demos/vesper --write-movie /absolute/path/vesper.avi \
  --fixed-fps 30 --quit-after 1470 -- --tour
```

This records 49 seconds of the running project at 1280 × 720, including the final
source-code view. The supplied MP4 is compressed from that capture with opening and
closing fades and an original procedural ambient soundtrack.

The supplied logo retains the Godot/Luau attribution embedded in `icon.svg`.
