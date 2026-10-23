extends SceneTree

func _init() -> void:
    var script = load("res://Player.luau")
    var player := CharacterBody2D.new()
    player.set_script(script)
    assert(player.call("addNumbers", 2.0, 3.0) == 5.0)
    var shader = load("res://tint.lushader")
    assert(shader != null and shader.get_diagnostics().is_empty())
    var derived = load("res://DerivedActor.luau")
    var actor := Node2D.new()
    actor.set_script(derived)
    assert(actor.call("total") == 12.0)
    actor.free()
    player.free()
    print("LUDOT_EXPORT_TESTS_PASSED")
    quit()
