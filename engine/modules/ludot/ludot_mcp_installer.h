#pragma once
#ifdef TOOLS_ENABLED
#include "core/object/object.h"
#include "scene/gui/dialogs.h"

class Button;
class CheckBox;
class Label;
class LineEdit;
class VBoxContainer;

class LudotMcpInstallerDialog : public AcceptDialog {
	GDCLASS(LudotMcpInstallerDialog, AcceptDialog);

	struct ProviderRow {
		String id;
		String name;
		String config_path;
		bool already_configured = false;
		CheckBox *checkbox = nullptr;
	};

	CheckBox *addon_checkbox = nullptr;
	Label *addon_status_label = nullptr;
	VBoxContainer *providers_list = nullptr;
	LineEdit *command_edit = nullptr;
	Label *status_label = nullptr;
	Vector<ProviderRow> provider_rows;

	void _select_all();
	void _select_none();
	void _on_install_pressed();

protected:
	void _notification(int p_what);

public:
	void refresh();
	LudotMcpInstallerDialog();
};

class LudotMcpInstaller : public Object {
	GDCLASS(LudotMcpInstaller, Object);

protected:
	static void _bind_methods();

public:
	static Dictionary get_addon_status();
	static Dictionary install_addon();
	static Dictionary resolve_server_command();
	static Array detect_providers();
	static Dictionary configure_provider(const String &p_provider_id, const String &p_command, const PackedStringArray &p_args);
	static void open_dialog();
};
#endif
