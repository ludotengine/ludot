---
layout: home
title: Ludot
titleTemplate: A game engine you script in Luau
---

<Home>

<div class="landing-file">

```lushader
shader spatial
import "./Noise"
import "./Light".{ sunlight }
renderMode("unshaded", "cull_back")

export Pale: vec3 = vec3(0.93, 0.84, 0.68)
export Rust: vec3 = vec3(0.72, 0.42, 0.24)
export Dark: vec3 = vec3(0.38, 0.22, 0.15)
export Storm: vec3 = vec3(0.86, 0.36, 0.2)
export StormCenter: vec3 = vec3(0.55, -0.32, 0.77)

local World: vec3
local Local: vec3
local Up: vec3

function vertex()
    World = (ModelMatrix * vec4(Vertex, 1.0)).xyz
    Local = normalize(Vertex)
    Up = normalize((ModelMatrix * vec4(Vertex, 0.0)).xyz)
end

function fragment()
    local p: vec3 = Local
    -- Twist the coordinates around the storm before sampling the bands.
    local center: vec3 = normalize(StormCenter)
    local spot: number = exp(-length(p - center) * 9.0)
    local twist: number = spot * 5.0 + Time * 0.02
    local swirl: vec3 = mix(p, p * cos(twist) + cross(center, p) * sin(twist), spot)
    local shear: number = fbm(swirl * vec3(2.0, 9.0, 2.0) + vec3(Time * 0.01, 0, 0), 5)
    local latitude: number = swirl.y * 7.5 + shear * 2.4
    local band: number = sin(latitude) * 0.5 + 0.5
    local fine: number = sin(latitude * 3.1 + shear * 5.0) * 0.5 + 0.5
    local color: vec3 = mix(Rust, Pale, smoothstep(0.25, 0.75, band))
    color = mix(color, Dark, smoothstep(0.7, 1.0, fine) * 0.35)
    color = mix(color, Storm, smoothstep(0.25, 0.7, spot))
    local up: vec3 = normalize(Up)
    Albedo = color * sunlight(World, up, up)
end
```

</div>

<div class="landing-file">

```lushader
shader spatial
import "./Noise"
import "./Light".{ sunlight }
renderMode("unshaded", "cull_disabled", "blend_mix", "depth_draw_never")

export Tint: vec3 = vec3(0.96, 0.95, 0.92)
export Cover: number = 0.53 in 0 to 1 step 0.01
export Drift: number = 0.012

local World: vec3
local Local: vec3
local Up: vec3

function vertex()
	World = (ModelMatrix * vec4(Vertex, 1.0)).xyz
	Local = normalize(Vertex)
	Up = normalize((ModelMatrix * vec4(Vertex, 0.0)).xyz)
end

function fragment()
	local swirl: vec3 = Local * 3.0 + vec3(Time * Drift, 0, 0)
	local warp: number = fbm(swirl * 1.3, 3)
	local shape: number = fbm(swirl + vec3(warp * 1.6, warp, 0), 6)
	local bands: number = 0.12 * sin(Local.y * 9.0 + warp * 4.0)
	local cloud: number = smoothstep(Cover, Cover + 0.12, shape + bands)
	local up: vec3 = normalize(Up)
	Albedo = Tint * sunlight(World, up, up) * (0.82 + 0.18 * shape)
	Alpha = cloud * 0.92
end
```

</div>

<div class="landing-file">

```luau
import "@sim/Clock" as Clock

class OrbitBody extends Node3D = {
	export Radius: number = 250
	export AngularSpeed: number = 0.005
	export Phase: number = 0.7

	function _process(delta: number)
		const angle = self.Phase + Clock.Time * self.AngularSpeed
		self.Position = Vector3.new(math.cos(angle), 0, math.sin(angle)) * self.Radius
	end
}

return OrbitBody
```

</div>

</Home>
