// Ludot documentation language service. MIT license; see repository LICENSE.
#include "Luau/AstQuery.h"
#include "Luau/Autocomplete.h"
#include "Luau/BuiltinDefinitions.h"
#include "Luau/Common.h"
#include "Luau/Frontend.h"
#include "Luau/JsonEmitter.h"
#include "Luau/ToString.h"
#include "LudotApi.h"

#include <cstring>
#include <memory>

namespace {
struct BrowserFiles : Luau::FileResolver {
	std::string source;
	std::optional<Luau::SourceCode> readSource(const Luau::ModuleName& name) override {
		if (name != "main") return std::nullopt;
		return Luau::SourceCode{source, Luau::SourceCode::Module};
	}
	std::optional<Luau::ModuleInfo> resolveModule(const Luau::ModuleInfo*, Luau::AstExpr*, const Luau::TypeCheckLimits&) override {
		return std::nullopt;
	}
	std::string getHumanReadableModuleName(const Luau::ModuleName& name) const override { return name; }
};
struct BrowserConfig : Luau::ConfigResolver {
	Luau::Config config;
	BrowserConfig() { config.mode = Luau::Mode::Strict; }
	const Luau::Config& getConfig(const Luau::ModuleName&, const Luau::TypeCheckLimits&) const override { return config; }
};
struct BrowserSession {
	BrowserFiles files;
	BrowserConfig config;
	Luau::Frontend frontend;
	std::string api;
	std::string checkedSource;
	bool checked = false;

	static Luau::FrontendOptions options() {
		Luau::FrontendOptions options;
		options.retainFullTypeGraphs = true;
		options.moduleTimeLimitSec = 2.0;
		return options;
	}
	BrowserSession(const std::string& api) : frontend(Luau::SolverMode::New, &files, &config, options()), api(api) {
		Luau::unfreeze(frontend.globals.globalTypes);
		Luau::registerBuiltinGlobals(frontend, frontend.globals);
		if (!api.empty() && !loadLudotApi(frontend, api)) throw std::runtime_error("Cannot load the engine API");
		Luau::freeze(frontend.globals.globalTypes);
	}
};
}

extern "C" const char* queryScript(const char* source, int line, int column, const char* api) {
	static std::string result;
	static std::unique_ptr<BrowserSession> session;
	Luau::Json::JsonEmitter json;
	try {
		for (Luau::FValue<bool>* flag = Luau::FValue<bool>::list; flag; flag = flag->next)
			if (std::strncmp(flag->name, "Luau", 4) == 0) flag->value = true;
		if (!session || session->api != api) session = std::make_unique<BrowserSession>(api);
		auto& state = *session;
		if (!state.checked || state.checkedSource != source) {
			state.files.source = source;
			state.frontend.markDirty("main");
			state.frontend.check("main");
			state.checkedSource = source;
			state.checked = true;
		}
		json.writeRaw("{\"diagnostics\":[");
		bool first = true;
		if (auto checked = state.frontend.getCheckResult("main", false)) {
			for (const auto& error : checked->errors) {
				if (!first) json.writeRaw(',');
				first = false;
				auto item = json.writeObject();
				item.writePair("line", error.location.begin.line);
				item.writePair("column", error.location.begin.column);
				item.writePair("endLine", error.location.end.line);
				item.writePair("endColumn", error.location.end.column);
				item.writePair("message", Luau::toString(error, Luau::TypeErrorToStringOptions{&state.files}));
			}
		}
		json.writeRaw("],\"completions\":[");
		first = true;
		const Luau::Position position{unsigned(line), unsigned(column)};
		auto completions = Luau::autocomplete(state.frontend, "main", position, {});
		for (const auto& [name, entry] : completions.entryMap) {
			if (!first) json.writeRaw(',');
			first = false;
			auto item = json.writeObject();
			item.writePair("label", name);
			item.writePair("type", entry.kind == Luau::AutocompleteEntryKind::Type ? "type" : entry.kind == Luau::AutocompleteEntryKind::Keyword ? "keyword" : "variable");
			if (entry.type) item.writePair("detail", Luau::toString(*entry.type));
		}
		json.writeRaw("],\"hover\":");
		std::string hover;
		auto module = state.frontend.moduleResolver.getModule("main");
		auto parsed = state.frontend.getSourceModule("main");
		if (module && parsed)
			if (auto type = Luau::findTypeAtPosition(*module, *parsed, position)) hover = Luau::toString(*type);
		Luau::Json::write(json, hover);
		json.writeRaw('}');
	} catch (const std::exception& error) {
		json = Luau::Json::JsonEmitter();
		auto item = json.writeObject();
		item.writePair("error", std::string(error.what()));
	}
	result = json.str();
	return result.c_str();
}
