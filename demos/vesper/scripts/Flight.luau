import "@sim/Clock" as Clock
import "@sim/Tour" as Tour
import "@sim/Scene" as Scene
import type Body from "@sim/Scene"

--- Camera control. Free flight rides along with the nearest world, so you can hover over
--- its surface while it orbits. The planet camera circles one world. The tour plays the
--- recorded shots.

const KEY_W, KEY_A, KEY_S, KEY_D = 87, 65, 83, 68
const KEY_SPACE, KEY_CTRL, KEY_SHIFT, KEY_ESCAPE = 32, 4194326, 4194325, 4194305
const KEY_O, KEY_P, KEY_T, KEY_0 = 79, 80, 84, 48

class Flight extends Node3D = {
    export Speed: number = 30 in 1 to 400 step 1
    export Sensitivity: number = 0.0025
    private Pitch: number = -0.3
    private Yaw: number = 0
    private Mode: string = "free"
    private Anchor: Node3D? = nil
    private AnchorLast: Vector3 = Vector3.new(0, 0, 0)
    private Focus: Body? = nil
    private Distance: number = 4
    private TourTime: number = 0
    private Held: {[number]: boolean} = {}
    private Still: number = -1
    private StillPath: string = ""
    private StillFrames: number = 0

    function _ready()
        const args = OS:getCmdlineUserArgs()
        if args:has("--tour") then self.Mode = "tour" end
        -- --still=<seconds> --shot=<png> renders one moment of the tour, for framing shots.
        for i = 0, args:size() - 1 do
            const arg = args:get(i)
            if string.sub(arg, 1, 8) == "--still=" then
                self.Mode, self.Still = "tour", tonumber(string.sub(arg, 9)) or 0
            elseif string.sub(arg, 1, 7) == "--shot=" then
                self.StillPath = string.sub(arg, 8)
            end
        end
        const camera = self:getNode("Camera") :: Camera3D
        camera.GlobalPosition = Vector3.new(-300, 420, 980)
        camera:lookAt(Vector3.new(120, 0, 0), Vector3.new(0, 1, 0))
        self.Pitch, self.Yaw = camera.Rotation.X, camera.Rotation.Y
    end

    function pressed(key: number): boolean
        const down = Input:isPhysicalKeyPressed(key)
        const edge = down and not self.Held[key]
        self.Held[key] = down
        return edge
    end

    function _unhandledInput(event: InputEvent)
        if self.Mode == "tour" then return end
        if event:isClass("InputEventMouseButton") then
            const button = event :: InputEventMouseButton
            if button.ButtonIndex == 2 then Input.MouseMode = if button.Pressed then 2 else 0 end
            if button.Pressed and button.ButtonIndex == 4 then self:zoom(0.85) end
            if button.Pressed and button.ButtonIndex == 5 then self:zoom(1.18) end
        elseif event:isClass("InputEventMouseMotion") and Input.MouseMode == 2 then
            const motion = event :: InputEventMouseMotion
            self.Yaw -= motion.Relative.X * self.Sensitivity
            self.Pitch = math.clamp(self.Pitch - motion.Relative.Y * self.Sensitivity, -1.55, 1.55)
        end
    end

    function zoom(factor: number)
        if self.Mode == "planet" then
            self.Distance = math.clamp(self.Distance * factor, 1.25, 12)
        else
            self.Speed = math.clamp(self.Speed / factor, 1, 400)
        end
    end

    --- Axes for shots: X toward the sun, Y up, Z along the orbit.
    function frame(body: Body): (Vector3, Vector3, Vector3, Vector3)
        const origin = body.Node.GlobalPosition
        if body.Name == "Sun" then
            return origin, Vector3.new(1, 0, 0), Vector3.new(0, 1, 0), Vector3.new(0, 0, 1)
        end
        const toSun = (origin * -1):normalized()
        const up = Vector3.new(0, 1, 0)
        return origin, toSun, up, up:cross(toSun):normalized()
    end

    function find(name: string): Body
        for _, body in Scene.Bodies do
            if body.Name == name then return body end
        end
        error("No world named " .. name)
    end

    --- Ground height under a world-space point, from the planet's own terrain function.
    function groundAt(body: Body, position: Vector3): number
        const ground = body.Ground
        const surface = body.Surface
        if ground == nil or surface == nil then return body.Radius end
        return ground(surface:toLocal(position))
    end

    function nearest(position: Vector3): Body?
        local best: Body? = nil
        local score = math.huge
        for _, body in Scene.Bodies do
            const distance = body.Node.GlobalPosition:distanceTo(position) / body.Radius
            if body.Name ~= "Sun" and distance < 10 and distance < score then
                best, score = body, distance
            end
        end
        return best
    end

    function _process(delta: number)
        if not Clock.Paused then Clock.Time += delta * Clock.Rate end
        const camera = self:getNode("Camera") :: Camera3D
        if self:pressed(KEY_O) then Scene.OrbitStrength = if Scene.OrbitStrength > 0 then 0 else 0.5 end
        if self:pressed(KEY_P) then Clock.Paused = not Clock.Paused end
        if self:pressed(KEY_T) then
            self.Mode = if self.Mode == "tour" then "free" else "tour"
            self.TourTime = 0
        end
        if self:pressed(KEY_ESCAPE) or self:pressed(KEY_0) then
            self.Mode = "free"
            Input.MouseMode = 0
        end
        for index, body in Scene.Bodies do
            if index > 1 and index <= 10 and self:pressed(48 + index - 1) then
                self.Mode, self.Focus, self.Distance = "planet", body, 3.2
            end
        end
        if self.Still >= 0 then
            Clock.Time, self.TourTime = self.Still, self.Still
            self.StillFrames += 1
            if self.StillFrames == 4 then
                self:getViewport():getTexture():getImage():savePng(self.StillPath)
                self:getTree():quit()
            end
        end
        if self.Mode == "tour" then
            self:tour(camera, delta)
        elseif self.Mode == "planet" then
            self:orbit(camera)
        else
            self:fly(camera, delta)
        end
    end

    function tour(camera: Camera3D, delta: number)
        self.TourTime += delta
        const shot, t = Tour.at(self.TourTime)
        if shot == nil then return end
        Scene.OrbitStrength = shot.Orbits
        if shot.Body == "Belt" then
            -- Thread the asteroid belt, looking along it toward the sun's glow.
            const a = 0.9 + t * 0.035
            const r = 486
            camera.GlobalPosition = Vector3.new(math.cos(a) * r, 9 - t * 2, math.sin(a) * r)
            camera:lookAt(Vector3.new(math.cos(a + 0.12) * (r - 25), 0, math.sin(a + 0.12) * (r - 25)), Vector3.new(0, 1, 0))
            return
        end
        const body = self:find(shot.Body)
        const origin, x, y, z = self:frame(body)
        const scale = body.Radius
        const function place(p: Vector3): Vector3
            return origin + (x * p.X + y * p.Y + z * p.Z) * scale
        end
        const look = shot.Look:lerp(shot.LookTo or shot.Look, t)
        const point = shot.From:lerp(shot.To, t)
        if shot.Altitude ~= nil then
            -- Ground shot: hover over a surface point and look along the sunward horizon.
            const up = (x * point.X + y * point.Y + z * point.Z):normalized()
            const ahead = (x - up * x:dot(up)):normalized()
            const right = ahead:cross(up)
            -- Average the ground around the point so the camera glides over peaks.
            local ground = 0
            for _, offset in {Vector3.new(0, 0, 0), ahead * 0.04, ahead * -0.04, right * 0.04, right * -0.04} do
                ground += self:groundAt(body, origin + (up + offset):normalized() * scale) / 5
            end
            const position = origin + up * (ground + scale * shot.Altitude)
            camera.GlobalPosition = position
            camera:lookAt(position + ahead * look.X + up * look.Y + right * look.Z, up)
            return
        end
        camera.GlobalPosition = place(point)
        camera:lookAt(place(look), y)
    end

    function orbit(camera: Camera3D)
        const body = self.Focus :: Body
        const center = body.Node.GlobalPosition
        camera.Rotation = Vector3.new(self.Pitch, self.Yaw, 0)
        camera.GlobalPosition = center + camera.GlobalBasis.Z * body.Radius * self.Distance
    end

    function fly(camera: Camera3D, delta: number)
        -- Ride along with the nearest world so its surface holds still beneath you.
        const body = self:nearest(camera.GlobalPosition)
        if body ~= nil then
            if self.Anchor == body.Node then
                camera.GlobalPosition += body.Node.GlobalPosition - self.AnchorLast
            end
            self.Anchor = body.Node
            self.AnchorLast = body.Node.GlobalPosition
        else
            self.Anchor = nil
        end
        camera.Rotation = Vector3.new(self.Pitch, self.Yaw, 0)
        const function axis(positive: number, negative: number): number
            return (if Input:isPhysicalKeyPressed(positive) then 1 else 0) - (if Input:isPhysicalKeyPressed(negative) then 1 else 0)
        end
        const move = Vector3.new(axis(KEY_D, KEY_A), axis(KEY_SPACE, KEY_CTRL), axis(KEY_S, KEY_W))
        local speed = self.Speed
        if body ~= nil then
            -- Slow down near the ground.
            const altitude = (camera.GlobalPosition:distanceTo(body.Node.GlobalPosition) - self:groundAt(body, camera.GlobalPosition)) / body.Radius
            speed *= math.clamp(altitude, 0.04, 1)
        end
        if Input:isPhysicalKeyPressed(KEY_SHIFT) then speed *= 5 end
        camera:translateObjectLocal(move * speed * delta)
        if body ~= nil then
            -- Keep above the ground.
            const offset = camera.GlobalPosition - body.Node.GlobalPosition
            const floor = self:groundAt(body, camera.GlobalPosition) + body.Radius * 0.01
            if offset:length() < floor then
                camera.GlobalPosition = body.Node.GlobalPosition + offset:normalized() * floor
            end
        end
    end
}
return Flight
