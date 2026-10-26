-- Edit-mode verification of the already constructed preview. No playtest or require.
const preview = _G.VesperPreview
assert(preview ~= nil, "Build the Vesper preview first")
const system, Worlds, Flight = preview.System, preview.Worlds, preview.Flight
const camera = workspace.CurrentCamera
assert(#system.Bodies == 7, "Sun plus six worlds")
assert(#system.Root.AsteroidBelt:GetChildren() == 1800, "Complete asteroid belt")
local meshes, triangles = 0, 0
for _, body in system.Bodies do
    for _, patch in body.Lit do
        const count = #patch.Mesh:GetFaces()
        assert(count <= 20000, "EditableMesh triangle limit")
        for _, vertex in patch.Vertices do patch.Mesh:GetColor(vertex.ColorId) end
        meshes += 1
        triangles += count
    end
    if #body.Terrain > 0 then
        assert(#body.Terrain == 6, "Six terrain patches")
        const probe = body.Surface:PointToWorldSpace(Vector3.yAxis * body.Radius * 1.5)
        const height = Worlds.ground(body, probe)
        assert(height > body.Radius * 0.7 and height < body.Radius * 1.4, "Terrain raycasts use real geometry")
        for _,patch in body.Terrain do
            const vertex=patch.Vertices[math.floor(#patch.Vertices/2)]
            const p=body.Surface:PointToWorldSpace(vertex.Position.Unit*body.Radius*1.5)
            local expected=vertex.Position.Magnitude
            if body.Spec.Shape.SeaLevel~=nil then
                expected=math.max(expected,body.Spec.Shape.Radius*(1+body.Spec.Shape.Relief*body.Spec.Shape.SeaLevel))
            end
            assert(math.abs(Worlds.ground(body,p)-expected)<0.04,"Ground picks the near hemisphere, not the back of the planet")
        end
    end
end
local earth = system.Bodies[3]
assert(earth.Name == "Earth")
const before = earth.Frame.Position
const time = system.Time
system.Paused = true
Worlds.update(system, 2, camera.CFrame.Position, false)
assert(system.Time == time and (earth.Frame.Position - before).Magnitude < 1e-5, "Pause preserves orbital positions")
system.Paused = false
Worlds.update(system, 2, camera.CFrame.Position, false)
assert((earth.Frame.Position - before).Magnitude > 0.1, "Orbits advance")
const state = Flight.new(system, camera)
state.Mode, state.Focus, state.Distance = "planet", earth, 3.2
Flight.update(state, 0)
assert(math.abs((camera.CFrame.Position - earth.Frame.Position).Magnitude - earth.Radius * 3.2) < 0.001, "Planet camera distance")
state.Mode, state.Position = "free", earth.Frame.Position + Vector3.yAxis * earth.Radius * 0.7
Flight.update(state, 0)
assert((state.Position - earth.Frame.Position).Magnitude >= Worlds.ground(earth, state.Position) + 0.69, "Free camera clears terrain")
const offset = state.Position - earth.Frame.Position
Worlds.update(system, 0.5, camera.CFrame.Position, false)
Flight.update(state, 0)
assert((state.Position - earth.Frame.Position - offset).Magnitude < 0.05, "Free flight rides with its world")
system.OrbitStrength = 0
Worlds.update(system, 0, camera.CFrame.Position, false)
assert(earth.Orbit.Part.Transparency == 1, "Orbit visibility off")
system.OrbitStrength = 0.5
Worlds.update(system, 0, camera.CFrame.Position, false)
assert(earth.Orbit.Part.Transparency < 1, "Orbit visibility on")
state.Mode = "tour"
for _, moment in {2, 9, 17, 23, 29, 34, 39, 45, 51} do
    system.Time = moment
    Worlds.update(system, 0, camera.CFrame.Position, false)
    state.TourTime = moment
    Flight.update(state, 0)
    const p = camera.CFrame.Position
    assert(p.X == p.X and p.Y == p.Y and p.Z == p.Z and p.Magnitude < 100000, "Finite tour camera")
end
for _, part in system.Sky:GetChildren() do
    assert(part.Size.X <= 2048 and part.Size.Y <= 2048 and part.Size.Z <= 2048, "Sky tile size limit")
end
print("VESPER_STUDIO_CHECKS_PASSED", meshes, "lit meshes", triangles, "lit triangles")
return {Status="Passed",Bodies=#system.Bodies,LitMeshes=meshes,LitTriangles=triangles,Asteroids=#system.Root.AsteroidBelt:GetChildren()}
