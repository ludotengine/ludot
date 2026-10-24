--- State shared between the system builder and the camera. Modules are ordinary Luau
--- tables, so both scripts see the same bodies without any engine round trips.

export type Body = {
    Node: Node3D, Name: string, Radius: number,
    Surface: Node3D?, Ground: ((direction: Vector3) -> number)?,
}

return {
    Bodies = {} :: {Body},
    OrbitStrength = 0.5,
}
