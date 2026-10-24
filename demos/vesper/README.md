# Vesper

A small solar system to fly around, in the spirit of Outer Wilds: a glowing sun, a
desert world of mesas, an Earth with oceans, clouds and a moon, a cratered Mars, an
asteroid belt, a ringed gas giant, and Pluto with its heart-shaped plain.

Everything is generated when the game starts. Gameplay and world building are written
in **Luau**, and every surface, sky, and atmosphere effect is a **lushader**. The
project has no GDScript, textures, or imported models.

Sizes, distances, and speeds are stylized for flying around, not for astronomy. The
art direction is inspired by Outer Wilds, but all geometry and shader code here is
original.

## Run

Use the Ludot build from this monorepo. Stock Godot cannot run these scripts.

```sh
ludot --path demos/vesper
```

Add `-- --tour` to play the recorded camera tour.

## Controls

| Input | Action |
| --- | --- |
| Hold right mouse | Look around |
| W A S D | Fly |
| Space / Ctrl | Up / down |
| Shift | Boost |
| Mouse wheel | Flight speed, or zoom in the planet camera |
| 1 to 6 | Planet camera: circle Ember, Earth, the Moon, Mars, the giant, or Pluto |
| Esc or 0 | Back to free flight |
| O | Show or hide orbits |
| P | Pause the planets |
| T | Start or stop the tour |

Free flight rides along with the nearest world, so its surface holds still under you
while it orbits. You slow down near the ground and cannot fly into it.

## How it works

| File | What it does |
| --- | --- |
| `scripts/System.luau` | Every world: orbit, terrain recipe, palette, and air |
| `scripts/Terrain.luau` | Cube-sphere meshes displaced by noise, ridges, terraces, canyons, and craters |
| `scripts/SolarSystem.luau` | Builds the sun, sky, worlds, oceans, clouds, air, rings, belt, and orbit lines |
| `scripts/OrbitBody.luau` | Moves each world along its orbit and turns its surface |
| `scripts/Flight.luau` | Free flight, the planet camera, and tour playback |
| `scripts/Tour.luau` | The tour's camera shots, in sun-relative planet coordinates |
| `shaders/Terrain.lushader` | Height, slope, snow, and plains coloring, lit by the sun |
| `shaders/Atmosphere.lushader` | Planet fog: ray-marches the air between the camera and the scene depth |
| `shaders/Ocean.lushader` | Shallow shelves and surf from scene depth, plus sun glints |
| `shaders/Clouds.lushader` | Drifting, warped cloud bands |
| `shaders/Sun.lushader`, `Corona.lushader` | Boiling granulation and a halo that works from any distance |
| `shaders/GasGiant.lushader`, `Rings.lushader` | Sheared bands with a storm vortex, and rings shadowed by the planet |
| `shaders/Stars.lushader` | Procedural star layers and a dusty band across the sky |
| `shaders/Orbit.lushader` | Orbit lines with a trail behind each world |
| `shaders/Noise.lushader`, `Light.lushader` | Shared `shader include` libraries for noise and sunlight |
