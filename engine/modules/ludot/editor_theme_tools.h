#pragma once
#ifdef TOOLS_ENABLED
#include "scene/gui/box_container.h"
class OptionButton;
class LineEdit;
class TextEdit;
class Label;
class LudotThemeTools : public VBoxContainer {
	GDCLASS(LudotThemeTools, VBoxContainer);
	OptionButton *themes = nullptr;
	LineEdit *theme_name = nullptr;
	TextEdit *code = nullptr;
	Label *message = nullptr;
	Dictionary capture() const;
	bool apply(const Dictionary &values);
	void refresh();
	void choose(int index);
	void save_theme();
	void delete_theme();
	void export_code();
	void import_code();

public:
	LudotThemeTools();
};
#endif
