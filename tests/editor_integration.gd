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
    # Exercise the actual context-menu action, including AST dialect protection.
    var formatter_source = 'import "@helpers/setup"\nclass FormatTest extends Node = {\n\texport Speed: number = 1 in 0 to 2 step 0.1\n\tonready private Head = self:getNode("A/Child/Node")\n\tfunction _ready()\n\t\tconst value={a=1,b="café class import"}\n\t\t-- stylua: ignore\n\t\tconst ignored={x=1}\n\t\tprint(value)\n\tend\n}\n'
    code.text = formatter_source
    var click = InputEventMouseButton.new()
    click.button_index = MOUSE_BUTTON_RIGHT
    click.pressed = true
    click.position = Vector2(10, 10)
    code.gui_input.emit(click)
    var format_menu: PopupMenu = null
    var format_id = -1
    for popup in editor.find_children("*", "PopupMenu", true, false):
        for index in range(popup.item_count):
            if popup.get_item_text(index) == "Format Document":
                format_menu = popup
                format_id = popup.get_item_id(index)
    check(format_menu != null, "Luau offers Format Document in its context menu")
    if format_menu != null:
        format_menu.id_pressed.emit(format_id)
        check(code.text.contains('const value = { a = 1, b = "café class import" }'), "StyLua formats const, Unicode, and table expressions")
        check(code.text.contains('export Speed: number = 1 in 0 to 2 step 0.1'), "Formatting preserves export range declarations")
        check(code.text.contains('onready private Head = self:getNode("A/Child/Node")'), "Formatting preserves onready properties")
        check(code.text.contains('import "@helpers/setup"'), "Formatting preserves Ludot imports")
        check(code.text.contains("const ignored={x=1}"), "StyLua ignore comments preserve the following statement")
        var formatted_once = code.text
        format_menu.id_pressed.emit(format_id)
        check(code.text == formatted_once, "Dialect formatting is idempotent")
        code.undo()
        check(code.text == formatter_source, "Formatting is one undo operation")
        code.text = 'const first={x="café"}\nconst second={y=2}\n'
        code.select(1, 0, 1, code.get_line(1).length())
        for index in range(format_menu.item_count):
            if format_menu.get_item_text(index) == "Format Selection":
                format_menu.id_pressed.emit(format_menu.get_item_id(index))
        check(code.get_line(0) == 'const first={x="café"}' and code.get_line(1) == "const second = { y = 2 }", "Selection formatting preserves unselected source and maps UTF-8 offsets")
        var extensions = 'enum Action = { Join, Chat }\nconst result = match Action.Join with case Action.Join then "join" otherwise "chat" end\nconst name = node?:getParent()?.Name ?? "missing"\nlocal value = nil\nvalue ??= true\nfor i in 1 to 10 step 5 do print(i) end\nconst output = things |> first() ?? {} |> sort()\n'
        code.text = extensions + 'const formatted={x=1}\n'
        format_menu.id_pressed.emit(format_id)
        check(code.text.begins_with(extensions), "StyLua preserves all lowered language constructs byte for byte")
        check(code.text.contains("const formatted = { x = 1 }"), "StyLua still formats statements around protected language syntax")
        var preserved = code.text
        format_menu.id_pressed.emit(format_id)
        check(code.text == preserved, "Formatting new syntax is idempotent")
        format_menu.hide()
    code.text = saved_editor_source
    code.text = "if true then"
    code.set_caret_line(0)
    code.set_caret_column(code.get_line(0).length())
    var enter = InputEventKey.new()
    enter.keycode = KEY_ENTER
    enter.pressed = true
    code.gui_input.emit(enter)
    check(code.get_line_count() == 3 and code.get_line(2) == "end" and code.get_caret_line() == 1, "Enter inserts an indented block and end")
    code.text = "if true then\nend"
    code.set_caret_line(0)
    code.set_caret_column(code.get_line(0).length())
    code.gui_input.emit(enter)
    check(code.text.count("end") == 1, "Block completion preserves an existing end")
    for opener in ["match value with", "case Action.Join then", "otherwise"]:
        code.text = opener
        code.set_caret_line(0)
        code.set_caret_column(opener.length())
        code.gui_input.emit(enter)
        check(code.get_caret_line() == 1 and code.get_caret_column() > 0, "Enter indents " + opener)
        check(code.text.count("end") == (1 if opener.ends_with("with") else 0), "Match arms share the match block's end")
    code.text = saved_editor_source
    var match_source = "enum Action = { Join, Chat }\nconst action: Action = Action.Join\nmatch action with case Action.Join then print(1) end"
    var match_warnings = script.get_lint_warnings(match_source)
    check(match_warnings.any(func(w): return w.code == "match_exhaustiveness"), "Non-exhaustive match statements produce native warnings")
    var ignored_match = script.get_lint_warnings("--# selene: allow(match_exhaustiveness)\n" + match_source)
    check(not ignored_match.any(func(w): return w.code == "match_exhaustiveness"), "Selene ignore comments suppress native match warnings")
    var naming_warnings = script.get_lint_warnings("const bad_name = 1\nprint(bad_name)")
    check(naming_warnings.any(func(w): return w.code == "naming_convention"), "Naming rules flag inconsistent immutable binding names")
    var ignored_warnings = script.get_lint_warnings("--# selene: allow(naming_convention)\nconst bad_name = 1\nprint(bad_name)")
    check(not ignored_warnings.any(func(w): return w.code == "naming_convention"), "File-level Selene comments suppress naming diagnostics")
    var unused_warnings = script.get_lint_warnings("const unused = 1")
    check(unused_warnings.any(func(w): return w.code == "unused_variable"), "Selene checks immutable bindings in unsaved buffers")
    var suppressed_unused = script.get_lint_warnings("-- selene: allow(unused_variable)\nconst unused = 1")
    check(not suppressed_unused.any(func(w): return w.code == "unused_variable"), "Selene comments suppress the following statement")
    var enum_script = LuauScript.new()
    enum_script.source_code = 'enum Choice = { First = "hello, world", Second = "second: value" }\nclass EnumInspectorProbe extends Node = { export Choice: Choice = Choice.First }'
    check(ResourceSaver.save(enum_script, "res://EnumInspectorProbe.luau") == OK, "Enum Inspector script saves")
    enum_script.take_over_path("res://EnumInspectorProbe.luau")
    check(enum_script.reload() == OK, "String enum Inspector script loads")
    var enum_node = Node.new()
    enum_node.set_script(enum_script)
    get_tree().root.add_child(enum_node)
    EditorInterface.inspect_object(enum_node)
    for frame in range(4):
        await get_tree().process_frame
    var enum_editors = EditorInterface.get_inspector().find_children("*", "LuauEnumProperty", true, false)
    check(not enum_editors.is_empty(), "Inspector selects the Luau enum property editor")
    if not enum_editors.is_empty():
        var options: OptionButton = enum_editors[0].find_children("*", "OptionButton", true, false)[0]
        check(options.item_count == 2 and options.get_item_text(0) == "First", "Enum dropdown labels use member names")
        options.select(1)
        options.item_selected.emit(1)
        await get_tree().process_frame
        check(enum_node.get("Choice") == "second: value", "Enum dropdown stores the underlying string including punctuation")
    EditorInterface.inspect_object(null)
    enum_node.free()
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
    var property_source = 'class Player extends CharacterBody3D = { function _physicsProcess(delta: number) print(self.Velo' + String.chr(0xffff) + 'city) end }'
    var property_help = script.get_symbol_help(property_source, "Velocity")
    check(property_help.get("value_type") == "Vector3", "Property hover reports its datatype")
    check(property_help.get("declaring_class") == "CharacterBody3D" and property_help.get("native_member") == "velocity", "Property hover maps to the native property documentation")
    var signal_source = 'const button = Button.new()\nprint(button.Pre' + String.chr(0xffff) + 'ssed)'
    var signal_help = script.get_symbol_help(signal_source, "Pressed")
    check(signal_help.get("native_kind") == "signal" and signal_help.get("native_member") == "pressed", "Inherited signal hover identifies the native signal rather than a property")
    var fire_help = script.get_symbol_help('const changed = signal with (value: number)\nchanged:fi' + String.chr(0xffff) + 're(1)', "fire")
    check('connected handler' in fire_help.get("description", ""), "Luau signal method hover uses the Luau library contract")
    var button_docs = LuauScript.get_api_docs("Button")
    check('Button.new()' in button_docs.get("description", "") and 'self:addChild(button)' in button_docs.get("description", ""), "Class docs contain the shared Luau button example")
    check('func _ready' not in button_docs.get("description", ""), "Luau class docs omit GDScript code")
    var signal_docs = LuauScript.get_api_docs("Button.Pressed")
    check(signal_docs.get("kind") == "signal" and 'Emitted when' in signal_docs.get("description", ""), "Signal docs retain the declaring class description")
    check('object.Pressed:connect' in signal_docs.get("description", ""), "Signal docs include an argument-aware Luau connection example")
    var documentation_settings = EditorInterface.get_editor_settings()
    var saved_constant_case = documentation_settings.get_setting("text_editor/luau/lint/constant_casing")
    var saved_indent_type = documentation_settings.get_setting("text_editor/luau/stylua/indent_type")
    var saved_indent_width = documentation_settings.get_setting("text_editor/luau/stylua/indent_width")
    documentation_settings.set_setting("text_editor/luau/lint/constant_casing", "SCREAMING_SNAKE_CASE")
    documentation_settings.set_setting("text_editor/luau/stylua/indent_type", "Spaces")
    documentation_settings.set_setting("text_editor/luau/stylua/indent_width", 2)
    button_docs = LuauScript.get_api_docs("Button")
    check('    const BUTTON = Button.new()' in button_docs.get("description", ""), "Examples follow naming and indentation settings")
    check('BUTTON.Text = "Click me"' in button_docs.get("description", ""), "Renaming examples preserves string literals")
    documentation_settings.set_setting("text_editor/luau/lint/constant_casing", saved_constant_case)
    documentation_settings.set_setting("text_editor/luau/stylua/indent_type", saved_indent_type)
    documentation_settings.set_setting("text_editor/luau/stylua/indent_width", saved_indent_width)
    code.text = 'const button = Button.new()\nprint(button.Pressed)'
    code.symbol_hovered.emit("Button", 0, code.get_line(0).find("Button") + 2)
    await get_tree().process_frame
    await check_documentation_card(code, 'button.Pressed:connect', 'func _ready')
    await close_hover_cards(code)
    code.symbol_hovered.emit("Pressed", 1, code.get_line(1).find("Pressed") + 2)
    await get_tree().process_frame
    await check_documentation_card(code, 'Emitted when', 'There is currently no description')
    await close_hover_cards(code)
    code.text = saved_editor_source.replace('print("Ready!")', 'print(self.Died)')
    code.symbol_hovered.emit("Died", 3, code.get_line(3).find("Died") + 2)
    await get_tree().process_frame
    await check_documentation_card(code, 'Emitted when the player dies.', 'There is currently no description')
    await close_hover_cards(code)
    script.source_code = saved_editor_source
    EditorInterface.get_script_editor().update_docs_from_script(script)
    editor.go_to_help.emit("class_name:Player")
    for frame in range(4):
        await get_tree().process_frame
    var player_reference = ""
    for help_tab in EditorInterface.get_script_editor().find_children("*", "EditorHelp", true, false):
        for label in help_tab.find_children("*", "RichTextLabel", true, false):
            if not label.is_finished():
                await label.finished
            player_reference += label.get_parsed_text()
    check('Died' in player_reference and 'Emitted when the player dies.' in player_reference, "Public Luau signals and comments appear in the full script class reference: " + player_reference)
    EditorInterface.edit_script(script)
    await get_tree().process_frame
    var type_source = 'const speed: num' + String.chr(0xffff) + 'ber = 5'
    var type_help = script.get_symbol_help(type_source, "number")
    check(type_help.get("symbol_type") == "Type" and type_help.get("value_type") == "number", "Primitive type annotations provide a type hover")
    var variable_source = 'const speed = 5\nprint(sp' + String.chr(0xffff) + 'eed)'
    check(script.get_symbol_help(variable_source, "speed").get("value_type") == "number", "Variable hover reports the inferred type")
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
            check(not generated.source_code.contains("local "), "Templates use const for immutable bindings")
            if base == "CharacterBody3D":
                check(generated.source_code.contains('"ui_up", "ui_down"'), "3D movement includes forward and backward input")
                check(generated.source_code.contains("velocity.Z = direction.Z"), "3D movement updates the Z axis")
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
    var theme_panels = EditorInterface.get_base_control().find_children("*", "LudotThemeTools", true, false)
    check(theme_panels.size() == 1, "Editor Settings provides the Themes tab")
    if theme_panels.size() == 1:
        var panel = theme_panels[0]
        var preset: OptionButton = panel.find_children("*", "OptionButton", true, false)[0]
        check(preset.item_count >= 5, "All five palette presets are available")
        preset.item_selected.emit(2)
        var settings = EditorInterface.get_editor_settings()
        check(settings.get_setting("interface/theme/base_color").to_html(false) == "2e3440", "Nord sets the editor window color")
        var code_box: TextEdit = panel.find_children("*", "TextEdit", true, false)[0]
        var buttons = panel.find_children("*", "Button", true, false)
        for button in buttons:
            if button.text == "Copy Current Theme Code": button.pressed.emit()
        var nord_code = code_box.text
        check(nord_code.begins_with("LDT1:"), "Theme export produces a versioned share code")
        preset.item_selected.emit(4)
        code_box.text = nord_code
        for button in buttons:
            if button.text == "Apply Theme Code": button.pressed.emit()
        check(settings.get_setting("interface/theme/base_color").to_html(false) == "2e3440", "Theme share code restores editor colors")
        var name_field: LineEdit = panel.get_node("ThemeName")
        name_field.text = "Regression Nord"
        for button in buttons:
            if button.text == "Save Theme": button.pressed.emit()
        check(settings.get_setting("interface/theme/ludot_saved_themes").has("Regression Nord"), "Named themes are stored in editor settings")
        check(not settings.property_get_revert("interface/theme/ludot_saved_themes").has("Regression Nord"), "Saving a theme preserves the empty default for persistence")
        for button in buttons:
            if button.text == "Delete Saved Theme": button.pressed.emit()
        check(not settings.get_setting("interface/theme/ludot_saved_themes").has("Regression Nord"), "Saved themes can be deleted")
        code_box.text = "LDT1:broken"
        for button in buttons:
            if button.text == "Apply Theme Code": button.pressed.emit()
        check(settings.get_setting("interface/theme/base_color").to_html(false) == "2e3440", "Invalid theme codes leave colors unchanged")
    # Do not leave the deliberately incomplete completion buffer dirty at exit.
    code.text = script.source_code
    if failures.is_empty():
        print("LUDOT_EDITOR_TESTS_PASSED")
    get_tree().quit(0 if failures.is_empty() else 1)

func check_documentation_card(code: CodeEdit, expected: String, excluded: String) -> void:
    var found = false
    for child in code.get_children():
        if child is PopupPanel:
            found = true
            var text = ""
            for label in child.find_children("*", "RichTextLabel", true, false):
                text += label.get_parsed_text() + "\n"
            check(expected in text and excluded not in text, "Hover uses the language-specific documentation: " + text)
    check(found, "Documentation hover opens a card")

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
