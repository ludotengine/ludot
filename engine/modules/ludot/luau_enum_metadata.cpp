#include "luau_enum_metadata.h"

#include "Luau/Parser.h"
#include "luau_script.h"

#include "core/core_constants.h"
#include "core/io/file_access.h"
#include "core/object/class_db.h"

namespace {
struct Declarations : Luau::AstVisitor {
	HashMap<String, Luau::AstStatLocal *> values;
	HashMap<String, Luau::AstType *> aliases;
	bool visit(Luau::AstStatLocal *node) override {
		for (auto local : node->vars) {
			values[String::utf8(local->name.value)] = node;
		}
		return false;
	}
	bool visit(Luau::AstStatTypeAlias *node) override {
		aliases[String::utf8(node->name.value)] = node->type;
		return false;
	}
	bool visit(Luau::AstStatClass *) override { return false; }
	bool visit(Luau::AstExprFunction *) override { return false; }
};
Dictionary resolve(Luau::AstStatBlock *root, const String &prefix, const String &name, const String &path, HashSet<String> &seen) {
	const String key = path + "#" + prefix + "." + name;
	if (seen.has(key)) {
		return {};
	}
	seen.insert(key);
	Declarations declarations;
	root->visit(&declarations);
	// Engine enums are annotated as Enum.<Name>. Scripts hold EnumItems; scenes store the values.
	if (prefix == "Enum") {
		const auto &catalog = ludot_engine_enum_catalog();
		for (size_t i = 0; i < catalog.enums.size(); ++i) {
			if (String::utf8(catalog.enums[i].name.c_str()) != name) {
				continue;
			}
			PackedStringArray labels;
			Array values;
			for (const auto &item : catalog.enums[i].items) {
				labels.push_back(String::utf8(item.name.c_str()));
				values.push_back(int64_t(item.value));
			}
			Dictionary result;
			result["names"] = labels;
			result["values"] = values;
			result["string"] = false;
			result["engine"] = int(i);
			return result;
		}
		return {};
	}
	if (prefix.is_empty()) {
		if (auto alias = declarations.aliases.getptr(name)) {
			auto reference = (*alias)->as<Luau::AstTypeReference>();
			if (reference) {
				return resolve(root, reference->prefix ? String::utf8(reference->prefix->value) : String(), String::utf8(reference->name.value), path, seen);
			}
		}
		if (auto declaration = declarations.values.getptr(name); declaration && (*declaration)->enumInfo) {
			PackedStringArray labels;
			Array values;
			bool strings = false;
			for (const auto &member : (*declaration)->enumInfo->members) {
				labels.push_back(String::utf8(member.name.value));
				if (auto value = member.value->as<Luau::AstExprConstantString>()) {
					strings = true;
					values.push_back(String::utf8(value->value.data, value->value.size));
				} else if (auto value = member.value->as<Luau::AstExprConstantNumber>()) {
					values.push_back(int64_t(value->value));
				}
			}
			Dictionary result;
			result["names"] = labels;
			result["values"] = values;
			result["string"] = strings;
			return result;
		}
	}
	const String binding = prefix.is_empty() ? name : prefix;
	auto declaration = declarations.values.getptr(binding);
	if (!declaration) {
		return {};
	}
	Luau::AstExpr *value = nullptr;
	for (size_t i = 0; i < (*declaration)->vars.size && i < (*declaration)->values.size; ++i) {
		if (String::utf8((*declaration)->vars.data[i]->name.value) == binding) {
			value = (*declaration)->values.data[i];
		}
	}
	if (!value) {
		return {};
	}
	if (prefix.is_empty()) {
		if (auto member = value->as<Luau::AstExprIndexName>()) {
			if (auto local = member->expr->as<Luau::AstExprLocal>()) {
				return resolve(root, String::utf8(local->local->name.value), String::utf8(member->index.value), path, seen);
			}
		}
	}
	auto call = value->as<Luau::AstExprCall>();
	if (!call || call->args.size != 1) {
		return {};
	}
	auto function = call->func->as<Luau::AstExprGlobal>();
	auto literal = call->args.data[0]->as<Luau::AstExprConstantString>();
	if (!function || function->name != "require" || !literal) {
		return {};
	}
	const String imported = LuauLanguage::get_singleton()->resolve_module(String::utf8(literal->value.data, literal->value.size), path);
	if (imported.is_empty()) {
		return {};
	}
	const CharString source = FileAccess::get_file_as_string(imported).utf8();
	Luau::Allocator allocator;
	Luau::AstNameTable names(allocator);
	const auto parsed = Luau::Parser::parse(source.get_data(), source.length(), names, allocator);
	if (!parsed.errors.empty()) {
		return {};
	}
	return resolve(parsed.root, String(), name, imported, seen);
}
} //namespace
Dictionary ludot_enum_metadata(Luau::AstStatBlock *root, Luau::AstType *type, const String &path) {
	while (type && type->is<Luau::AstTypeGroup>()) {
		type = type->as<Luau::AstTypeGroup>()->type;
	}
	auto reference = type ? type->as<Luau::AstTypeReference>() : nullptr;
	if (!reference || reference->parameters.size) {
		return {};
	}
	HashSet<String> seen;
	return resolve(root, reference->prefix ? String::utf8(reference->prefix->value) : String(), String::utf8(reference->name.value), path, seen);
}
