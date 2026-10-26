--!strict
-- Temporarily install as ClientLoader for a Studio client regression, then restore it.
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const RunService = game:GetService("RunService")
const Worlds = require(ReplicatedStorage.Client.Vesper.Worlds)
const Environment = require(ReplicatedStorage.Client.Vesper.Environment)
for _, child in workspace:GetChildren() do
	if child:GetAttribute("VesperPreview") == true then
		child:Destroy()
	end
end
Environment.setup()
const root = Instance.new("Folder")
root.Name = "VesperBudgetTest"
root.Parent = workspace
const system = Worlds.build(root)
Worlds.update(system, 0, Vector3.new(0, 400, 1000), true)
const function test()
	for round = 1, 2 do
		for _, name in { "Ember", "Earth", "Moon", "Mars", "Pluto" } do
			local body = system.Bodies[1]
			for _, candidate in system.Bodies do
				if candidate.Name == name then
					body = candidate
				end
			end
			system.PreferredTerrain = body
			Worlds.streamTerrain(system, body.Frame.Position + Vector3.new(0, 0, body.Radius * 3))
			const deadline = os.clock() + 45
			while system.TerrainLoading == true and os.clock() < deadline do
				RunService.Heartbeat:Wait()
			end
			assert(system.TerrainLoading == false, "Terrain load timeout: " .. name)
			assert(system.ActiveTerrain == body, tostring(root:GetAttribute("TerrainStreamingError")))
			local active = 0
			for _, candidate in system.Bodies do
				for _, patch in candidate.Terrain do
					if patch.Mesh ~= nil then
						active += 1
						assert(candidate == body, "Previous planet retained editable terrain")
					end
				end
			end
			assert(active == 6, "Expected exactly six active terrain patches")
			const ground = Worlds.ground(body, body.Frame.Position + Vector3.new(0, body.Radius * 3, 0))
			assert(ground > body.Radius * 0.5 and ground < body.Radius * 1.6)
			print("BUDGET_PASS", round, name, active, ground)
		end
	end
	system.PreferredTerrain = nil
	Worlds.streamTerrain(system, Vector3.new(0, 50000, 0))
	while system.TerrainLoading == true do
		RunService.Heartbeat:Wait()
	end
	assert(system.ActiveTerrain == nil, "Distant camera retained editable terrain")
	root:SetAttribute("Result", "PASS")
	print("VESPER_CLIENT_BUDGET_REGRESSION_PASS")
end
task.spawn(test)
