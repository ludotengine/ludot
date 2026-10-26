--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const RunService = game:GetService("RunService")
const Lighting = game:GetService("Lighting")
const Worlds = require(ReplicatedStorage.Client.Vesper.Worlds)
const Flight = require(ReplicatedStorage.Client.Vesper.Flight)
const Environment = require(ReplicatedStorage.Client.Vesper.Environment)

const function start()
	const camera = workspace.CurrentCamera :: Camera
	camera.CameraType = Enum.CameraType.Scriptable
	camera.FieldOfView = 60
	camera.CFrame = CFrame.lookAt(Vector3.new(-300, 420, 980), Vector3.new(120, 0, 0))
	-- The edit-mode preview is replaced by assets generated on this client.
	for _, child in Lighting:GetChildren() do
		if child:GetAttribute("VesperPreview") == true then
			child:Destroy()
		end
	end
	for _, child in workspace:GetChildren() do
		if child:GetAttribute("VesperPreview") == true then
			child:Destroy()
		end
	end
	const air = Environment.setup()
	const root = Instance.new("Folder")
	root.Name = "Vesper"
	root.Parent = workspace
	const system = Worlds.build(root)
	const flight = Flight.new(system, camera)
	Flight.connect(flight)
	const function update(delta: number)
		Worlds.update(system, delta, camera.CFrame.Position, false)
		Flight.update(flight, delta)
		Environment.update(air, system, camera)
	end
	RunService:BindToRenderStep("Vesper", Enum.RenderPriority.Camera.Value + 1, update)
	update(0)
	root:SetAttribute("Ready", true)
	print("Vesper ready: original terrain, six worlds, editable meshes and images.")
end

return { start = start }
