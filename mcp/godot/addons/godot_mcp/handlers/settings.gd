@tool
class_name MCPSettingsHandlers
extends RefCounted
const Coerce := preload("../type_coerce.gd")
## Domain handler: read-only project and editor settings, plus Ludot's conventions.
##
## Project settings live in project.godot. Editor settings are the user's
## preferences, shared by every project. Ludot keeps its API casing in project
## settings and its naming, lint and formatter preferences in editor settings, so
## an agent needs both before it writes Luau that matches the user's style.

const MAX_SETTINGS := 2000
## Names that can hold credentials are listed but their values are withheld.
const SECRET_WORDS: Array[String] = ["pass", "token", "secret", "credential", "api_key", "private_key"]
const LINT_PREFIX := "text_editor/luau/lint/"
const FORMAT_PREFIX := "text_editor/luau/stylua/"
const NAMING_KINDS: Array[String] = ["variable", "constant", "function", "method", "property", "class"]
## Project files that change how Luau resolves, lints or formats.
const PROJECT_CONFIG_FILES: Array[String] = [".luaurc", "selene.toml", "stylua.toml", ".stylua.toml"]

var _router: MCPCommandRouter


func _init(router: MCPCommandRouter) -> void:
	_router = router


func register(handlers: Dictionary) -> void:
	handlers["cmd_list_settings"] = _cmd_list_settings
	handlers["cmd_get_ludot_conventions"] = _cmd_get_ludot_conventions


# -- handlers ----------------------------------------------------------------

func _cmd_list_settings(params: Dictionary) -> Dictionary:
	var scope := str(params.get("scope", "project"))
	var prefix := str(params.get("prefix", ""))
	var max_results := clampi(int(params.get("max_results", 500)), 1, MAX_SETTINGS)
	var source: Object = _source(scope)
	if source == null:
		return _router._fail("VALIDATION_ERROR", "scope must be 'project' or 'editor'.")
	var settings: Array = []
	var truncated := false
	for property: Dictionary in source.get_property_list():
		var name := str(property.get("name", ""))
		if not _is_setting(property) or not name.begins_with(prefix):
			continue
		if settings.size() >= max_results:
			truncated = true
			break
		settings.append(_describe(source, property))
	return _router._ok({"scope": scope, "prefix": prefix, "settings": settings, "truncated": truncated})


func _cmd_get_ludot_conventions(_params: Dictionary) -> Dictionary:
	var editor := EditorInterface.get_editor_settings()
	var naming := {}
	for kind in NAMING_KINDS:
		naming[kind] = _editor_value(editor, LINT_PREFIX + kind + "_casing")
	var config_files := {}
	for file_name in PROJECT_CONFIG_FILES:
		var path := "res://" + file_name
		if FileAccess.file_exists(path):
			config_files[file_name] = FileAccess.get_file_as_string(path)
	var version := Engine.get_version_info()
	return _router._ok({
		"engine_version": str(version.get("string", "")),
		"is_ludot": ClassDB.class_exists("LuauScript") or str(version.get("status", "")).begins_with("lu"),
		"api_casing": {
			"method": ProjectSettings.get_setting("luau/api/method_casing", "camelCase"),
			"property": ProjectSettings.get_setting("luau/api/property_casing", "PascalCase"),
		},
		"naming_casing": naming,
		"lint": {
			"enabled": _editor_value(editor, LINT_PREFIX + "enabled"),
			"selene_enabled": _editor_value(editor, LINT_PREFIX + "selene_enabled"),
		},
		"formatter": _editor_group(editor, FORMAT_PREFIX),
		"completion": _editor_group(editor, "text_editor/luau/completion/"),
		"modules": {
			"aliases": Coerce.to_json(ProjectSettings.get_setting("luau/modules/aliases", {})),
			"require_autocomplete": ProjectSettings.get_setting("luau/editor/require_autocomplete", true),
		},
		"indentation": {
			"type": "Spaces" if int(editor.get_setting("text_editor/behavior/indent/type")) == 1 else "Tabs",
			"size": _editor_value(editor, "text_editor/behavior/indent/size"),
		},
		"project_config_files": config_files,
	})


# -- helpers -----------------------------------------------------------------

func _source(scope: String) -> Object:
	match scope:
		"project":
			return ProjectSettings
		"editor":
			return EditorInterface.get_editor_settings()
	return null


## Settings carry a slash-separated name; categories, groups and script members don't.
func _is_setting(property: Dictionary) -> bool:
	var usage := int(property.get("usage", 0))
	if usage & (PROPERTY_USAGE_CATEGORY | PROPERTY_USAGE_GROUP | PROPERTY_USAGE_SUBGROUP):
		return false
	return str(property.get("name", "")).contains("/")


func _describe(source: Object, property: Dictionary) -> Dictionary:
	var name := str(property.get("name", ""))
	var entry := {"name": name, "type": type_string(int(property.get("type", TYPE_NIL)))}
	if _is_secret(name):
		entry["value"] = null
		entry["redacted"] = true
	else:
		entry["value"] = Coerce.to_json(source.get(name))
	if int(property.get("hint", PROPERTY_HINT_NONE)) == PROPERTY_HINT_ENUM:
		entry["options"] = str(property.get("hint_string", "")).split(",")
	return entry


func _is_secret(name: String) -> bool:
	var lower := name.to_lower()
	for word in SECRET_WORDS:
		if lower.contains(word):
			return true
	return false


func _editor_value(editor: EditorSettings, name: String) -> Variant:
	if not editor.has_setting(name):
		return null
	return Coerce.to_json(editor.get_setting(name))


func _editor_group(editor: EditorSettings, prefix: String) -> Dictionary:
	var group := {}
	for property: Dictionary in editor.get_property_list():
		var name := str(property.get("name", ""))
		if name.begins_with(prefix) and not _is_secret(name):
			group[name.trim_prefix(prefix)] = Coerce.to_json(editor.get_setting(name))
	return group
