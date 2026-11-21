#ifdef TOOLS_ENABLED
#include "luau_formatter.h"

#include "Luau/Allocator.h"
#include "Luau/Parser.h"

#include "core/io/file_access.h"
#include "core/os/os.h"
#include "editor/script/script_text_editor.h"
#include "editor/settings/editor_settings.h"
#include "scene/gui/code_edit.h"

#include <algorithm>
#include <functional>
#include <cctype>
#include <vector>

namespace LuauFormatter {
static const String prefix = "text_editor/luau/stylua/";
void initialize() {
	EDITOR_DEF(prefix + "enabled", true);
	EDITOR_DEF(prefix + "format_on_save", true);
	EDITOR_DEF(prefix + "executable", "stylua");
	EDITOR_DEF(prefix + "column_width", 120);
	EDITOR_DEF(prefix + "indent_width", 4);
	EDITOR_DEF(prefix + "indent_type", "Tabs");
	EDITOR_DEF(prefix + "line_endings", "Unix");
	EDITOR_DEF(prefix + "quote_style", "AutoPreferDouble");
	EDITOR_DEF(prefix + "call_parentheses", "Always");
	EDITOR_DEF(prefix + "collapse_simple_statement", "Never");
	EDITOR_DEF(prefix + "block_newline_gaps", "Never");
	EDITOR_DEF(prefix + "space_after_function_names", "Never");
	EDITOR_DEF(prefix + "sort_requires", false);
	const char *options[][2] = {
		{ "indent_type", "Tabs,Spaces" },
		{ "line_endings", "Unix,Windows" },
		{ "quote_style", "AutoPreferDouble,AutoPreferSingle,ForceDouble,ForceSingle" },
		{ "call_parentheses", "Always,NoSingleString,NoSingleTable,None,Input" },
		{ "collapse_simple_statement", "Never,Input,FunctionOnly,ConditionalOnly,Always" },
		{ "block_newline_gaps", "Never,Preserve" },
		{ "space_after_function_names", "Never,Definitions,Calls,Always" },
	};
	for (const auto &option : options) {
		EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, prefix + option[0], PROPERTY_HINT_ENUM, option[1]));
	}
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::INT, prefix + "indent_width", PROPERTY_HINT_RANGE, "1,16,1"));
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::INT, prefix + "column_width", PROPERTY_HINT_RANGE, "20,1000,1"));
}
bool enabled() {
	return EDITOR_GET(prefix + "enabled");
}

// Opaque dialect declarations are identified by the native AST, never by regex.
// They travel through StyLua as unique comments and are restored byte for byte.
struct Adapter : Luau::AstVisitor {
	struct Edit {
		size_t begin;
		size_t end;
		std::string text;
		std::string replacement;
	};
	std::string source;
	std::vector<size_t> lines{ 0 };
	std::vector<Edit> edits;
	std::string marker = "__LUDOT_STYLUA_";
	Adapter(const std::string &text) : source(text) {
		while (source.find(marker) != std::string::npos) {
			marker += "_";
		}
		for (size_t i = 0; i < source.size(); ++i) {
			if (source[i] == '\n') {
				lines.push_back(i + 1);
			}
		}
	}
	size_t offset(Luau::Position p) { return lines.at(p.line) + p.column; }
	size_t relative_offset(size_t start, Luau::Position position) {
		for (unsigned int line = 0; line < position.line; ++line) {
			start = source.find('\n', start) + 1;
		}
		return start + position.column;
	}
	void protect(size_t begin, size_t end, const std::string &keyword = "") {
		std::string id = marker + std::to_string(edits.size()) + "__";
		edits.push_back({ begin, end, source.substr(begin, end - begin), keyword + "--" + id + ((end < source.size() && (source[end] == '\n' || source[end] == '\r')) ? "" : "\n") });
	}
	bool visit(Luau::AstStatBlock *node) override {
		for (auto *statement : node->body) {
			struct DialectSyntax : Luau::AstVisitor {
				bool found = false;
				bool visit(Luau::AstNode *node) override {
					found = found || node->syntax != Luau::LudotSyntax::None;
					return !found;
				}
			} syntax;
			statement->visit(&syntax);
			size_t start = offset(statement->location.begin);
			if (syntax.found) {
				protect(start, offset(statement->location.end));
				continue;
			}
			if (source.compare(start, 6, "import") != 0 || (start + 6 < source.size() && (std::isalnum(static_cast<unsigned char>(source[start + 6])) || source[start + 6] == '_'))) {
				statement->visit(this);
				continue;
			}
			// Import expansions share their original start location. The lexer
			// supplies the namespace suffix omitted from the lowered local AST.
			Luau::Allocator allocator;
			Luau::AstNameTable names(allocator);
			Luau::Lexer lexer(source.data() + start, source.size() - start, names);
			while (lexer.next().type != Luau::Lexeme::Eof) {
				if (lexer.current().type != Luau::Lexeme::QuotedString && lexer.current().type != Luau::Lexeme::RawString) {
					continue;
				}
				Luau::Position end = lexer.current().location.end;
				if (lexer.next().type == Luau::Lexeme::Name && std::string(lexer.current().name) == "as") {
					end = lexer.next().location.end;
				}
				protect(start, relative_offset(start, end));
				break;
			}
		}
		return false;
	}
	bool visit(Luau::AstStatClass *node) override {
		size_t begin = offset(node->location.begin), end = offset(node->location.end);
		// The class header terminates before the first member. Use the lexer to
		// find its opening brace without confusing strings or comments with syntax.
		Luau::Allocator allocator;
		Luau::AstNameTable names(allocator);
		// Lexer startPosition changes coordinates, not the buffer offset.
		Luau::Lexer header(source.data() + begin, end - begin, names);
		while (header.next().type != Luau::Lexeme::Eof) {
			if (header.current().type == '{') {
				protect(begin, relative_offset(begin, header.current().location.end), "do ");
				break;
			}
		}
		protect(end - 1, end, "end ");
		for (const auto &member : node->members) {
			if (const auto *property = Luau::get_if<Luau::AstClassProperty>(&member)) {
				size_t start = offset((property->onreadyLocation ? *property->onreadyLocation : property->qualifierLocation).begin);
				size_t finish = offset(property->nameLocation.end);
				if (property->ty) {
					finish = offset(property->ty->location.end);
				}
				if (property->defaultValue) {
					finish = offset(property->defaultValue->location.end);
				}
				if (property->rangeMax) {
					finish = offset(property->rangeMax->location.end);
				}
				if (property->rangeStep) {
					finish = offset(property->rangeStep->location.end);
				}
				protect(start, finish);
			} else if (const auto *method = Luau::get_if<Luau::AstClassMethod>(&member)) {
				if (method->qualifierLocation) {
					protect(offset(method->qualifierLocation->begin), offset(method->function->location.end));
				} else {
					method->function->visit(this);
				}
			}
		}
		return false;
	}
};

String selene_source(const String &source) {
	CharString bytes = source.utf8();
	Adapter adapter(std::string(bytes.get_data(), bytes.length()));
	Luau::Allocator allocator;
	Luau::AstNameTable names(allocator);
	auto parsed = Luau::Parser::parse(bytes.get_data(), bytes.length(), names, allocator);
	if (!parsed.errors.empty()) {
		return String();
	}
	parsed.root->visit(&adapter);
	std::string result = adapter.source;
	std::sort(adapter.edits.begin(), adapter.edits.end(), [](const auto &a, const auto &b) { return a.begin > b.begin; });
	for (const auto &edit : adapter.edits) {
		std::string replacement = edit.text;
		for (char &character : replacement) {
			if (character != '\n' && character != '\r') {
				character = ' ';
			}
		}
		std::string keyword;
		if (edit.replacement.find("do ") == 0) {
			keyword = "do";
		}
		if (edit.replacement.find("end ") == 0) {
			keyword = "end";
		}
		if (!keyword.empty()) {
			replacement.replace(0, keyword.size(), keyword);
		}
		result.replace(edit.begin, edit.end - edit.begin, replacement);
	}
	// Both tokens have five bytes, so source coordinates are retained.
	Luau::Allocator lexer_allocator;
	Luau::AstNameTable lexer_names(lexer_allocator);
	Luau::Lexer lexer(result.data(), result.size(), lexer_names);
	Adapter offsets(result);
	lexer.setSkipComments(false);
	std::vector<Adapter::Edit> directives;
	while (lexer.next().type != Luau::Lexeme::Eof) {
		if (lexer.current().type == Luau::Lexeme::Name && std::string(lexer.current().name) == "const") {
			result.replace(offsets.offset(lexer.current().location.begin), 5, "local");
		}
		if (lexer.current().type == Luau::Lexeme::Comment) {
			size_t begin = offsets.offset(lexer.current().location.begin), end = offsets.offset(lexer.current().location.end);
			String comment = String::utf8(result.substr(begin, end - begin).c_str());
			int start = comment.find("selene: allow(");
			if (start < 0) {
				continue;
			}
			int finish = comment.find(")", start);
			if (finish < 0) {
				continue;
			}
			start += String("selene: allow(").length();
			PackedStringArray rules = comment.substr(start, finish - start).split(","), kept;
			bool removed = false;
			for (const String &rule : rules) {
				if (rule.strip_edges() == "naming_convention" || rule.strip_edges() == "match_exhaustiveness" || rule.strip_edges() == "signal_declaration") {
					removed = true;
				} else {
					kept.push_back(rule.strip_edges());
				}
			}
			if (!removed) {
				continue;
			}
			String replacement = kept.is_empty() ? String(" ").repeat(comment.length()) : comment.substr(0, start) + String(", ").join(kept) + comment.substr(finish);
			directives.push_back({ begin, end, "", std::string(replacement.utf8().get_data()) });
		}
	}
	for (auto it = directives.rbegin(); it != directives.rend(); ++it) {
		result.replace(it->begin, it->end - it->begin, it->replacement);
	}
	return String::utf8(result.c_str());
}

// Formats source with StyLua and the user's settings. Ludot declarations travel through
// StyLua as opaque comments and are restored byte for byte. p_range adds StyLua's range
// arguments for a selection, given the adapter that maps editor offsets.
static bool run_stylua(const String &original, String &r_result, String &r_error, const std::function<void(const Adapter &, List<String> &)> &p_range = nullptr) {
	CharString bytes = original.utf8();
	Adapter adapter(std::string(bytes.get_data(), bytes.length()));
	Luau::Allocator allocator;
	Luau::AstNameTable names(allocator);
	auto parsed = Luau::Parser::parse(adapter.source.data(), adapter.source.size(), names, allocator);
	if (!parsed.errors.empty()) {
		r_error = "Cannot format a script with syntax errors.";
		return false;
	}
	parsed.root->visit(&adapter);
	std::string input = adapter.source;
	std::vector<Adapter::Edit> sorted = adapter.edits;
	std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) { return a.begin > b.begin; });
	for (const auto &edit : sorted) {
		input.replace(edit.begin, edit.end - edit.begin, edit.replacement);
	}
	// Temporary files are unique and removed by FileAccess when released.
	Error temp_error;
	Ref<FileAccess> file = FileAccess::create_temp(FileAccess::READ_WRITE, "ludot-stylua", "luau", false, &temp_error);
	Ref<FileAccess> config = FileAccess::create_temp(FileAccess::READ_WRITE, "ludot-stylua", "toml", false, &temp_error);
	if (file.is_null() || config.is_null()) {
		r_error = "Could not create StyLua temporary files.";
		return false;
	}
	file->store_string(String::utf8(input.c_str()));
	file->flush();
	String settings = "syntax = \"Luau\"\n";
	for (const char *key : { "column_width", "indent_width", "indent_type", "line_endings", "quote_style", "call_parentheses", "collapse_simple_statement", "block_newline_gaps", "space_after_function_names" }) {
		Variant value = EDITOR_GET(prefix + key);
		settings += String(key) + " = " + (value.get_type() == Variant::STRING ? String(value).json_escape().quote() : String(value)) + "\n";
	}
	settings += "[sort_requires]\nenabled = " + String(bool(EDITOR_GET(prefix + "sort_requires")) ? "true" : "false") + "\n";
	config->store_string(settings);
	config->flush();
	const String input_path = file->get_path_absolute();
	const String config_path = config->get_path_absolute();
	// External tools must reopen these files. Windows denies access while our write handles remain open.
	file->close();
	config->close();
	List<String> args;
	args.push_back("--config-path");
	args.push_back(config_path);
	args.push_back("--no-editorconfig");
	args.push_back(input_path);
	if (p_range) {
		p_range(adapter, args);
	}
	String output;
	int exit_code = -1;
	String executable = EDITOR_GET(prefix + "executable");
	if (executable == "stylua") {
		String bundled = OS::get_singleton()->get_executable_path().get_base_dir().path_join("ludot.stylua");
#ifdef WINDOWS_ENABLED
		bundled += ".exe";
#endif
		if (FileAccess::exists(bundled)) {
			executable = bundled;
		}
	}
	Error error = OS::get_singleton()->execute(executable, args, &output, &exit_code, true);
	if (error != OK || exit_code != 0) {
		r_error = "StyLua formatting failed. Check the executable in Editor Settings. " + output;
		return false;
	}
	String result = FileAccess::get_file_as_string(input_path);
	for (size_t i = 0; i < adapter.edits.size(); ++i) {
		String id = String::utf8((adapter.marker + std::to_string(i) + "__").c_str());
		int at = result.find("--" + id);
		if (at < 0) {
			r_error = "StyLua did not preserve a Ludot declaration.";
			return false;
		}
		const auto &edit = adapter.edits[i];
		int start = at;
		if (edit.replacement.find("do ") == 0) {
			start -= 3;
		}
		if (edit.replacement.find("end ") == 0) {
			start -= 4;
		}
		result = result.substr(0, start) + String::utf8(edit.text.c_str()) + result.substr(at + 2 + id.length());
	}
	CharString formatted = result.utf8();
	Luau::Allocator check_allocator;
	Luau::AstNameTable check_names(check_allocator);
	if (!Luau::Parser::parse(formatted.get_data(), formatted.length(), check_names, check_allocator).errors.empty()) {
		r_error = "Formatted script failed Ludot syntax validation. Source was kept unchanged.";
		return false;
	}
	r_result = result;
	return true;
}

bool format_source(const String &source, String &r_result, String &r_error) {
	return run_stylua(source, r_result, r_error);
}

void format(ScriptTextEditor *editor, bool selection, bool on_save) {
	Ref<Script> script = editor->get_edited_resource();
	if (script.is_null() || script->get_language()->get_name() != "Luau" || !enabled()) {
		return;
	}
	if (on_save && !bool(EDITOR_GET(prefix + "format_on_save"))) {
		return;
	}
	CodeEdit *text = editor->get_code_editor()->get_text_editor();
	if (selection && !text->has_selection()) {
		return;
	}
	String original = text->get_text();
	std::function<void(const Adapter &, List<String> &)> range;
	if (selection) {
		range = [&](const Adapter &adapter, List<String> &args) {
			auto range_offset = [&](int line, int column) {
				size_t position = adapter.lines.at(line) + original.get_slice("\n", line).substr(0, column).utf8().length();
				int64_t mapped = position;
				for (const auto &edit : adapter.edits) {
					if (edit.end <= position) {
						mapped += int64_t(edit.replacement.size()) - int64_t(edit.end - edit.begin);
					} else if (edit.begin < position) {
						mapped -= position - edit.begin;
					}
				}
				return mapped;
			};
			args.push_front(itos(range_offset(text->get_selection_to_line(), text->get_selection_to_column())));
			args.push_front("--range-end");
			args.push_front(itos(range_offset(text->get_selection_from_line(), text->get_selection_from_column())));
			args.push_front("--range-start");
		};
	}
	String result;
	String error;
	if (!run_stylua(original, result, error, range)) {
		if (!on_save || !error.begins_with("Cannot format a script with syntax errors")) {
			WARN_PRINT(error);
		}
		return;
	}
	if (result == original) {
		return;
	}
	Dictionary state = editor->get_edit_state();
	text->begin_complex_operation();
	text->select_all();
	text->insert_text_at_caret(result);
	text->end_complex_operation();
	editor->set_edit_state(state);
}
} //namespace LuauFormatter
#endif
