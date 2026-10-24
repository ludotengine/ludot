import "@sim/Clock" as Clock

--- Free flight, with a repeatable cinematic path for the captured demo.
class Flight extends Node3D = {
    export Speed: number = 12 in 1 to 80 step 1
    private Pitch: number = -0.5
    private Yaw: number = 0
    private Tour: boolean = false
    private Time: number = 0
    private Held: {[number]: boolean} = {}
    private CodeVisible: boolean = false

    function _ready()
        if Engine:isEditorHint() then return end
        const camera = self:getNode("Camera")
        camera:lookAt(Vector3(0, 0, 0), Vector3(0, 1, 0))
        self.Pitch = camera.Rotation.X
        self.Yaw = camera.Rotation.Y
        self.Tour = OS:getCmdlineUserArgs():has("--tour")
    end

    function pressed(key: number): boolean
        const down = Input:isPhysicalKeyPressed(key)
        const edge = down and not self.Held[key]
        self.Held[key] = down
        return edge
    end

    function _unhandledInput(event: InputEvent)
        if Engine:isEditorHint() then return end
        if event:isClass("InputEventMouseButton") then
            const button = event :: InputEventMouseButton
            if button.ButtonIndex == 2 then
                Input.MouseMode = if button.Pressed then 2 else 0
            end
        end
        if event:isClass("InputEventMouseMotion") and Input:isMouseButtonPressed(2) then
            const motion = event :: InputEventMouseMotion
            self.Tour = false
            self.Yaw -= motion.Relative.X * 0.003
            self.Pitch = math.clamp(self.Pitch - motion.Relative.Y * 0.003, -1.5, 1.5)
        end
    end

    function focus(name: string)
        const camera = self:getNode("Camera")
        const body = self:getParent():getBody(name)
        const p = body.GlobalPosition
        camera.GlobalPosition = p - p:normalized() * 7 + Vector3(2, 3, 2)
        camera:lookAt(p, Vector3(0, 1, 0))
        self.Pitch = camera.Rotation.X
        self.Yaw = camera.Rotation.Y
        self.Tour = false
    end

    function _process(delta: number)
        if Engine:isEditorHint() then return end
        const camera = self:getNode("Camera")
        const game = self:getParent()
        if self:pressed(4194305) then Input.MouseMode = 0 end
        if self:pressed(82) then
            self.Tour = not self.Tour
            self.Time = 0
            self.Pitch = camera.Rotation.X
            self.Yaw = camera.Rotation.Y
        end
        if self:pressed(80) then Clock.Paused = not Clock.Paused end
        if self:pressed(79) then game:toggleOrbits() end
        if self:pressed(67) then self.CodeVisible = not self.CodeVisible end
        if self:pressed(49) then self:focus("EARTH") end
        if self:pressed(50) then self:focus("MARS") end
        if self:pressed(51) then self:focus("PLUTO") end
        if self:pressed(48) then
            self.Tour = false
            camera.GlobalPosition = Vector3(0, 62, 100)
            camera:lookAt(Vector3(0, 0, 0), Vector3(0, 1, 0))
            self.Pitch = camera.Rotation.X
            self.Yaw = camera.Rotation.Y
        end
        if self.Tour then
            self.Time += delta
            const t = self.Time
            local target = Vector3(0, 0, 0)
            local position = Vector3(math.sin(t * 0.035) * 18, 62, 100)
            if t < 6 then
                game:setChapter("A SMALL UNIVERSE", "Seven worlds, one warm star. A solar system made entirely with Luau.")
            elseif t < 15 then
                const p = game:getBody("EARTH").GlobalPosition
                const angle = (t - 6) * 0.07
                target = p
                position = p - p:normalized() * 6.5 + Vector3(math.sin(angle) * 3, 2.8, math.cos(angle) * 2)
                game:setChapter("01  /  THE BLUE WORLD", "Faceted continents, polar ice, a thin atmosphere, and one small moon.")
            elseif t < 23 then
                const p = game:getBody("MARS").GlobalPosition
                target = p
                position = p - p:normalized() * 5.8 + Vector3(2.4, 2.1, math.sin(t * 0.2) * 2)
                game:setChapter("02  /  RUST & DUST", "A quiet red planet on the inner edge of the asteroid belt.")
            elseif t < 30 then
                const a = (t - 23) * 0.035 + 0.2
                position = Vector3(math.cos(a) * 38, 0.35 + math.sin(t * 0.3) * 0.25, math.sin(a) * 38)
                target = Vector3(0, 0, 0)
                game:setChapter("03  /  THE DRIFT", "260 low-poly fragments, threaded by the visible orbital paths.")
            elseif t < 38 then
                const p = game:getBody("PLUTO").GlobalPosition
                target = p
                position = p + p:normalized() * 6 + Vector3(-2.5, 2, 1)
                game:setChapter("04  /  THE FAR SHORE", "An icy dwarf world. The sun is a distant point of warmth.")
            else
                position = Vector3(6, 62, 105)
                target = Vector3(22, 0, 0)
                game:setChapter("UNDER THE HOOD", "Native Luau orbits. Native lushader atmospheres. Live source, live simulation.")
            end
            -- Short eased camera transitions keep the tour a continuous flight.
            const blend = math.min(1, delta * 2.3)
            camera.GlobalPosition = camera.GlobalPosition:lerp(position, blend)
            camera:lookAt(target, Vector3(0, 1, 0))
            game:showCode(t >= 38 or self.CodeVisible)
            self.Pitch = camera.Rotation.X
            self.Yaw = camera.Rotation.Y
        else
            camera.Rotation = Vector3(self.Pitch, self.Yaw, 0)
            local x = 0
            local y = 0
            local z = 0
            if Input:isPhysicalKeyPressed(87) then z -= 1 end
            if Input:isPhysicalKeyPressed(83) then z += 1 end
            if Input:isPhysicalKeyPressed(65) then x -= 1 end
            if Input:isPhysicalKeyPressed(68) then x += 1 end
            if Input:isPhysicalKeyPressed(32) then y += 1 end
            if Input:isPhysicalKeyPressed(4194326) then y -= 1 end
            local speed = self.Speed
            if Input:isPhysicalKeyPressed(4194325) then speed *= 4 end
            camera:translateObjectLocal(Vector3(x, y, z) * delta * speed)
            game:setChapter("FREE FLIGHT", "1  Earth    2  Mars    3  Pluto    0  Overview    |    R  Start the cinematic tour")
            game:showCode(self.CodeVisible)
        end
    end
}
return Flight
