/**************************************************************************/
/*  engine_update_label.cpp                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "engine_update_label.h"

#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/object/callable_mp.h"
#include "core/os/os.h"
#include "core/version.h"
#include "editor/editor_string_names.h"
#include "editor/settings/editor_settings.h"
#include "scene/gui/dialogs.h"
#include "scene/main/http_request.h"
#include "scene/main/scene_tree.h"

namespace {
constexpr const char *LUDOT_RELEASES_API_URL = "https://api.github.com/repos/ludotengine/ludot/releases/latest";
constexpr const char *LUDOT_REPO_URL = "https://github.com/ludotengine/ludot";

struct LudotVersion {
	int major = 0;
	int minor = 0;
	int patch = 0;
	int ludot_rev = 0;
};

int parse_ludot_status_rev(const String &p_status) {
	if (p_status.begins_with("lu.")) {
		return p_status.trim_prefix("lu.").to_int();
	}
	return 0;
}

bool parse_ludot_tag(const String &p_tag, LudotVersion &r_version, String *r_display = nullptr) {
	String trimmed = p_tag.strip_edges();
	if (trimmed.begins_with("v") || trimmed.begins_with("V")) {
		trimmed = trimmed.substr(1);
	}
	if (trimmed.is_empty()) {
		return false;
	}

	String core = trimmed;
	String status;
	int dash = trimmed.find_char('-');
	if (dash >= 0) {
		core = trimmed.substr(0, dash);
		status = trimmed.substr(dash + 1);
	}

	PackedStringArray parts = core.split(".");
	if (parts.size() < 2) {
		return false;
	}

	r_version.major = parts[0].to_int();
	r_version.minor = parts[1].to_int();
	r_version.patch = parts.size() >= 3 ? parts[2].to_int() : 0;
	r_version.ludot_rev = parse_ludot_status_rev(status);

	if (r_display) {
		*r_display = trimmed;
	}
	return r_version.major > 0;
}

bool is_version_greater(const LudotVersion &p_a, const LudotVersion &p_b) {
	if (p_a.major != p_b.major) {
		return p_a.major > p_b.major;
	}
	if (p_a.minor != p_b.minor) {
		return p_a.minor > p_b.minor;
	}
	if (p_a.patch != p_b.patch) {
		return p_a.patch > p_b.patch;
	}
	return p_a.ludot_rev > p_b.ludot_rev;
}

PackedStringArray github_headers() {
	PackedStringArray headers;
	headers.push_back("Accept: application/vnd.github+json");
	headers.push_back("User-Agent: Ludot-Editor");
	return headers;
}
} // namespace

bool EngineUpdateLabel::is_newer_version(const String &p_remote_tag, int p_current_major, int p_current_minor, int p_current_patch, const String &p_current_status, UpdateMode p_mode, String *r_display_version) {
	LudotVersion remote;
	String display;
	if (!parse_ludot_tag(p_remote_tag, remote, &display)) {
		return false;
	}

	LudotVersion current;
	current.major = p_current_major;
	current.minor = p_current_minor;
	current.patch = p_current_patch;
	current.ludot_rev = parse_ludot_status_rev(p_current_status);

	if (p_mode == UpdateMode::NEWEST_PATCH && (remote.major != current.major || remote.minor != current.minor)) {
		return false;
	}

	if (!is_version_greater(remote, current)) {
		return false;
	}

	if (r_display_version) {
		*r_display_version = display;
	}
	return true;
}

bool EngineUpdateLabel::_can_check_updates() const {
	return int(EDITOR_GET("network/connection/network_mode")) == EditorSettings::NETWORK_ONLINE &&
			UpdateMode(int(EDITOR_GET("network/connection/check_for_updates"))) != UpdateMode::DISABLED;
}

void EngineUpdateLabel::_check_update() {
	checked_update = true;
	_set_status(UpdateStatus::BUSY);
	http->request(LUDOT_RELEASES_API_URL, github_headers());
}

void EngineUpdateLabel::_http_request_completed(int p_result, int p_response_code, const PackedStringArray &p_headers, const PackedByteArray &p_body) {
	if (p_result != OK) {
		_set_status(UpdateStatus::ERROR);
		_set_message(vformat(TTR("Failed to check for updates. Error: %d."), p_result), theme_cache.error_color);
		return;
	}

	if (p_response_code != 200) {
		_set_status(UpdateStatus::ERROR);
		_set_message(vformat(TTR("Failed to check for updates. Response code: %d."), p_response_code), theme_cache.error_color);
		return;
	}

	Dictionary release;
	{
		const uint8_t *r = p_body.ptr();
		String s = String::utf8((const char *)r, p_body.size());

		Variant result = JSON::parse_string(s);
		if (result == Variant()) {
			_set_status(UpdateStatus::ERROR);
			_set_message(TTR("Failed to parse release JSON."), theme_cache.error_color);
			return;
		}
		if (result.get_type() != Variant::DICTIONARY) {
			_set_status(UpdateStatus::ERROR);
			_set_message(TTR("Received JSON data is not a valid release object."), theme_cache.error_color);
			return;
		}
		release = result;
	}

	if (bool(release.get("draft", false)) || bool(release.get("prerelease", false))) {
		_set_status(UpdateStatus::UP_TO_DATE);
		return;
	}

	const String tag_name = release.get("tag_name", "");
	const UpdateMode update_mode = UpdateMode(int(EDITOR_GET("network/connection/check_for_updates")));

	available_newer_version = String();
	available_release_tag = String();
	available_release_url = String();

	String display_version;
	if (is_newer_version(tag_name, GODOT_VERSION_MAJOR, GODOT_VERSION_MINOR, GODOT_VERSION_PATCH, GODOT_VERSION_STATUS, update_mode, &display_version)) {
		available_newer_version = display_version;
		available_release_tag = tag_name;
		available_release_url = release.get("html_url", vformat("%s/releases/tag/%s", LUDOT_REPO_URL, tag_name));
		_set_status(UpdateStatus::UPDATE_AVAILABLE);
		_set_message(vformat(TTR("Update available: %s."), available_newer_version), theme_cache.update_color);
	} else {
		_set_status(UpdateStatus::UP_TO_DATE);
	}
}

void EngineUpdateLabel::_start_update() {
	if (available_release_tag.is_empty()) {
		return;
	}
	set_disabled(true);
	_set_message(vformat(TTR("Downloading Ludot %s installer..."), available_newer_version), theme_cache.default_color);
	set_tooltip_text("");

#ifdef WINDOWS_ENABLED
	const String script_name = "install.ps1";
#else
	const String script_name = "install.sh";
#endif
	const String url = vformat("%s/releases/download/%s/%s", LUDOT_REPO_URL, available_release_tag, script_name);
	installer_http->request(url, github_headers());
}

void EngineUpdateLabel::_installer_request_completed(int p_result, int p_response_code, const PackedStringArray &p_headers, const PackedByteArray &p_body) {
	if (p_result != OK || p_response_code != 200 || p_body.is_empty()) {
		_set_status(UpdateStatus::UPDATE_AVAILABLE);
		_set_message(vformat(TTR("Update available: %s (download failed, click to retry)."), available_newer_version), theme_cache.error_color);
		return;
	}

#ifdef WINDOWS_ENABLED
	const String script_path = OS::get_singleton()->get_cache_path().path_join("ludot_update.ps1");
#else
	const String script_path = OS::get_singleton()->get_cache_path().path_join("ludot_update.sh");
#endif
	{
		Ref<FileAccess> file = FileAccess::open(script_path, FileAccess::WRITE);
		if (file.is_null()) {
			_set_status(UpdateStatus::UPDATE_AVAILABLE);
			_set_message(vformat(TTR("Update available: %s (could not write installer)."), available_newer_version), theme_cache.error_color);
			return;
		}
		file->store_buffer(p_body.ptr(), p_body.size());
	}

	List<String> args;
	Error err = FAILED;
#ifdef WINDOWS_ENABLED
	args.push_back("-NoProfile");
	args.push_back("-ExecutionPolicy");
	args.push_back("Bypass");
	args.push_back("-Command");
	args.push_back(vformat("Start-Sleep -Seconds 1; & '%s'; Remove-Item -Force -ErrorAction SilentlyContinue '%s'", script_path.replace("'", "''"), script_path.replace("'", "''")));
	err = OS::get_singleton()->create_process("powershell.exe", args);
#else
	args.push_back("-c");
	args.push_back("sleep 1; sh \"$1\"; status=$?; rm -f \"$1\"; if [ $status -eq 0 ] && [ -x \"$HOME/.local/bin/ludot\" ]; then \"$HOME/.local/bin/ludot\" >/dev/null 2>&1 & fi");
	args.push_back("ludot-updater");
	args.push_back(script_path);
	err = OS::get_singleton()->create_process("/bin/sh", args);
#endif

	if (err != OK) {
		_set_status(UpdateStatus::UPDATE_AVAILABLE);
		_set_message(vformat(TTR("Update available: %s (failed to launch installer)."), available_newer_version), theme_cache.error_color);
		return;
	}

	if (SceneTree *tree = get_tree()) {
		tree->quit();
	}
}

void EngineUpdateLabel::_dialog_custom_action(const StringName &p_action) {
	if (p_action == SNAME("open_release") && !available_release_url.is_empty()) {
		update_dialog->hide();
		OS::get_singleton()->shell_open(available_release_url);
	}
}

void EngineUpdateLabel::_set_message(const String &p_message, const Color &p_color) {
	if (is_disabled()) {
		add_theme_color_override("font_disabled_color", p_color);
	} else {
		add_theme_color_override(SceneStringName(font_color), p_color);
	}
	set_text(p_message);
}

void EngineUpdateLabel::_set_status(UpdateStatus p_status) {
	status = p_status;
	if (status == UpdateStatus::BUSY || status == UpdateStatus::UP_TO_DATE) {
		// Hide the label to prevent unnecessary distraction.
		hide();
		return;
	} else {
		show();
	}

	switch (status) {
		case UpdateStatus::OFFLINE: {
			set_disabled(false);
			if (int(EDITOR_GET("network/connection/network_mode")) == EditorSettings::NETWORK_OFFLINE) {
				_set_message(TTR("Offline mode, update checks disabled."), theme_cache.disabled_color);
			} else {
				_set_message(TTR("Update checks disabled."), theme_cache.disabled_color);
			}
			set_accessibility_live(AccessibilityServerEnums::AccessibilityLiveMode::LIVE_OFF);
			set_tooltip_text("");
			break;
		}

		case UpdateStatus::ERROR: {
			set_disabled(false);
			set_accessibility_live(AccessibilityServerEnums::AccessibilityLiveMode::LIVE_POLITE);
			set_tooltip_text(TTR("An error has occurred. Click to try again."));
		} break;

		case UpdateStatus::UPDATE_AVAILABLE: {
			set_disabled(false);
			set_accessibility_live(AccessibilityServerEnums::AccessibilityLiveMode::LIVE_POLITE);
			set_tooltip_text(TTR("Click to update Ludot or open the release page."));
		} break;

		default: {
		}
	}
}

void EngineUpdateLabel::_notification(int p_what) {
	switch (p_what) {
		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {
			if (!EditorSettings::get_singleton()->check_changed_settings_in_group("network/connection")) {
				break;
			}

			if (_can_check_updates()) {
				_check_update();
			} else {
				_set_status(UpdateStatus::OFFLINE);
			}
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			theme_cache.default_color = get_theme_color(SceneStringName(font_color), "Button");
			theme_cache.disabled_color = get_theme_color("font_disabled_color", "Button");
			theme_cache.error_color = get_theme_color("error_color", EditorStringName(Editor));
			theme_cache.update_color = get_theme_color("warning_color", EditorStringName(Editor));
		} break;

		case NOTIFICATION_READY: {
			if (_can_check_updates()) {
				_check_update();
			} else {
				_set_status(UpdateStatus::OFFLINE);
			}
		} break;
	}
}

void EngineUpdateLabel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("offline_clicked"));
}

void EngineUpdateLabel::pressed() {
	switch (status) {
		case UpdateStatus::OFFLINE: {
			emit_signal("offline_clicked");
		} break;

		case UpdateStatus::ERROR: {
			_check_update();
		} break;

		case UpdateStatus::UPDATE_AVAILABLE: {
			if (!update_dialog) {
				update_dialog = memnew(ConfirmationDialog);
				update_dialog->set_title(TTR("Update Ludot"));
				update_dialog->set_ok_button_text(TTR("Update Now"));
				update_dialog->add_button(TTR("Open Release Page"), true, "open_release");
				update_dialog->connect(SceneStringName(confirmed), callable_mp(this, &EngineUpdateLabel::_start_update));
				update_dialog->connect("custom_action", callable_mp(this, &EngineUpdateLabel::_dialog_custom_action));
				add_child(update_dialog);
			}
			update_dialog->set_text(vformat(TTR("Ludot %s is available.\n\nUpdate Now will download the official Ludot installer, verify its SHA-256 checksum, and close the editor to install the new version."), available_newer_version));
			update_dialog->popup_centered();
		} break;

		default: {
		}
	}
}

EngineUpdateLabel::EngineUpdateLabel() {
	set_underline_mode(UNDERLINE_MODE_ON_HOVER);

	const String proxy_host = EDITOR_GET("network/http_proxy/host");
	const int proxy_port = EDITOR_GET("network/http_proxy/port");

	http = memnew(HTTPRequest);
	http->set_https_proxy(proxy_host, proxy_port);
	http->set_timeout(10.0);
	add_child(http);
	http->connect("request_completed", callable_mp(this, &EngineUpdateLabel::_http_request_completed));

	installer_http = memnew(HTTPRequest);
	installer_http->set_https_proxy(proxy_host, proxy_port);
	installer_http->set_timeout(30.0);
	add_child(installer_http);
	installer_http->connect("request_completed", callable_mp(this, &EngineUpdateLabel::_installer_request_completed));
}
