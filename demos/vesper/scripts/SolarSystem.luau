import "@sim/System" as System
import "@sim/Terrain" as Terrain
import "@sim/Scene" as Scene
import type World from "@sim/System"

--- Builds the whole system at startup: sun, sky, worlds, air, rings, belt, and orbits.

const function material(path: string, priority: number?): ShaderMaterial
    const shader = ResourceLoader:load(path) :: LuauShader
    assert(shader:getDiagnostics() == "", shader:getDiagnostics())
    const surface = ShaderMaterial.new()
    surface.Shader = shader
    surface.RenderPriority = priority or 0
    return surface
end

const function sphere(radius: number, segments: number): SphereMesh
    const mesh = SphereMesh.new()
    mesh.Radius = radius
    mesh.Height = radius * 2
    mesh.RadialSegments = segments
    mesh.Rings = segments // 2
    return mesh
end

const function shell(parent: Node3D, mesh: Mesh, skin: ShaderMaterial): MeshInstance3D
    const node = MeshInstance3D.new()
    node.Mesh = mesh
    node.MaterialOverride = skin
    node.CastShadow = 0
    parent:addChild(node)
    return node
end

--- Palette colors are authored in sRGB. Shader math happens in linear space.
const function vec3(color: Color): Vector3
    const linear = color:srgbToLinear()
    return Vector3.new(linear.R, linear.G, linear.B)
end

class SolarSystem extends Node3D = {
    private Orbits: Node3D? = nil
    private Trails: {{Material: ShaderMaterial, Body: Node3D, Center: Node3D?}} = {}
    private Belt: Node3D? = nil

    function _ready()
        self:addEnvironment()
        self:addSky()
        self:addSun()
        const orbits = Node3D.new()
        orbits.Name = "Orbits"
        self:addChild(orbits)
        self.Orbits = orbits
        for _, world in System.Worlds do
            self:addWorld(world, self)
        end
        self:addBelt()
    end

    function addEnvironment()
        const environment = Environment.new()
        environment.BackgroundMode = 1 -- clear color
        environment.BackgroundColor = Color.new(0, 0, 0)
        environment.TonemapMode = 3 -- ACES
        environment.TonemapExposure = 1.05
        environment.GlowEnabled = true
        environment.GlowIntensity = 0.9
        environment.GlowStrength = 1.1
        environment.GlowBloom = 0.08
        environment.GlowHdrThreshold = 1.0
        environment.GlowBlendMode = 1 -- screen
        const world = WorldEnvironment.new()
        world.Environment = environment
        self:addChild(world)
    end

    function addSky()
        const sky = shell(self, sphere(5200, 48), material("res://shaders/Stars.lushader", -100))
        sky.Name = "Sky"
    end

    function addSun()
        const sun = Node3D.new()
        sun.Name = "Sun"
        self:addChild(sun)
        shell(sun, sphere(System.Sun.Radius, 96), material("res://shaders/Sun.lushader"))
        const corona = material("res://shaders/Corona.lushader", 10)
        corona:setShaderParameter("StarRadius", System.Sun.Radius)
        shell(sun, sphere(System.Sun.Radius * 6, 48), corona)
        table.insert(Scene.Bodies, { Node = sun, Name = "Sun", Radius = System.Sun.Radius })
    end

    function addWorld(world: World, parent: Node3D)
        const body = Node3D.new()
        body.Name = world.Name
        body:setScript(ResourceLoader:load("res://scripts/OrbitBody.luau"))
        body:set("Radius", world.Orbit)
        body:set("AngularSpeed", world.Speed)
        body:set("Phase", world.Phase)
        parent:addChild(body)
        const spin = Node3D.new()
        spin.Name = "Surface"
        body:addChild(spin)
        body:set("Spin", world.Spin)

        local radius = 34
        local ground: ((direction: Vector3) -> number)? = nil
        local air = world.Air
        if world.Giant then
            shell(spin, sphere(radius, 160), material("res://shaders/GasGiant.lushader"))
            self:addRings(body, radius)
        else
            const shape = world.Shape :: any
            const palette = world.Palette :: any
            const surface = Terrain.build(shape)
            radius = surface.Radius
            ground = surface.Ground
            const skin = material("res://shaders/Terrain.lushader")
            for _, key in {"Deep", "Shore", "Low", "High", "Peak", "Cliff"} do
                skin:setShaderParameter(key, vec3(palette[key]))
            end
            skin:setShaderParameter("SeaLevel", surface.SeaLevel)
            skin:setShaderParameter("PolarIce", world.PolarIce)
            if air ~= nil then skin:setShaderParameter("Haze", vec3(air.Day)) end
            shell(spin, surface.Mesh, skin)
            if surface.SeaLevel > 0 then
                shell(body, sphere(radius, 192), material("res://shaders/Ocean.lushader"))
            end
        end
        if world.Clouds ~= nil then
            const clouds = material("res://shaders/Clouds.lushader")
            clouds:setShaderParameter("Tint", vec3(world.Clouds))
            shell(body, sphere(radius * 1.1, 160), clouds)
        end
        if air ~= nil then
            const fog = material("res://shaders/Atmosphere.lushader", 5)
            fog:setShaderParameter("PlanetRadius", radius)
            fog:setShaderParameter("Thickness", air.Height)
            fog:setShaderParameter("Density", air.Density)
            fog:setShaderParameter("Falloff", air.Falloff)
            fog:setShaderParameter("Haze", air.Haze)
            fog:setShaderParameter("Day", vec3(air.Day))
            fog:setShaderParameter("Sunset", vec3(air.Sunset))
            shell(body, sphere(radius + air.Height, 96), fog)
        end
        self:addOrbitLine(world, body, parent)
        table.insert(Scene.Bodies, { Node = body, Name = world.Name, Radius = radius, Surface = spin, Ground = ground })
        for _, moon in world.Moons or {} do
            self:addWorld(moon, body)
        end
    end

    function addRings(body: Node3D, radius: number)
        const inner, outer = radius * 1.35, radius * 2.3
        const mesh = ImmediateMesh.new()
        const skin = material("res://shaders/Rings.lushader", 2)
        skin:setShaderParameter("PlanetRadius", radius)
        mesh:surfaceBegin(4, skin) -- triangle strip
        for i = 0, 256 do
            const a = i / 256 * math.pi * 2
            const direction = Vector3.new(math.cos(a), 0, math.sin(a))
            mesh:surfaceSetUv(Vector2.new(0, 0))
            mesh:surfaceAddVertex(direction * inner)
            mesh:surfaceSetUv(Vector2.new(1, 0))
            mesh:surfaceAddVertex(direction * outer)
        end
        mesh:surfaceEnd()
        const rings = shell(body, mesh, skin)
        rings.Rotation = Vector3.new(0.38, 0, 0.12)
    end

    function addOrbitLine(world: World, body: Node3D, center: Node3D)
        const skin = material("res://shaders/Orbit.lushader", 1)
        const mesh = ImmediateMesh.new()
        mesh:surfaceBegin(1, skin) -- lines
        const steps = 512
        for i = 0, steps - 1 do
            for _, k in {i, i + 1} do
                const a = k / steps * math.pi * 2
                mesh:surfaceSetUv(Vector2.new(k / steps, 0))
                mesh:surfaceAddVertex(Vector3.new(math.cos(a), 0, math.sin(a)) * world.Orbit)
            end
        end
        mesh:surfaceEnd()
        -- Moon orbits follow their planet, so they live under it.
        const holder = if center == self then self.Orbits :: Node3D else center
        shell(holder, mesh, skin)
        table.insert(self.Trails, { Material = skin, Body = body })
    end

    function addBelt()
        const belt = Node3D.new()
        belt.Name = "Belt"
        self:addChild(belt)
        self.Belt = belt
        const skin = material("res://shaders/Terrain.lushader")
        const stone: {[string]: string} = {
            Low = "#4f4740", High = "#655c54", Peak = "#7d746a", Cliff = "#3a332e", Deep = "#2c2622", Shore = "#453e38",
        }
        for key, hex in stone do
            skin:setShaderParameter(key, vec3(Color.html(hex)))
        end
        skin:setShaderParameter("Haze", Vector3.new(0.3, 0.25, 0.2))
        const spec = System.Belt
        const kinds = 6
        for kind = 1, kinds do
            const rock = Terrain.build({ Radius = 1, Relief = 0.32, Resolution = 24, Seed = 100 + kind,
                Frequency = 2.4, Mountains = 0.3, Terraces = 0, Canyons = 0, Craters = 2, CraterSize = 0.45 })
            const field = MultiMesh.new()
            field.TransformFormat = 1 -- 3D
            field.InstanceCount = spec.Count // kinds
            field.Mesh = rock.Mesh
            for i = 0, field.InstanceCount - 1 do
                const a = math.random() * math.pi * 2
                const r = spec.Inner + (spec.Outer - spec.Inner) * (math.random() + math.random()) / 2
                const size = 0.35 + math.random() ^ 5 * 5
                const axis = Vector3.new(math.random() - 0.5, math.random() - 0.5, math.random() - 0.5):normalized()
                const basis = Basis.new(axis, math.random() * 6.28):scaled(Vector3.new(size, size * (0.6 + math.random() * 0.4), size))
                const height = (math.random() - 0.5) * spec.Thickness
                field:setInstanceTransform(i, Transform3D.new(basis, Vector3.new(math.cos(a) * r, height, math.sin(a) * r)))
            end
            const node = MultiMeshInstance3D.new()
            node.Multimesh = field
            node.MaterialOverride = skin
            belt:addChild(node)
        end
    end

    function _process(delta: number)
        for _, trail in self.Trails do
            -- The trail shader brightens the arc just behind each world.
            const p = trail.Body.Position
            trail.Material:setShaderParameter("Phase", (math.atan2(p.Z, p.X) / (math.pi * 2)) % 1)
            trail.Material:setShaderParameter("Strength", Scene.OrbitStrength)
        end
        const belt = self.Belt :: Node3D
        belt:rotateY(delta * 0.004)
    end
}
return SolarSystem
