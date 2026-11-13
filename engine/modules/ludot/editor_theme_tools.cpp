#ifdef TOOLS_ENABLED
#include "editor_theme_tools.h"

#include "core/core_bind.h"
#include "core/crypto/crypto_core.h"
#include "core/io/compression.h"
#include "core/io/json.h"
#include "core/object/callable_mp.h"
#include "core/os/keyboard.h"
#include "core/os/os.h"
#include "editor/settings/editor_settings.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/text_edit.h"
#include "servers/display/display_server.h"

#include <iterator>

namespace {
const char *saved_key = "interface/theme/ludot_saved_themes";
struct Palette {
	const char *name;
	const char *background;
	const char *foreground;
	const char *accent;
	const char *comment;
	const char *keyword;
	const char *string;
	const char *number;
	const char *type;
	const char *error;
};
// Palette adaptations for Ludot. Original theme projects are credited in docs/themes.md.
const Palette palettes[] = {
	{ "Kanagawa", "1f1f28", "dcd7ba", "7e9cd8", "727169", "957fb8", "98bb6c", "d27e99", "7aa89f", "e82424" },
	{ "Catppuccin Mocha", "1e1e2e", "cdd6f4", "89b4fa", "6c7086", "cba6f7", "a6e3a1", "fab387", "f9e2af", "f38ba8" },
	{ "Nord", "2e3440", "d8dee9", "88c0d0", "616e88", "81a1c1", "a3be8c", "b48ead", "8fbcbb", "bf616a" },
	{ "Everforest Dark", "2d353b", "d3c6aa", "a7c080", "859289", "d699b6", "a7c080", "d699b6", "dbbc7f", "e67e80" },
	{ "Gruvbox Dark", "282828", "ebdbb2", "83a598", "928374", "fb4934", "b8bb26", "d3869b", "fabd2f", "fb4934" },
};
bool shareable(const PropertyInfo &property) {
	String key = property.name;
	if (property.type == Variant::COLOR) {
		return key.begins_with("text_editor/theme/highlighting/") || key.begins_with("interface/theme/");
	}
	return key == "interface/theme/contrast" || key == "interface/theme/icon_saturation" || key == "interface/theme/draw_extra_borders" || key == "interface/theme/draw_relationship_lines" || key == "interface/theme/corner_radius";
}
} //namespace

Dictionary LudotThemeTools::capture() const {
	Dictionary values;
	List<PropertyInfo> properties;
	EditorSettings::get_singleton()->get_property_list(&properties);
	for (const PropertyInfo &property : properties) {
		if (!shareable(property)) {
			continue;
		}
		Variant value = EditorSettings::get_singleton()->get(property.name);
		values[property.name] = property.type == Variant::COLOR ? Variant(String("#") + Color(value).to_html()) : value;
	}
	return values;
}

bool LudotThemeTools::apply(const Dictionary &values) {
	Dictionary current = capture();
	// Validate the whole external payload before modifying any settings.
	for (const Variant &key : values.get_key_list()) {
		if (!current.has(key)) {
			return false;
		}
		Variant value = values[key];
		Variant original = current[key];
		if (original.get_type() == Variant::STRING) {
			if (value.get_type() != Variant::STRING || !Color::html_is_valid(value)) {
				return false;
			}
		} else if (original.get_type() == Variant::BOOL) {
			if (value.get_type() != Variant::BOOL) {
				return false;
			}
		} else {
			if (!value.is_num() || !Math::is_finite(double(value))) {
				return false;
			}
			double number = value;
			String setting = key;
			double minimum = setting.ends_with("contrast") ? -1 : 0;
			double maximum = setting.ends_with("corner_radius") ? 6 : (setting.ends_with("icon_saturation") || setting.ends_with("draw_relationship_lines") ? 2 : 1);
			if (number < minimum || number > maximum) {
				return false;
			}
		}
	}
	EditorSettings *settings = EditorSettings::get_singleton();
	settings->set_manually("interface/theme/color_preset", "Custom");
	settings->set_manually("text_editor/theme/color_theme", "Custom");
	for (const Variant &key : values.get_key_list()) {
		Variant value = values[key];
		settings->set_manually(key, value.get_type() == Variant::STRING ? Variant(Color::html(value)) : value);
	}
	settings->notify_changes();
	settings->emit_signal("settings_changed");
	EditorSettings::save();
	message->set_text(TTR("Theme applied. Edit individual colors in General > Text Editor > Theme or Interface > Theme."));
	return true;
}

void LudotThemeTools::refresh() {
	themes->clear();
	for (const Palette &palette : palettes) {
		themes->add_item(palette.name);
	}
	Dictionary saved = EDITOR_GET(saved_key);
	for (const Variant &key : saved.get_key_list()) {
		themes->add_item(String(key));
	}
}

void LudotThemeTools::choose(int index) {
	if (index >= int(std::size(palettes))) {
		Dictionary saved = EDITOR_GET(saved_key);
		String name = themes->get_item_text(index);
		apply(saved[name]);
		theme_name->set_text(name);
		return;
	}
	const Palette &palette = palettes[index];
	Dictionary values = capture();
	Color background = Color::html(palette.background), foreground = Color::html(palette.foreground);
	for (const Variant &key : values.get_key_list()) {
		String name = key;
		if (values[key].get_type() != Variant::STRING) {
			continue;
		}
		Color color = foreground;
		if (name.contains("background")) {
			color = background;
		} else if (name.contains("comment")) {
			color = Color::html(palette.comment);
		} else if (name.contains("keyword")) {
			color = Color::html(palette.keyword);
		} else if (name.contains("string")) {
			color = Color::html(palette.string);
		} else if (name.contains("number_color") && !name.contains("line_number")) {
			color = Color::html(palette.number);
		} else if (name.contains("type_color")) {
			color = Color::html(palette.type);
		} else if (name.contains("function") || name.ends_with("accent_color")) {
			color = Color::html(palette.accent);
		} else if (name.contains("mismatch") || name.contains("mark_color")) {
			color = Color::html(palette.error);
		} else if (name.contains("text_selected")) {
			color = foreground;
		} else if (name.contains("selection") || name.contains("selected")) {
			color = Color::html(palette.accent).lerp(background, 0.6);
		} else if (name.contains("current_line")) {
			color = foreground * Color(1, 1, 1, 0.06);
		} else if (name.contains("guideline") || name.contains("highlighted") || name.contains("folded")) {
			color = foreground * Color(1, 1, 1, 0.15);
		} else if (name.contains("line_number")) {
			color = Color::html(palette.comment);
		}
		if (name.ends_with("base_color")) {
			color = background;
		}
		values[key] = String("#") + color.to_html();
	}
	values["interface/theme/contrast"] = 0.2;
	apply(values);
	theme_name->set_text(palette.name);
}

void LudotThemeTools::save_theme() {
	String name = theme_name->get_text().strip_edges();
	if (name.is_empty()) {
		message->set_text(TTR("Enter a theme name."));
		return;
	}
	for (const Palette &palette : palettes) {
		if (name == palette.name) {
			message->set_text(TTR("Choose a new name to save a copy of a built-in theme."));
			return;
		}
	}
	Dictionary saved = Dictionary(EDITOR_GET(saved_key)).duplicate(true);
	saved[name] = capture();
	EditorSettings::get_singleton()->set_manually(saved_key, saved);
	EditorSettings::save();
	refresh();
	for (int index = 0; index < themes->get_item_count(); ++index) {
		if (themes->get_item_text(index) == name) {
			themes->select(index);
		}
	}
	message->set_text(TTR("Theme saved."));
}
void LudotThemeTools::delete_theme() {
	Dictionary saved = Dictionary(EDITOR_GET(saved_key)).duplicate(true);
	if (!saved.erase(theme_name->get_text().strip_edges())) {
		return;
	}
	EditorSettings::get_singleton()->set_manually(saved_key, saved);
	EditorSettings::save();
	refresh();
	message->set_text(TTR("Saved theme deleted."));
}
void LudotThemeTools::export_code() {
	CharString json = JSON::stringify(capture(), "", true).utf8();
	Vector<uint8_t> compressed;
	compressed.resize(Compression::get_max_compressed_buffer_size(json.length(), Compression::MODE_DEFLATE));
	int64_t size = Compression::compress(compressed.ptrw(), reinterpret_cast<const uint8_t *>(json.get_data()), json.length(), Compression::MODE_DEFLATE);
	if (size <= 0) {
		return;
	}
	compressed.resize(size);
	String share = "LDT1:" + CoreBind::Marshalls::get_singleton()->raw_to_base64(compressed);
	code->set_text(share);
	DisplayServer::get_singleton()->clipboard_set(share);
	message->set_text(TTR("Theme code copied to the clipboard."));
}
void LudotThemeTools::import_code() {
	String share = code->get_text().strip_edges();
	if (!share.begins_with("LDT1:") || share.length() > 65536) {
		message->set_text(TTR("Invalid theme code."));
		return;
	}
	CharString encoded = share.substr(5).utf8();
	Vector<uint8_t> compressed;
	compressed.resize(encoded.length() / 4 * 3 + 4);
	size_t decoded = 0;
	if (CryptoCore::b64_decode(compressed.ptrw(), compressed.size(), &decoded, reinterpret_cast<const uint8_t *>(encoded.get_data()), encoded.length()) != OK) {
		message->set_text(TTR("Invalid theme code."));
		return;
	}
	compressed.resize(decoded);
	Vector<uint8_t> bytes;
	if (compressed.is_empty() || Compression::decompress_dynamic(&bytes, 65536, compressed.ptr(), compressed.size(), Compression::MODE_DEFLATE) != OK) {
		message->set_text(TTR("Invalid theme code."));
		return;
	}
	Ref<JSON> json;
	json.instantiate();
	if (json->parse(String::utf8(reinterpret_cast<const char *>(bytes.ptr()), bytes.size())) != OK || json->get_data().get_type() != Variant::DICTIONARY || !apply(json->get_data())) {
		message->set_text(TTR("Invalid or unsupported theme colors."));
	}
}
LudotThemeTools::LudotThemeTools() {
	set_name(TTR("Themes"));
	EDITOR_DEF(saved_key, Dictionary());
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::DICTIONARY, saved_key, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	Label *help = memnew(Label);
	help->set_text(TTR("Choose a preset, then customize colors in General. Save a named theme or share a compact code."));
	help->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	add_child(help);
	themes = memnew(OptionButton);
	themes->set_allow_reselect(true);
	add_child(themes);
	refresh();
	themes->connect("item_selected", callable_mp(this, &LudotThemeTools::choose));
	theme_name = memnew(LineEdit);
	theme_name->set_name("ThemeName");
	theme_name->set_placeholder(TTR("Theme name"));
	add_child(theme_name);
	HBoxContainer *buttons = memnew(HBoxContainer);
	add_child(buttons);
	Button *save = memnew(Button);
	save->set_text(TTR("Save Theme"));
	save->connect("pressed", callable_mp(this, &LudotThemeTools::save_theme));
	buttons->add_child(save);
	Button *remove = memnew(Button);
	remove->set_text(TTR("Delete Saved Theme"));
	remove->connect("pressed", callable_mp(this, &LudotThemeTools::delete_theme));
	buttons->add_child(remove);
	code = memnew(TextEdit);
	code->set_custom_minimum_size(Size2(0, 120));
	code->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	code->set_line_wrapping_mode(TextEdit::LINE_WRAPPING_BOUNDARY);
	add_child(code);
	Button *copy = memnew(Button);
	copy->set_text(TTR("Copy Current Theme Code"));
	copy->connect("pressed", callable_mp(this, &LudotThemeTools::export_code));
	add_child(copy);
	Button *import = memnew(Button);
	import->set_text(TTR("Apply Theme Code"));
	import->connect("pressed", callable_mp(this, &LudotThemeTools::import_code));
	add_child(import);
	message = memnew(Label);
	message->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	add_child(message);
}
#endif
