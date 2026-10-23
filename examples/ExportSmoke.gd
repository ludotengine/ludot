extends Node

# Release templates strip assert(), so every check must fail the process explicitly.
func check(condition: bool, message: String) -> bool:
    if not condition:
        printerr("Export check failed: " + message)
        get_tree().quit(1)
    return condition

func _ready() -> void:
    var script = load("res://Player.luau")
    var player := CharacterBody2D.new()
    player.set_script(script)
    var sum = player.call("addNumbers", 2.0, 3.0)
    player.free()
    if not check(sum == 5.0, "Aliased import runs in an exported script"): return
    var shader = load("res://tint.lushader")
    if not check(shader != null and shader.get_diagnostics().is_empty(), "Luau shader loads from the pack"): return
    var actor := Node2D.new()
    actor.set_script(load("res://DerivedActor.luau"))
    var total = actor.call("total")
    actor.free()
    if not check(total == 12.0, "Derived Luau script runs from the pack"): return
    print("LUDOT_EXPORT_TESTS_PASSED")
    get_tree().quit()
