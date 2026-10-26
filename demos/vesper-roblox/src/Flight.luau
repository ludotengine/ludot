--!strict
const ReplicatedStorage = game:GetService("ReplicatedStorage")
const UserInputService = game:GetService("UserInputService")
const Worlds = require(ReplicatedStorage.Client.Vesper.Worlds)
const Tour = require(ReplicatedStorage.Client.Vesper.Tour)

export type State = {
	System: Worlds.System,
	Camera: Camera,
	Mode: string,
	Pitch: number,
	Yaw: number,
	Position: Vector3,
	Speed: number,
	Sensitivity: number,
	Distance: number,
	Focus: Worlds.Body?,
	Anchor: Worlds.Body?,
	LastAnchor: Vector3,
	TourTime: number,
}
const Flight = {}

function Flight.nearest(system: Worlds.System, position: Vector3): Worlds.Body?
	local best: Worlds.Body? = nil
	local score = 10
	for index, body in system.Bodies do
		if index == 1 then
			continue
		end
		const distance = (body.Frame.Position - position).Magnitude / body.Radius
		if distance < score then
			best, score = body, distance
		end
	end
	return best
end

function Flight.new(system: Worlds.System, camera: Camera): State
	const frame = CFrame.lookAt(Vector3.new(-300, 420, 980), Vector3.new(120, 0, 0))
	const pitch, yaw = frame:ToOrientation()
	camera.CameraType = Enum.CameraType.Scriptable
	camera.FieldOfView = 60
	camera.CFrame = frame
	return {
		System = system,
		Camera = camera,
		Mode = "free",
		Pitch = pitch,
		Yaw = yaw,
		Position = frame.Position,
		Speed = 30,
		Sensitivity = 0.0025,
		Distance = 3.2,
		Focus = nil,
		Anchor = nil,
		LastAnchor = Vector3.zero,
		TourTime = 0,
	}
end

function Flight.connect(state: State)
	const numbers = {
		Enum.KeyCode.One,
		Enum.KeyCode.Two,
		Enum.KeyCode.Three,
		Enum.KeyCode.Four,
		Enum.KeyCode.Five,
		Enum.KeyCode.Six,
	}
	const function inputBegan(input: InputObject, processed: boolean)
		if UserInputService:GetFocusedTextBox() ~= nil then
			return
		end
		local planetKey = false
		for _, key in numbers do
			if input.KeyCode == key then
				planetKey = true
				break
			end
		end
		if processed == true and planetKey == false then
			return
		end
		const code = input.KeyCode
		if code == Enum.KeyCode.P then
			state.System.Paused = not state.System.Paused
		end
		if code == Enum.KeyCode.O then
			state.System.OrbitStrength = if state.System.OrbitStrength > 0 then 0 else 0.5
		end
		if code == Enum.KeyCode.T then
			state.Mode = if state.Mode == "tour" then "free" else "tour"
			state.TourTime = 0
		end
		if code == Enum.KeyCode.Escape or code == Enum.KeyCode.Zero then
			state.Mode = "free"
			UserInputService.MouseBehavior = Enum.MouseBehavior.Default
			UserInputService.MouseIconEnabled = true
		end
		for index, key in numbers do
			if code == key then
				state.Mode, state.Focus, state.Distance = "planet", state.System.Bodies[index + 1], 3.2
			end
		end
		if input.UserInputType == Enum.UserInputType.MouseButton2 and state.Mode ~= "tour" then
			UserInputService.MouseBehavior = Enum.MouseBehavior.LockCurrentPosition
			UserInputService.MouseIconEnabled = false
		end
	end
	const function inputEnded(input: InputObject)
		if input.UserInputType == Enum.UserInputType.MouseButton2 then
			UserInputService.MouseBehavior = Enum.MouseBehavior.Default
			UserInputService.MouseIconEnabled = true
		end
	end
	const function inputChanged(input: InputObject)
		if state.Mode == "tour" or UserInputService:GetFocusedTextBox() ~= nil then
			return
		end
		if
			input.UserInputType == Enum.UserInputType.MouseMovement
			and UserInputService:IsMouseButtonPressed(Enum.UserInputType.MouseButton2) == true
		then
			state.Yaw -= input.Delta.X * state.Sensitivity
			state.Pitch = math.clamp(state.Pitch - input.Delta.Y * state.Sensitivity, -1.55, 1.55)
		elseif input.UserInputType == Enum.UserInputType.MouseWheel then
			const factor = if input.Position.Z > 0 then 0.85 else 1.18
			if state.Mode == "planet" then
				state.Distance = math.clamp(state.Distance * factor, 1.25, 12)
			else
				state.Speed = math.clamp(state.Speed / factor, 1, 400)
			end
		end
	end
	UserInputService.InputBegan:Connect(inputBegan)
	UserInputService.InputEnded:Connect(inputEnded)
	UserInputService.InputChanged:Connect(inputChanged)
end

function Flight.tour(state: State, delta: number)
	state.TourTime += delta
	const shot, t = Tour.at(state.TourTime)
	if shot == nil then
		state.TourTime = 0
		return
	end
	state.System.OrbitStrength = shot.Orbits
	if shot.Body == "Belt" then
		const a, r = 0.9 + t * 0.035, 486
		state.Camera.CFrame = CFrame.lookAt(
			Vector3.new(math.cos(a) * r, 9 - t * 2, math.sin(a) * r),
			Vector3.new(math.cos(a + 0.12) * (r - 25), 0, math.sin(a + 0.12) * (r - 25))
		)
		return
	end
	local body = state.System.Bodies[1]
	for _, candidate in state.System.Bodies do
		if candidate.Name == shot.Body then
			body = candidate
			break
		end
	end
	state.System.PreferredTerrain = body
	const origin = body.Frame.Position
	const x = if body.Name == "Sun" then Vector3.xAxis else -origin.Unit
	const y = Vector3.yAxis
	const z = if body.Name == "Sun" then Vector3.zAxis else y:Cross(x).Unit
	const function place(point: Vector3): Vector3
		return origin + (x * point.X + y * point.Y + z * point.Z) * body.Radius
	end
	const look = shot.Look:Lerp(shot.LookTo or shot.Look, t)
	const point = shot.From:Lerp(shot.To, t)
	if shot.Altitude ~= nil then
		const up = (x * point.X + y * point.Y + z * point.Z).Unit
		const ahead = (x - up * x:Dot(up)).Unit
		const right = ahead:Cross(up)
		local ground = 0
		for _, offset in { Vector3.zero, ahead * 0.04, ahead * -0.04, right * 0.04, right * -0.04 } do
			ground += Worlds.ground(body, origin + (up + offset).Unit * body.Radius) / 5
		end
		const position = origin + up * (ground + math.max(body.Radius * shot.Altitude, 0.7))
		state.Camera.CFrame = CFrame.lookAt(position, position + ahead * look.X + up * look.Y + right * look.Z, up)
	else
		state.Camera.CFrame = CFrame.lookAt(place(point), place(look), y)
	end
end

function Flight.update(state: State, delta: number)
	state.System.PreferredTerrain = if state.Mode == "planet" then state.Focus else nil
	if state.Mode == "tour" then
		Flight.tour(state, delta)
		state.Position = state.Camera.CFrame.Position
		state.Pitch, state.Yaw = state.Camera.CFrame:ToOrientation()
		return
	end
	const rotation = CFrame.fromOrientation(state.Pitch, state.Yaw, 0)
	if state.Mode == "planet" then
		const body = state.Focus :: Worlds.Body
		state.Position = body.Frame.Position + rotation.ZVector * body.Radius * state.Distance
	else
		const body = Flight.nearest(state.System, state.Position)
		if body ~= nil then
			if state.Anchor == body then
				state.Position += body.Frame.Position - state.LastAnchor
			end
			state.Anchor, state.LastAnchor = body, body.Frame.Position
		else
			state.Anchor = nil
		end
		if UserInputService:GetFocusedTextBox() == nil then
			const function axis(positive: Enum.KeyCode, negative: Enum.KeyCode): number
				return (if UserInputService:IsKeyDown(positive) == true then 1 else 0)
					- (if UserInputService:IsKeyDown(negative) == true then 1 else 0)
			end
			const move = Vector3.new(
				axis(Enum.KeyCode.D, Enum.KeyCode.A),
				axis(Enum.KeyCode.Space, Enum.KeyCode.LeftControl),
				axis(Enum.KeyCode.S, Enum.KeyCode.W)
			)
			local speed = state.Speed
			if body ~= nil then
				const altitude = (
					(state.Position - body.Frame.Position).Magnitude - Worlds.ground(body, state.Position)
				) / body.Radius
				speed *= math.clamp(altitude, 0.04, 1)
			end
			if UserInputService:IsKeyDown(Enum.KeyCode.LeftShift) == true then
				speed *= 5
			end
			state.Position += rotation:VectorToWorldSpace(move) * speed * math.min(delta, 0.1)
		end
		if body ~= nil then
			const offset = state.Position - body.Frame.Position
			const floor = Worlds.ground(body, state.Position) + math.max(body.Radius * 0.01, 0.7)
			if offset.Magnitude < floor then
				state.Position = body.Frame.Position + offset.Unit * floor
			end
		end
	end
	state.Camera.CFrame = CFrame.new(state.Position) * rotation
end

return Flight
