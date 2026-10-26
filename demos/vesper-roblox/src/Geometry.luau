--!strict
const AssetService = game:GetService("AssetService")
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const Math = require(ReplicatedStorage.Client.Vesper.Math)
const MeshAssets = require(ReplicatedStorage.Client.Vesper.MeshAssets)

export type Vertex = { Position: Vector3, Normal: Vector3, Base: Color3, ColorId: number }
export type Patch = {
	Part: MeshPart,
	Mesh: EditableMesh?,
	Center: Vector3,
	Vertices: { Vertex },
	Radius: number,
	Image: EditableImage?,
	BaseSize: Vector3,
	AssetId: number?,
	Template: MeshPart?,
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
	if mesh.FixedSize == false then
		mesh:RemoveUnused()
	end
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
	return {
		Part = part,
		Mesh = mesh,
		Center = center,
		Vertices = vertices,
		Radius = 0,
		Image = image,
		BaseSize = part.Size,
	}
end

const loadedMeshes: { [number]: EditableMesh } = {}

function Geometry.asset(
	parent: Instance,
	name: string,
	id: number,
	center: Vector3,
	scale: number,
	image: EditableImage?,
	transparent: boolean,
	strength: number,
	unique: boolean
): Patch
	local mesh = if unique == false then loadedMeshes[id] else nil
	if mesh == nil then
		mesh = AssetService:CreateEditableMeshAsync(Content.fromAssetId(id), { FixedSize = true })
		assert(mesh ~= nil, "Vesper: fixed-size EditableMesh memory exhausted while loading " .. name)
		if unique == false then
			loadedMeshes[id] = mesh
		end
	end
	const fixed = mesh :: EditableMesh
	const vertices: { Vertex } = {}
	if image == nil then
		for _, vertex in fixed:GetVertices() do
			const face = fixed:GetVertexFaces(vertex)[1]
			const colorId = fixed:GetVertexFaceColor(vertex, face)
			const normalId = fixed:GetVertexFaceNormal(vertex, face)
			table.insert(vertices, {
				Position = fixed:GetPosition(vertex) * scale + center,
				Normal = fixed:GetNormal(normalId) :: Vector3,
				Base = fixed:GetColor(colorId) :: Color3,
				ColorId = colorId,
			})
		end
	end
	const patch = Geometry.finish(fixed, parent, name, center, vertices, image, transparent, strength)
	patch.Part.Size *= scale
	patch.BaseSize = patch.Part.Size
	return patch
end

function Geometry.terrain(world: any, parent: Instance): { Patch }
	const patches: { Patch } = {}
	for side, asset in (MeshAssets.Terrain :: any)[world.Name] do
		const center = Vector3.new(asset.Center[1], asset.Center[2], asset.Center[3])
		const template = AssetService:CreateMeshPartAsync(Content.fromAssetId(asset.StaticId))
		const part = template:Clone()
		part.Name = "Terrain" .. side
		part.Anchored = true
		part.CanCollide, part.CanTouch, part.CanQuery, part.CastShadow = false, false, false, false
		part.Material = Enum.Material.Neon
		part.Color = Color3.new(0.6, 0.6, 0.6)
		part.CFrame = CFrame.new(center)
		part.Parent = parent
		table.insert(patches, {
			Part = part,
			Mesh = nil,
			Center = center,
			Vertices = {},
			Radius = 0,
			Image = nil,
			BaseSize = part.Size,
			AssetId = asset.Id,
			Template = template,
		})
	end
	return patches
end

function Geometry.activateTerrain(patches: { Patch })
	for _, patch in patches do
		const mesh =
			AssetService:CreateEditableMeshAsync(Content.fromAssetId(patch.AssetId :: number), { FixedSize = true })
		assert(mesh ~= nil, "Vesper: active terrain exceeds the client editable-mesh budget")
		patch.Mesh = mesh
		const vertices: { Vertex } = {}
		for _, vertex in mesh:GetVertices() do
			const face = mesh:GetVertexFaces(vertex)[1]
			const colorId = mesh:GetVertexFaceColor(vertex, face)
			table.insert(vertices, {
				Position = mesh:GetPosition(vertex) + patch.Center,
				Normal = mesh:GetNormal(mesh:GetVertexFaceNormal(vertex, face)) :: Vector3,
				Base = mesh:GetColor(colorId) :: Color3,
				ColorId = colorId,
			})
		end
		const display = AssetService:CreateMeshPartAsync(Content.fromObject(mesh))
		patch.Part:ApplyMesh(display)
		patch.Part.Size = display.Size
		display:Destroy()
		patch.Mesh, patch.Vertices = mesh, vertices
	end
end

function Geometry.releaseTerrain(patches: { Patch })
	for _, patch in patches do
		const mesh = patch.Mesh
		if mesh ~= nil then
			patch.Part:ApplyMesh(patch.Template :: MeshPart)
			patch.Part.Size = (patch.Template :: MeshPart).Size
			patch.Mesh, patch.Vertices = nil, {}
			mesh:Destroy()
		end
	end
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
	const id = if detail <= 5 then MeshAssets.Rock else MeshAssets.Sphere
	const patch = Geometry.asset(parent, name, id, Vector3.zero, radius, image, transparent, strength, image == nil)
	patch.Radius = radius
	return patch
end

function Geometry.billboard(parent: Instance, name: string, radius: number, image: EditableImage): Patch
	return Geometry.asset(parent, name, MeshAssets.Quad, Vector3.zero, radius, image, true, 1, false)
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
	const mesh = patch.Mesh
	if mesh ~= nil then
		mesh:BatchSetValues(ids, values)
	end
end

return Geometry
