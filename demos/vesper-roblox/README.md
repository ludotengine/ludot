# Vesper for Roblox

A Roblox port of the Ludot Vesper demo. It keeps the same planet recipes, palettes,
relative sizes, circular orbits, axial rotation, moon, camera shots, and free-flight
controls. All assets are original Vesper geometry or procedurally generated images;
no Creator Store models are used. Original mesh templates and starfield images are
published under the planet experience’s creator group (426749571). The complete
asset IDs are recorded in `assets/roblox-assets.json` and `src/MeshAssets.luau`.

The connected **planet** project has the source installed at
`ReplicatedStorage.Client.Vesper`. Its existing content was preserved, and the
previous loaders, lighting, spawn, and loose parts are backed up in
`ServerStorage.VesperOriginalContent`. `Workspace.VesperPreview` is the edit-mode
preview. The client replaces it with its own generated assets when the game runs.

## Open and build

A standalone Rojo project is included:

```sh
rokit install
rojo build default.project.json -o vesper.rbxlx
```

Open the resulting place in Roblox Studio. For live source editing, run
`rojo serve default.project.json` and connect the Rojo plugin.

In **Experience Settings → Security**, enable **Allow Mesh / Image APIs** for a
published experience. Roblox applies device-specific editable-asset memory budgets;
only the nearest or focused terrain world has six fixed-size EditableMesh patches.
The previous world is released before another is loaded. Distant terrain uses baked
MeshParts, the sky uses ordinary image assets, and small mesh templates are shared.
If another allocation exceeds the device budget, terrain retains its baked appearance.
The scene is built
on each client because editable mesh/image content is not replicated automatically.
The server disables avatar spawning; this is a desktop free-flight simulator.
Using another creator requires permission to load these assets or republishing them
under that creator. Keep the MeshAssets module and JSON manifest in sync.

## Controls

| Input | Action |
| --- | --- |
| Hold right mouse | Look around |
| WASD | Fly in camera space |
| Space / left Ctrl | Up / down |
| Left Shift | Boost |
| Mouse wheel | Flight speed or planet-camera zoom |
| 1–6 | Focus Ember, Earth, Moon, Mars, the giant, or Pluto |
| Esc / 0 | Return to free flight |
| P | Pause planetary motion |
| O | Toggle orbit paths |
| T | Toggle the original Vesper camera tour |

The free camera rides with the nearest planet and checks clearance against its
actual EditableMesh triangles. Clearance has a minimum of 0.7 Roblox studs to
accommodate Roblox's near clipping plane.

## Port details

- **Terrain:** real displaced meshes exported from Ludot's original FastNoiseLite
  terrain generator. Each world has six patches, with 48 subdivisions
  per cube face and 4,608 triangles per patch. Normals, height, and plains masks are
  preserved; the original clockwise winding is reversed for Roblox. This is lower
  tessellation than the Ludot demo, with the same terrain sampling function. Nearby
  terrain loads fixed-size meshes for vertex lighting and exact camera raycasts;
  distant terrain uses prelit assets without consuming editable memory.
- **Lighting:** the original point-sun calculation is applied to terrain vertices.
  Untextured surfaces use Roblox's Neon material so vertex RGB remains visible.
  Textured clouds and the gas giant use small CPU-lit EditableImages with PBR
  emission. Texture updates are spread across frames and serialized so only one
  displayed image is written at a time.
- **Sun and sky:** procedural granulation, a depth-occluded corona, and a six-face
  starfield. The sky uses transparent Parts with depth-tested SurfaceGui image tiles and
  published procedural images, consuming no editable mesh/image memory. This avoids
  Roblox's mesh-extents limit and the editable-content skybox rendering limitation
  found during the Studio preview.
- **Earth:** displaced continents, height/slope/snow/plains colors, a separate ocean
  with shallow-water and shoreline colors, and drifting cloud geometry.
- **Gas giant:** procedural bands, a storm, translucent striped rings, and a computed
  planetary shadow on the rings.
- **Atmosphere:** depth-tested, sun-oriented rim meshes outside a planet, plus
  camera-altitude-driven Atmosphere haze near the ground.
- **Asteroids:** 1,800 MeshParts share six deformed EditableMeshes. Their baked
  lighting is kept aligned with the sun.

Roblox does not expose Ludot's custom shader pipeline or scene depth to ordinary
experience scripts. Fog is therefore an approximation rather than the original
ray-marched shader. Ocean specular/refraction, the sun's per-pixel limb darkening,
and continuously evolving gas/cloud noise are also approximate. The sun and cloud
shells animate; the noise patterns themselves are generated at startup.

## Source

| Module | Responsibility |
| --- | --- |
| `Config` | Original world data and palettes |
| `Math` | Terrain palette and point-sun lighting |
| `Geometry` | EditableMesh construction, normals, UVs, colors, and material binding |
| `Textures` | Procedural EditableImages, sky tiles, and texture lighting |
| `Worlds` | Planet assembly, orbits, rotation, rings, and asteroid instancing |
| `Flight` | Free flight, planet focus, ground clearance, and tour camera |
| `Tour` | Original Vesper camera shot data |
| `Environment` | Bloom, color correction, atmosphere, and altitude haze |
| `init` | One-time client startup and frame integration |

`assets/TerrainData.model.json` contains little-endian vertex/index buffers encoded as
base64 StringValues. `assets/manifest.json` records source geometry hashes and
sampling metadata. `tools/export_terrain.gd` and `tools/pack_terrain.py` regenerate
these buffers using the Ludot editor and the original demo. They are retained as
source assets outside the replicated runtime tree.

## Verification

The source passes strict Roblox Luau analysis. The complete scene was built and
visually inspected in Studio edit mode, including terrain, stars, point lighting,
clouds, ocean, the giant, and atmosphere. Edit-mode checks cover buffer integrity,
mesh limits, outward winding/normals, orbital motion, pause, terrain raycasts,
planet-camera distance, and the camera tour.

Studio client playtests cover complete startup, free flight, Earth focus, pause,
and orbit visibility. `tests/terrain-budget.client.luau` checks two complete rounds
of terrain switching, exactly six active patches, release of previous meshes, and
ground raycasts. Install it temporarily as ClientLoader for that regression, then
restore `ClientLoader.client.luau`. Published production-client and lower-memory
device behavior remain unverified.
