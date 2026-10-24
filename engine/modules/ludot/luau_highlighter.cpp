#include "luau_highlighter.h"
#ifdef TOOLS_ENABLED
#include "luau_script.h"
#include "Luau/Lexer.h"
#include "core/object/class_db.h"
#include "editor/settings/editor_settings.h"
#include "scene/gui/text_edit.h"

void LuauSyntaxHighlighter::_update_cache() {
    dirty = true;
    words.clear();
    keyword = EDITOR_GET("text_editor/theme/highlighting/keyword_color");
    control = EDITOR_GET("text_editor/theme/highlighting/control_flow_keyword_color");
    number = EDITOR_GET("text_editor/theme/highlighting/number_color");
    string = EDITOR_GET("text_editor/theme/highlighting/string_color");
    comment = EDITOR_GET("text_editor/theme/highlighting/comment_color");
    doc_comment = EDITOR_GET("text_editor/theme/highlighting/doc_comment_color");
    symbol = EDITOR_GET("text_editor/theme/highlighting/symbol_color");
    function = EDITOR_GET("text_editor/theme/highlighting/function_color");
    member = EDITOR_GET("text_editor/theme/highlighting/member_variable_color");
    LocalVector<StringName> classes;
    ClassDB::get_class_list(classes);
    const Color engine_type = EDITOR_GET("text_editor/theme/highlighting/engine_type_color");
    for (const StringName &word_name : classes) words.insert(word_name, engine_type);
    classes.clear();
    ScriptServer::get_global_class_list(classes);
    const Color user_type = EDITOR_GET("text_editor/theme/highlighting/user_type_color");
    for (const StringName &word_name : classes) words.insert(word_name, user_type);
    const Color base_type = EDITOR_GET("text_editor/theme/highlighting/base_type_color");
    for (const char *word_name : {"number", "string", "boolean", "thread", "buffer", "any", "unknown", "never", "Vector2", "Vector3", "Vector4", "Color", "CFrame"}) words.insert(word_name, base_type);
    const auto language = LuauLanguage::get_singleton();
    for (const String &word_name : language->get_reserved_words()) words.insert(word_name, language->is_control_flow_keyword(word_name) ? control : keyword);
    words.insert("self", keyword);
    words.insert("typeof", base_type);
}

void LuauSyntaxHighlighter::rebuild() {
    version = text_edit->get_version();
    dirty = false;
    lines.clear();
    lines.resize(text_edit->get_line_count());
    const String source = text_edit->get_text();
    const CharString bytes = source.utf8();
    const PackedStringArray source_lines = source.split("\n");
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);
    Luau::Lexer lexer(bytes.get_data(), bytes.length(), names);
    const Color normal = text_edit->get_font_color();
    bool after_member = false;
    bool class_name = false;
    while (true) {
        const auto token = lexer.next();
        using T = Luau::Lexeme;
        if (token.type == T::Eof) break;
        Color color = symbol;
        if (token.type == T::Comment || token.type == T::BlockComment || token.type == T::BrokenComment) {
            color = comment;
            if (token.type == T::Comment && String::utf8(token.data, token.getLength()).begins_with("-")) color = doc_comment;
        } else if (token.type == T::QuotedString || token.type == T::RawString || token.type == T::BrokenString || token.type == T::InterpStringBegin || token.type == T::InterpStringMid || token.type == T::InterpStringEnd || token.type == T::InterpStringSimple) {
            color = string;
        } else if (token.type == T::Number) {
            color = number;
        } else if (token.type >= T::Reserved_BEGIN && token.type < T::Reserved_END) {
            const String word = String::utf8(token.toString().c_str()).unquote();
            color = LuauLanguage::get_singleton()->is_control_flow_keyword(word) ? control : keyword;
        } else if (token.type == T::Name) {
            const String word = String::utf8(token.name);
            const auto found = words.getptr(word);
            color = found != nullptr ? *found : class_name ? Color(EDITOR_GET("text_editor/theme/highlighting/user_type_color")) : after_member ? member : normal;
            if (lexer.lookahead().type == '(' && found == nullptr) color = function;
            class_name = word == "class";
        }
        // Lexer locations count UTF-8 bytes; TextEdit columns count code points.
        for (unsigned line = token.location.begin.line; line <= token.location.end.line && line < unsigned(lines.size()); ++line) {
            const CharString line_bytes = source_lines[line].utf8();
            const int start_byte = line == token.location.begin.line ? int(token.location.begin.column) : 0;
            const int end_byte = line == token.location.end.line ? int(token.location.end.column) : line_bytes.length();
            const int start = String::utf8(line_bytes.get_data(), MIN(start_byte, line_bytes.length())).length();
            const int end = String::utf8(line_bytes.get_data(), MIN(end_byte, line_bytes.length())).length();
            if (start == end) continue;
            Dictionary begin_color; begin_color["color"] = color;
            Dictionary end_color; end_color["color"] = normal;
            lines.write[line][start] = begin_color;
            lines.write[line][end] = end_color;
        }
        after_member = token.type == '.' || token.type == ':';
    }
}
Dictionary LuauSyntaxHighlighter::_get_line_syntax_highlighting_impl(int p_line) {
    if (dirty || version != text_edit->get_version()) rebuild();
    return lines[p_line];
}
Ref<EditorSyntaxHighlighter> LuauSyntaxHighlighter::_create() const {
    Ref<LuauSyntaxHighlighter> result;
    result.instantiate();
    return result;
}
#endif
