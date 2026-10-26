--!strict
const Math = {}

function Math.smooth(a: number, b: number, value: number): number
	const t = math.clamp((value - a) / (b - a), 0, 1)
	return t * t * (3 - 2 * t)
end

function Math.fbm(point: Vector3, octaves: number): number
	local sum, amplitude, total = 0, 0.5, 0
	local p = point
	for _ = 1, octaves do
		sum += math.noise(p.X, p.Y, p.Z) * amplitude
		total += amplitude
		p = p * 2.03 + Vector3.new(7.13, 3.71, 11.3)
		amplitude *= 0.5
	end
	return math.clamp(0.5 + sum / total, 0, 1)
end

-- The original point sun, including its wrapped terminator and warm dusk tint.
function Math.light(position: Vector3, normal: Vector3, radial: Vector3): Color3
	const toSun = -position.Unit
	const surface = math.clamp((normal:Dot(toSun) + 0.2) / 1.2, 0, 1)
	const day = Math.smooth(-0.18, 0.25, radial:Dot(toSun))
	const dusk = 1 - Math.smooth(0, 0.45, math.abs(radial:Dot(toSun)))
	const tint = Color3.new(1, 0.86, 0.68):Lerp(Color3.new(1, 0.55, 0.3), dusk * 0.6)
	return Color3.new(0.035 + tint.R * surface * day, 0.045 + tint.G * surface * day, 0.085 + tint.B * surface * day)
end

function Math.shade(base: Color3, light: Color3): Color3
	return Color3.new(base.R * math.sqrt(light.R), base.G * math.sqrt(light.G), base.B * math.sqrt(light.B))
end

function Math.terrainColor(
	world: any,
	position: Vector3,
	normal: Vector3,
	height: number,
	plains: number,
	sea: number
): Color3
	const palette = world.Palette
	const up = position.Unit
	const slope = 1 - math.clamp(normal:Dot(up), 0, 1)
	const grain = Math.fbm(position * 2.2, 3) - 0.5
	const h = height + grain * 0.05
	local color = palette.Low:Lerp(palette.High, Math.smooth(0.35, 0.8, h))
	if sea > 0 then
		color = palette.Shore:Lerp(color, Math.smooth(sea + 0.005, sea + 0.03, h))
		color = palette.Deep:Lerp(color, Math.smooth(sea - 0.02, sea + 0.002, h))
	end
	color = color:Lerp(palette.Cliff, Math.smooth(0.18, 0.45, slope + grain * 0.2))
	local snow = Math.smooth(0.86, 0.9, h + grain * 0.1)
	if world.PolarIce > 0 then
		snow = math.max(snow, Math.smooth(world.PolarIce, world.PolarIce + 0.05, math.abs(up.Y) + grain * 0.15))
	end
	color = color:Lerp(palette.Peak, snow * (1 - Math.smooth(0.35, 0.6, slope)))
	return color:Lerp(palette.Peak:Lerp(Color3.new(), 0.3), plains * 0.8)
end

return Math
