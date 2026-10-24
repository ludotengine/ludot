import "@sim/System" as System
import "@sim/Clock" as Clock

const function material(path: string)
    const shader = ResourceLoader:load(path) :: LuauShader
    assert(shader:getDiagnostics() == "", path .. ": " .. shader:getDiagnostics())
    const surface = ShaderMaterial()
    surface.Shader = shader
    return surface
end

const function sphere(radius: number, segments: number, rings: number)
    const mesh = SphereMesh()
    mesh.Radius = radius
    mesh.Height = radius * 2
    mesh.RadialSegments = segments
    mesh.Rings = rings
    return mesh
end

const function label(parent: Node, text: string, x: number, y: number, size: number, color: Color)
    const item = Label()
    item.Text = text
    item.Position = Vector2(x, y)
    item:addThemeFontSizeOverride("font_size", size)
    item:addThemeColorOverride("font_color", color)
    parent:addChild(item)
    return item
end

--- Builds the complete scene from native Luau bindings. No GDScript bootstrap.
class SolarSystem extends Node3D = {
    private Bodies: {[string]: Node3D} = {}
    private Ui: {[string]: Label} = {}
    private Halos: {MeshInstance3D} = {}
    private Markers: {{Body: Node3D, Marker: Label3D}} = {}
    private CodeViews: {CodeEdit} = {}
    private Orbits: Node3D? = nil
    private CodePanel: Control? = nil
    private CodeScrolled: boolean = false

    function _ready()
        if Engine:isEditorHint() then return end
        math.randomseed(731)
        const environment = Environment()
        environment.BackgroundMode = 1
        environment.BackgroundColor = Color(0.003, 0.005, 0.012)
        environment.TonemapMode = 0
        const world = WorldEnvironment()
        world.Environment = environment
        self:addChild(world)

        const orbitRoot = Node3D()
        orbitRoot.Name = "Orbits"
        self:addChild(orbitRoot)
        self.Orbits = orbitRoot
        const orbitInk = material("res://shaders/Ink.lushader")
        orbitInk:setShaderParameter("Tint", Vector4(0.16, 0.35, 0.43, 0.4))
        for _, spec in System.Planets do
            const mesh = ImmediateMesh()
            mesh:surfaceBegin(1, orbitInk)
            for i = 0, 255 do
                const a = i / 256 * math.pi * 2
                const b = (i + 1) / 256 * math.pi * 2
                mesh:surfaceAddVertex(Vector3(math.cos(a) * spec.Orbit, 0, math.sin(a) * spec.Orbit))
                mesh:surfaceAddVertex(Vector3(math.cos(b) * spec.Orbit, 0, math.sin(b) * spec.Orbit))
            end
            mesh:surfaceEnd()
            const orbit = MeshInstance3D()
            orbit.Mesh = mesh
            orbitRoot:addChild(orbit)
        end

        const star = MeshInstance3D()
        star.Name = "Sun"
        star.Mesh = sphere(3.6, 28, 14)
        star.MaterialOverride = material("res://shaders/Sun.lushader")
        self:addChild(star)
        const corona = MeshInstance3D()
        const quad = QuadMesh()
        quad.Size = Vector2(22, 22)
        corona.Mesh = quad
        corona.MaterialOverride = material("res://shaders/Corona.lushader")
        self:addChild(corona)
        table.insert(self.Halos, corona)

        const orbitScript = ResourceLoader:load("res://scripts/OrbitBody.luau")
        for _, spec in System.Planets do
            const body = Node3D()
            body.Name = spec.Name
            body:setScript(orbitScript)
            body:set("Radius", spec.Orbit)
            body:set("AngularSpeed", spec.Speed)
            body:set("Phase", spec.Phase)
            body:set("Spin", 0.1)
            body.Position = Vector3(math.cos(spec.Phase) * spec.Orbit, 0, math.sin(spec.Phase) * spec.Orbit)
            const skin = material("res://shaders/Planet.lushader")
            skin:setShaderParameter("Ocean", spec.Ocean)
            skin:setShaderParameter("Land", spec.Land)
            skin:setShaderParameter("Coverage", spec.Coverage)
            skin:setShaderParameter("Ice", spec.Ice)
            skin:setShaderParameter("Bands", spec.Bands)
            if spec.Name == "PLUTO" then skin:setShaderParameter("Ambient", 0.38) end
            const surface = MeshInstance3D()
            surface.Mesh = sphere(spec.Radius, 24, 12)
            surface.MaterialOverride = skin
            body:addChild(surface)
            self:addChild(body)
            self.Bodies[spec.Name] = body
            const marker = Label3D()
            marker.Text = spec.Name
            marker.FontSize = 42
            marker.PixelSize = 0.03
            marker.Billboard = 1
            marker.Modulate = Color(0.61, 0.74, 0.77, 0.85)
            marker.Position = Vector3(0, spec.Radius + 0.7, 0)
            body:addChild(marker)
            table.insert(self.Markers, {Body = body, Marker = marker})
            if spec.Name == "EARTH" then
                const shell = MeshInstance3D()
                shell.Mesh = sphere(spec.Radius * 1.09, 32, 16)
                shell.MaterialOverride = material("res://shaders/Atmosphere.lushader")
                body:addChild(shell)
                const moon = MeshInstance3D()
                moon.Mesh = sphere(0.35, 12, 6)
                moon.Position = Vector3(3.3, 0.35, 0)
                const moonSkin = material("res://shaders/Planet.lushader")
                moonSkin:setShaderParameter("Ocean", Vector3(0.23, 0.24, 0.26))
                moonSkin:setShaderParameter("Land", Vector3(0.62, 0.64, 0.61))
                moonSkin:setShaderParameter("Coverage", 0.7)
                moon.MaterialOverride = moonSkin
                body:addChild(moon)
            end
            if spec.Name == "SATURN" then
                const ringMesh = ImmediateMesh()
                const ringSkin = material("res://shaders/Ink.lushader")
                ringSkin:setShaderParameter("Tint", Vector4(0.60, 0.45, 0.25, 0.16))
                ringMesh:surfaceBegin(1, ringSkin)
                for ring = 1, 14 do
                    const radius = 2.35 + ring * 0.07
                    for i = 0, 127 do
                        const a = i / 128 * math.pi * 2
                        const b = (i + 1) / 128 * math.pi * 2
                        ringMesh:surfaceAddVertex(Vector3(math.cos(a) * radius, math.cos(a) * 0.35, math.sin(a) * radius))
                        ringMesh:surfaceAddVertex(Vector3(math.cos(b) * radius, math.cos(b) * 0.35, math.sin(b) * radius))
                    end
                end
                ringMesh:surfaceEnd()
                const rings = MeshInstance3D()
                rings.Mesh = ringMesh
                body:addChild(rings)
            end
        end

        const rockMesh = sphere(1, 7, 3)
        const rockSkin = material("res://shaders/Planet.lushader")
        rockSkin:setShaderParameter("Ocean", Vector3(0.12, 0.10, 0.09))
        rockSkin:setShaderParameter("Land", Vector3(0.40, 0.31, 0.22))
        rockSkin:setShaderParameter("Coverage", 0.65)
        rockSkin:setShaderParameter("Ice", 0)
        for i = 1, 260 do
            const a = math.random() * math.pi * 2
            const radius = 35 + math.random() * 5
            const rock = MeshInstance3D()
            rock.Mesh = rockMesh
            rock.MaterialOverride = rockSkin
            rock.Position = Vector3(math.cos(a) * radius, (math.random() - 0.5) * 1.4, math.sin(a) * radius)
            const size = 0.10 + math.random() * 0.23
            rock.Scale = Vector3(size * 1.4, size * 0.7, size)
            rock.Rotation = Vector3(math.random() * 5, math.random() * 5, math.random() * 5)
            self:addChild(rock)
        end

        const skyMesh = ImmediateMesh()
        const skySkin = material("res://shaders/Ink.lushader")
        skySkin:setShaderParameter("Tint", Vector4(0.70, 0.79, 0.91, 0.8))
        skySkin:setShaderParameter("Width", 1.5)
        skyMesh:surfaceBegin(0, skySkin)
        for i = 1, 1100 do
            const a = math.random() * math.pi * 2
            const y = math.random() * 2 - 1
            const r = math.sqrt(1 - y * y)
            skyMesh:surfaceAddVertex(Vector3(math.cos(a) * r, y, math.sin(a) * r) * 900)
        end
        skyMesh:surfaceEnd()
        const stars = MeshInstance3D()
        stars.Mesh = skyMesh
        self:addChild(stars)
        self:buildHud()
        print("VESPER_READY: 7 planets, 260 asteroids, 1100 stars; all Luau + lushader")
    end

    function getBody(name: string): Node3D
        return self.Bodies[name]
    end

    function setChapter(title: string, detail: string)
        self.Ui.Chapter.Text = title
        self.Ui.Detail.Text = detail
    end

    function showCode(visible: boolean)
        if self.CodePanel ~= nil then self.CodePanel.Visible = visible end
        if not visible then self.CodeScrolled = false end
    end

    function toggleOrbits()
        if self.Orbits ~= nil then self.Orbits.Visible = not self.Orbits.Visible end
    end

    function buildHud()
        const layer = CanvasLayer()
        layer.Name = "HUD"
        self:addChild(layer)
        const root = Control()
        root.Size = Vector2(1280, 720)
        root.MouseFilter = 2
        layer:addChild(root)
        const white = Color(0.91, 0.92, 0.87)
        const dim = Color(0.46, 0.61, 0.65)
        label(root, "LUDOT ENGINE  /  FIELD NOTES 001", 34, 26, 12, dim)
        label(root, "VESPER", 32, 43, 38, white)
        label(root, "a miniature solar system", 35, 89, 14, Color(0.63, 0.69, 0.71))
        label(root, "LUAU + LUSHADER", 1084, 32, 12, Color(0.93, 0.63, 0.32))
        self.Ui.Chapter = label(root, "A SMALL UNIVERSE", 35, 578, 27, white)
        self.Ui.Detail = label(root, "Seven worlds, one warm star. Nothing is quite to scale.", 36, 617, 15, Color(0.62, 0.70, 0.73))
        label(root, "WASD  MOVE    RMB  LOOK    SHIFT  BOOST    SPACE / CTRL  VERTICAL    P  PAUSE    O  ORBITS    R  TOUR    C  CODE", 35, 679, 12, dim)

        const panel = ColorRect()
        panel.Position = Vector2(680, 130)
        panel.Size = Vector2(565, 516)
        panel.Color = Color(0.018, 0.027, 0.041, 0.97)
        root:addChild(panel)
        self.CodePanel = panel
        panel.Visible = false
        label(panel, "LIVE SOURCE  /  NO GDSCRIPT", 18, 13, 12, Color(0.93, 0.63, 0.32))
        const font = SystemFont()
        font.FontNames = PackedStringArray({"Adwaita Mono", "DejaVu Sans Mono", "monospace"})
        for i, path in {"res://scripts/OrbitBody.luau", "res://shaders/Atmosphere.lushader"} do
            label(panel, path:gsub("res://", ""), 18, 44 + (i - 1) * 230, 12, Color(0.51, 0.76, 0.82))
            const code = CodeEdit()
            code.Position = Vector2(14, 67 + (i - 1) * 230)
            code.Size = Vector2(538, 194)
            code.Editable = false
            code:setDrawLineNumbers(true)
            code.Text = FileAccess.getFileAsString(path)
            code:addThemeFontOverride("font", font)
            code:addThemeFontSizeOverride("font_size", 11)
            code:addThemeColorOverride("font_color", Color(0.82, 0.85, 0.85))
            code:addThemeColorOverride("font_readonly_color", Color(0.82, 0.85, 0.85))
            const highlighter = CodeHighlighter()
            highlighter:setSymbolColor(Color(0.65, 0.72, 0.75))
            highlighter:setNumberColor(Color(0.93, 0.67, 0.40))
            highlighter:setFunctionColor(Color(0.53, 0.77, 0.91))
            highlighter:setMemberVariableColor(Color(0.77, 0.82, 0.87))
            for _, word in {"class", "extends", "export", "private", "const", "local", "function", "end", "if", "then", "return", "import", "as", "shader", "renderMode"} do
                highlighter:addKeywordColor(word, Color(0.86, 0.56, 0.53))
            end
            for _, word in {"number", "boolean", "vec3", "vec4", "Node3D"} do
                highlighter:addKeywordColor(word, Color(0.50, 0.77, 0.70))
            end
            highlighter:addColorRegion("--", "", Color(0.44, 0.53, 0.56), true)
            highlighter:addColorRegion('"', '"', Color(0.76, 0.80, 0.55), false)
            code.SyntaxHighlighter = highlighter
            panel:addChild(code)
            table.insert(self.CodeViews, code)
        end
    end

    function _process(_delta: number)
        if Engine:isEditorHint() then return end
        if self.CodePanel ~= nil and self.CodePanel.Visible and not self.CodeScrolled then
            self.CodeViews[1]:setVScroll(12)
            self.CodeViews[2]:setVScroll(3)
            self.CodeScrolled = true
        end
        const camera = self:getNode("Flight/Camera")
        for _, item in self.Markers do
            const distance = camera.GlobalPosition:distanceTo(item.Body.GlobalPosition)
            item.Marker.PixelSize = math.clamp(distance * 0.00045, 0.005, 0.06)
        end
        for _, halo in self.Halos do
            halo.GlobalRotation = camera.GlobalRotation
        end
    end
}
return SolarSystem
