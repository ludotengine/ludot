#include "luau_documentation.h"
#ifdef TOOLS_ENABLED
#include "Luau/Lexer.h"
#include "Luau/LudotNaming.h"
#include "luau_script.h"
#include "ludot_doc_examples.gen.h"

#include "core/io/json.h"
#include "editor/settings/editor_settings.h"

String ludot_luau_example(const String &p_key) {
	static const Dictionary examples = JSON::parse_string(String::utf8(reinterpret_cast<const char *>(ludot_doc_examples)));
	const String key = p_key.begins_with("Signal.") && !examples.has(p_key) ? String("Signal") : p_key;
	if (!examples.has(key)) {
		return String();
	}
	const Dictionary entry = examples[key];
	const Dictionary bindings = entry["names"];
	const String source = entry["code"];
	const CharString bytes = source.utf8();
	std::string result;
	std::vector<size_t> lines{ 0 };
	for (int i = 0; i < bytes.length(); ++i) {
		if (bytes[i] == '\n') {
			lines.push_back(i + 1);
		}
	}
	Luau::Allocator allocator;
	Luau::AstNameTable names(allocator);
	Luau::Lexer lexer(bytes.get_data(), bytes.length(), names);
	size_t copied = 0;
	Luau::Lexeme::Type previous = Luau::Lexeme::Eof;
	while (true) {
		const auto token = lexer.next();
		if (token.type == Luau::Lexeme::Eof) {
			break;
		}
		const auto before = previous;
		previous = token.type;
		if (token.type != Luau::Lexeme::Name || !bindings.has(String::utf8(token.name))) {
			continue;
		}
		const Array binding = bindings[String::utf8(token.name)];
		const String kind = binding[0];
		const String native = binding[1];
		if ((kind == "api_property" && before != '.') || (kind == "api_method" && before != '.' && before != ':')) {
			continue;
		}
		String name = String::utf8(token.name);
		if (kind == "api_method" || kind == "api_property" || kind == "api_callback") {
			name = LuauLanguage::api_name(native, kind == "api_property");
		} else {
			const String style = EditorSettings::get_singleton() ? String(EDITOR_GET("text_editor/luau/lint/" + kind + "_casing")) : (kind == "class" || kind == "property" ? String("PascalCase") : String("camelCase"));
			Luau::ApiCase casing;
			if (Luau::parseApiCase(style.utf8().get_data(), casing)) {
				name = String::utf8(Luau::engineApiName(native.utf8().get_data(), casing).c_str());
			} else if (style == "SCREAMING_SNAKE_CASE") {
				name = native.to_snake_case().to_upper();
			}
		}
		const size_t begin = lines[token.location.begin.line] + token.location.begin.column;
		const size_t end = lines[token.location.end.line] + token.location.end.column;
		result.append(bytes.get_data() + copied, begin - copied);
		result += name.utf8().get_data();
		copied = end;
	}
	result.append(bytes.get_data() + copied, bytes.length() - copied);
	String code = String::utf8(result.c_str());
	const String prefix = "text_editor/luau/stylua/";
	// Use the formatter's indentation settings without spawning StyLua per hover.
	const String indent = EditorSettings::get_singleton() && String(EDITOR_GET(prefix + "indent_type")) == "Spaces" ? String(" ").repeat(MAX(1, int(EDITOR_GET(prefix + "indent_width")))) : String("\t");
	PackedStringArray source_lines = code.split("\n");
	for (int line = 0; line < source_lines.size(); ++line) {
		const String text = source_lines[line];
		int depth = 0;
		while (depth < text.length() && text[depth] == '\t') {
			++depth;
		}
		source_lines.set(line, indent.repeat(depth) + text.substr(depth));
	}
	return String("\n").join(source_lines);
}

String ludot_luau_doc(const String &p_text, const String &p_class, const String &p_member) {
	String text = p_text;
	const String code = ludot_luau_example(p_class + (p_member.is_empty() ? String() : "." + p_member));
	bool inserted = false;
	for (const char *kind : { "codeblocks", "codeblock", "gdscript", "csharp" }) {
		const String open = "[" + String(kind);
		const String close = "[/" + String(kind) + "]";
		int from = 0;
		for (int begin = text.find(open, from); begin >= 0; begin = text.find(open, from)) {
			const int end = text.find(close, begin);
			if (end < 0) {
				break;
			}
			if (text.substr(begin).begins_with("[codeblock lang=luau]")) {
				inserted = true;
				from = end + close.length();
				continue;
			}
			const String replacement = !inserted && !code.is_empty() ? "[codeblock lang=luau]\n" + code + "\n[/codeblock]" : "[i]A Luau translation of this example is not available yet.[/i]";
			inserted = inserted || !code.is_empty();
			text = text.substr(0, begin) + replacement + text.substr(end + close.length());
			from = begin + replacement.length();
		}
	}
	if (!inserted && !code.is_empty()) {
		text += "\n[codeblock lang=luau]\n" + code + "\n[/codeblock]";
	}
	return text;
}
#endif
