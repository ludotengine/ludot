@tool
extends EditorPlugin

var failures: Array[String] = []

func check(value: bool, message: String) -> void:
    if not value:
        failures.append(message)
        push_error(message)

func _enter_tree() -> void:
    run.call_deferred()

func run() -> void:
    for frame in range(8):
        await get_tree().process_frame
    EditorInterface.open_scene_from_path("res://main.tscn")
    for frame in range(4):
        await get_tree().process_frame
    var script = load("res://Player.luau")
    EditorInterface.edit_script(script)
    for frame in range(3):
        await get_tree().process_frame
    var editor = EditorInterface.get_script_editor().get_current_editor()
    var code: CodeEdit = editor.get_base_editor()
    var highlighter = code.syntax_highlighter
    check(highlighter.get_class() == "LuauSyntaxHighlighter", "Luau selects its highlighter automatically")
    if highlighter.get_class() != "LuauSyntaxHighlighter":
        get_tree().quit(1)
        return
    var normal: Color = code.get_theme_color("font_color")
    check(highlighter.get_line_syntax_highlighting(0)[0].color != normal, "class keyword has a color")
    check(highlighter.get_line_syntax_highlighting(3)[14].color != normal, "strings have a color")
    var theme = EditorInterface.get_editor_theme()
    check(theme.has_icon("LuauScript", "EditorIcons"), "Luau resource icon is available")
    var icon_size = theme.get_icon("LuauScript", "EditorIcons").get_size()
    check(icon_size.x > 0 and icon_size.x == icon_size.y, "Luau icon is square at the editor scale")
    # The real editor highlighter must handle Unicode columns, interpolation,
    # arbitrary long brackets, and edits that change a multiline token.
    code.text = 'local café = "hello" -- comment\nlocal s = [==========[hello\nworld]==========]\nlocal x = `value {1 + 2}`'
    var colors: Dictionary = highlighter.get_line_syntax_highlighting(0)
    check(colors.has(13), "UTF-8 string starts at the TextEdit code-point column")
    check(highlighter.get_line_syntax_highlighting(2)[0].color != normal, "Long bracket strings span lines")
    check(highlighter.get_line_syntax_highlighting(3).size() > 4, "Interpolation expressions receive their own colors")
    code.text = 'local s = 1\nlocal nextValue = 2'
    check(highlighter.get_line_syntax_highlighting(1)[0].color != normal, "Edits invalidate multiline highlighting")
    code.text = 'class Player extends Node = { function _ready() self: end }'
    code.set_caret_line(0)
    code.set_caret_column(code.text.find("self:") + 5)
    code.request_code_completion(true)
    var found_method = false
    for option in code.get_code_completion_options():
        if option.display_text == "getChildCount":
            found_method = true
            check(option.kind == CodeEdit.KIND_FUNCTION, "Native methods use function completion icons")
            check(option.icon != null, "Native method completion includes an icon")
    check(found_method, "Typed native method completion reaches CodeEdit")
    var original_source: String = script.source_code
    var scene_source = 'class Player extends Node = { function _ready() const node = self:getNode("..") node:Start() end }'
    script.source_code = scene_source
    check(script.get_diagnostics().is_empty(), "Parent lookup infers attached script without a cast: " + str(script.get_diagnostics()))
    var scene_completions = script.get_completions('class Player extends Node = { function _ready() const node = self:getNode("..") node:' + String.chr(0xffff) + ' end }')
    check(scene_completions.has("Start"), "Parent lookup completion preserves custom script method names")
    script.source_code = scene_source.replace('const node =', 'const node: NodeNode =')
    check(script.get_diagnostics().is_empty(), "Script class names work directly as instance annotations")
    script.source_code = scene_source.replace('node:Start()', 'node:Add("wrong")')
    check(not script.get_diagnostics().is_empty(), "Inferred script methods retain their argument types")
    script.source_code = scene_source.replace('node:Start()', 'node:MissingMethod()')
    check(not script.get_diagnostics().is_empty(), "Inferred nodes reject unknown methods")
    script.source_code = scene_source.replace('self:getNode("..")', 'self:getParent()')
    check(script.get_diagnostics().is_empty(), "getParent infers the attached script")
    script.source_code = scene_source.replace('self:getNode("..")', 'self:getNodeOrNull("..")')
    check(not script.get_diagnostics().is_empty(), "getNodeOrNull retains its nullable result")
    script.source_code = scene_source.replace('self:getNode("..")', 'self:getNodeOrNull("..")').replace('node:Start()', 'if node ~= nil then node:Start() end')
    check(script.get_diagnostics().is_empty(), "Nullable node results narrow after a nil check")
    var parent_script = EditorInterface.get_edited_scene_root().get_script()
    var parent_source: String = parent_script.source_code
    parent_script.source_code = parent_source.replace("function Start()", "function Begin()")
    scene_completions = script.get_completions('class Player extends Node = { function _ready() const node = self:getNode("..") node:' + String.chr(0xffff) + ' end }')
    check(scene_completions.has("Begin") and not scene_completions.has("Start"), "Changing the parent script invalidates inferred scene types")
    parent_script.source_code = parent_source
    script.source_code = original_source
    # Exercise the same ScriptCreateDialog used by Attach Script.
    var dialog = ClassDB.instantiate("ScriptCreateDialog")
    EditorInterface.get_base_control().add_child(dialog)
    dialog.config("CharacterBody2D", "res://Hero.luau", false, false)
    dialog.popup_centered()
    await get_tree().process_frame
    var selectors = dialog.find_children("*", "OptionButton", true, false)
    var language_selector: OptionButton
    for selector in selectors:
        for i in range(selector.item_count):
            if selector.get_item_text(i) == "Luau":
                language_selector = selector
                selector.select(i)
                selector.item_selected.emit(i)
                break
    check(language_selector != null, "Script dialog offers Luau")
    for checkbox in dialog.find_children("*", "CheckBox", true, false):
        if checkbox.accessibility_name == "Use Template":
            checkbox.button_pressed = true
            checkbox.pressed.emit()
    await get_tree().process_frame
    var movement_found = false
    for selector in selectors:
        for i in range(selector.item_count):
            if "Basic Movement" in selector.get_item_text(i):
                movement_found = true
                selector.select(i)
                selector.item_selected.emit(i)
    check(movement_found, "CharacterBody2D offers its Luau movement template")
    if movement_found:
        dialog.get_ok_button().pressed.emit()
        await get_tree().process_frame
        check(FileAccess.file_exists("res://Hero.luau"), "Attach Script creates the selected template")
        if FileAccess.file_exists("res://Hero.luau"):
            var hero = load("res://Hero.luau")
            check(hero.get_diagnostics().is_empty(), "Movement template passes Luau type checking: " + str(hero.get_diagnostics()))
            var body = CharacterBody2D.new()
            body.set_script(hero)
            check(body.get("Speed") == 300, "Movement template exports its fields")
            body.free()
    for base in ["Node", "Object", "CharacterBody3D"]:
        var name_stem = "Walker" if base == "CharacterBody3D" else base
        var path = "res://" + name_stem + ".luau"
        dialog.config(base, path, false, false)
        dialog.popup_centered()
        await get_tree().process_frame
        dialog.get_ok_button().pressed.emit()
        await get_tree().process_frame
        check(FileAccess.file_exists(path), "Attach Script creates " + base + " template")
        if FileAccess.file_exists(path):
            var generated = load(path)
            check(generated.get_diagnostics().is_empty(), base + " template passes type checking: " + str(generated.get_diagnostics()))
            var expected_name = "Walker" if base == "CharacterBody3D" else base + "Script"
            check(generated.source_code.begins_with("class " + expected_name + " extends " + base), "Template class names avoid native type collisions")
    dialog.queue_free()
    # Do not leave the deliberately incomplete completion buffer dirty at exit.
    code.text = script.source_code
    if failures.is_empty():
        print("LUDOT_EDITOR_TESTS_PASSED")
    get_tree().quit(0 if failures.is_empty() else 1)
