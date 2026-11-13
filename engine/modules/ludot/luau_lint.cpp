#ifdef TOOLS_ENABLED
#include "luau_lint.h"

#include "Luau/Parser.h"
#include "luau_formatter.h"

#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/os/os.h"
#include "editor/settings/editor_settings.h"

#include <cctype>
#include <vector>

namespace {
const String prefix = "text_editor/luau/lint/";
struct Naming : Luau::AstVisitor {
	List<ScriptLanguage::Warning> *warnings;
	void check(Luau::AstName name, Luau::Location location, const char *kind) {
		String text = String::utf8(name.value);
		if (text.begins_with("__ludot_") || text == "self" || text == "_") {
			return;
		}
		while (text.begins_with("_")) {
			text = text.substr(1);
		}
		String style = EDITOR_GET(prefix + kind + "_casing");
		if (style == "Any" || text.is_empty()) {
			return;
		}
		bool valid = true;
		if (style == "camelCase") {
			valid = text[0] >= 'a' && text[0] <= 'z' && !text.contains("_");
		} else if (style == "PascalCase") {
			valid = text[0] >= 'A' && text[0] <= 'Z' && !text.contains("_");
		} else if (style == "snake_case") {
			valid = text == text.to_lower();
		} else if (style == "SCREAMING_SNAKE_CASE") {
			valid = text == text.to_upper();
		}
		if (valid) {
			return;
		}
		ScriptLanguage::Warning warning;
		warning.start_line = location.begin.line + 1;
		warning.end_line = location.end.line + 1;
		warning.code = 2000;
		warning.string_code = "naming_convention";
		warning.message = String::utf8(name.value) + " should use " + style + " for " + kind + " names.";
		warnings->push_back(warning);
	}
	bool visit(Luau::AstStatLocal *node) override {
		for (size_t i = 0; i < node->vars.size; ++i) {
			const char *kind = node->isConst ? "constant" : "variable";
			if (i < node->values.size && node->values.data[i]->is<Luau::AstExprFunction>()) {
				kind = "function";
			}
			check(node->vars.data[i]->name, node->vars.data[i]->location, kind);
		}
		return true;
	}
	bool visit(Luau::AstStatLocalFunction *node) override {
		check(node->name->name, node->name->location, "function");
		return true;
	}
	bool visit(Luau::AstStatFunction *node) override {
		if (auto *name = node->name->as<Luau::AstExprGlobal>()) {
			check(name->name, name->location, "function");
		}
		if (auto *name = node->name->as<Luau::AstExprIndexName>()) {
			check(name->index, name->indexLocation, "method");
		}
		return true;
	}
	bool visit(Luau::AstExprFunction *node) override {
		for (auto *arg : node->args) {
			check(arg->name, arg->location, "variable");
		}
		return true;
	}
	bool visit(Luau::AstStatClass *node) override {
		check(node->name->name, node->name->location, "class");
		for (const auto &member : node->members) {
			if (const auto *property = Luau::get_if<Luau::AstClassProperty>(&member)) {
				check(property->name, property->nameLocation, "property");
			}
			if (const auto *method = Luau::get_if<Luau::AstClassMethod>(&member)) {
				check(method->functionName, method->nameLocation, "method");
			}
		}
		return true;
	}
};
struct Statements : Luau::AstVisitor {
	std::vector<Luau::Location> locations;
	bool visit(Luau::AstStat *node) override {
		locations.push_back(node->location);
		return true;
	}
};
void suppress(const String &source, const Luau::ParseResult &parsed, List<ScriptLanguage::Warning> *warnings) {
	Statements statements;
	parsed.root->visit(&statements);
	PackedStringArray lines = source.split("\n");
	for (const auto &comment : parsed.commentLocations) {
		int line = comment.location.begin.line;
		String directive = lines[line].substr(comment.location.begin.column).strip_edges();
		bool global = directive.begins_with("--# selene: allow(");
		if (!global && !directive.begins_with("-- selene: allow(")) {
			continue;
		}
		int start = directive.find("("), end = directive.find(")", start);
		if (end < 0) {
			continue;
		}
		PackedStringArray codes = directive.substr(start + 1, end - start - 1).split(",");
		unsigned int first = 0, last = UINT32_MAX;
		if (global) {
			if (parsed.root->body.size && !(comment.location.end <= parsed.root->body.data[0]->location.begin)) {
				continue;
			}
		} else {
			Luau::Position next(UINT32_MAX, UINT32_MAX);
			for (const auto &location : statements.locations) {
				if (location.begin >= comment.location.end && location.begin < next) {
					next = location.begin;
					first = location.begin.line;
					last = location.end.line;
				}
			}
			if (next.line == UINT32_MAX) {
				continue;
			}
		}
		for (auto *element = warnings->front(); element;) {
			auto *following = element->next();
			const auto &warning = element->get();
			bool match = false;
			for (const String &code : codes) {
				if (code.strip_edges() == warning.string_code || code.strip_edges() == "all") {
					match = true;
				}
			}
			if (match && unsigned(warning.start_line - 1) >= first && unsigned(warning.start_line - 1) <= last) {
				warnings->erase(element);
			}
			element = following;
		}
	}
}
} //namespace
void ludot_initialize_lint() {
	EDITOR_DEF(prefix + "enabled", true);
	EDITOR_DEF(prefix + "selene_enabled", true);
	EDITOR_DEF(prefix + "selene_executable", "selene");
	EDITOR_DEF(prefix + "selene_config", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, prefix + "selene_config", PROPERTY_HINT_GLOBAL_FILE, "*.toml"));
	for (const char *kind : { "variable", "constant", "function", "method", "property", "class" }) {
		String key = prefix + kind + "_casing";
		EDITOR_DEF(key, String(kind) == "property" || String(kind) == "class" ? "PascalCase" : "camelCase");
		EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, key, PROPERTY_HINT_ENUM, "Any,camelCase,PascalCase,snake_case,SCREAMING_SNAKE_CASE"));
	}
}
void ludot_lint(const String &source, const Luau::ParseResult &parsed, List<ScriptLanguage::Warning> *warnings) {
	if (!warnings || !EditorSettings::get_singleton() || !bool(EDITOR_GET(prefix + "enabled"))) {
		return;
	}
	Naming naming;
	naming.warnings = warnings;
	parsed.root->visit(&naming);
	if (bool(EDITOR_GET(prefix + "selene_enabled"))) {
		Error temp_error;
		Ref<FileAccess> file = FileAccess::create_temp(FileAccess::WRITE, "ludot-selene", "luau", false, &temp_error);
		Ref<FileAccess> config = FileAccess::create_temp(FileAccess::WRITE, "ludot-selene", "toml", false, &temp_error);
		if (file.is_valid() && config.is_valid()) {
			file->store_string(LuauFormatter::selene_source(source));
			file->flush();
			// The native checker already validates engine globals and their types.
			config->store_string("std = \"luau\"\n[lints]\nundefined_variable = \"allow\"\nincorrect_standard_library_use = \"allow\"\n");
			config->flush();
			String config_path = EDITOR_GET(prefix + "selene_config");
			List<String> args;
			args.push_back("--display-style");
			args.push_back("Json");
			args.push_back("--no-summary");
			args.push_back("--config");
			args.push_back(config_path.is_empty() ? config->get_path_absolute() : config_path);
			args.push_back(file->get_path_absolute());
			String executable = EDITOR_GET(prefix + "selene_executable");
			if (executable == "selene") {
				String bundled = OS::get_singleton()->get_executable_path().get_base_dir().path_join("ludot.selene");
#ifdef WINDOWS_ENABLED
				bundled += ".exe";
#endif
				if (FileAccess::exists(bundled)) {
					executable = bundled;
				}
			}
			String output;
			int exit_code = -1;
			Error error = OS::get_singleton()->execute(executable, args, &output, &exit_code, true);
			if (error == OK && (exit_code == 0 || exit_code == 1)) {
				for (const String &line : output.split("\n")) {
					if (!line.begins_with("{")) {
						continue;
					}
					Variant value = JSON::parse_string(line);
					if (value.get_type() != Variant::DICTIONARY) {
						continue;
					}
					Dictionary diagnostic = value;
					if (!diagnostic.has("primary_label")) {
						continue;
					}
					Dictionary label = diagnostic["primary_label"], span = label.get("span", Dictionary());
					ScriptLanguage::Warning warning;
					warning.start_line = int(span.get("start_line", 0)) + 1;
					warning.end_line = int(span.get("end_line", 0)) + 1;
					warning.code = 3000;
					warning.string_code = diagnostic.get("code", "selene");
					warning.message = diagnostic.get("message", "Selene warning");
					warnings->push_back(warning);
				}
			} else {
				ScriptLanguage::Warning warning;
				warning.start_line = warning.end_line = 1;
				warning.code = 3001;
				warning.string_code = "selene_unavailable";
				warning.message = "Selene could not run. Check its executable and configuration in Editor Settings. " + output.substr(0, 500);
				warnings->push_back(warning);
			}
		}
	}
	suppress(source, parsed, warnings);
}
#endif
