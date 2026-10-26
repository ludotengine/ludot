--!strict
-- Planet recipes and palettes from the original Vesper demo.
--- Every world in the system. Sizes and distances are stylized for flying around,
--- not for astronomy.

export type Palette = {
	Deep: Color3,
	Shore: Color3,
	Low: Color3,
	High: Color3,
	Peak: Color3,
	Cliff: Color3,
}

export type Shape = {
	Radius: number,
	Relief: number,
	Resolution: number,
	Seed: number,
	Frequency: number,
	Mountains: number,
	Terraces: number,
	Canyons: number,
	Craters: number,
	CraterSize: number,
	SeaLevel: number?,
	Plains: Vector3?,
}

export type Air = {
	Height: number,
	Density: number,
	Falloff: number,
	Haze: number,
	Day: Color3,
	Sunset: Color3,
}

export type World = {
	Name: string,
	Orbit: number,
	Phase: number,
	Speed: number,
	Spin: number,
	Shape: Shape?,
	Palette: Palette?,
	Air: Air?,
	Clouds: Color3?,
	PolarIce: number,
	Giant: boolean?,
	Moons: { World }?,
}

const function rgb(hex: string): Color3
	return Color3.fromHex(hex)
end

const Sun = { Radius = 46, Color = rgb("#ffb347") }

const Ember: World = {
	Name = "Ember",
	Orbit = 150,
	Phase = 2.1,
	Speed = 0.010,
	Spin = 0.02,
	PolarIce = 0,
	Shape = {
		Radius = 9,
		Relief = 0.09,
		Resolution = 112,
		Seed = 11,
		Frequency = 1.4,
		Mountains = 0.2,
		Terraces = 5,
		Canyons = 0.35,
		Craters = 0,
		CraterSize = 0,
	},
	Palette = {
		Deep = rgb("#5a2414"),
		Shore = rgb("#8a3b1c"),
		Low = rgb("#b24f26"),
		High = rgb("#cf7a45"),
		Peak = rgb("#e8b07a"),
		Cliff = rgb("#7a2f18"),
	},
	Air = { Height = 1.6, Density = 0.42, Falloff = 4.0, Haze = 0.45, Day = rgb("#e0915e"), Sunset = rgb("#f2703a") },
}

const Moon: World = {
	Name = "Moon",
	Orbit = 34,
	Phase = 0.6,
	Speed = 0.09,
	Spin = 0.0,
	PolarIce = 0,
	Shape = {
		Radius = 3.4,
		Relief = 0.07,
		Resolution = 72,
		Seed = 4,
		Frequency = 2.5,
		Mountains = 0,
		Terraces = 0,
		Canyons = 0,
		Craters = 38,
		CraterSize = 0.22,
	},
	Palette = {
		Deep = rgb("#4a4a50"),
		Shore = rgb("#5d5d63"),
		Low = rgb("#7a7a7e"),
		High = rgb("#9c9c9c"),
		Peak = rgb("#c4c2bc"),
		Cliff = rgb("#55555a"),
	},
}

const Earth: World = {
	Name = "Earth",
	Orbit = 250,
	Phase = 0.7,
	Speed = 0.0055,
	Spin = 0.012,
	PolarIce = 0.82,
	Shape = {
		Radius = 14,
		Relief = 0.06,
		Resolution = 128,
		Seed = 3,
		Frequency = 1.1,
		Mountains = 0.55,
		Terraces = 0,
		Canyons = 0,
		Craters = 0,
		CraterSize = 0,
		SeaLevel = 0.04,
	},
	Palette = {
		Deep = rgb("#173a3f"),
		Shore = rgb("#d8c48a"),
		Low = rgb("#4f8a3a"),
		High = rgb("#3b6531"),
		Peak = rgb("#eef2f0"),
		Cliff = rgb("#6b5a4a"),
	},
	Air = { Height = 2.6, Density = 0.5, Falloff = 4.5, Haze = 0.35, Day = rgb("#6fa6ff"), Sunset = rgb("#f2a070") },
	Clouds = rgb("#f4f1ea"),
	Moons = { Moon },
}

const Mars: World = {
	Name = "Mars",
	Orbit = 360,
	Phase = 2.7,
	Speed = 0.0035,
	Spin = 0.011,
	PolarIce = 0.9,
	Shape = {
		Radius = 11,
		Relief = 0.1,
		Resolution = 128,
		Seed = 8,
		Frequency = 1.6,
		Mountains = 0.45,
		Terraces = 0,
		Canyons = 0.9,
		Craters = 22,
		CraterSize = 0.2,
	},
	Palette = {
		Deep = rgb("#5b2516"),
		Shore = rgb("#7c3420"),
		Low = rgb("#a3482a"),
		High = rgb("#c26a3d"),
		Peak = rgb("#e6b48a"),
		Cliff = rgb("#6a2a18"),
	},
	Air = { Height = 1.8, Density = 0.3, Falloff = 3.5, Haze = 0.5, Day = rgb("#d9a07a"), Sunset = rgb("#8fb0d8") },
}

const Giant: World = {
	Name = "Giant",
	Orbit = 600,
	Phase = 4.4,
	Speed = 0.0016,
	Spin = 0.02,
	PolarIce = 0,
	Giant = true,
	Air = { Height = 4.5, Density = 0.8, Falloff = 2.5, Haze = 1, Day = rgb("#f2c28a"), Sunset = rgb("#d86a3a") },
}

const Pluto: World = {
	Name = "Pluto",
	Orbit = 820,
	Phase = 3.5,
	Speed = 0.0009,
	Spin = 0,
	PolarIce = 0.0,
	Shape = {
		Radius = 7,
		Relief = 0.045,
		Resolution = 112,
		Seed = 21,
		Frequency = 1.5,
		Mountains = 0.5,
		Terraces = 0,
		Canyons = 0.2,
		Craters = 18,
		CraterSize = 0.16,
		Plains = Vector3.new(0.92, 0.2, 0.38),
	},
	Palette = {
		Deep = rgb("#4f342a"),
		Shore = rgb("#7a5240"),
		Low = rgb("#9c6a50"),
		High = rgb("#bf8f70"),
		Peak = rgb("#f3ebe2"),
		Cliff = rgb("#5a3b2e"),
	},
	Air = { Height = 0.7, Density = 0.08, Falloff = 3.0, Haze = 1, Day = rgb("#7f95d6"), Sunset = rgb("#a998d6") },
}

return {
	Sun = Sun,
	Worlds = { Ember, Earth, Mars, Giant, Pluto },
	Belt = { Inner = 468, Outer = 506, Count = 1800, Thickness = 7 },
}
