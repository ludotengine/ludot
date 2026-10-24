--- Camera shots for the cinematic tour. Positions are in planet radii, in a frame that
--- points toward the sun, so each shot keeps its lighting wherever the planet is.

export type Shot = {
    Body: string, Duration: number, From: Vector3, To: Vector3,
    Look: Vector3, LookTo: Vector3?, Orbits: number, Altitude: number?,
}

-- Frame axes: X toward the sun, Y up out of the orbital plane, Z along the orbit.
-- Space shots place the camera at From..To and look at Look, in planet radii.
-- Ground shots fly Altitude radii above the terrain at direction From..To and look
-- Look.X ahead toward the sun, Look.Y up, and Look.Z to the right.
const shots: {Shot} = {
    { Body = "Sun", Duration = 6, From = Vector3.new(-6, 7, 21), To = Vector3.new(-3.5, 5.2, 17.5),
        Look = Vector3.new(2.5, 0, 2), Orbits = 0.55 },
    { Body = "Ember", Duration = 7, From = Vector3.new(0.3, 1, -0.3), To = Vector3.new(0.36, 1, 0.08), Altitude = 0.035,
        Look = Vector3.new(1, -0.2, 0), Orbits = 0 },
    { Body = "Earth", Duration = 7, From = Vector3.new(2.2, 0.9, -3.0), To = Vector3.new(2.9, 0.6, -1.8),
        Look = Vector3.new(0, 0, 0), Orbits = 0 },
    { Body = "Earth", Duration = 6, From = Vector3.new(0.55, -0.4, 1), To = Vector3.new(0.6, -0.34, 1), Altitude = 0.03,
        Look = Vector3.new(1, -0.14, -0.1), Orbits = 0 },
    { Body = "Mars", Duration = 6, From = Vector3.new(0.4, 0.2, -1), To = Vector3.new(0.62, 0.36, -1), Altitude = 0.14,
        Look = Vector3.new(1, -0.75, 0.5), Orbits = 0 },
    { Body = "Belt", Duration = 5, From = Vector3.new(0, 0, 0), To = Vector3.new(0, 0, 0),
        Look = Vector3.new(0, 0, 0), Orbits = 0 },
    { Body = "Giant", Duration = 6, From = Vector3.new(2.4, 1.1, -3.6), To = Vector3.new(2.9, 0.75, -2.5),
        Look = Vector3.new(0, 0, 0), Orbits = 0 },
    { Body = "Pluto", Duration = 5, From = Vector3.new(2.4, 0.9, -1.4), To = Vector3.new(2.0, 0.6, -0.4),
        Look = Vector3.new(0, 0, 0), Orbits = 0 },
    { Body = "Sun", Duration = 6, From = Vector3.new(-4, 4, 15), To = Vector3.new(-9, 11, 27),
        Look = Vector3.new(3, 0, 1), Orbits = 0.6 },
}

const function ease(t: number): number
    return t * t * (3 - 2 * t)
end

--- Returns the shot and its progress at a time in the tour, or nil after the last shot.
const function at(time: number): (Shot?, number)
    local start = 0
    for _, shot in shots do
        if time < start + shot.Duration then
            return shot, ease((time - start) / shot.Duration)
        end
        start += shot.Duration
    end
    return nil, 1
end

local total = 0
for _, shot in shots do total += shot.Duration end

return { at = at, Length = total, Shots = shots }
