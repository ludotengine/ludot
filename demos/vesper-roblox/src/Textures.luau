--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const AssetService = game:GetService("AssetService")
const Math = require(ReplicatedStorage.Client.Vesper.Math)
const Geometry = require(ReplicatedStorage.Client.Vesper.Geometry)
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
	const images, patches = {}, {}
	const sides = { "Back", "Front", "Right", "Left", "Up", "Down" }
	for index, side in sides do
		const image = Textures.image(512, 512, function(_point: Vector3, u: number, v: number): (Color3, number)
			const x, y = u * 2 - 1, 1 - v * 2
			const directions = {
				Vector3.new(x, y, 1),
				Vector3.new(-x, y, -1),
				Vector3.new(1, y, -x),
				Vector3.new(-1, y, x),
				Vector3.new(x, 1, -y),
				Vector3.new(x, -1, y),
			}
			const p = directions[index].Unit
			const plane = math.abs(p:Dot(Vector3.new(0.4, 0.8, 0.3).Unit))
			const dust = math.exp(-plane * 15) * Math.fbm(p * 6, 5)
			return Color3.new(dust * 0.022, dust * 0.015, dust * 0.03), 1
		end)
		const random = Random.new(743 + index)
		for _ = 1, 450 do
			const x, y = random:NextInteger(0, 511), random:NextInteger(0, 511)
			const brightness, warm = random:NextNumber(0.25, 1), random:NextNumber()
			const color = Color3.new(brightness, brightness * (0.8 + warm * 0.2), brightness * (0.7 + (1 - warm) * 0.3))
			if brightness > 0.96 then
				image:DrawCircle(
					Vector2.new(x, y),
					1,
					color,
					0,
					Enum.ImageCombineType.Overwrite,
					Enum.AntiAliasing.Enabled
				)
			else
				image:DrawRectangle(Vector2.new(x, y), Vector2.one, color, 0, Enum.ImageCombineType.Overwrite)
			end
		end
		table.insert(images, image)
		-- MeshPart extents are limited to 2048 studs. Twenty-five tiles cover
		-- each 10,000-stud sky face, all sampling one shared EditableImage.
		for row = 0, 4 do
			for column = 0, 4 do
				const mesh = AssetService:CreateEditableMesh()
				assert(mesh ~= nil, "Vesper: EditableMesh budget exhausted while creating the starfield")
				const positions, coordinates = {}, {}
				local center = Vector3.zero
				for vertex, offset in { Vector2.new(0, 0), Vector2.new(1, 0), Vector2.new(1, 1), Vector2.new(0, 1) } do
					const uv = Vector2.new((column + offset.X) / 5, (row + offset.Y) / 5)
					const x, y = uv.X * 2 - 1, 1 - uv.Y * 2
					const directions = {
						Vector3.new(x, y, 1),
						Vector3.new(-x, y, -1),
						Vector3.new(1, y, -x),
						Vector3.new(-1, y, x),
						Vector3.new(x, 1, -y),
						Vector3.new(x, -1, y),
					}
					positions[vertex], coordinates[vertex] = directions[index] * 5000, uv
					center += positions[vertex] / 4
				end
				const ids, uvIds = {}, {}
				for vertex, p in positions do
					ids[vertex] = mesh:AddVertex(p - center)
					uvIds[vertex] = mesh:AddUV(coordinates[vertex])
				end
				for _, tri in { { 1, 2, 3 }, { 1, 3, 4 } } do
					local a, b, c = tri[1], tri[2], tri[3]
					const p, q, r = positions[a], positions[b], positions[c]
					if (q - p):Cross(r - p):Dot(p) > 0 then
						b, c = c, b
					end
					const face = mesh:AddTriangle(ids[a], ids[b], ids[c])
					mesh:SetFaceUVs(face, { uvIds[a], uvIds[b], uvIds[c] })
				end
				table.insert(
					patches,
					Geometry.finish(mesh, sky, side .. row .. "_" .. column, center, {}, image, false, 1)
				)
			end
		end
	end
	return { Sky = sky, Images = images, Patches = patches }
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
