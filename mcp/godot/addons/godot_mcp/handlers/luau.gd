@tool
class_name MCPLuauHandlers
extends RefCounted
## Domain handler: Luau scripts in a Ludot editor.
##
## Everything goes through the editor's own LuauScript statics, so checks, lint,
## StyLua formatting, templates, module resolution, and API names match what the
## editor does with the user's settings. Nothing here runs the script. On stock
## Godot, LuauScript does not exist and every command fails with a precondition.

var _router: MCPCommandRouter


func _init(router: MCPCommandRouter) -> void:
	_router = router


func register(handlers: Dictionary) -> void:
	handlers["cmd_luau_diagnose"] = _cmd_luau_diagnose
	handlers["cmd_luau_format"] = _cmd_luau_format
	handlers["cmd_luau_create_script"] = _cmd_luau_create_script
	handlers["cmd_luau_api"] = _cmd_luau_api
	handlers["cmd_luau_enums"] = _cmd_luau_enums
	handlers["cmd_luau_resolve_module"] = _cmd_luau_resolve_module
	handlers["cmd_luau_inspect"] = _cmd_luau_inspect


# -- handlers ----------------------------------------------------------------

func _cmd_luau_diagnose(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var loaded := _read_luau(params)
	if not loaded.get("ok", false):
		return loaded
	var path: String = loaded["path"]
	var text: String = loaded["text"]
	var errors: Array = ClassDB.class_call_static("LuauScript", "check_code", text, path)
	var warnings: Array = ClassDB.class_call_static("LuauScript", "lint_code", text, path)
	var format: Dictionary = ClassDB.class_call_static("LuauScript", "format_code", text)
	var body := {
		"script_path": path,
		"ok": errors.is_empty(),
		"errors": errors,
		"warnings": warnings,
		"format": {
			"ok": format.get("ok", false),
			"changed": format.get("changed", false),
			"error": format.get("error", null),
		},
	}
	# The server turns these into a diff and drops them from the result.
	if format.get("changed", false):
		body["original"] = text
		body["formatted"] = format.get("formatted", text)
	return _router._ok(body)


func _cmd_luau_format(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var loaded := _read_luau(params)
	if not loaded.get("ok", false):
		return loaded
	var path: String = loaded["path"]
	var text: String = loaded["text"]
	var format: Dictionary = ClassDB.class_call_static("LuauScript", "format_code", text)
	if not format.get("ok", false):
		return _router._fail("VALIDATION_ERROR", str(format.get("error", "StyLua could not format the script.")))
	var formatted := str(format.get("formatted", text))
	var body := {"script_path": path, "changed": formatted != text, "original": text, "formatted": formatted}
	if bool(params.get("dry_run", false)) or formatted == text:
		body["receipt"] = MCPReceipt.make("noop", [])
		return _router._ok(body)
	var before_state := MCPReceipt.file_state(path)
	var ur := EditorInterface.get_editor_undo_redo()
	ur.create_action("Format script %s" % path, UndoRedo.MERGE_DISABLE, _router.asset_action_context())
	ur.add_do_method(_router, "_write_file_text", path, formatted)
	ur.add_undo_method(_router, "_write_file_text", path, text)
	ur.commit_action()
	body["receipt"] = MCPReceipt.make("updated", [
		MCPReceipt.change(path, "write", before_state, MCPReceipt.file_state(path)),
	])
	return _router._ok(body)


func _cmd_luau_create_script(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var path := str(params.get("script_path", ""))
	if not path.begins_with("res://") or not path.ends_with(".luau"):
		return _router._fail("VALIDATION_ERROR", "script_path must be a res:// .luau file.", "script_path")
	var base_class := str(params.get("base_class", "Node"))
	if not ClassDB.class_exists(base_class):
		return _router._fail("VALIDATION_ERROR", "Unknown engine base class '%s'." % base_class, "base_class")
	var class_name_text := str(params.get("class_name", ""))
	if class_name_text.is_empty():
		class_name_text = path.get_file().get_basename()
	var templates: Array = ClassDB.class_call_static("LuauScript", "get_templates", base_class)
	var template := str(params.get("template", ""))
	if not template.is_empty():
		var known := false
		for entry in templates:
			known = known or str(entry.get("name", "")) == template
		if not known:
			return _router._fail("VALIDATION_ERROR", "No template '%s' for %s." % [template, base_class], "template")
	var content: String = ClassDB.class_call_static("LuauScript", "create_from_template", template, class_name_text, base_class)
	var existed := FileAccess.file_exists(path)
	var body := {
		"script_path": path,
		"content": content,
		"templates": templates,
		"previous_existed": existed,
	}
	if bool(params.get("dry_run", false)):
		body["created"] = false
		return _router._ok(body)
	if existed:
		return _router._fail("VALIDATION_ERROR", "A script already exists at '%s'. Use write_script to replace it." % path, "script_path")
	var before_state := MCPReceipt.file_state(path)
	var ur := EditorInterface.get_editor_undo_redo()
	ur.create_action("Create script %s" % path, UndoRedo.MERGE_DISABLE, _router.asset_action_context())
	ur.add_do_method(_router, "_write_file_text", path, content)
	ur.add_undo_method(_router, "_remove_file_with_uid", path)
	ur.commit_action()
	body["created"] = FileAccess.file_exists(path)
	body["receipt"] = MCPReceipt.make("created", [
		MCPReceipt.change(path, "write", before_state, MCPReceipt.file_state(path)),
	])
	return _router._ok(body)


func _cmd_luau_api(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var symbol := str(params.get("symbol", "")).strip_edges()
	if symbol.is_empty():
		return _router._fail("VALIDATION_ERROR", "'symbol' must name a class, member, or library.", "symbol")
	return _router._ok(ClassDB.class_call_static("LuauScript", "get_api_docs", symbol))


func _cmd_luau_enums(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var query := str(params.get("query", ""))
	var enums: Array = ClassDB.class_call_static("LuauScript", "find_enums", query)
	return _router._ok({"query": query, "enums": enums})


func _cmd_luau_resolve_module(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var requested := str(params.get("path", ""))
	if requested.is_empty():
		return _router._fail("VALIDATION_ERROR", "'path' must be an import path.", "path")
	var from := str(params.get("from_path", "res://main.luau"))
	var resolved: String = ClassDB.class_call_static("LuauScript", "resolve_module_path", requested, from)
	return _router._ok({
		"path": requested,
		"from_path": from,
		"resolved": resolved if not resolved.is_empty() else null,
		"found": not resolved.is_empty(),
	})


func _cmd_luau_inspect(params: Dictionary) -> Dictionary:
	var failure := _require_ludot()
	if not failure.is_empty():
		return failure
	var loaded := _read_luau(params)
	if not loaded.get("ok", false):
		return loaded
	var line := int(params.get("line", 0))
	var column := int(params.get("column", 0))
	var result: Dictionary = ClassDB.class_call_static("LuauScript", "inspect_code", loaded["text"], loaded["path"], line, column)
	if result.is_empty():
		return _router._fail("VALIDATION_ERROR", "Line %d is outside the script." % line, "line")
	var completions: Array = result.get("completions", [])
	var limit := int(params.get("max_completions", 100))
	var body := {
		"script_path": loaded["path"],
		"line": line,
		"column": column,
		"symbol": result.get("symbol", ""),
		"help": result.get("help", {}),
		"completions": completions.slice(0, limit),
		"completions_truncated": completions.size() > limit,
	}
	return _router._ok(body)


# -- helpers -----------------------------------------------------------------

func _require_ludot() -> Dictionary:
	if ClassDB.class_exists("LuauScript") and ClassDB.class_has_method("LuauScript", "check_code"):
		return {}
	return _router._fail("PRECONDITION_FAILED", "Luau tools need a Ludot editor (4.7.2-lu.4 or newer).", "ludot_editor")


## The script text: the content param when given (an unsaved draft), else the file.
func _read_luau(params: Dictionary) -> Dictionary:
	var path := str(params.get("script_path", ""))
	if not path.begins_with("res://") or not path.ends_with(".luau"):
		return _router._fail("VALIDATION_ERROR", "script_path must be a res:// .luau file.", "script_path")
	if params.has("content") and params["content"] != null:
		return {"ok": true, "path": path, "text": str(params["content"])}
	if not FileAccess.file_exists(path):
		return _router._fail("RESOURCE_NOT_FOUND", "No script at '%s'." % path)
	return {"ok": true, "path": path, "text": FileAccess.get_file_as_string(path)}
