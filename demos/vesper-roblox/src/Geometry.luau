--!strict
const AssetService = game:GetService("AssetService")
const EncodingService = game:GetService("EncodingService")
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const Math = require(ReplicatedStorage.Client.Vesper.Math)

export type Vertex = { Position: Vector3, Normal: Vector3, Base: Color3, ColorId: number }
export type Patch = {
	Part: MeshPart,
	Mesh: EditableMesh,
	Center: Vector3,
	Vertices: { Vertex },
	Radius: number,
	Image: EditableImage?,
}

const Geometry = {}
local white: EditableImage? = nil
const imageAppearances: { [EditableImage]: SurfaceAppearance? } = {}

function Geometry.appearance(image: EditableImage?, strength: number, transparent: boolean): SurfaceAppearance
	const cached = if image ~= nil then imageAppearances[image] else nil
	if cached ~= nil then
		return cached:Clone()
	end
	if white == nil then
		white = AssetService:CreateEditableImage({ Size = Vector2.new(4, 4) })
		assert(white ~= nil, "Vesper: EditableImage budget exhausted; enable Mesh / Image APIs in Experience Settings.")
		const pixels = buffer.create(4 * 4 * 4)
		buffer.fill(pixels, 0, 255)
		white:WritePixelsBuffer(Vector2.zero, Vector2.new(4, 4), pixels)
	end
	const maps: { [string]: Content } = { EmissiveMask = Content.fromObject(white :: EditableImage) }
	if image ~= nil then
		maps.ColorMap = Content.fromObject(image)
	end
	const appearance = AssetService:CreateSurfaceAppearanceAsync(maps)
	appearance.EmissiveStrength = strength
	appearance.AlphaMode = if transparent == true then Enum.AlphaMode.Transparency else Enum.AlphaMode.Overlay
	if image ~= nil then
		imageAppearances[image] = appearance
		return appearance:Clone()
	end
	return appearance
end

function Geometry.finish(
	mesh: EditableMesh,
	parent: Instance,
	name: string,
	center: Vector3,
	vertices: { Vertex },
	image: EditableImage?,
	transparent: boolean,
	strength: number
): Patch
	mesh:RemoveUnused()
	const part = AssetService:CreateMeshPartAsync(Content.fromObject(mesh), {
		CollisionFidelity = Enum.CollisionFidelity.Box,
		RenderFidelity = Enum.RenderFidelity.Precise,
	})
	part.Name = name
	part.Anchored = true
	part.CanCollide, part.CanTouch, part.CanQuery, part.CastShadow = false, false, false, false
	part.Color = if image == nil then Color3.new(0.6, 0.6, 0.6) else Color3.new(1, 1, 1)
	part.Material = if image == nil then Enum.Material.Neon else Enum.Material.SmoothPlastic
	part.DoubleSided = transparent
	part.Transparency = if transparent == true then 0.02 else 0
	-- PBR emissive maps bypass vertex RGB. Untextured geometry uses the legacy
	-- Neon pipeline so our point-sun vertex colors remain visible.
	if image ~= nil then
		Geometry.appearance(image, strength, transparent).Parent = part
	end
	part.CFrame = CFrame.new(center)
	part.Parent = parent
	return { Part = part, Mesh = mesh, Center = center, Vertices = vertices, Radius = 0, Image = image }
end

function Geometry.terrain(world: any, parent: Instance): { Patch }
	const patches: { Patch } = {}
	const folder = ReplicatedStorage.Client.Vesper.TerrainData[world.Name]
	for side = 1, 6 do
		const raw = EncodingService:Base64Decode(buffer.fromstring(folder["Face" .. side].Value))
		const count, indexCount, sea = buffer.readu32(raw, 0), buffer.readu32(raw, 4), buffer.readf32(raw, 8)
		const mesh = AssetService:CreateEditableMesh()
		assert(mesh ~= nil, "Vesper: EditableMesh budget exhausted while creating " .. world.Name)
		const positions: { Vector3 }, normals: { Vector3 }, bases: { Color3 } = {}, {}, {}
		local minimum, maximum =
			Vector3.new(math.huge, math.huge, math.huge), Vector3.new(-math.huge, -math.huge, -math.huge)
		for i = 1, count do
			const offset = 12 + (i - 1) * 32
			const p = Vector3.new(
				buffer.readf32(raw, offset),
				buffer.readf32(raw, offset + 4),
				buffer.readf32(raw, offset + 8)
			)
			const n = Vector3.new(
				buffer.readf32(raw, offset + 12),
				buffer.readf32(raw, offset + 16),
				buffer.readf32(raw, offset + 20)
			)
			positions[i], normals[i] = p, n
			bases[i] =
				Math.terrainColor(world, p, n, buffer.readf32(raw, offset + 24), buffer.readf32(raw, offset + 28), sea)
			minimum, maximum = minimum:Min(p), maximum:Max(p)
		end
		const center = (minimum + maximum) * 0.5
		const ids: { number }, normalIds: { number }, colorIds: { number }, vertices: { Vertex } = {}, {}, {}, {}
		for i, p in positions do
			ids[i] = mesh:AddVertex(p - center)
			normalIds[i] = mesh:AddNormal(normals[i])
			colorIds[i] = mesh:AddColor(bases[i], 1)
			vertices[i] = { Position = p, Normal = normals[i], Base = bases[i], ColorId = colorIds[i] }
		end
		const first = 12 + count * 32
		for index = 0, indexCount - 1, 3 do
			const a = buffer.readu16(raw, first + index * 2) + 1
			const b = buffer.readu16(raw, first + (index + 1) * 2) + 1
			const c = buffer.readu16(raw, first + (index + 2) * 2) + 1
			const face = mesh:AddTriangle(ids[a], ids[b], ids[c])
			mesh:SetFaceNormals(face, { normalIds[a], normalIds[b], normalIds[c] })
			mesh:SetFaceColors(face, { colorIds[a], colorIds[b], colorIds[c] })
		end
		table.insert(patches, Geometry.finish(mesh, parent, "Terrain" .. side, center, vertices, nil, false, 1))
		task.wait()
	end
	return patches
end

function Geometry.sphere(
	parent: Instance,
	name: string,
	radius: number,
	detail: number,
	image: EditableImage?,
	transparent: boolean,
	strength: number
): Patch
	const mesh = AssetService:CreateEditableMesh()
	assert(mesh ~= nil, "Vesper: EditableMesh budget exhausted while creating " .. name)
	const ids: { { number } }, normalIds: { { number } }, colors: { { number } }, uvs: { { number } } = {}, {}, {}, {}
	const vertices: { Vertex } = {}
	const longitude = detail * 2
	for y = 0, detail do
		ids[y + 1], normalIds[y + 1], colors[y + 1], uvs[y + 1] = {}, {}, {}, {}
		const latitude = math.pi * y / detail
		for x = 0, longitude do
			const angle = math.pi * 2 * x / longitude
			const n = Vector3.new(
				math.sin(latitude) * math.cos(angle),
				math.cos(latitude),
				math.sin(latitude) * math.sin(angle)
			)
			const p = n * radius
			const id, color = mesh:AddVertex(p), mesh:AddColor(Color3.new(1, 1, 1), 1)
			ids[y + 1][x + 1] = id
			normalIds[y + 1][x + 1] = mesh:AddNormal(n)
			colors[y + 1][x + 1] = color
			uvs[y + 1][x + 1] = mesh:AddUV(Vector2.new(x / longitude, y / detail))
			-- The two unreferenced seam poles are removed with their attributes.
			if not (y == 0 and x == 0) and not (y == detail and x == longitude) then
				table.insert(vertices, { Position = p, Normal = n, Base = Color3.new(1, 1, 1), ColorId = color })
			end
		end
	end
	const function triangle(y1: number, x1: number, y2: number, x2: number, y3: number, x3: number)
		const a, b, c = mesh:GetPosition(ids[y1][x1]), mesh:GetPosition(ids[y2][x2]), mesh:GetPosition(ids[y3][x3])
		if (b - a):Cross(c - a):Dot(a) < 0 then
			y2, y3, x2, x3 = y3, y2, x3, x2
		end
		const face = mesh:AddTriangle(ids[y1][x1], ids[y2][x2], ids[y3][x3])
		mesh:SetFaceNormals(face, { normalIds[y1][x1], normalIds[y2][x2], normalIds[y3][x3] })
		mesh:SetFaceColors(face, { colors[y1][x1], colors[y2][x2], colors[y3][x3] })
		mesh:SetFaceUVs(face, { uvs[y1][x1], uvs[y2][x2], uvs[y3][x3] })
	end
	for y = 1, detail do
		for x = 1, longitude do
			if y > 1 then
				triangle(y, x, y + 1, x, y, x + 1)
			end
			if y < detail then
				triangle(y, x + 1, y + 1, x, y + 1, x + 1)
			end
		end
	end
	const patch = Geometry.finish(mesh, parent, name, Vector3.zero, vertices, image, transparent, strength)
	patch.Radius = radius
	return patch
end

function Geometry.billboard(parent: Instance, name: string, radius: number, image: EditableImage): Patch
	const mesh = AssetService:CreateEditableMesh()
	assert(mesh ~= nil, "Vesper: EditableMesh budget exhausted while creating " .. name)
	const ids, uv = {}, {}
	for i, p in
		{
			Vector3.new(-radius, -radius, 0),
			Vector3.new(radius, -radius, 0),
			Vector3.new(radius, radius, 0),
			Vector3.new(-radius, radius, 0),
		}
	do
		ids[i] = mesh:AddVertex(p)
		uv[i] = mesh:AddUV(({ Vector2.new(0, 1), Vector2.new(1, 1), Vector2.new(1, 0), Vector2.new(0, 0) })[i])
	end
	for _, tri in { { 1, 2, 3 }, { 1, 3, 4 } } do
		const face = mesh:AddTriangle(ids[tri[1]], ids[tri[2]], ids[tri[3]])
		mesh:SetFaceUVs(face, { uv[tri[1]], uv[tri[2]], uv[tri[3]] })
	end
	return Geometry.finish(mesh, parent, name, Vector3.zero, {}, image, true, 1)
end

function Geometry.place(patch: Patch, frame: CFrame)
	patch.Part.CFrame = frame * CFrame.new(patch.Center)
end

function Geometry.light(patch: Patch, frame: CFrame)
	const ids, values = {}, {}
	for i, vertex in patch.Vertices do
		const position = frame:PointToWorldSpace(vertex.Position)
		const normal = frame:VectorToWorldSpace(vertex.Normal)
		const radial = frame:VectorToWorldSpace(vertex.Position.Unit)
		ids[i] = vertex.ColorId
		values[i] = Math.shade(vertex.Base, Math.light(position, normal, radial))
	end
	patch.Mesh:BatchSetValues(ids, values)
end

return Geometry
