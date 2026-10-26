--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const AssetService = game:GetService("AssetService")
const Math = require(ReplicatedStorage.Client.Vesper.Math)
const MeshAssets = require(ReplicatedStorage.Client.Vesper.MeshAssets)
const Textures = {}

function Textures.image(
	width: number,
	height: number,
	sample: (Vector3, number, number) -> (Color3, number)
): EditableImage
	const image = AssetService:CreateEditableImage({ Size = Vector2.new(width, height) })
	assert(image ~= nil, "Vesper: EditableImage budget exhausted; enable Mesh / Image APIs in Experience Settings.")
	const pixels = buffer.create(width * height * 4)
	for y = 0, height - 1 do
		const v = (y + 0.5) / height
		const latitude = v * math.pi
		for x = 0, width - 1 do
			const u = (x + 0.5) / width
			const a = u * math.pi * 2
			const p =
				Vector3.new(math.sin(latitude) * math.cos(a), math.cos(latitude), math.sin(latitude) * math.sin(a))
			const color, alpha = sample(p, u, v)
			const offset = (y * width + x) * 4
			buffer.writeu8(pixels, offset, math.clamp(math.floor(color.R * 255), 0, 255))
			buffer.writeu8(pixels, offset + 1, math.clamp(math.floor(color.G * 255), 0, 255))
			buffer.writeu8(pixels, offset + 2, math.clamp(math.floor(color.B * 255), 0, 255))
			buffer.writeu8(pixels, offset + 3, math.clamp(math.floor(alpha * 255), 0, 255))
		end
		if y % 16 == 0 then
			task.wait()
		end
	end
	image:WritePixelsBuffer(Vector2.zero, image.Size, pixels)
	return image
end

function Textures.sun(): EditableImage
	return Textures.image(512, 256, function(p: Vector3, _u: number, _v: number): (Color3, number)
		const flow = p * 5
		const cells = Math.fbm(flow + Vector3.one * Math.fbm(flow * 0.7, 3) * 2.5, 5)
		return Color3.new(1, 0.28, 0.025):Lerp(Color3.new(1, 0.83, 0.42), Math.smooth(0.32, 0.78, cells)), 1
	end)
end

function Textures.giant(): EditableImage
	const center = Vector3.new(0.55, -0.32, 0.77).Unit
	return Textures.image(256, 128, function(p: Vector3, _u: number, _v: number): (Color3, number)
		const spot = math.exp(-(p - center).Magnitude * 9)
		const twist = spot * 5
		const swirl = p:Lerp(p * math.cos(twist) + center:Cross(p) * math.sin(twist), spot)
		const shear = Math.fbm(swirl * Vector3.new(2, 9, 2), 5)
		const latitude = swirl.Y * 7.5 + shear * 2.4
		const band, fine = math.sin(latitude) * 0.5 + 0.5, math.sin(latitude * 3.1 + shear * 5) * 0.5 + 0.5
		local color = Color3.new(0.72, 0.42, 0.24):Lerp(Color3.new(0.93, 0.84, 0.68), Math.smooth(0.25, 0.75, band))
		color = color:Lerp(Color3.new(0.38, 0.22, 0.15), Math.smooth(0.7, 1, fine) * 0.35)
		return color:Lerp(Color3.new(0.86, 0.36, 0.2), Math.smooth(0.25, 0.7, spot)), 1
	end)
end

function Textures.clouds(): EditableImage
	return Textures.image(256, 128, function(p: Vector3, _u: number, _v: number): (Color3, number)
		const swirl = p * 3
		const warp = Math.fbm(swirl * 1.3, 3)
		const shape = Math.fbm(swirl + Vector3.new(warp * 1.6, warp, 0), 6)
		const bands = 0.12 * math.sin(p.Y * 9 + warp * 4)
		const cloud = Math.smooth(0.53, 0.65, shape + bands)
		return Color3.new(0.96, 0.95, 0.92), cloud * 0.92
	end)
end

function Textures.halo(color: Color3, rim: number, corona: boolean): EditableImage
	return Textures.image(256, 256, function(_p: Vector3, u: number, v: number): (Color3, number)
		const radius = Vector2.new(u * 2 - 1, v * 2 - 1).Magnitude
		local alpha = 0
		if corona == true then
			alpha = math.exp(-radius * 8) * 0.42 + math.exp(-((radius - rim) / 0.035) ^ 2) * 0.25
			alpha *= 1 - Math.smooth(0.75, 1, radius)
		else
			alpha = math.exp(-((radius - rim) / (1 - rim)) ^ 2 * 3) * 0.48
			alpha *= Math.smooth(rim - 0.06, rim, radius) * (1 - Math.smooth(0.97, 1, radius))
			alpha *= 0.2 + 0.8 * Math.smooth(-0.5, 0.8, u * 2 - 1)
		end
		return color, alpha
	end)
end

function Textures.sky(parent: Instance): any
	const sky = Instance.new("Folder")
	sky.Name = "Starfield"
	sky.Parent = parent
	const patches = {}
	const sides = { "Back", "Front", "Right", "Left", "Up", "Down" }
	for index, side in sides do
		const imageId = MeshAssets.Sky[index]
		-- SurfaceGui image rectangles share the six images without allocating
		-- 150 editable meshes for the sky's otherwise trivial quad geometry.
		const axes = {
			{ Vector3.xAxis, Vector3.yAxis },
			{ -Vector3.xAxis, Vector3.yAxis },
			{ -Vector3.zAxis, Vector3.yAxis },
			{ Vector3.zAxis, Vector3.yAxis },
			{ Vector3.xAxis, -Vector3.zAxis },
			{ Vector3.xAxis, Vector3.zAxis },
		}
		const right, up = axes[index][1], axes[index][2]
		const back = right:Cross(up)
		for row = 0, 4 do
			for column = 0, 4 do
				const center = back * 5000
					+ right * ((column + 0.5) / 5 * 10000 - 5000)
					+ up * (5000 - (row + 0.5) / 5 * 10000)
				const part = Instance.new("Part")
				part.Name = side .. row .. "_" .. column
				part.Size = Vector3.new(2000, 2000, 0.05)
				part.Anchored = true
				part.CanCollide, part.CanTouch, part.CanQuery, part.CastShadow = false, false, false, false
				part.Transparency = 1
				const rotation = CFrame.fromMatrix(Vector3.zero, right, up, back)
				part.CFrame = CFrame.new(center) * rotation
				part.Parent = sky
				const surface = Instance.new("SurfaceGui")
				surface.Name = "Stars"
				surface.Face = Enum.NormalId.Front
				surface.SizingMode = Enum.SurfaceGuiSizingMode.FixedSize
				surface.CanvasSize = Vector2.new(100, 100)
				surface.AlwaysOnTop = false
				surface.LightInfluence = 0
				surface.Brightness = 1
				surface.MaxDistance = 0
				surface.Parent = part
				const label = Instance.new("ImageLabel")
				label.Name = "Tile"
				label.Size = UDim2.fromScale(1, 1)
				label.BackgroundTransparency = 1
				label.ImageContent = Content.fromAssetId(imageId)
				label.ImageRectOffset = Vector2.new(column * 100, row * 100)
				label.ImageRectSize = Vector2.new(100, 100)
				label.Parent = surface
				table.insert(patches, { Part = part, Center = center, Rotation = rotation })
			end
		end
	end
	return { Sky = sky, Patches = patches }
end

const bases: { [EditableImage]: buffer } = {}

-- Editable PBR emission does not multiply vertex RGB. Light these small cloud
-- and gas maps on the CPU, spreading work across frames and preserving alpha.
function Textures.light(image: EditableImage, frame: CFrame, radius: number)
	local base = bases[image]
	if base == nil then
		base = image:ReadPixelsBuffer(Vector2.zero, image.Size)
		bases[image] = base
	end
	const source = base :: buffer
	const width, height = image.Size.X, image.Size.Y
	const pixels = buffer.create(width * height * 4)
	for y = 0, height - 1 do
		const latitude = (y + 0.5) / height * math.pi
		for x = 0, width - 1 do
			const angle = (x + 0.5) / width * math.pi * 2
			const n = Vector3.new(
				math.sin(latitude) * math.cos(angle),
				math.cos(latitude),
				math.sin(latitude) * math.sin(angle)
			)
			const offset = (y * width + x) * 4
			const color = Color3.fromRGB(
				buffer.readu8(source, offset),
				buffer.readu8(source, offset + 1),
				buffer.readu8(source, offset + 2)
			)
			const normal = frame:VectorToWorldSpace(n)
			const shaded = Math.shade(color, Math.light(frame:PointToWorldSpace(n * radius), normal, normal))
			buffer.writeu8(pixels, offset, math.clamp(shaded.R * 255, 0, 255))
			buffer.writeu8(pixels, offset + 1, math.clamp(shaded.G * 255, 0, 255))
			buffer.writeu8(pixels, offset + 2, math.clamp(shaded.B * 255, 0, 255))
			buffer.writeu8(pixels, offset + 3, buffer.readu8(source, offset + 3))
		end
		if y % 8 == 0 then
			task.wait()
		end
	end
	image:WritePixelsBuffer(Vector2.zero, image.Size, pixels)
end

return Textures
