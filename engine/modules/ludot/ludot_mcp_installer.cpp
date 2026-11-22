#ifdef TOOLS_ENABLED
#include "ludot_mcp_installer.h"

#include "ludot_mcp_addon.gen.h"

#include "core/config/project_settings.h"
#include "core/io/compression.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "core/object/object_id.h"
#include "core/object/script_language.h"
#include "core/os/os.h"
#include "editor/editor_node.h"
#include "editor/file_system/editor_file_system.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/separator.h"

namespace {

const char *MCP_SERVER_KEY = "ludot-mcp";

enum class ProviderFormat {
	JSON_STANDARD, // root_key -> { command, args }
	JSON_VSCODE, // servers -> { type: "stdio", command, args }
	JSON_COPILOT, // mcpServers -> { type: "local", command, args }
	JSON_OPENCODE, // mcp -> { type: "local", command: [cmd, ...args] }
	JSON_ZED, // context_servers -> { command: { path, args, env } }
	TOML_CODEX, // [mcp_servers.<name>]
	YAML_GOOSE, // extensions: <name>: ...
};

struct ProviderDef {
	String id;
	String name;
	String config_path;
	Vector<String> extra_check_paths;
	String root_key;
	ProviderFormat format = ProviderFormat::JSON_STANDARD;
	bool installed = false;
	bool already_configured = false;
	bool is_project_scope = false;
};

String get_home_dir() {
#ifdef WINDOWS_ENABLED
	String home = OS::get_singleton()->get_environment("USERPROFILE");
	if (home.is_empty()) {
		home = OS::get_singleton()->get_environment("HOMEDRIVE") + OS::get_singleton()->get_environment("HOMEPATH");
	}
	return home;
#else
	return OS::get_singleton()->get_environment("HOME");
#endif
}

String get_xdg_config_dir(const String &p_home) {
	String xdg = OS::get_singleton()->get_environment("XDG_CONFIG_HOME");
	if (!xdg.is_empty()) {
		return xdg;
	}
	return p_home.path_join(".config");
}

String get_app_data_dir(const String &p_home) {
#ifdef WINDOWS_ENABLED
	String appdata = OS::get_singleton()->get_environment("APPDATA");
	return appdata.is_empty() ? p_home.path_join("AppData/Roaming") : appdata;
#elif defined(MACOS_ENABLED)
	return p_home.path_join("Library/Application Support");
#else
	return get_xdg_config_dir(p_home);
#endif
}

String find_executable_in_path(const String &p_name) {
	String path_env = OS::get_singleton()->get_environment("PATH");
	if (path_env.is_empty()) {
		return String();
	}
#ifdef WINDOWS_ENABLED
	const String sep = ";";
	PackedStringArray exts = { "", ".exe", ".cmd", ".bat" };
#else
	const String sep = ":";
	PackedStringArray exts = { "" };
#endif
	PackedStringArray dirs = path_env.split(sep, false);
	for (const String &dir : dirs) {
		for (const String &ext : exts) {
			String candidate = dir.path_join(p_name + ext);
			if (FileAccess::exists(candidate)) {
				return candidate;
			}
		}
	}
	return String();
}

bool has_any_command(const Vector<String> &p_commands) {
	for (const String &cmd : p_commands) {
		if (!find_executable_in_path(cmd).is_empty()) {
			return true;
		}
	}
	return false;
}

bool dir_or_file_exists(const String &p_path) {
	return !p_path.is_empty() && (DirAccess::dir_exists_absolute(p_path) || FileAccess::exists(p_path));
}

String strip_jsonc_comments_and_trailing_commas(const String &p_input) {
	String no_comments;
	int len = p_input.length();
	bool in_string = false;
	bool escaped = false;
	int i = 0;
	while (i < len) {
		char32_t c = p_input[i];
		if (in_string) {
			no_comments += c;
			if (escaped) {
				escaped = false;
			} else if (c == '\\') {
				escaped = true;
			} else if (c == '"') {
				in_string = false;
			}
			i++;
			continue;
		}
		if (c == '"') {
			in_string = true;
			no_comments += c;
			i++;
			continue;
		}
		if (c == '/' && i + 1 < len && p_input[i + 1] == '/') {
			i += 2;
			while (i < len && p_input[i] != '\n') {
				i++;
			}
			continue;
		}
		if (c == '/' && i + 1 < len && p_input[i + 1] == '*') {
			i += 2;
			while (i + 1 < len && !(p_input[i] == '*' && p_input[i + 1] == '/')) {
				i++;
			}
			i = MIN(i + 2, len);
			continue;
		}
		no_comments += c;
		i++;
	}

	// Remove trailing commas before '}' or ']' outside strings.
	String cleaned;
	int clen = no_comments.length();
	in_string = false;
	escaped = false;
	for (int j = 0; j < clen; j++) {
		char32_t c = no_comments[j];
		if (in_string) {
			cleaned += c;
			if (escaped) {
				escaped = false;
			} else if (c == '\\') {
				escaped = true;
			} else if (c == '"') {
				in_string = false;
			}
			continue;
		}
		if (c == '"') {
			in_string = true;
			cleaned += c;
			continue;
		}
		if (c == ',') {
			int k = j + 1;
			while (k < clen && (no_comments[k] == ' ' || no_comments[k] == '\t' || no_comments[k] == '\r' || no_comments[k] == '\n')) {
				k++;
			}
			if (k < clen && (no_comments[k] == '}' || no_comments[k] == ']')) {
				continue;
			}
		}
		cleaned += c;
	}
	return cleaned;
}

bool is_ludot_mcp_name(const String &p_key) {
	String lower = p_key.to_lower().strip_edges();
	return lower == "ludot" || lower == "ludot-mcp" || lower == "ludot_mcp" || lower == "ludot-editor-mcp" ||
			lower == "godot-mcp" || lower == "godot_mcp" || lower == "godot-editor-mcp";
}

bool text_mentions_mcp_server(const String &p_text) {
	String lower = p_text.to_lower();
	return lower.contains("ludot-editor-mcp") || lower.contains("godot-editor-mcp");
}

bool entry_is_ludot_mcp(const Variant &p_entry) {
	if (p_entry.get_type() != Variant::DICTIONARY) {
		return false;
	}
	Dictionary dict = p_entry;
	if (dict.has("command")) {
		Variant cmd = dict["command"];
		if (cmd.get_type() == Variant::STRING && text_mentions_mcp_server(String(cmd))) {
			return true;
		}
		if (cmd.get_type() == Variant::ARRAY) {
			Array arr = cmd;
			for (int i = 0; i < arr.size(); i++) {
				if (text_mentions_mcp_server(String(arr[i]))) {
					return true;
				}
			}
		}
		if (cmd.get_type() == Variant::DICTIONARY) {
			Dictionary zed_cmd = cmd;
			if (zed_cmd.has("path") && text_mentions_mcp_server(String(zed_cmd["path"]))) {
				return true;
			}
			if (zed_cmd.has("args") && zed_cmd["args"].get_type() == Variant::ARRAY) {
				Array arr = zed_cmd["args"];
				for (int i = 0; i < arr.size(); i++) {
					if (text_mentions_mcp_server(String(arr[i]))) {
						return true;
					}
				}
			}
		}
	}
	if (dict.has("args") && dict["args"].get_type() == Variant::ARRAY) {
		Array arr = dict["args"];
		for (int i = 0; i < arr.size(); i++) {
			if (text_mentions_mcp_server(String(arr[i]))) {
				return true;
			}
		}
	}
	return false;
}

bool dict_servers_has_ludot_mcp(const Dictionary &p_servers) {
	for (const Variant &key : p_servers.get_key_list()) {
		if (is_ludot_mcp_name(String(key)) || entry_is_ludot_mcp(p_servers[key])) {
			return true;
		}
	}
	return false;
}

bool json_file_has_ludot_mcp(const String &p_path, const String &p_root_key, const String &p_claude_project_path = String()) {
	if (!FileAccess::exists(p_path)) {
		return false;
	}
	String raw = FileAccess::get_file_as_string(p_path);
	if (raw.strip_edges().is_empty()) {
		return false;
	}
	Ref<JSON> parser;
	parser.instantiate();
	if (parser->parse(strip_jsonc_comments_and_trailing_commas(raw)) != OK) {
		return false;
	}
	if (parser->get_data().get_type() != Variant::DICTIONARY) {
		return false;
	}
	Dictionary root = parser->get_data();
	if (root.has(p_root_key) && root[p_root_key].get_type() == Variant::DICTIONARY) {
		if (dict_servers_has_ludot_mcp(root[p_root_key])) {
			return true;
		}
	}
	if (!p_claude_project_path.is_empty() && root.has("projects") && root["projects"].get_type() == Variant::DICTIONARY) {
		Dictionary projects = root["projects"];
		if (projects.has(p_claude_project_path) && projects[p_claude_project_path].get_type() == Variant::DICTIONARY) {
			Dictionary proj_entry = projects[p_claude_project_path];
			if (proj_entry.has("mcpServers") && proj_entry["mcpServers"].get_type() == Variant::DICTIONARY) {
				if (dict_servers_has_ludot_mcp(proj_entry["mcpServers"])) {
					return true;
				}
			}
		}
	}
	return false;
}

bool toml_file_has_ludot_mcp(const String &p_path) {
	if (!FileAccess::exists(p_path)) {
		return false;
	}
	String text = FileAccess::get_file_as_string(p_path);
	PackedStringArray lines = text.split("\n");
	bool in_mcp_section = false;
	for (const String &raw_line : lines) {
		String line = raw_line.strip_edges();
		if (line.begins_with("#") || line.is_empty()) {
			continue;
		}
		if (line.begins_with("[") && line.ends_with("]")) {
			String header = line.substr(1, line.length() - 2).strip_edges();
			if (header.begins_with("mcp_servers.")) {
				in_mcp_section = true;
				String name = header.substr(String("mcp_servers.").length());
				int dot = name.find_char('.');
				if (dot >= 0) {
					name = name.substr(0, dot);
				}
				name = name.trim_prefix("\"").trim_suffix("\"").trim_prefix("'").trim_suffix("'");
				if (is_ludot_mcp_name(name)) {
					return true;
				}
			} else {
				in_mcp_section = false;
			}
			continue;
		}
		if (in_mcp_section && text_mentions_mcp_server(line)) {
			return true;
		}
	}
	return false;
}

bool yaml_file_has_ludot_mcp(const String &p_path) {
	if (!FileAccess::exists(p_path)) {
		return false;
	}
	String text = FileAccess::get_file_as_string(p_path);
	PackedStringArray lines = text.split("\n");
	bool in_extensions = false;
	for (const String &raw_line : lines) {
		if (raw_line.strip_edges().is_empty() || raw_line.strip_edges().begins_with("#")) {
			continue;
		}
		bool indented = raw_line.begins_with(" ") || raw_line.begins_with("\t");
		if (!indented) {
			in_extensions = raw_line.strip_edges().begins_with("extensions:");
			continue;
		}
		if (in_extensions) {
			String stripped = raw_line.strip_edges();
			int colon = stripped.find_char(':');
			if (colon > 0) {
				String key = stripped.substr(0, colon).strip_edges().trim_prefix("\"").trim_suffix("\"").trim_prefix("'").trim_suffix("'");
				if (is_ludot_mcp_name(key)) {
					return true;
				}
			}
			if (text_mentions_mcp_server(stripped)) {
				return true;
			}
		}
	}
	return false;
}

bool provider_is_configured(const ProviderDef &p_def, const String &p_project_dir) {
	switch (p_def.format) {
		case ProviderFormat::TOML_CODEX:
			if (toml_file_has_ludot_mcp(p_def.config_path)) {
				return true;
			}
			break;
		case ProviderFormat::YAML_GOOSE:
			if (yaml_file_has_ludot_mcp(p_def.config_path)) {
				return true;
			}
			break;
		default:
			if (json_file_has_ludot_mcp(p_def.config_path, p_def.root_key, p_def.id == "claude-code" ? p_project_dir : String())) {
				return true;
			}
			break;
	}
	for (const String &extra : p_def.extra_check_paths) {
		if (p_def.format == ProviderFormat::JSON_VSCODE) {
			if (json_file_has_ludot_mcp(extra, "servers") || json_file_has_ludot_mcp(extra, "mcpServers")) {
				return true;
			}
		} else if (json_file_has_ludot_mcp(extra, p_def.root_key)) {
			return true;
		}
	}
	return false;
}

Vector<ProviderDef> build_provider_catalog() {
	const String home = get_home_dir();
	const String xdg = get_xdg_config_dir(home);
	const String app_data = get_app_data_dir(home);
	const String project_dir = ProjectSettings::get_singleton()->get_resource_path();
	const String vscode_user = app_data.path_join("Code/User");

	Vector<ProviderDef> list;

	// 1. Claude Code
	{
		ProviderDef p;
		p.id = "claude-code";
		p.name = "Claude Code";
		p.config_path = home.path_join(".claude.json");
		if (!project_dir.is_empty()) {
			p.extra_check_paths.push_back(project_dir.path_join(".mcp.json"));
		}
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "claude" }) || dir_or_file_exists(home.path_join(".claude")) || FileAccess::exists(p.config_path);
		list.push_back(p);
	}

	// 2. Claude Desktop
	{
		ProviderDef p;
		p.id = "claude-desktop";
		p.name = "Claude Desktop";
		String dir = app_data.path_join("Claude");
		p.config_path = dir.path_join("claude_desktop_config.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 3. OpenAI Codex
	{
		ProviderDef p;
		p.id = "codex";
		p.name = "OpenAI Codex";
		String codex_home = OS::get_singleton()->get_environment("CODEX_HOME");
		if (codex_home.is_empty()) {
			codex_home = home.path_join(".codex");
		}
		p.config_path = codex_home.path_join("config.toml");
		p.root_key = "mcp_servers";
		p.format = ProviderFormat::TOML_CODEX;
		p.installed = has_any_command({ "codex" }) || DirAccess::dir_exists_absolute(codex_home);
		list.push_back(p);
	}

	// 4. Gemini CLI
	{
		ProviderDef p;
		p.id = "gemini";
		p.name = "Gemini CLI";
		p.config_path = home.path_join(".gemini/settings.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "gemini" }) || FileAccess::exists(p.config_path);
		list.push_back(p);
	}

	// 5. Antigravity (AGY / ACP)
	{
		ProviderDef p;
		p.id = "antigravity";
		p.name = "Antigravity (AGY / ACP)";
		String agy_alt = home.path_join(".gemini/antigravity/mcp_config.json");
		String agy_cfg = home.path_join(".gemini/config/mcp_config.json");
		p.config_path = FileAccess::exists(agy_alt) ? agy_alt : agy_cfg;
		if (p.config_path != agy_alt) {
			p.extra_check_paths.push_back(agy_alt);
		}
		if (!project_dir.is_empty()) {
			p.extra_check_paths.push_back(project_dir.path_join(".agents/mcp_config.json"));
		}
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "agy", "antigravity" }) ||
				DirAccess::dir_exists_absolute(home.path_join(".gemini/config")) ||
				DirAccess::dir_exists_absolute(home.path_join(".gemini/antigravity")) ||
				DirAccess::dir_exists_absolute(xdg.path_join("antigravity"));
		list.push_back(p);
	}

	// 6. OpenCode
	{
		ProviderDef p;
		p.id = "opencode";
		p.name = "OpenCode";
		String dir = xdg.path_join("opencode");
		p.config_path = dir.path_join("opencode.json");
		if (!project_dir.is_empty()) {
			p.extra_check_paths.push_back(project_dir.path_join("opencode.json"));
		}
		p.root_key = "mcp";
		p.format = ProviderFormat::JSON_OPENCODE;
		p.installed = has_any_command({ "opencode" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 7. Cursor
	{
		ProviderDef p;
		p.id = "cursor";
		p.name = "Cursor";
		p.config_path = home.path_join(".cursor/mcp.json");
		if (!project_dir.is_empty()) {
			p.extra_check_paths.push_back(project_dir.path_join(".cursor/mcp.json"));
		}
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "cursor" }) || DirAccess::dir_exists_absolute(home.path_join(".cursor")) || DirAccess::dir_exists_absolute(app_data.path_join("Cursor"));
		list.push_back(p);
	}

	// 8. VS Code
	{
		ProviderDef p;
		p.id = "vscode";
		p.name = "VS Code";
		p.config_path = vscode_user.path_join("mcp.json");
		if (!project_dir.is_empty()) {
			p.extra_check_paths.push_back(project_dir.path_join(".vscode/mcp.json"));
		}
		p.root_key = "servers";
		p.format = ProviderFormat::JSON_VSCODE;
		p.installed = has_any_command({ "code" }) || DirAccess::dir_exists_absolute(vscode_user);
		list.push_back(p);
	}

	// 9. Windsurf
	{
		ProviderDef p;
		p.id = "windsurf";
		p.name = "Windsurf";
		String dir = home.path_join(".codeium/windsurf");
		p.config_path = dir.path_join("mcp_config.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "windsurf" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 10. Zed
	{
		ProviderDef p;
		p.id = "zed";
		p.name = "Zed";
#ifdef WINDOWS_ENABLED
		String dir = app_data.path_join("Zed");
#else
		String dir = xdg.path_join("zed");
#endif
		p.config_path = dir.path_join("settings.json");
		p.root_key = "context_servers";
		p.format = ProviderFormat::JSON_ZED;
		p.installed = has_any_command({ "zed", "zeditor" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 11. GitHub Copilot CLI
	{
		ProviderDef p;
		p.id = "copilot";
		p.name = "GitHub Copilot CLI";
		String copilot_home = OS::get_singleton()->get_environment("COPILOT_HOME");
		if (copilot_home.is_empty()) {
			copilot_home = home.path_join(".copilot");
		}
		p.config_path = copilot_home.path_join("mcp-config.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_COPILOT;
		p.installed = has_any_command({ "copilot" }) || DirAccess::dir_exists_absolute(copilot_home);
		list.push_back(p);
	}

	// 12. Cline (VS Code)
	{
		ProviderDef p;
		p.id = "cline";
		p.name = "Cline (VS Code)";
		String dir = vscode_user.path_join("globalStorage/saoudrizwan.claude-dev");
		p.config_path = dir.path_join("settings/cline_mcp_settings.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 13. Roo Code (VS Code)
	{
		ProviderDef p;
		p.id = "roo-code";
		p.name = "Roo Code (VS Code)";
		String dir = vscode_user.path_join("globalStorage/rooveterinaryinc.roo-cline");
		p.config_path = dir.path_join("settings/mcp_settings.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 14. Goose (Block)
	{
		ProviderDef p;
		p.id = "goose";
		p.name = "Goose";
#ifdef WINDOWS_ENABLED
		String dir = app_data.path_join("Block/goose/config");
#else
		String dir = xdg.path_join("goose");
#endif
		p.config_path = dir.path_join("config.yaml");
		p.root_key = "extensions";
		p.format = ProviderFormat::YAML_GOOSE;
		p.installed = has_any_command({ "goose" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 15. Qwen Code
	{
		ProviderDef p;
		p.id = "qwen";
		p.name = "Qwen Code";
		String dir = home.path_join(".qwen");
		p.config_path = dir.path_join("settings.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "qwen" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 16. Amp
	{
		ProviderDef p;
		p.id = "amp";
		p.name = "Amp";
		String dir = xdg.path_join("amp");
		p.config_path = dir.path_join("settings.json");
		p.root_key = "amp.mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "amp" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 17. Kiro
	{
		ProviderDef p;
		p.id = "kiro";
		p.name = "Kiro";
		String dir = home.path_join(".kiro");
		p.config_path = dir.path_join("settings/mcp.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "kiro", "kiro-cli" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 18. Factory Droid
	{
		ProviderDef p;
		p.id = "droid";
		p.name = "Factory Droid";
		String dir = home.path_join(".factory");
		p.config_path = dir.path_join("mcp.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = has_any_command({ "droid" }) || DirAccess::dir_exists_absolute(dir);
		list.push_back(p);
	}

	// 19. Project .mcp.json (Workspace / ACP)
	if (!project_dir.is_empty()) {
		ProviderDef p;
		p.id = "project-mcp";
		p.name = "Project .mcp.json (Workspace / ACP)";
		p.config_path = project_dir.path_join(".mcp.json");
		p.root_key = "mcpServers";
		p.format = ProviderFormat::JSON_STANDARD;
		p.installed = true;
		p.is_project_scope = true;
		list.push_back(p);
	}

	for (ProviderDef &p : list) {
		if (p.installed) {
			p.already_configured = provider_is_configured(p, project_dir);
		}
	}

	return list;
}

String find_existing_addon_plugin_res_path() {
	const String root = ProjectSettings::get_singleton()->get_resource_path();
	if (root.is_empty()) {
		return "res://addons/godot_mcp/plugin.cfg";
	}
	Ref<DirAccess> dir = DirAccess::open(root);
	if (dir.is_valid()) {
		dir->list_dir_begin();
		for (String name = dir->get_next(); !name.is_empty(); name = dir->get_next()) {
			if (dir->current_is_dir() && name.to_lower() == "addons") {
				String candidate = root.path_join(name).path_join("godot_mcp/plugin.cfg");
				if (FileAccess::exists(candidate)) {
					return "res://" + name + "/godot_mcp/plugin.cfg";
				}
			}
		}
		dir->list_dir_end();
	}
	return "res://addons/godot_mcp/plugin.cfg";
}

uint32_t read_u32_le(const uint8_t *p_data, size_t p_offset) {
	return uint32_t(p_data[p_offset]) |
			(uint32_t(p_data[p_offset + 1]) << 8) |
			(uint32_t(p_data[p_offset + 2]) << 16) |
			(uint32_t(p_data[p_offset + 3]) << 24);
}

String quote_arg_for_display(const String &p_arg) {
	if (p_arg.contains_char(' ') || p_arg.contains_char('\t') || p_arg.contains_char('"')) {
		return "\"" + p_arg.replace("\"", "\\\"") + "\"";
	}
	return p_arg;
}

void parse_command_line(const String &p_text, String &r_command, PackedStringArray &r_args) {
	r_command = String();
	r_args.clear();
	Vector<String> tokens;
	String current;
	bool in_quotes = false;
	char32_t quote_char = 0;
	bool has_token = false;
	for (int i = 0; i < p_text.length(); i++) {
		char32_t c = p_text[i];
		if (in_quotes) {
			if (c == '\\' && i + 1 < p_text.length() && (p_text[i + 1] == quote_char || p_text[i + 1] == '\\')) {
				current += p_text[i + 1];
				i++;
			} else if (c == quote_char) {
				in_quotes = false;
			} else {
				current += c;
			}
		} else {
			if (c == '"' || c == '\'') {
				in_quotes = true;
				quote_char = c;
				has_token = true;
			} else if (c == ' ' || c == '\t') {
				if (has_token) {
					tokens.push_back(current);
					current = String();
					has_token = false;
				}
			} else {
				current += c;
				has_token = true;
			}
		}
	}
	if (has_token) {
		tokens.push_back(current);
	}
	if (!tokens.is_empty()) {
		r_command = tokens[0];
		for (int i = 1; i < tokens.size(); i++) {
			r_args.push_back(tokens[i]);
		}
	}
}

String find_local_mcp_dir() {
	String exe_dir = OS::get_singleton()->get_executable_path().get_base_dir();
	Vector<String> candidates;
	candidates.push_back(exe_dir.path_join("mcp"));
#ifdef MACOS_ENABLED
	String bundle_res = OS::get_singleton()->get_bundle_resource_dir();
	if (!bundle_res.is_empty()) {
		candidates.push_back(bundle_res.path_join("mcp"));
	}
#endif
	// Source checkout layout: <repo>/engine/bin/<editor> -> <repo>/mcp
	candidates.push_back(exe_dir.path_join("../../mcp").simplify_path());
	for (const String &dir : candidates) {
		if (FileAccess::exists(dir.path_join("pyproject.toml"))) {
			return dir;
		}
	}
	return String();
}

String json_quote(const String &p_str) {
	return "\"" + p_str.json_escape() + "\"";
}

ObjectID g_dialog_id;

} // namespace

Dictionary LudotMcpInstaller::get_addon_status() {
	String plugin_res = find_existing_addon_plugin_res_path();
	String plugin_abs = ProjectSettings::get_singleton()->globalize_path(plugin_res);
	bool installed = FileAccess::exists(plugin_abs);
	bool enabled = false;
	if (installed && EditorNode::get_singleton() != nullptr) {
		enabled = EditorNode::get_singleton()->is_addon_plugin_enabled(plugin_res);
	}
	Dictionary result;
	result["installed"] = installed;
	result["enabled"] = enabled;
	result["path"] = plugin_res.get_base_dir();
	result["plugin_cfg"] = plugin_res;
	return result;
}

Dictionary LudotMcpInstaller::install_addon() {
	Dictionary result;
	result["ok"] = false;

	const String project_root = ProjectSettings::get_singleton()->get_resource_path();
	if (project_root.is_empty()) {
		result["error"] = TTR("No open project found.");
		return result;
	}

	Vector<uint8_t> raw;
	raw.resize(LUDOT_MCP_ADDON_RAW_SIZE);
	int64_t decompressed = Compression::decompress(raw.ptrw(), LUDOT_MCP_ADDON_RAW_SIZE, LUDOT_MCP_ADDON_ZLIB, LUDOT_MCP_ADDON_COMPRESSED_SIZE, Compression::MODE_DEFLATE);
	if (decompressed != int64_t(LUDOT_MCP_ADDON_RAW_SIZE) || raw.size() < 4) {
		result["error"] = TTR("Failed to decompress the embedded Ludot MCP addon.");
		return result;
	}

	const String plugin_res = find_existing_addon_plugin_res_path();
	const String target_res_dir = plugin_res.get_base_dir();
	const String target_abs_dir = ProjectSettings::get_singleton()->globalize_path(target_res_dir);

	Error mk_err = DirAccess::make_dir_recursive_absolute(target_abs_dir);
	if (mk_err != OK) {
		result["error"] = vformat(TTR("Could not create addon directory '%s' (error %d)."), target_abs_dir, mk_err);
		return result;
	}

	const uint8_t *bytes = raw.ptr();
	size_t offset = 0;
	uint32_t file_count = read_u32_le(bytes, offset);
	offset += 4;

	int files_written = 0;
	Vector<String> written_res_paths;
	for (uint32_t i = 0; i < file_count; i++) {
		if (offset + 4 > size_t(raw.size())) {
			result["error"] = TTR("Corrupted embedded MCP addon archive.");
			return result;
		}
		uint32_t path_len = read_u32_le(bytes, offset);
		offset += 4;
		if (offset + path_len + 4 > size_t(raw.size())) {
			result["error"] = TTR("Corrupted embedded MCP addon archive.");
			return result;
		}
		String rel_path = String::utf8(reinterpret_cast<const char *>(bytes + offset), path_len);
		offset += path_len;

		uint32_t data_len = read_u32_le(bytes, offset);
		offset += 4;
		if (offset + data_len > size_t(raw.size())) {
			result["error"] = TTR("Corrupted embedded MCP addon archive.");
			return result;
		}

		if (rel_path.is_empty() || rel_path.is_absolute_path() || rel_path.contains("..")) {
			result["error"] = vformat(TTR("Invalid file path in embedded MCP addon: '%s'."), rel_path);
			return result;
		}

		String full_path = target_abs_dir.path_join(rel_path);
		Error parent_err = DirAccess::make_dir_recursive_absolute(full_path.get_base_dir());
		if (parent_err != OK) {
			result["error"] = vformat(TTR("Could not create directory '%s' (error %d)."), full_path.get_base_dir(), parent_err);
			return result;
		}

		Error open_err = OK;
		Ref<FileAccess> out = FileAccess::open(full_path, FileAccess::WRITE, &open_err);
		if (out.is_null() || open_err != OK) {
			result["error"] = vformat(TTR("Could not write '%s' (error %d)."), full_path, open_err);
			return result;
		}
		if (data_len > 0) {
			out->store_buffer(bytes + offset, data_len);
		}
		out->close();
		offset += data_len;
		written_res_paths.push_back(target_res_dir.path_join(rel_path));
		files_written++;
	}

	// Register newly extracted script class_name globals immediately so enabling the
	// plugin succeeds without waiting for an asynchronous EditorFileSystem scan.
	bool registered_classes = false;
	for (const String &res_file : written_res_paths) {
		if (!res_file.ends_with(".gd") && !res_file.ends_with(".luau")) {
			continue;
		}
		for (int lang_idx = 0; lang_idx < ScriptServer::get_language_count(); lang_idx++) {
			ScriptLanguage *lang = ScriptServer::get_language(lang_idx);
			if (lang == nullptr || lang->get_extension() != res_file.get_extension()) {
				continue;
			}
			String base_type;
			String icon_path;
			bool is_abstract = false;
			bool is_tool = false;
			String global_name = lang->get_global_class_name(res_file, &base_type, &icon_path, &is_abstract, &is_tool);
			if (!global_name.is_empty()) {
				ScriptServer::remove_global_class_by_path(res_file);
				ScriptServer::add_global_class(global_name, base_type, lang->get_name(), res_file, is_abstract, is_tool);
				registered_classes = true;
			}
			break;
		}
	}
	if (registered_classes) {
		ScriptServer::save_global_classes();
	}

	if (EditorFileSystem::get_singleton() != nullptr) {
		EditorFileSystem::get_singleton()->scan_changes();
	}

	bool enabled = false;
	if (EditorNode::get_singleton() != nullptr) {
		if (EditorNode::get_singleton()->is_addon_plugin_enabled(plugin_res)) {
			EditorNode::get_singleton()->set_addon_plugin_enabled(plugin_res, false, true);
		}
		EditorNode::get_singleton()->set_addon_plugin_enabled(plugin_res, true, true);
		enabled = EditorNode::get_singleton()->is_addon_plugin_enabled(plugin_res);
		if (enabled) {
			ProjectSettings::get_singleton()->save();
		}
	}

	result["ok"] = true;
	result["path"] = target_res_dir;
	result["plugin_cfg"] = plugin_res;
	result["files_written"] = files_written;
	result["enabled"] = enabled;
	return result;
}

Dictionary LudotMcpInstaller::resolve_server_command() {
	String command;
	PackedStringArray args;

	const String exe_dir = OS::get_singleton()->get_executable_path().get_base_dir();
	const String home = get_home_dir();
#ifdef WINDOWS_ENABLED
	const String bin_name = "ludot-editor-mcp.exe";
	const String venv_sub = "Scripts/ludot-editor-mcp.exe";
#else
	const String bin_name = "ludot-editor-mcp";
	const String venv_sub = "bin/ludot-editor-mcp";
#endif

	Vector<String> direct_candidates;
	direct_candidates.push_back(exe_dir.path_join(bin_name));
	direct_candidates.push_back(exe_dir.path_join("../../build/mcp-venv").simplify_path().path_join(venv_sub));
	direct_candidates.push_back(exe_dir.path_join("../../mcp/.venv").simplify_path().path_join(venv_sub));
	direct_candidates.push_back(home.path_join(".local/bin").path_join(bin_name));

	String on_path = find_executable_in_path("ludot-editor-mcp");
	if (!on_path.is_empty()) {
		command = "ludot-editor-mcp";
	} else {
		for (const String &candidate : direct_candidates) {
			if (FileAccess::exists(candidate)) {
				command = candidate;
				break;
			}
		}
	}

	if (command.is_empty()) {
		String mcp_dir = find_local_mcp_dir();
		String uv_path = find_executable_in_path("uv");
		if (uv_path.is_empty()) {
			String home_uv = home.path_join(".local/bin/uv");
			if (FileAccess::exists(home_uv)) {
				uv_path = home_uv;
			}
		}
		if (!mcp_dir.is_empty() && !uv_path.is_empty()) {
			command = find_executable_in_path("uv").is_empty() ? uv_path : String("uv");
			args.push_back("run");
			args.push_back("--directory");
			args.push_back(mcp_dir);
			args.push_back("ludot-editor-mcp");
		} else {
			String uvx_path = find_executable_in_path("uvx");
			if (!uvx_path.is_empty()) {
				command = "uvx";
				args.push_back("--from");
				args.push_back("git+https://github.com/ludotengine/ludot.git#subdirectory=mcp");
				args.push_back("ludot-editor-mcp");
			} else {
				command = "ludot-editor-mcp";
			}
		}
	}

	String display = quote_arg_for_display(command);
	for (const String &arg : args) {
		display += " " + quote_arg_for_display(arg);
	}

	Dictionary result;
	result["command"] = command;
	result["args"] = args;
	result["display"] = display;
	return result;
}

Array LudotMcpInstaller::detect_providers() {
	Vector<ProviderDef> catalog = build_provider_catalog();
	Array out;
	for (const ProviderDef &p : catalog) {
		Dictionary item;
		item["id"] = p.id;
		item["name"] = p.name;
		item["config_path"] = p.config_path;
		item["installed"] = p.installed;
		item["already_configured"] = p.already_configured;
		item["scope"] = p.is_project_scope ? "project" : "global";
		out.push_back(item);
	}
	return out;
}

Dictionary LudotMcpInstaller::configure_provider(const String &p_provider_id, const String &p_command, const PackedStringArray &p_args) {
	Dictionary result;
	result["ok"] = false;
	result["skipped"] = false;

	if (p_command.strip_edges().is_empty()) {
		result["error"] = TTR("MCP server command cannot be empty.");
		return result;
	}

	Vector<ProviderDef> catalog = build_provider_catalog();
	const ProviderDef *target = nullptr;
	for (const ProviderDef &p : catalog) {
		if (p.id == p_provider_id) {
			target = &p;
			break;
		}
	}
	if (target == nullptr) {
		result["error"] = vformat(TTR("Unknown AI provider '%s'."), p_provider_id);
		return result;
	}

	result["path"] = target->config_path;
	if (target->already_configured) {
		result["ok"] = true;
		result["skipped"] = true;
		return result;
	}

	Error dir_err = DirAccess::make_dir_recursive_absolute(target->config_path.get_base_dir());
	if (dir_err != OK) {
		result["error"] = vformat(TTR("Could not create config directory for %s (error %d)."), target->name, dir_err);
		return result;
	}

	if (target->format == ProviderFormat::TOML_CODEX) {
		String existing;
		if (FileAccess::exists(target->config_path)) {
			existing = FileAccess::get_file_as_string(target->config_path);
		}
		String args_toml = "[";
		for (int i = 0; i < p_args.size(); i++) {
			if (i > 0) {
				args_toml += ", ";
			}
			args_toml += json_quote(p_args[i]);
		}
		args_toml += "]";

		String snippet = vformat("[mcp_servers.%s]\ncommand = %s\nargs = %s\n", MCP_SERVER_KEY, json_quote(p_command), args_toml);
		String output = existing.strip_edges(false, true);
		if (!output.is_empty()) {
			output += "\n\n";
		}
		output += snippet;

		Error open_err = OK;
		Ref<FileAccess> file = FileAccess::open(target->config_path, FileAccess::WRITE, &open_err);
		if (file.is_null() || open_err != OK) {
			result["error"] = vformat(TTR("Could not write %s (error %d)."), target->config_path, open_err);
			return result;
		}
		file->store_string(output);
		file->close();
		result["ok"] = true;
		return result;
	}

	if (target->format == ProviderFormat::YAML_GOOSE) {
		String existing;
		if (FileAccess::exists(target->config_path)) {
			existing = FileAccess::get_file_as_string(target->config_path);
		}
		String args_yaml = "[";
		for (int i = 0; i < p_args.size(); i++) {
			if (i > 0) {
				args_yaml += ", ";
			}
			args_yaml += json_quote(p_args[i]);
		}
		args_yaml += "]";

		String entry;
		entry += vformat("  %s:\n", MCP_SERVER_KEY);
		entry += "    name: Ludot Editor MCP\n";
		entry += "    type: stdio\n";
		entry += vformat("    cmd: %s\n", json_quote(p_command));
		entry += vformat("    args: %s\n", args_yaml);
		entry += "    enabled: true\n";

		String output;
		bool inserted = false;
		if (!existing.strip_edges().is_empty()) {
			PackedStringArray lines = existing.split("\n");
			Vector<String> out_lines;
			for (int i = 0; i < lines.size(); i++) {
				String line = lines[i];
				String trimmed = line.strip_edges();
				if (!inserted && !line.begins_with(" ") && !line.begins_with("\t") && (trimmed == "extensions:" || trimmed == "extensions: {}")) {
					out_lines.push_back("extensions:");
					PackedStringArray entry_lines = entry.trim_suffix("\n").split("\n");
					for (const String &el : entry_lines) {
						out_lines.push_back(el);
					}
					inserted = true;
				} else {
					out_lines.push_back(line);
				}
			}
			output = String("\n").join(out_lines);
			if (!inserted) {
				output = output.strip_edges(false, true) + "\n\nextensions:\n" + entry;
			} else if (!output.ends_with("\n")) {
				output += "\n";
			}
		} else {
			output = "extensions:\n" + entry;
		}

		Error open_err = OK;
		Ref<FileAccess> file = FileAccess::open(target->config_path, FileAccess::WRITE, &open_err);
		if (file.is_null() || open_err != OK) {
			result["error"] = vformat(TTR("Could not write %s (error %d)."), target->config_path, open_err);
			return result;
		}
		file->store_string(output);
		file->close();
		result["ok"] = true;
		return result;
	}

	// JSON formats.
	Dictionary root;
	if (FileAccess::exists(target->config_path)) {
		String raw = FileAccess::get_file_as_string(target->config_path);
		if (!raw.strip_edges().is_empty()) {
			Ref<JSON> parser;
			parser.instantiate();
			Error parse_err = parser->parse(strip_jsonc_comments_and_trailing_commas(raw));
			if (parse_err != OK || parser->get_data().get_type() != Variant::DICTIONARY) {
				result["error"] = vformat(TTR("Could not parse existing JSON in %s; file was left unchanged."), target->config_path);
				return result;
			}
			root = parser->get_data();
		}
	}

	Dictionary servers;
	if (root.has(target->root_key)) {
		if (root[target->root_key].get_type() != Variant::DICTIONARY) {
			result["error"] = vformat(TTR("Expected '%s' to be a JSON object in %s."), target->root_key, target->config_path);
			return result;
		}
		servers = root[target->root_key];
	}

	Array args_array;
	for (const String &arg : p_args) {
		args_array.push_back(arg);
	}

	Dictionary entry;
	switch (target->format) {
		case ProviderFormat::JSON_STANDARD: {
			entry["command"] = p_command;
			entry["args"] = args_array;
		} break;
		case ProviderFormat::JSON_VSCODE: {
			entry["type"] = "stdio";
			entry["command"] = p_command;
			entry["args"] = args_array;
		} break;
		case ProviderFormat::JSON_COPILOT: {
			entry["type"] = "local";
			entry["command"] = p_command;
			entry["args"] = args_array;
		} break;
		case ProviderFormat::JSON_OPENCODE: {
			Array cmd_list;
			cmd_list.push_back(p_command);
			for (const String &arg : p_args) {
				cmd_list.push_back(arg);
			}
			entry["type"] = "local";
			entry["command"] = cmd_list;
		} break;
		case ProviderFormat::JSON_ZED: {
			Dictionary zed_cmd;
			zed_cmd["path"] = p_command;
			zed_cmd["args"] = args_array;
			zed_cmd["env"] = Dictionary();
			entry["command"] = zed_cmd;
		} break;
		default:
			break;
	}

	servers[MCP_SERVER_KEY] = entry;
	root[target->root_key] = servers;

	String serialized = JSON::stringify(root, "  ", true, true) + "\n";
	Error open_err = OK;
	Ref<FileAccess> file = FileAccess::open(target->config_path, FileAccess::WRITE, &open_err);
	if (file.is_null() || open_err != OK) {
		result["error"] = vformat(TTR("Could not write %s (error %d)."), target->config_path, open_err);
		return result;
	}
	file->store_string(serialized);
	file->close();
	result["ok"] = true;
	return result;
}

void LudotMcpInstaller::open_dialog() {
	if (EditorNode::get_singleton() == nullptr) {
		return;
	}
	LudotMcpInstallerDialog *dialog = Object::cast_to<LudotMcpInstallerDialog>(ObjectDB::get_instance(g_dialog_id));
	if (dialog == nullptr) {
		dialog = memnew(LudotMcpInstallerDialog);
		EditorNode::get_singleton()->get_gui_base()->add_child(dialog);
		g_dialog_id = dialog->get_instance_id();
	}
	dialog->refresh();
	dialog->popup_centered(Size2(560, 520) * EDSCALE);
}

void LudotMcpInstaller::_bind_methods() {
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("get_addon_status"), &LudotMcpInstaller::get_addon_status);
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("install_addon"), &LudotMcpInstaller::install_addon);
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("resolve_server_command"), &LudotMcpInstaller::resolve_server_command);
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("detect_providers"), &LudotMcpInstaller::detect_providers);
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("configure_provider", "provider_id", "command", "args"), &LudotMcpInstaller::configure_provider);
	ClassDB::bind_static_method("LudotMcpInstaller", D_METHOD("open_dialog"), &LudotMcpInstaller::open_dialog);
}

void LudotMcpInstallerDialog::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		connect("confirmed", callable_mp(this, &LudotMcpInstallerDialog::_on_install_pressed));
	}
}

void LudotMcpInstallerDialog::_select_all() {
	for (const ProviderRow &row : provider_rows) {
		if (!row.already_configured && row.checkbox != nullptr) {
			row.checkbox->set_pressed(true);
		}
	}
}

void LudotMcpInstallerDialog::_select_none() {
	for (const ProviderRow &row : provider_rows) {
		if (!row.already_configured && row.checkbox != nullptr) {
			row.checkbox->set_pressed(false);
		}
	}
}

void LudotMcpInstallerDialog::_on_install_pressed() {
	PackedStringArray summary;
	PackedStringArray errors;

	if (addon_checkbox->is_pressed()) {
		Dictionary addon_res = LudotMcpInstaller::install_addon();
		if (bool(addon_res.get("ok", false))) {
			String addon_path = addon_res.get("path", "res://addons/godot_mcp");
			bool enabled = bool(addon_res.get("enabled", false));
			if (enabled) {
				summary.push_back(vformat(TTR("Addon installed and enabled at %s."), addon_path));
			} else {
				summary.push_back(vformat(TTR("Addon installed at %s (enable it under Project Settings > Plugins)."), addon_path));
			}
		} else {
			errors.push_back(String(addon_res.get("error", TTR("Addon installation failed."))));
		}
	}

	String command;
	PackedStringArray args;
	parse_command_line(command_edit->get_text().strip_edges(), command, args);

	PackedStringArray configured_names;
	for (const ProviderRow &row : provider_rows) {
		if (row.already_configured || row.checkbox == nullptr || !row.checkbox->is_pressed()) {
			continue;
		}
		Dictionary res = LudotMcpInstaller::configure_provider(row.id, command, args);
		if (bool(res.get("ok", false))) {
			if (!bool(res.get("skipped", false))) {
				configured_names.push_back(row.name);
			}
		} else {
			errors.push_back(vformat("%s: %s", row.name, String(res.get("error", TTR("Unknown error")))));
		}
	}

	if (!configured_names.is_empty()) {
		summary.push_back(vformat(TTR("Configured AI providers: %s."), String(", ").join(configured_names)));
	}

	if (summary.is_empty() && errors.is_empty()) {
		status_label->set_text(TTR("Nothing selected. Check the project addon or at least one AI provider."));
		return;
	}

	refresh();
	String message = String("\n").join(summary);
	if (!errors.is_empty()) {
		if (!message.is_empty()) {
			message += "\n";
		}
		message += TTR("Errors:") + "\n" + String("\n").join(errors);
	}
	status_label->set_text(message);
}

void LudotMcpInstallerDialog::refresh() {
	Dictionary addon = LudotMcpInstaller::get_addon_status();
	bool addon_installed = bool(addon.get("installed", false));
	bool addon_enabled = bool(addon.get("enabled", false));
	String addon_path = addon.get("path", "res://addons/godot_mcp");

	if (addon_installed && addon_enabled) {
		addon_status_label->set_text(vformat(TTR("Status: Installed and enabled (%s). Check to reinstall or update."), addon_path));
		addon_checkbox->set_pressed(false);
	} else if (addon_installed) {
		addon_status_label->set_text(vformat(TTR("Status: Installed at %s, currently disabled. Check to update and enable."), addon_path));
		addon_checkbox->set_pressed(true);
	} else {
		addon_status_label->set_text(TTR("Status: Not installed in this project yet."));
		addon_checkbox->set_pressed(true);
	}

	for (int i = providers_list->get_child_count() - 1; i >= 0; i--) {
		Node *child = providers_list->get_child(i);
		providers_list->remove_child(child);
		memdelete(child);
	}
	provider_rows.clear();

	Array providers = LudotMcpInstaller::detect_providers();
	int detected_count = 0;
	for (int i = 0; i < providers.size(); i++) {
		Dictionary item = providers[i];
		if (!bool(item.get("installed", false))) {
			continue;
		}
		detected_count++;

		ProviderRow row;
		row.id = item.get("id", "");
		row.name = item.get("name", "");
		row.config_path = item.get("config_path", "");
		row.already_configured = bool(item.get("already_configured", false));

		CheckBox *cb = memnew(CheckBox);
		cb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		cb->set_tooltip_text(row.config_path);
		if (row.already_configured) {
			cb->set_text(vformat(TTR("%s — Already installed"), row.name));
			cb->set_pressed(true);
			cb->set_disabled(true);
		} else {
			cb->set_text(row.name);
			cb->set_pressed(false);
		}
		providers_list->add_child(cb);
		row.checkbox = cb;
		provider_rows.push_back(row);
	}

	if (detected_count == 0) {
		Label *none = memnew(Label);
		none->set_text(TTR("No supported AI providers were detected on this system. You can still install the project addon."));
		none->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
		providers_list->add_child(none);
	}

	if (command_edit->get_text().strip_edges().is_empty()) {
		Dictionary resolved = LudotMcpInstaller::resolve_server_command();
		command_edit->set_text(String(resolved.get("display", "ludot-editor-mcp")));
	}
}

LudotMcpInstallerDialog::LudotMcpInstallerDialog() {
	set_title(TTR("Setup Ludot MCP"));
	set_ok_button_text(TTR("Install"));
	add_cancel_button(TTR("Close"));
	set_hide_on_ok(false);

	VBoxContainer *root = memnew(VBoxContainer);
	root->add_theme_constant_override("separation", Math::round(8 * EDSCALE));
	add_child(root);

	Label *addon_header = memnew(Label);
	addon_header->set_text(TTR("Project Addon"));
	addon_header->set_theme_type_variation("HeaderSmall");
	root->add_child(addon_header);

	addon_checkbox = memnew(CheckBox);
	addon_checkbox->set_text(TTR("Install and enable Ludot Editor MCP addon in this project"));
	addon_checkbox->set_pressed(true);
	root->add_child(addon_checkbox);

	addon_status_label = memnew(Label);
	addon_status_label->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	root->add_child(addon_status_label);

	root->add_child(memnew(HSeparator));

	HBoxContainer *providers_header_row = memnew(HBoxContainer);
	root->add_child(providers_header_row);

	Label *providers_header = memnew(Label);
	providers_header->set_text(TTR("AI Providers"));
	providers_header->set_theme_type_variation("HeaderSmall");
	providers_header_row->add_child(providers_header);
	providers_header_row->add_spacer();

	Button *select_all_btn = memnew(Button);
	select_all_btn->set_text(TTR("Select All"));
	select_all_btn->connect("pressed", callable_mp(this, &LudotMcpInstallerDialog::_select_all));
	providers_header_row->add_child(select_all_btn);

	Button *select_none_btn = memnew(Button);
	select_none_btn->set_text(TTR("Addon Only"));
	select_none_btn->connect("pressed", callable_mp(this, &LudotMcpInstallerDialog::_select_none));
	providers_header_row->add_child(select_none_btn);

	Button *refresh_btn = memnew(Button);
	refresh_btn->set_text(TTR("Refresh"));
	refresh_btn->connect("pressed", callable_mp(this, &LudotMcpInstallerDialog::refresh));
	providers_header_row->add_child(refresh_btn);

	Label *providers_help = memnew(Label);
	providers_help->set_text(TTR("Select installed AI providers to register Ludot MCP with, or leave all unchecked to install the project addon only. Providers that already have Ludot MCP configured are greyed out."));
	providers_help->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	root->add_child(providers_help);

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_custom_minimum_size(Size2(0, 200) * EDSCALE);
	scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	root->add_child(scroll);

	providers_list = memnew(VBoxContainer);
	providers_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(providers_list);

	root->add_child(memnew(HSeparator));

	Label *cmd_label = memnew(Label);
	cmd_label->set_text(TTR("MCP Server Command (written to selected providers):"));
	root->add_child(cmd_label);

	command_edit = memnew(LineEdit);
	command_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	command_edit->set_placeholder("ludot-editor-mcp");
	root->add_child(command_edit);

	status_label = memnew(Label);
	status_label->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	root->add_child(status_label);
}
#endif
