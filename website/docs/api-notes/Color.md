Construct a color from red, green, blue, and optional alpha components. Typical display colors use components between 0 and 1. High dynamic range workflows can use values above 1.

```luau
const blue = Color.new(0.2, 0.5, 1.0, 1.0)
const white = Color.new(1, 1, 1, 1)
print(blue:lerp(white, 0.25))
```

Default component names are `R`, `G`, `B`, and `A`. Color-space conversion and interpolation are separate operations; choose the method that matches the rendering operation. See [Rendering and materials](../../engine/rendering.md).
