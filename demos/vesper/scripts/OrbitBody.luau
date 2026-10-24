import "@sim/Clock" as Clock

--- Moves a world along a circular orbit around its parent and turns its surface.
class OrbitBody extends Node3D = {
    export Radius: number = 250
    export AngularSpeed: number = 0.005
    export Phase: number = 0.7
    export Spin: number = 0.012

    function _process(delta: number)
        const angle = self.Phase + Clock.Time * self.AngularSpeed
        self.Position = Vector3.new(math.cos(angle), 0, math.sin(angle)) * self.Radius
        const surface = self:getNodeOrNull("Surface") :: Node3D?
        if surface ~= nil then
            surface.Rotation = Vector3.new(0, Clock.Time * self.Spin, 0)
        end
    end
}
return OrbitBody
