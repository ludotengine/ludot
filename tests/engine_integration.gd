extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
    if not condition:
        failures.append(message)
        push_error(message)

func _init() -> void:
    var nested_script = ClassDB.instantiate("LuauScript")
    nested_script.set_source_code("""class NestedConstruction extends Node = {
    private Child: Node = Node()
    function child() return self.Child end
}""")
    check(nested_script.reload() == OK, "Nested native initializer compiles")
    var parent := Node.new()
    parent.set_script(nested_script)
    var child = parent.call("child")
    check(child is Node and child != parent, "A native field initializer creates a separate child object")
    if child is Node and child != parent:
        child.free()
    parent.free()
    var script = load("res://Player.luau")
    check(script != null and script.can_instantiate(), "Luau script loads")
    script.set_source_code(script.get_source_code().replace("private Values", 'private Identity: {any} = {}\n    function identityStable(): boolean if self.Identity[1] == nil then self.Identity[1] = self end return self.Identity[1] == self end\n    private Values'))
    check(script.reload(true) == OK, "Identity regression setup compiles")
    var player := CharacterBody2D.new()
    player.name = "Initial"
    player.set_script(script)
    player.call("_ready")
    player.call("damage", 25.0)
    check(player.call("identityStable") == true, "An instance can retain a reference to itself")
    check(player.get("Health") == 75.0, "Script method changes instance state")
    check(player.get("Speed") == 1.0, "Export default is available")
    var exported := false
    for property in script.get_script_property_list():
        if property.name == "Speed":
            var limits: PackedStringArray = property.hint_string.split(",")
            exported = property.hint == PROPERTY_HINT_RANGE and limits.size() == 3 and limits[0].to_float() == 0.0 and limits[1].to_float() == 1.0 and is_equal_approx(limits[2].to_float(), 0.1)
    check(exported, "Export range metadata reaches the inspector")
    player.call("pushValue", 2.0)
    check(player.call("valueCount") == 2, "Class defaults allocate a Luau table")
    var modified: String = script.get_source_code().replace("self.Health -= amount", "self.Health -= amount * 2")
    modified = modified.replace("function damage", 'function nativeName(): string return self:getName() end\n    function renameNative(name: string) self.Name = name; assert(self:getName() == name); self:setName(name .. "!"); assert(self.Name == name .. "!") end\n    function damage')
    modified = modified.replace("function damage", """function nativeFeatures(): boolean
        assert(Vector2.Zero == Vector2(0, 0))
        assert(Color.fromHsv(0, 1, 1).R == 1)
        assert(Node.ProcessModeInherit == 0)
        assert(FileAccess.fileExists("res://Player.luau"))
        local values = Array({10, 20})
        values[1] = 7
        assert(values[1] == 7 and values:size() == 2)
        local packed = PackedFloat32Array({1, 2})
        packed[2] = 4
        packed:append(3)
        assert(packed[2] == 4 and packed:size() == 3)
        local timer = Timer()
        local calls = 0
        local function onTimeout() calls += 1 end
        timer.Timeout:connect(onTimeout)
        assert(timer.Timeout:isConnected(onTimeout))
        timer.Timeout:emit()
        timer.Timeout:disconnect(onTimeout)
        timer.Timeout:emit()
        timer:free()
        assert(calls == 1)
        return true
    end
    function damage""")
    script.set_source_code(modified)
    check(script.reload(true) == OK, "State-preserving reload compiles")
    check(player.get("Health") == 75.0, "Reload retains private state")
    check(player.call("identityStable") == true, "Reload preserves object identity for external references")
    check(player.call("valueCount") == 2, "Reload retains Luau tables without converting them")
    player.call("damage", 5.0)
    check(player.get("Health") == 65.0, "Reload changes running methods")
    check(player.call("nativeName") == "Initial", "Native methods use camelCase")
    check(player.call("nativeFeatures") == true, "Static APIs, constants, collections and signal Callables work")
    player.call("renameNative", "Renamed")
    check(player.name == "Renamed!", "Native properties use PascalCase")
    check(player.call("addNumbers", 1.0, 2.0) == 3.0, "Imported function runs in script methods")
    var math_module = load("res://modules/mathutils.luau")
    math_module.set_source_code("return {add = function(a: number, b: number) return a + b + 10 end}")
    check(math_module.reload(true) == OK, "Module reload invalidates dependent scripts")
    check(player.call("addNumbers", 1.0, 2.0) == 13.0, "Dependent import uses the new module")
    check(player.get("Health") == 65.0, "Module reload retains script instance state")
    script.set_source_code("class Broken extends CharacterBody2D = { function")
    check(script.reload(true) != OK, "Invalid reload is rejected")
    player.call("damage", 5.0)
    check(player.get("Health") == 55.0, "Invalid reload retains working methods")
    var derived = load("res://DerivedActor.luau")
    check(derived != null and derived.can_instantiate(), "An imported script class can be extended")
    var actor := Node2D.new()
    actor.set_script(derived)
    check(actor.call("total") == 12.0, "Inherited and derived field defaults run")
    check(actor.call("baseValue") == 10.0, "Inherited methods are available through engine calls")
    var base = derived.get_base_script()
    check(base != null and base.get_global_name() == "BaseActor", "Script inheritance metadata is available")
    if base != null:
        base.set_source_code(base.get_source_code().replace("return self.Life", "return self.Life + 1"))
        check(base.reload(true) == OK, "A base script reloads with live derived instances")
        check(actor.call("baseValue") == 11.0, "Base edits propagate to derived scripts")
        check(actor.get("Armor") == 2.0, "Derived fields survive a base reload")
    actor.free()
    var shader = load("res://tint.lushader")
    check(shader != null, "Luau shader loads as an engine resource")
    if shader != null:
        var generated: String = shader.get_code()
        var valid_source: String = shader.get_source()
        shader.set_source("shader canvas_item\nfunction fragment() local t = {} end")
        check(shader.get_code() == generated, "Invalid shader edit retains the previous program")
        check(not shader.get_diagnostics().is_empty(), "Shader diagnostics are available")
        shader.set_source(valid_source)
        check(generated.contains("vec4 tinted(vec4 color)") and generated.contains("uniform vec4 Tint"), "A shader include reaches the generated code")
        var library = load("res://shaders/tint_common.lushader")
        check(library != null and library.is_library(), "A shader include loads as a library")
        if library != null:
            var library_source: String = library.get_source()
            library.set_source(library_source.replace("color * Tint", "color * Tint * 2.0"))
            check(shader.get_code().contains("((color * Tint) * 2.0)"), "Editing a shader include recompiles its dependents: " + shader.get_code())
            library.set_source(library_source)
            check(not shader.get_code().contains("2.0"), "Restoring a shader include recompiles its dependents")
        var spatial := LuauShader.new()
        spatial.set_source("""shader spatial
renderMode("unshaded", "cull_disabled")
export Cutoff: number = 0.5 in 0 to 1 step 0.05
export Steps: int = 3 in 1 to 8
local Height: number
local Tint: flat<vec3>
local Id: int
local Scale: number = 2
local function wave(x: number): number
    return sin(x * Scale)
end
function vertex()
    Height = wave(Vertex.y + Time)
    Tint = vec3(1)
    Id = 1
end
function fragment()
    local total: number = 0
    for i = 1, Steps do
        if i % 2 == 0 then continue end
        total += Height / float(i)
    end
    local k: int = 0
    while k < 4 do k += 1 end
    if total < Cutoff then discard() elseif k > 3 then Albedo = vec3(total) else Albedo = vec3(0) end
end""")
        check(spatial.get_diagnostics().is_empty(), "Shader loops, helpers, varyings and render modes pass engine validation: " + str(spatial.get_diagnostics()))
        check(spatial.get_code().contains("render_mode unshaded, cull_disabled;"), "Shader render modes reach the engine")
    script.set_source_code('class Checker extends Node = { private Health: number = 1 function _ready() self.Health = "wrong" end }')
    var type_errors: Array = script.get_diagnostics()
    check(not type_errors.is_empty(), "Native editor type checker reports type errors")
    print("Native type diagnostics: ", type_errors)
    var completions: PackedStringArray = script.get_completions("class Checker extends Node = { function _ready() self:" + String.chr(0xffff) + " end }")
    check(completions.has("getChildCount"), "Native editor completion includes typed engine methods")
    completions = script.get_completions('import add from "@hel' + String.chr(0xffff))
    check(completions.has("@helpers"), "Native import completion lists aliases: " + str(completions))
    completions = script.get_completions('import add from "@helpers/' + String.chr(0xffff))
    check(completions.has("@helpers/mathutils"), "Native import completion lists alias modules: " + str(completions))
    completions = script.get_completions('local m = require("@helpers/' + String.chr(0xffff))
    check(completions.has("@helpers/mathutils"), "Native require completion uses aliases")
    completions = script.get_completions('import add from "@helpers/mathutils" ' + String.chr(0xffff))
    check(not completions.has("@helpers/mathutils"), "Path completion stops after a closed import string")
    var ref_script = ClassDB.instantiate("LuauScript")
    ref_script.set_source_code("class Owned extends RefCounted = { private Value: number = 1 }")
    check(ref_script.reload() == OK, "RefCounted script compiles")
    var ref_owner = RefCounted.new()
    var owner_reference = weakref(ref_owner)
    ref_owner.set_script(ref_script)
    ref_owner = null
    check(owner_reference.get_ref() == null, "A script does not retain its RefCounted owner")
    var migrating = ClassDB.instantiate("LuauScript")
    migrating.set_source_code("""class Migrating extends Node = {
    private Count: number = 1
    private Label: string = "a"
    private Flag: boolean = true
    private Items: {number} = {}
    private Spot: Vector2 = Vector2(1, 2)
    function setUp()
        self.Count = 42
        self.Label = "7"
        self.Flag = false
        table.insert(self.Items, 3)
        self.Spot = Vector2(3, 4)
    end
    function itemCount(): number return #self.Items end
}""")
    check(migrating.reload() == OK, "Field migration script compiles")
    var migrated := Node.new()
    migrated.set_script(migrating)
    migrated.call("setUp")
    migrating.set_source_code(migrating.get_source_code().replace("Count: number = 1", "Count: string = \"\"").replace("Label: string = \"a\"", "Label: number = 0").replace("Flag: boolean = true", "Flag: number = 5").replace("Items: {number}", "Items: {any}").replace("Spot: Vector2 = Vector2(1, 2)", "Spot: Vector2i = Vector2i(0, 0)").replace("self.Count = 42", "self.Count = \"42\"").replace("self.Label = \"7\"", "self.Label = 7").replace("self.Flag = false", "self.Flag = 1").replace("self.Spot = Vector2(3, 4)", "self.Spot = Vector2i(3, 4)"))
    check(migrating.reload(true) == OK, "Field type changes reload")
    check(migrated.get("Count") == "42", "A number field converts to string on reload: " + str(migrated.get("Count")))
    check(migrated.get("Label") == 7.0, "A numeric string converts to number on reload: " + str(migrated.get("Label")))
    check(migrated.get("Flag") == 5.0, "An incompatible value resets to the new default: " + str(migrated.get("Flag")))
    check(migrated.call("itemCount") == 1, "A table keeps its contents when the new type accepts them")
    check(migrated.get("Spot") == Vector2i(3, 4), "An engine value converts to the new engine type: " + str(migrated.get("Spot")))
    migrated.free()
    var callbacks = ClassDB.instantiate("LuauScript")
    callbacks.set_source_code("""class Callbacks extends Node = {
    private Hits: number = 0
    private Source: Timer? = nil
    private Stored: (() -> number)? = nil
    function connectAll()
        self.Source = Timer()
        self.Source.Timeout:connect(function()
            self.Hits += 1
        end)
        self.Stored = function(): number
            return 1
        end
    end
    function fire()
        self.Source.Timeout:emit()
    end
    function stored(): number
        return self.Stored()
    end
    function release()
        self.Source:free()
    end
}""")
    check(callbacks.reload() == OK, "Closure reload script compiles")
    var holder := Node.new()
    holder.set_script(callbacks)
    holder.call("connectAll")
    holder.call("fire")
    callbacks.set_source_code(callbacks.get_source_code().replace("self.Hits += 1", "self.Hits += 10").replace("return 1", "return 2"))
    check(callbacks.reload(true) == OK, "Closure reload applies")
    holder.call("fire")
    check(holder.get("Hits") == 11.0, "A connected callback runs its reloaded body: " + str(holder.get("Hits")))
    check(holder.call("stored") == 2.0, "A stored function runs its reloaded body")
    holder.call("release")
    holder.free()
    player.free()
    if failures.is_empty():
        print("LUDOT_ENGINE_TESTS_PASSED")
    quit(0 if failures.is_empty() else 1)
