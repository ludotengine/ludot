extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
    if not condition:
        failures.append(message)
        push_error(message)

func _init() -> void:
    var script = load("res://Player.luau")
    check(script != null and script.can_instantiate(), "Luau script loads")
    var player := CharacterBody2D.new()
    player.name = "Initial"
    player.set_script(script)
    player.call("_ready")
    player.call("damage", 25.0)
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
    modified = modified.replace("function damage", 'function nativeName(): string return self:getName() end\n    function renameNative(name: string) self.Name = name end\n    function damage')
    script.set_source_code(modified)
    check(script.reload(true) == OK, "State-preserving reload compiles")
    check(player.get("Health") == 75.0, "Reload retains private state")
    check(player.call("valueCount") == 2, "Reload retains Luau tables without converting them")
    player.call("damage", 5.0)
    check(player.get("Health") == 65.0, "Reload changes running methods")
    check(player.call("nativeName") == "Initial", "Native methods use camelCase")
    player.call("renameNative", "Renamed")
    check(player.name == "Renamed", "Native properties use PascalCase")
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
    var shader = load("res://tint.lushader")
    check(shader != null, "Luau shader loads as an engine resource")
    if shader != null:
        var generated: String = shader.get_code()
        shader.set_source("shader canvas_item\nfunction fragment() local t = {} end")
        check(shader.get_code() == generated, "Invalid shader edit retains the previous program")
        check(not shader.get_diagnostics().is_empty(), "Shader diagnostics are available")
    script.set_source_code('class Checker extends Node = { private Health: number = 1 function _ready() self.Health = "wrong" end }')
    var type_errors: Array = script.get_diagnostics()
    check(not type_errors.is_empty(), "Native editor type checker reports type errors")
    print("Native type diagnostics: ", type_errors)
    var completions: PackedStringArray = script.get_completions("class Checker extends Node = { function _ready() self:" + String.chr(0xffff) + " end }")
    check(completions.has("getChildCount"), "Native editor completion includes typed engine methods")
    player.free()
    if failures.is_empty():
        print("LUDOT_ENGINE_TESTS_PASSED")
    quit(0 if failures.is_empty() else 1)
