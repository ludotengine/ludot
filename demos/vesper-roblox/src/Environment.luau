--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const Lighting = game:GetService("Lighting")
const Math = require(ReplicatedStorage.Client.Vesper.Math)
const Worlds = require(ReplicatedStorage.Client.Vesper.Worlds)
const Flight = require(ReplicatedStorage.Client.Vesper.Flight)
const Environment = {}

function Environment.setup(preview: boolean?): Atmosphere
	Lighting.Brightness = 0
	Lighting.Ambient, Lighting.OutdoorAmbient = Color3.new(), Color3.new()
	Lighting.EnvironmentDiffuseScale, Lighting.EnvironmentSpecularScale = 0, 0
	Lighting.ExposureCompensation = 0
	Lighting.GlobalShadows = false
	Lighting.FogEnd = 100000
	const atmosphere = Instance.new("Atmosphere")
	atmosphere.Name = "VesperAir"
	atmosphere:SetAttribute("VesperPreview", preview == true)
	atmosphere.Density, atmosphere.Haze, atmosphere.Glare = 0, 0, 0
	atmosphere.Parent = Lighting
	const bloom = Instance.new("BloomEffect")
	bloom.Name, bloom.Intensity, bloom.Size, bloom.Threshold = "VesperBloom", 0.35, 42, 1.4
	bloom:SetAttribute("VesperPreview", preview == true)
	bloom.Parent = Lighting
	const correction = Instance.new("ColorCorrectionEffect")
	correction.Name, correction.Contrast, correction.Saturation = "VesperColor", 0.06, -0.04
	correction:SetAttribute("VesperPreview", preview == true)
	correction.Parent = Lighting
	return atmosphere
end

function Environment.update(air: Atmosphere, system: Worlds.System, camera: Camera)
	const body = Flight.nearest(system, camera.CFrame.Position)
	if body == nil or body.Spec.Air == nil then
		air.Density, air.Haze = 0, 0
		return
	end
	const spec = body.Spec.Air
	const offset = camera.CFrame.Position - body.Frame.Position
	const altitude = offset.Magnitude - Worlds.ground(body, camera.CFrame.Position)
	const depth = 1 - Math.smooth(0, spec.Height, altitude)
	const sunward = offset.Unit:Dot(-body.Frame.Position.Unit)
	const day = Math.smooth(-0.2, 0.2, sunward)
	const sunset = 1 - Math.smooth(0, 0.45, math.abs(sunward))
	air.Color = Color3.new(0.05, 0.065, 0.12):Lerp(spec.Day, day):Lerp(spec.Sunset, sunset * 0.5)
	air.Decay = spec.Sunset
	air.Density = depth * math.min(spec.Density * 0.65, 0.45)
	air.Haze = depth * (0.7 + spec.Haze * 1.8)
	air.Glare = depth * sunset * 0.15
	air.Offset = 0.15
end

return Environment
