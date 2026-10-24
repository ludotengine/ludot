export type Planet = {
    Name: string, Orbit: number, Radius: number, Phase: number,
    Speed: number, Ocean: Vector3, Land: Vector3, Coverage: number,
    Ice: number, Bands: number,
}

-- A deliberately compressed system, sized for exploration rather than astronomy.
return {
    Planets = {
        { Name = "MERCURY", Orbit = 8, Radius = 0.48, Phase = 2.2, Speed = 0.11, Ocean = Vector3(0.15,0.12,0.10), Land = Vector3(0.48,0.38,0.29), Coverage = 0.8, Ice = 0, Bands = 0 },
        { Name = "VENUS", Orbit = 12, Radius = 0.85, Phase = 4.1, Speed = 0.075, Ocean = Vector3(0.39,0.20,0.07), Land = Vector3(0.94,0.66,0.30), Coverage = 0.5, Ice = 0, Bands = 0.5 },
        { Name = "EARTH", Orbit = 18, Radius = 1.5, Phase = 0.8, Speed = 0.032, Ocean = Vector3(0.025,0.20,0.39), Land = Vector3(0.23,0.49,0.26), Coverage = -0.25, Ice = 0.6, Bands = 0 },
        { Name = "MARS", Orbit = 27, Radius = 1.05, Phase = 2.6, Speed = 0.022, Ocean = Vector3(0.42,0.16,0.09), Land = Vector3(0.76,0.29,0.13), Coverage = 0.45, Ice = 0.25, Bands = 0 },
        { Name = "JUPITER", Orbit = 44, Radius = 2.55, Phase = 4.6, Speed = 0.010, Ocean = Vector3(0.37,0.17,0.10), Land = Vector3(0.87,0.66,0.39), Coverage = 0.4, Ice = 0, Bands = 1 },
        { Name = "SATURN", Orbit = 53, Radius = 1.85, Phase = 0.15, Speed = 0.007, Ocean = Vector3(0.40,0.30,0.16), Land = Vector3(0.83,0.73,0.49), Coverage = 0.5, Ice = 0, Bands = 1 },
        { Name = "PLUTO", Orbit = 65, Radius = 0.75, Phase = 3.4, Speed = 0.004, Ocean = Vector3(0.31,0.24,0.25), Land = Vector3(0.72,0.65,0.57), Coverage = 0.1, Ice = 0.8, Bands = 0 },
    },
}
