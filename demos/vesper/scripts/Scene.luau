--- State shared between the system builder and the camera. Modules are ordinary Luau
--- tables, so both scripts see the same bodies without any engine round trips.

export type Body = {
    Node: Node3D, Name: string, Radius: number,
    Surface: Node3D?, Ground: ((direction: Vector3) -> number)?,
}

-- On the web the engine shares the page's main thread. Building the system there in
-- slices keeps an embedding page scrolling while the worlds appear.
const progressive = OS:hasFeature("web")
local sliceStart = os.clock()

const Scene = {
    Bodies = {} :: {Body},
    OrbitStrength = 0.5,
    -- Seconds of building per frame. Flight raises it while nothing is being drawn.
    SliceBudget = 0.008,
    -- Set once every world and the belt exist.
    Built = false,
}

--- Yields to the next frame once this frame has used its building budget. Does nothing
--- on desktop, where the system is built before the first frame.
function Scene.breathe()
    if progressive and os.clock() - sliceStart > Scene.SliceBudget then
        task.wait()
        sliceStart = os.clock()
    end
end

return Scene
