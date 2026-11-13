#ifdef TOOLS_ENABLED
#include "luau_editor_assist.h"

#include "Luau/Allocator.h"
#include "Luau/Lexer.h"

#include "editor/settings/editor_settings.h"
#include "scene/gui/code_edit.h"

void ludot_initialize_editor_assist() {
	EDITOR_DEF("text_editor/luau/completion/auto_insert_end", true);
}

bool ludot_complete_block(CodeEdit *editor) {
	if (!editor->is_auto_indent_enabled() || editor->get_caret_count() != 1 || editor->has_selection()) {
		return false;
	}
	int line = editor->get_caret_line(), column = editor->get_caret_column();
	String current = editor->get_line(line);
	if (!current.substr(column).strip_edges().is_empty() || editor->is_in_comment(line, column) != -1 || editor->is_in_string(line, column) != -1) {
		return false;
	}
	// Lex the complete prefix so long strings and multiline comments cannot
	// impersonate a block opener on the current line.
	String prefix;
	for (int index = 0; index < line; ++index) {
		prefix += editor->get_line(index) + "\n";
	}
	prefix += current.substr(0, column);
	CharString bytes = prefix.utf8();
	Luau::Allocator allocator;
	Luau::AstNameTable names(allocator);
	Luau::Lexer lexer(bytes.get_data(), bytes.length(), names);
	Luau::Lexeme::Type last = Luau::Lexeme::Eof;
	bool function = false, branch = false;
	int parentheses = 0;
	while (lexer.next().type != Luau::Lexeme::Eof) {
		if (lexer.current().location.begin.line != unsigned(line)) {
			continue;
		}
		last = lexer.current().type;
		if (last == '(') {
			++parentheses;
		}
		if (last == ')') {
			--parentheses;
		}
		function |= last == Luau::Lexeme::ReservedFunction;
		branch |= last == Luau::Lexeme::ReservedElse || last == Luau::Lexeme::ReservedElseif;
	}
	if (last != Luau::Lexeme::ReservedThen && last != Luau::Lexeme::ReservedDo && last != Luau::Lexeme::ReservedElse && !(function && parentheses == 0 && last != Luau::Lexeme::ReservedEnd)) {
		return false;
	}
	int indentation = 0;
	while (indentation < current.length() && (current[indentation] == ' ' || current[indentation] == '\t')) {
		++indentation;
	}
	String base = current.substr(0, indentation);
	bool closed = branch || !bool(EDITOR_GET("text_editor/luau/completion/auto_insert_end"));
	for (int index = line + 1; !closed && index < editor->get_line_count(); ++index) {
		String following = editor->get_line(index);
		if (following.strip_edges().is_empty()) {
			continue;
		}
		int whitespace = 0;
		while (whitespace < following.length() && (following[whitespace] == ' ' || following[whitespace] == '\t')) {
			++whitespace;
		}
		if (whitespace < indentation) {
			break;
		}
		if (whitespace > indentation) {
			continue;
		}
		String statement = following.strip_edges();
		closed = statement == "end" || statement.begins_with("end ") || statement.begins_with("end)") || statement.begins_with("end,") || statement.begins_with("else");
		break;
	}
	String indent = editor->is_indent_using_spaces() ? String(" ").repeat(editor->get_indent_size()) : "\t";
	editor->begin_complex_operation();
	editor->insert_text_at_caret("\n" + base + indent + (closed ? "" : "\n" + base + "end"));
	editor->set_caret_line(line + 1);
	editor->set_caret_column(base.length() + indent.length());
	editor->end_complex_operation();
	return true;
}
#endif
