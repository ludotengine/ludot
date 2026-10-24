import "@sim/Clock" as Clock

--- A circular orbit with an independent axial rotation.
class OrbitBody extends Node3D = {
    export Radius: number = 18
    export AngularSpeed: number = 0.032
    export Phase: number = 0.8
    export Spin: number = 0.12
    private Elapsed: number = 0

    function _process(delta: number)
        if Engine:isEditorHint() then return end
        if not Clock.Paused then
            self.Elapsed += delta
        end
        const angle = self.Phase + self.Elapsed * self.AngularSpeed
        self.Position = Vector3(
            math.cos(angle) * self.Radius, 0,
            math.sin(angle) * self.Radius
        )
        self.Rotation = Vector3(0, self.Elapsed * self.Spin, 0)
    end
}
return OrbitBody
