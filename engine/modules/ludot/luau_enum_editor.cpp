#ifdef TOOLS_ENABLED
#include "luau_enum_editor.h"

#include "luau_script.h"

#include "core/object/callable_mp.h"
#include "editor/inspector/editor_inspector.h"
#include "scene/gui/option_button.h"

class LuauEnumProperty : public EditorProperty {
	GDCLASS(LuauEnumProperty, EditorProperty);
	OptionButton *options;
	Array values;
	void selected(int index) { emit_changed(get_edited_property(), values[index]); }

protected:
	void _set_read_only(bool read_only) override { options->set_disabled(read_only); }

public:
	void update_property() override {
		const Variant value = get_edited_property_value();
		options->select(values.find(value));
		if (values.find(value) < 0) {
			options->set_text(String(value));
		}
	}
	void setup(const Dictionary &metadata) {
		const PackedStringArray labels = metadata["names"];
		values = metadata["values"];
		for (int i = 0; i < labels.size(); ++i) {
			options->add_item(labels[i]);
			options->set_item_tooltip(i, String(values[i]));
		}
	}
	LuauEnumProperty() {
		options = memnew(OptionButton);
		options->set_auto_translate_mode(AUTO_TRANSLATE_MODE_DISABLED);
		options->set_clip_text(true);
		options->set_h_size_flags(SIZE_EXPAND_FILL);
		add_child(options);
		add_focusable(options);
		options->connect("item_selected", callable_mp(this, &LuauEnumProperty::selected));
	}
};
class LuauEnumInspector : public EditorInspectorPlugin {
	GDCLASS(LuauEnumInspector, EditorInspectorPlugin);

public:
	bool can_handle(Object *object) override {
		Ref<LuauScript> script = object->get_script();
		return script.is_valid();
	}
	bool parse_property(Object *object, const Variant::Type type, const String &path, const PropertyHint hint, const String &hint_text, const BitField<PropertyUsageFlags> usage, const bool wide) override {
		const Ref<LuauScript> script = object->get_script();
		const Dictionary metadata = script->get_enum_property(path);
		if (metadata.is_empty()) {
			return false;
		}
		auto property = memnew(LuauEnumProperty);
		property->setup(metadata);
		add_property_editor(path, property);
		return true;
	}
};
void ludot_initialize_enum_editor() {
	Ref<LuauEnumInspector> plugin;
	plugin.instantiate();
	EditorInspector::add_inspector_plugin(plugin);
}
#endif
