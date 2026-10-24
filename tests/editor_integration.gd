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
    var saved_editor_source: String = script.source_code
    check(not script.can_instantiate(), "Non-tool Luau scripts stay inactive in the editor")
    var scene_root = EditorInterface.get_edited_scene_root()
    check(scene_root.get_script() != null, "The scene retains its Luau script while editing")
    check(scene_root.get("Calls") == 0, "Export defaults are exposed by the editor placeholder")
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
    code.text = 'class Player extends Node = { export Speed: number = 1 in 0 to 10 step 0.1 }'
    var keyword_color: Color = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/keyword_color")
    for word in ["class", "extends", "export", "in", "to", "step"]:
        var column = code.text.find(word)
        check(highlighter.get_line_syntax_highlighting(0)[column].color == keyword_color, word + " uses the keyword color")
    code.text = 'class Player extends Node = { function test() const node = Node3D.new() node:free() end }'
    script.source_code = code.text
    check(script.get_diagnostics().is_empty(), "Native .new constructors type-check in the editor")
    var constructors = script.get_completions('class Player extends Node = { function test() const node = Node3D.' + String.chr(0xffff) + ' end }')
    check(constructors.has("new"), "Native class completion offers .new")
    script.source_code = saved_editor_source
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
    var hover_source = 'class Player extends Node = { function _ready() self:getNo' + String.chr(0xffff) + 'de("..") end }'
    var help: Dictionary = script.get_symbol_help(hover_source, "getNode", EditorInterface.get_edited_scene_root().get_child(0))
    check(help.get("declaring_class") == "Node" and help.get("native_member") == "get_node", "Luau hover maps camelCase to the native documentation member")
    check(help.get("return_type") == "Node" and help.parameters[0].name == "path", "Luau hover includes native parameter and return types")
    var boundary_help = script.get_symbol_help(hover_source.replace("getNo" + String.chr(0xffff) + "de", "getNode" + String.chr(0xffff)), "getNode")
    check(boundary_help.get("native_member") == "get_node", "Luau hover resolves the trailing edge of a function name")
    var local_source = '--- Adds two numbers.\nconst function add(a: number, b: number): number\n    return a + b\nend\nclass Player extends Node = { function _ready() ad' + String.chr(0xffff) + 'd(1, 2) end }'
    help = script.get_symbol_help(local_source, "add")
    check(help.get("return_type") == "number" and help.parameters.size() == 2, "Local Luau function hover retains its full signature")
    check("Adds two numbers" in help.get("description", ""), "Luau hover reads adjacent documentation comments")
    check(not help.has("declaring_class"), "User functions have no fabricated Godot documentation link")
    var overloaded_source = 'local overloaded = (function(value: any): any return value end) :: ((number) -> number) & ((string) -> string)\nlocal result = over' + String.chr(0xffff) + 'loaded("text")'
    help = script.get_symbol_help(overloaded_source, "overloaded")
    check(help.get("return_type") == "string", "Luau hover displays the overload selected for the actual call")
    code.text = hover_source.replace(String.chr(0xffff), "")
    code.symbol_hovered.emit("getNode", 0, code.text.find("getNode") + 3)
    await get_tree().process_frame
    await check_hover_card(code, "getNode", "path", "Node", true)
    await close_hover_cards(code)
    await get_tree().process_frame
    code.text = local_source.replace(String.chr(0xffff), "")
    code.symbol_hovered.emit("add", 4, code.get_line(4).find("add") + 1)
    await get_tree().process_frame
    await check_hover_card(code, "add", "a", "number", false)
    await close_hover_cards(code)
    await get_tree().process_frame
    code.text = 'class Player extends Node = { function _ready() self:_ready() end }'
    code.symbol_hovered.emit("_ready", 0, code.text.rfind("_ready") + 3)
    await get_tree().process_frame
    await check_hover_card(code, "_ready", "", "()", true, "https://docs.godotengine.org/en/latest/classes/class_node.html#class-node-private-method-ready")
    await close_hover_cards(code)
    await get_tree().process_frame
    code.text = original_source
    var gd_script = load("res://Hover.gd")
    EditorInterface.edit_script(gd_script)
    for frame in range(4): await get_tree().process_frame
    var gd_code: CodeEdit = EditorInterface.get_script_editor().get_current_editor().get_base_editor()
    var gd_source: String = gd_script.source_code
    var gd_help = gd_script.get_symbol_help(gd_source.replace("add(1, 2)", "ad" + String.chr(0xffff) + "d(1, 2)"), "add")
    check(gd_help.parameters[1].get("default") == "2", "GDScript hover includes default parameter values")
    gd_help = gd_script.get_symbol_help(gd_source.replace("add(1, 2)", "ad" + String.chr(0xffff) + "d(1, 2)") + "\nvar pending =", "add")
    check(gd_help.get("return_type") == "int", "GDScript function hover survives an incomplete edit elsewhere")
    var lambda_source = 'extends Node\nfunc _ready():\n    var doubler = func(value: int) -> int: return value * 2\n    dou' + String.chr(0xffff) + 'bler(4)'
    gd_help = gd_script.get_symbol_help(lambda_source, "doubler")
    check(gd_help.get("return_type") == "int" and gd_help.get("parameters", []).size() == 1, "GDScript lambda hover preserves its inferred callable signature")
    gd_code.symbol_hovered.emit("get_node", 5, 9)
    await get_tree().process_frame
    await check_hover_card(gd_code, "get_node", "path", "Node", true)
    await close_hover_cards(gd_code)
    await get_tree().process_frame
    gd_code.symbol_hovered.emit("add", 6, 6)
    await get_tree().process_frame
    await check_hover_card(gd_code, "add", "a", "int", false)
    await close_hover_cards(gd_code)
    await get_tree().process_frame
    gd_code.symbol_hovered.emit("_ready", 4, 8)
    await get_tree().process_frame
    await check_hover_card(gd_code, "_ready", "", "void", true, "https://docs.godotengine.org/en/latest/classes/class_node.html#class-node-private-method-ready")
    await close_hover_cards(gd_code)
    await get_tree().process_frame
    EditorInterface.edit_script(script)
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
    var shader = load("res://Sample.lushader")
    EditorInterface.edit_resource(shader)
    for frame in 4:
        await get_tree().process_frame
    var shader_code: CodeEdit = null
    for candidate in EditorInterface.get_base_control().find_children("*", "CodeEdit", true, false):
        if candidate.text.begins_with("shader spatial"):
            shader_code = candidate
            break
    check(shader_code != null, "The dedicated lushader editor opens")
    if shader_code != null:
        var shader_colors = shader_code.syntax_highlighter.get_line_syntax_highlighting(1)
        var range_line = shader_code.get_line(1)
        for word in ["export", "in", "to", "step"]:
            check(shader_colors[range_line.find(word)].color == keyword_color, "Lushader " + word + " uses the keyword color")
        check(shader_colors[range_line.find("number")].color == EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/base_type_color"), "Lushader scalar types use the type color")
    # Do not leave the deliberately incomplete completion buffer dirty at exit.
    code.text = script.source_code
    if failures.is_empty():
        print("LUDOT_EDITOR_TESTS_PASSED")
    get_tree().quit(0 if failures.is_empty() else 1)

func check_hover_card(code: CodeEdit, name_text: String, parameter: String, return_type: String, native: bool, wiki_url: String = "https://docs.godotengine.org/en/latest/classes/class_node.html#class-node-method-get-node") -> void:
    var found = false
    for child in code.get_children():
        if child is PopupPanel:
            found = true
            var screenshot_dir = OS.get_environment("LUDOT_HOVER_SCREENSHOTS")
            if not screenshot_dir.is_empty():
                await RenderingServer.frame_post_draw
                check(child.size.y <= 480 * EditorInterface.get_editor_scale(), "Hover card stays compact with a fixed footer")
                var language = "luau" if "class Player" in code.text else "gdscript"
                child.get_texture().get_image().save_png(screenshot_dir.path_join(language + "-" + name_text + ".png"))
            var text = ""
            for label in child.find_children("*", "RichTextLabel", true, false):
                text += label.get_parsed_text() + "\n"
            check(name_text in text and (parameter.is_empty() or ("Parameters" in text and parameter in text)) and "Returns" in text and return_type in text, "Hover card displays signature, parameters, and returns for " + name_text + ": " + text)
            check(("Open Godot documentation" in text) == native, "Only native documented methods offer Godot links")
            if native:
                var cards = child.find_children("*", "VBoxContainer", true, false)
                var card = cards[0]
                check(card.get_documentation_url() == wiki_url, "Hover links to the exact native member anchor")
    check(found, "Hovering " + name_text + " opens a function card")

func close_hover_cards(code: CodeEdit) -> void:
    for child in code.get_children():
        if child is PopupPanel:
            child.queue_free()
            await child.tree_exited
