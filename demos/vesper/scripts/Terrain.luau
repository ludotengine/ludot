import type Shape from "@sim/System"

--- Planet surfaces: a cube puffed into a sphere, then displaced by layered noise,
--- ridges, terraces, canyons, and craters.

type Grid = { Directions: {Vector3}, Indices: PackedInt32Array }

const grids: {[number]: Grid} = {}

--- Unit directions and triangles for a cube sphere. Vertices on face edges are shared,
--- so normals stay smooth across the seams. Cached per resolution.
const function grid(resolution: number): Grid
    const cached = grids[resolution]
    if cached ~= nil then return cached end
    const n = resolution
    const width = 2 * n + 1
    const xs: {number}, ys: {number}, zs: {number} = {}, {}, {}
    const lookup: {[number]: number} = {}
    const indices: {number} = {}
    -- Integer cube coordinates run from -n to n on every axis.
    const function vertex(x: number, y: number, z: number): number
        const key = ((x + n) * width + (y + n)) * width + (z + n)
        const existing = lookup[key]
        if existing ~= nil then return existing end
        -- Spherified cube: spreads vertices more evenly than normalizing the cube.
        const a, b, c = x / n, y / n, z / n
        table.insert(xs, a * math.sqrt(1 - b * b / 2 - c * c / 2 + b * b * c * c / 3))
        table.insert(ys, b * math.sqrt(1 - c * c / 2 - a * a / 2 + c * c * a * a / 3))
        table.insert(zs, c * math.sqrt(1 - a * a / 2 - b * b / 2 + a * a * b * b / 3))
        lookup[key] = #xs - 1
        return #xs - 1
    end
    const function corner(axis: number, side: number, u: number, v: number): number
        const p, q = -n + 2 * u, -n + 2 * v
        if axis == 1 then return vertex(side, p, q) end
        if axis == 2 then return vertex(p, side, q) end
        return vertex(p, q, side)
    end
    const function triangle(a: number, b: number, c: number)
        -- Godot treats clockwise triangles, seen from outside, as front faces.
        const ax, ay, az = xs[a + 1], ys[a + 1], zs[a + 1]
        const ux, uy, uz = xs[b + 1] - ax, ys[b + 1] - ay, zs[b + 1] - az
        const vx, vy, vz = xs[c + 1] - ax, ys[c + 1] - ay, zs[c + 1] - az
        const outward = (uy * vz - uz * vy) * ax + (uz * vx - ux * vz) * ay + (ux * vy - uy * vx) * az
        table.insert(indices, a)
        table.insert(indices, if outward > 0 then c else b)
        table.insert(indices, if outward > 0 then b else c)
    end
    for axis = 1, 3 do
        for _, side in {-n, n} do
            for i = 0, n - 1 do
                for j = 0, n - 1 do
                    const v00, v10 = corner(axis, side, i, j), corner(axis, side, i + 1, j)
                    const v01, v11 = corner(axis, side, i, j + 1), corner(axis, side, i + 1, j + 1)
                    triangle(v00, v10, v11)
                    triangle(v00, v11, v01)
                end
            end
        end
    end
    const directions: {Vector3} = {}
    for i = 1, #xs do directions[i] = Vector3.new(xs[i], ys[i], zs[i]) end
    const result = { Directions = directions, Indices = PackedInt32Array.new(indices) }
    grids[resolution] = result
    return result
end

const function noise(seed: number, frequency: number, octaves: number, kind: number): FastNoiseLite
    const field = FastNoiseLite.new()
    field.Seed = seed
    field.NoiseType = 1 -- smooth simplex
    field.Frequency = frequency / 100 -- samples are taken at direction * 100
    field.FractalType = kind -- 1 fBm, 2 ridged
    field.FractalOctaves = octaves
    return field
end

const function smoothstep(a: number, b: number, x: number): number
    const t = math.clamp((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)
end

type Crater = { X: number, Y: number, Z: number, Size: number, Depth: number }

const function craters(shape: Shape): {Crater}
    const list: {Crater} = {}
    for i = 1, shape.Craters do
        const y = math.random() * 2 - 1
        const a = math.random() * math.pi * 2
        const r = math.sqrt(1 - y * y)
        -- Many small craters, a few large ones.
        const size = shape.CraterSize * (0.25 + math.random() ^ 3 * 0.9)
        table.insert(list, { X = math.cos(a) * r, Y = y, Z = math.sin(a) * r, Size = size, Depth = 0.35 + math.random() * 0.4 })
    end
    return list
end

--- Height above the base radius, roughly -1 to 1, and a mask for flat plains or crater floors.
const function sampler(shape: Shape): (direction: Vector3) -> (number, number)
    const base = noise(shape.Seed, shape.Frequency, 6, 1)
    const ridge = noise(shape.Seed + 1, shape.Frequency * 1.7, 5, 2)
    const canyon = noise(shape.Seed + 2, shape.Frequency * 0.9, 3, 1)
    const holes = craters(shape)
    const plains = if shape.Plains ~= nil then shape.Plains:normalized() else nil
    const scale = 100
    return function(direction: Vector3): (number, number)
        const x, y, z = direction.X * scale, direction.Y * scale, direction.Z * scale
        local height = base:getNoise3d(x, y, z) * 1.2
        local mask = 0
        if shape.Mountains > 0 then
            -- Ridged noise reads as mountain chains. Keep them on raised ground.
            const peaks = ridge:getNoise3d(x, y, z) * 0.5 + 0.5
            height += shape.Mountains * peaks * peaks * smoothstep(-0.05, 0.35, height)
        end
        if shape.Canyons > 0 then
            const valley = 1 - smoothstep(0, 0.07, math.abs(canyon:getNoise3d(x, y, z)))
            height -= shape.Canyons * valley
        end
        if shape.Terraces > 0 then
            -- Stepped plateaus with steep walls, like desert mesas.
            const t = (height + 1) * 0.5 * shape.Terraces
            height = (math.floor(t) + smoothstep(0.3, 0.7, t - math.floor(t))) / shape.Terraces * 2 - 1
        end
        for _, crater in holes do
            const facing = direction.X * crater.X + direction.Y * crater.Y + direction.Z * crater.Z
            const distance = math.sqrt(math.max(2 * (1 - facing), 0)) / crater.Size
            if distance < 1.8 then
                const bowl = math.max(distance * distance - 1, -0.55)
                const rim = 0.3 * math.exp(-((distance - 1) / 0.22) ^ 2)
                height += crater.Depth * (math.min(bowl, 0) + rim)
                if distance < 0.9 then mask = math.max(mask, 0.6) end
            end
        end
        if plains ~= nil then
            -- Two overlapping lobes make a heart-shaped ice plain.
            const side = plains:cross(Vector3.new(0, 1, 0)):normalized() * 0.13
            const lobe = math.max((plains + side):normalized():dot(direction), (plains - side):normalized():dot(direction))
            const plain = smoothstep(0.955, 0.975, lobe)
            height = height * (1 - plain * 0.85) + plain * 0.15
            mask = math.max(mask, plain)
        end
        const sea = shape.SeaLevel
        if sea ~= nil and height < sea then
            -- Steeper ocean floors keep coastlines crisp.
            height = sea - (sea - height) * 1.8
        end
        return height, mask
    end
end

export type Surface = {
    Mesh: ArrayMesh, SeaLevel: number, Radius: number,
    Ground: (direction: Vector3) -> number,
}

--- Builds a planet mesh. Vertex color red is normalized height, green is the plains mask.
const function build(shape: Shape): Surface
    math.randomseed(shape.Seed)
    const cube = grid(shape.Resolution)
    const height = sampler(shape)
    const heights: {number} = {}
    const masks: {number} = {}
    local low: number, high: number = math.huge, -math.huge
    for i, direction in cube.Directions do
        const h, m = height(direction)
        heights[i], masks[i] = h, m
        if h < low then low = h end
        if h > high then high = h end
    end
    const span = high - low
    const positions: {Vector3} = {}
    const colors: {Color} = {}
    for i, direction in cube.Directions do
        const h: number = heights[i]
        positions[i] = direction * (shape.Radius * (1 + shape.Relief * h))
        colors[i] = Color.new((h - low) / span, masks[i], 0)
    end
    const arrays = Array.new()
    arrays:resize(13) -- Mesh.ARRAY_MAX
    arrays:set(0, PackedVector3Array.new(positions)) -- Mesh.ARRAY_VERTEX
    arrays:set(3, PackedColorArray.new(colors)) -- Mesh.ARRAY_COLOR
    arrays:set(12, cube.Indices) -- Mesh.ARRAY_INDEX
    const tool = SurfaceTool.new()
    tool:createFromArrays(arrays, 3) -- triangles
    tool:generateNormals()
    const seaLevel: number? = shape.SeaLevel
    const sea = if seaLevel ~= nil then (seaLevel - low) / span else -1
    const seaRadius = shape.Radius * (1 + shape.Relief * (seaLevel or -10))
    return {
        Mesh = tool:commit(),
        SeaLevel = sea,
        Radius = shape.Radius * (1 + shape.Relief * (seaLevel or 0)),
        -- Distance from the center to the ground (or sea) in a local direction.
        Ground = function(direction: Vector3): number
            const h = height(direction:normalized())
            return math.max(shape.Radius * (1 + shape.Relief * h), seaRadius)
        end,
    }
end

return { build = build }
