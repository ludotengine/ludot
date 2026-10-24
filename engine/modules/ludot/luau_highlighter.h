#pragma once
#ifdef TOOLS_ENABLED
#include "editor/script/syntax_highlighters.h"

// Uses the language lexer, including long brackets and interpolated strings.
class LuauSyntaxHighlighter : public EditorSyntaxHighlighter {
    GDCLASS(LuauSyntaxHighlighter, EditorSyntaxHighlighter)
    HashMap<String, Color> words;
    Vector<Dictionary> lines;
    uint32_t version = 0;
    bool dirty = true;
    Color keyword, control, number, string, comment, doc_comment, symbol, function, member;
    void rebuild();
protected:
    static void _bind_methods() {}
public:
    void _update_cache() override;
    Dictionary _get_line_syntax_highlighting_impl(int p_line) override;
    String _get_name() const override { return "Luau"; }
    PackedStringArray _get_supported_languages() const override { return {"Luau"}; }
    Ref<EditorSyntaxHighlighter> _create() const override;
};
#endif
