--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const AssetService = game:GetService("AssetService")
const Math = require(ReplicatedStorage.Client.Vesper.Math)
const Geometry = require(ReplicatedStorage.Client.Vesper.Geometry)
const Textures = require(ReplicatedStorage.Client.Vesper.Textures)
const Config = require(ReplicatedStorage.Client.Vesper.Config)

export type Body = {
	Name: string,
	Model: Instance,
	Spec: any,
	Radius: number,
	Frame: CFrame,
	Surface: CFrame,
	Parent: any,
	Terrain: { Geometry.Patch },
	Lit: { Geometry.Patch },
	Clouds: Geometry.Patch?,
	Halo: Geometry.Patch?,
	Orbit: Geometry.Patch?,
	Rings: Geometry.Patch?,
	LastLight: number,
}
export type System = {
	Root: Folder,
	Bodies: { Body },
	Sky: Folder,
	Corona: Geometry.Patch,
	Time: number,
	Paused: boolean,
	OrbitStrength: number,
	Assets: { any },
	TextureBusy: boolean,
	TextureIndex: number,
	TextureClock: number,
}
const Worlds = {}

function Worlds.ground(body: Body, worldPosition: Vector3): number
	if #body.Terrain == 0 then
		return body.Radius
	end
	const direction = body.Surface:PointToObjectSpace(worldPosition).Unit
	const start = direction * body.Radius * 1.6
	local height = body.Radius * 0.5
	for _, patch in body.Terrain do
		const face, point = patch.Mesh:RaycastLocal(start - patch.Center, direction * -body.Radius * 3)
		if face ~= nil then
			const hit = point + patch.Center
			if hit:Dot(direction) > 0 then
				height = math.max(height, hit.Magnitude)
			end
		end
	end
	if body.Spec.Shape.SeaLevel ~= nil then
		height = math.max(height, body.Spec.Shape.Radius * (1 + body.Spec.Shape.Relief * body.Spec.Shape.SeaLevel))
	end
	return height
end

function Worlds.ribbon(
	parent: Instance,
	name: string,
	inner: number,
	outer: number,
	segments: number,
	bands: number,
	ring: boolean
): Geometry.Patch
	const mesh = AssetService:CreateEditableMesh()
	assert(mesh ~= nil, "Vesper: EditableMesh budget exhausted while building " .. name)
	const ids, colorIds, vertices = {}, {}, {}
	for row = 0, bands do
		ids[row + 1], colorIds[row + 1] = {}, {}
		const radius = inner + (outer - inner) * row / bands
		for column = 0, segments do
			const a = column / segments * math.pi * 2
			const p = Vector3.new(math.cos(a), 0, math.sin(a)) * radius
			local color = Color3.new(0.16, 0.31, 0.34)
			local alpha = 0.45
			if ring == true then
				const stripe = math.sin(radius * 4) * math.sin(radius * 1.7) * 0.5 + 0.5
				const gap = 1 - Math.smooth(0.98, 1, math.sin(radius * 0.8))
				color = Color3.new(0.29, 0.2, 0.12):Lerp(Color3.new(0.83, 0.7, 0.48), stripe)
				alpha = (0.25 + stripe * 0.6) * gap
			end
			const id, colorId = mesh:AddVertex(p), mesh:AddColor(color, alpha)
			ids[row + 1][column + 1], colorIds[row + 1][column + 1] = id, colorId
			table.insert(vertices, { Position = p, Normal = Vector3.yAxis, Base = color, ColorId = colorId })
		end
	end
	for row = 1, bands do
		for column = 1, segments do
			for _, tri in
				{ { { row, column }, { row, column + 1 }, { row + 1, column } }, {
					{ row, column + 1 },
					{ row + 1, column + 1 },
					{ row + 1, column },
				} }
			do
				const a, b, c = tri[1], tri[2], tri[3]
				const face = mesh:AddTriangle(ids[a[1]][a[2]], ids[b[1]][b[2]], ids[c[1]][c[2]])
				mesh:SetFaceColors(face, { colorIds[a[1]][a[2]], colorIds[b[1]][b[2]], colorIds[c[1]][c[2]] })
			end
		end
	end
	return Geometry.finish(mesh, parent, name, Vector3.zero, vertices, nil, true, 1)
end

function Worlds.addBody(system: System, spec: any, parent: Body?)
	const model = Instance.new("Model")
	model.Name = spec.Name
	model.Parent = if parent ~= nil then parent.Model else system.Root
	const radius = if spec.Giant == true then 34 else spec.Shape.Radius
	const body: Body = {
		Name = spec.Name,
		Model = model,
		Spec = spec,
		Radius = radius,
		Frame = CFrame.identity,
		Surface = CFrame.identity,
		Parent = parent,
		Terrain = {},
		Lit = {},
		Clouds = nil,
		Halo = nil,
		Orbit = nil,
		Rings = nil,
		LastLight = -math.huge,
	}
	table.insert(system.Bodies, body)
	if spec.Giant == true then
		const image = Textures.giant()
		table.insert(system.Assets, image)
		table.insert(body.Lit, Geometry.sphere(model, "GasGiant", radius, 40, image, false, 1))
		body.Rings = Worlds.ribbon(model, "Rings", radius * 1.35, radius * 2.45, 192, 24, true)
	else
		body.Terrain = Geometry.terrain(spec, model)
		for _, patch in body.Terrain do
			table.insert(body.Lit, patch)
		end
		if spec.Shape.SeaLevel ~= nil then
			const sea = radius * (1 + spec.Shape.Relief * spec.Shape.SeaLevel)
			const ocean = Geometry.sphere(model, "Ocean", sea, 32, nil, false, 1)
			for _, vertex in ocean.Vertices do
				-- Terrain raycasts give the seabed; sea-level clamping is skipped here.
				const origin = vertex.Position.Unit * radius * 1.6
				local seabed = radius * 0.5
				for _, patch in body.Terrain do
					const face, point = patch.Mesh:RaycastLocal(origin - patch.Center, -origin * 2)
					if face ~= nil then
						const hit = point + patch.Center
						if hit:Dot(vertex.Normal) > 0 then
							seabed = math.max(seabed, hit.Magnitude)
						end
					end
				end
				const shallow = 1 - Math.smooth(0.01, 0.45, sea - seabed)
				const surf = 1 - Math.smooth(0.005, 0.035, math.abs(sea - seabed))
				vertex.Base = Color3.fromHex("#123c55")
					:Lerp(Color3.fromHex("#3a9c9d"), shallow)
					:Lerp(Color3.fromHex("#c3e0cf"), surf * 0.75)
			end
			table.insert(body.Lit, ocean)
		end
	end
	if spec.Clouds ~= nil then
		const image = Textures.clouds()
		table.insert(system.Assets, image)
		body.Clouds = Geometry.sphere(model, "Clouds", radius * 1.055, 40, image, true, 1)
		table.insert(body.Lit, body.Clouds :: Geometry.Patch)
	end
	if spec.Air ~= nil then
		const extent = radius + spec.Air.Height
		const image = Textures.halo(spec.Air.Day, radius / extent, false)
		table.insert(system.Assets, image)
		body.Halo = Geometry.billboard(model, "AtmosphereRim", extent, image)
	end
	body.Orbit = Worlds.ribbon(
		if parent ~= nil then parent.Model else system.Root,
		spec.Name .. "Orbit",
		spec.Orbit - 0.5,
		spec.Orbit + 0.5,
		256,
		1,
		false
	)
	for _, moon in spec.Moons or {} do
		Worlds.addBody(system, moon, body)
	end
end

function Worlds.asteroids(system: System)
	const random = Random.new(1047)
	const folder = Instance.new("Folder")
	folder.Name = "AsteroidBelt"
	folder.Parent = system.Root
	for kind = 1, 6 do
		const rock = Geometry.sphere(folder, "RockPrototype" .. kind, 1, 5, nil, false, 1)
		const offsets = {}
		for _, vertex in rock.Vertices do
			const p = vertex.Position
			const stretch = 0.8 + Math.fbm(p * 2.4 + Vector3.one * kind, 3) * 0.5
			offsets[vertex.Position] = p * stretch * Vector3.new(1, 0.72, 0.9)
		end
		for _, id in rock.Mesh:GetVertices() do
			const p = rock.Mesh:GetPosition(id)
			rock.Mesh:SetPosition(id, offsets[p])
		end
		for _, vertex in rock.Vertices do
			const n = vertex.Normal
			vertex.Base = Color3.fromHex("#4f4740"):Lerp(Color3.fromHex("#7d746a"), math.clamp(n.Y * 0.5 + 0.5, 0, 1))
			rock.Mesh:SetColor(
				vertex.ColorId,
				Math.shade(vertex.Base, Math.light(Vector3.new(100, 0, 0), vertex.Normal, vertex.Normal))
			)
		end
		for index = 1, Config.Belt.Count // 6 do
			const part = if index == 1 then rock.Part else rock.Part:Clone()
			part.Name = "Asteroid"
			const a = random:NextNumber(0, math.pi * 2)
			const r = Config.Belt.Inner
				+ (Config.Belt.Outer - Config.Belt.Inner) * (random:NextNumber() + random:NextNumber()) * 0.5
			const size = 0.35 + random:NextNumber() ^ 5 * 5
			part.Size = rock.Part.MeshSize * size
			const position = Vector3.new(
				math.cos(a) * r,
				random:NextNumber(-Config.Belt.Thickness / 2, Config.Belt.Thickness / 2),
				math.sin(a) * r
			)
			part.CFrame = CFrame.fromMatrix(position, position.Unit, Vector3.yAxis)
				* CFrame.Angles(random:NextNumber() * 6.28, 0, 0)
			part.Color = Color3.new(0.6, 0.6, 0.6)
			part.Parent = folder
		end
		table.insert(system.Assets, rock)
	end
end

function Worlds.build(root: Folder): System
	const skyData = Textures.sky(root)
	const sky = skyData.Sky
	const sunImage = Textures.sun()
	const sun = Geometry.sphere(root, "Sun", Config.Sun.Radius, 48, sunImage, false, 3.2)
	const coronaImage = Textures.halo(Config.Sun.Color, 1 / 6, true)
	const corona = Geometry.billboard(root, "SolarCorona", Config.Sun.Radius * 6, coronaImage)
	const system: System = {
		Root = root,
		Bodies = {},
		Sky = sky,
		Corona = corona,
		Time = 0,
		Paused = false,
		OrbitStrength = 0.5,
		Assets = { skyData, sunImage, coronaImage, sun },
		TextureBusy = false,
		TextureIndex = 0,
		TextureClock = 0,
	}
	table.insert(
		system.Bodies,
		{
			Name = "Sun",
			Model = root :: Instance,
			Spec = { Spin = 0, Orbit = 0, Phase = 0, Speed = 0 },
			Radius = Config.Sun.Radius,
			Frame = CFrame.identity,
			Surface = CFrame.identity,
			Parent = nil,
			Terrain = {},
			Lit = {},
			Clouds = nil,
			Halo = nil,
			Orbit = nil,
			Rings = nil,
			LastLight = 0,
		}
	)
	for _, spec in Config.Worlds do
		Worlds.addBody(system, spec, nil)
	end
	Worlds.asteroids(system)
	Worlds.update(system, 0, Vector3.new(-300, 420, 980), true)
	return system
end

function Worlds.update(system: System, delta: number, cameraPosition: Vector3, immediate: boolean)
	if system.Paused == false then
		system.Time += delta
	end
	const time = system.Time
	for index, body in system.Bodies do
		if index == 1 then
			continue
		end
		const spec = body.Spec
		const angle = spec.Phase + time * spec.Speed
		const center = if body.Parent ~= nil then body.Parent.Frame.Position else Vector3.zero
		const position = center + Vector3.new(math.cos(angle) * spec.Orbit, 0, math.sin(angle) * spec.Orbit)
		body.Frame = CFrame.new(position)
		body.Surface = body.Frame * CFrame.Angles(0, time * spec.Spin, 0)
		for _, patch in body.Terrain do
			Geometry.place(patch, body.Surface)
		end
		for _, patch in body.Lit do
			const frame = if patch == body.Clouds
				then body.Frame * CFrame.Angles(0, time * (spec.Spin + 0.012), 0)
				else body.Surface
			Geometry.place(patch, frame)
		end
		if body.Rings ~= nil then
			Geometry.place(body.Rings, body.Frame * CFrame.Angles(0.15, 0, -0.2))
		end
		if body.Orbit ~= nil then
			Geometry.place(body.Orbit, CFrame.new(center))
			body.Orbit.Part.Transparency = if system.OrbitStrength > 0 then 0.65 else 1
		end
		if body.Halo ~= nil then
			const distance = (cameraPosition - position).Magnitude
			const facing = CFrame.lookAt(position, cameraPosition)
			const localSun = facing:VectorToObjectSpace(-position.Unit)
			body.Halo.Part.CFrame = facing * CFrame.Angles(0, 0, math.atan2(localSun.Y, localSun.X))
			const scale = distance / math.sqrt(math.max(distance * distance - body.Radius * body.Radius, 1))
			body.Halo.Part.Size = body.Halo.Part.MeshSize * math.min(scale, 2)
			body.Halo.Part.Transparency = if distance < body.Radius * 1.15 then 1 else 0.02
		end
		const interval = if (cameraPosition - position).Magnitude < body.Radius * 8 then 0.2 else 2.5
		if immediate == true or os.clock() - body.LastLight > interval then
			body.LastLight = os.clock()
			for _, patch in body.Lit do
				const frame = if patch == body.Clouds
					then body.Frame * CFrame.Angles(0, time * (spec.Spin + 0.012), 0)
					else body.Surface
				if patch.Image == nil then
					Geometry.light(patch, frame)
				elseif immediate == true then
					Textures.light(patch.Image, frame, patch.Radius)
				end
			end
			if body.Rings ~= nil then
				const ids, values = {}, {}
				const ringFrame = body.Frame * CFrame.Angles(0.15, 0, -0.2)
				for i, vertex in body.Rings.Vertices do
					const p = ringFrame:PointToWorldSpace(vertex.Position)
					const ray = -p.Unit
					const towardCenter = position - p
					const along = towardCenter:Dot(ray)
					const eclipsed = along > 0 and (towardCenter - ray * along).Magnitude < body.Radius
					ids[i] = vertex.ColorId
					values[i] = if eclipsed then vertex.Base:Lerp(Color3.new(), 0.92) else vertex.Base
				end
				body.Rings.Mesh:BatchSetValues(ids, values)
			end
		end
	end
	if immediate == false and system.TextureBusy == false and os.clock() - system.TextureClock > 0.5 then
		const candidates = {}
		for _, body in system.Bodies do
			for _, patch in body.Lit do
				if patch.Image ~= nil then
					table.insert(candidates, { Body = body, Patch = patch })
				end
			end
		end
		if #candidates > 0 then
			system.TextureIndex = system.TextureIndex % #candidates + 1
			const item = candidates[system.TextureIndex]
			const frame = if item.Patch == item.Body.Clouds
				then item.Body.Frame * CFrame.Angles(0, time * (item.Body.Spec.Spin + 0.012), 0)
				else item.Body.Surface
			system.TextureBusy, system.TextureClock = true, os.clock()
			task.spawn(function()
				Textures.light(item.Patch.Image :: EditableImage, frame, item.Patch.Radius)
				system.TextureBusy = false
			end)
		end
	end
	for _, patch in system.Assets[1].Patches do
		Geometry.place(patch, CFrame.new(cameraPosition))
	end
	const sun = system.Assets[4]
	sun.Part.CFrame = CFrame.Angles(time * 0.004, time * 0.012, 0)
	system.Corona.Part.CFrame = CFrame.lookAt(Vector3.zero, cameraPosition)
	system.Corona.Part.Size = system.Corona.Part.MeshSize * (1 + math.sin(time * 0.4) * 0.008)
end

return Worlds
