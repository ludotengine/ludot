#pragma once
#include "core/object/script_language.h"
#include "core/object/script_instance.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/mutex.h"
#include "lua.h"

class LuauLanguage;
class LuauScriptInstance;
class LuauScript : public Script {
    GDCLASS(LuauScript, Script);
    friend class LuauScriptInstance;
    friend class LuauLanguage;
    String source;
    StringName class_name;
    StringName base_type = "RefCounted";
    Ref<LuauScript> base_script;
    bool valid = false;
    int class_ref = LUA_NOREF;
    List<PropertyInfo> properties;
    HashMap<StringName, Variant> defaults;
    HashMap<StringName, String> property_types;
    HashMap<StringName, MethodInfo> methods;
    HashSet<LuauScriptInstance *> instances;
protected:
    static void _bind_methods();
public:
    bool can_instantiate() const override { return valid && class_ref != LUA_NOREF && !base_type.is_empty(); }
    int get_class_reference() const { return class_ref; }
    Ref<Script> get_base_script() const override { return base_script; }
    StringName get_global_name() const override { return class_name; }
    bool inherits_script(const Ref<Script> &p_script) const override { return base_script == p_script || (base_script.is_valid() && base_script->inherits_script(p_script)); }
    StringName get_instance_base_type() const override { return base_type; }
    ScriptInstance *instance_create(Object *p_this) override;
    PlaceHolderScriptInstance *placeholder_instance_create(Object *p_this) override;
    bool has_source_code() const override { return !source.is_empty(); }
    String get_source_code() const override { return source; }
    void set_source_code(const String &p_code) override { source = p_code; }
    Error reload(bool p_keep_state = false) override;
    Array get_diagnostics() const;
    PackedStringArray get_completions(const String &p_code) const;
#ifdef TOOLS_ENABLED
    StringName get_doc_class_name() const override { return class_name; }
    Vector<DocData::ClassDoc> get_documentation() const override { return {}; }
    String get_class_icon_path() const override { return ""; }
#endif
    bool has_method(const StringName &p_method) const override;
    MethodInfo get_method_info(const StringName &p_method) const override;
    bool is_tool() const override { return false; }
    bool is_valid() const override { return valid; }
    bool is_abstract() const override { return false; }
    ScriptLanguage *get_language() const override;
    bool has_script_signal(const StringName &p_signal) const override { return false; }
    void get_script_signal_list(List<MethodInfo> *r_signals) const override {}
    bool get_property_default_value(const StringName &p_property, Variant &r_value) const override;
    void get_script_method_list(List<MethodInfo> *r_methods) const override;
    void get_script_property_list(List<PropertyInfo> *r_properties) const override { *r_properties = properties; }
    const Variant get_rpc_config() const override { return Dictionary(); }
    LuauScript();
    ~LuauScript();
};

class LuauScriptInstance : public ScriptInstance {
    friend class LuauScript;
    friend class LuauLanguage;
    Object *owner;
    Ref<LuauScript> script;
    int object_ref = LUA_NOREF;
public:
    bool set(const StringName &p_name, const Variant &p_value) override;
    bool get(const StringName &p_name, Variant &r_value) const override;
    void get_property_list(List<PropertyInfo> *r_properties) const override { script->get_script_property_list(r_properties); }
    Variant::Type get_property_type(const StringName &p_name, bool *r_valid) const override;
    void validate_property(PropertyInfo &p_property) const override {}
    bool property_can_revert(const StringName &p_name) const override { Variant v; return script->get_property_default_value(p_name, v); }
    bool property_get_revert(const StringName &p_name, Variant &r_value) const override { return script->get_property_default_value(p_name, r_value); }
    Object *get_owner() override { return owner; }
    void get_method_list(List<MethodInfo> *r_methods) const override { script->get_script_method_list(r_methods); }
    bool has_method(const StringName &p_method) const override { return script->has_method(p_method); }
    Variant callp(const StringName &p_method, const Variant **p_args, int p_count, Callable::CallError &r_error) override;
    void notification(int p_notification, bool p_reversed) override;
    Ref<Script> get_script() const override { return script; }
    ScriptLanguage *get_language() override;
    LuauScriptInstance(Object *p_owner, const Ref<LuauScript> &p_script);
    ~LuauScriptInstance();
};

class LuauLanguage : public ScriptLanguage {
    static LuauLanguage *singleton;
public:
    lua_State *state = nullptr;
    lua_State *debug_state = nullptr;
    lua_State *active_state = nullptr;
    lua_State *stepping_state = nullptr;
    struct DebugCursor {
        String source;
        int line = -1;
        int depth = 0;
        int prototype = 0;
        const void *pc = nullptr;
    };
    HashMap<lua_State *, DebugCursor> debug_cursors;
    uint64_t generation = 0;
    Object *construction_owner = nullptr;
    Mutex mutex;
    HashSet<LuauScript *> scripts;
    HashMap<String, int> module_cache;
    // Last executed chunk function per module or script, so a reload can retarget live closures.
    HashMap<String, int> chunk_refs;
    void commit_chunk(lua_State *L, const String &p_key, int p_chunk);
    HashMap<String, Ref<LuauScript>> module_resources;
    HashSet<String> loading;
    HashMap<String, HashSet<String>> module_dependents;
    HashMap<String, HashSet<String>> module_dependencies;
    HashSet<String> invalidate_modules(const String &p_path);
    HashSet<StringName> native_classes;
    struct NativeMember {
        enum Kind { PROPERTY, SIGNAL, METHOD };
        Kind kind = PROPERTY;
        StringName name;
        Variant::Type type = Variant::NIL;
        int method_ref = LUA_NOREF;
    };
    // Static members of native classes and builtin types, keyed by class or type name and then Luau name.
    HashMap<StringName, HashMap<String, NativeMember>> native_members;
    String last_error;
    static LuauLanguage *get_singleton() { return singleton; }
    static String api_name(const String &p_name, bool p_property = false);
    static void push_variant(lua_State *L, const Variant &p_value);
    static Variant to_variant(lua_State *L, int p_index, int p_depth = 0);
    // Converts without raising a Luau error. On failure last_error holds the reason.
    static bool try_variant(lua_State *L, int p_index, Variant &r_value);
    struct Profile {
        uint64_t call_count = 0;
        uint64_t total_time = 0;
        uint64_t self_time = 0;
        uint64_t internal_time = 0;
    };
    bool profiling = false;
    bool profile_native_calls = false;
    HashMap<String, Profile> profile_accumulated;
    HashMap<String, Profile> profile_frame;
    HashMap<String, Profile> profile_last_frame;
    struct ProfileCall {
        String signature;
        // Luau frames record their thread and call depth. Engine boundary calls use nullptr.
        lua_State *state = nullptr;
        int depth = -1;
        bool native = false;
        uint64_t start = 0;
        // Time from earlier intervals, before a coroutine yielded.
        uint64_t active = 0;
        uint64_t children = 0;
        uint64_t native_time = 0;
    };
    // Innermost call last. Frames of suspended coroutines move to profile_suspended.
    LocalVector<ProfileCall> profile_calls;
    HashMap<lua_State *, LocalVector<ProfileCall>> profile_suspended;
    HashMap<const void *, String> profile_signatures;
    void profile_enter(const String &p_signature, bool p_native = false, lua_State *p_state = nullptr, int p_depth = -1);
    // Closes the innermost boundary call, and any Luau frames an error unwound past it.
    void profile_exit();
    void profile_close();
    // Closes frames of p_state at or below p_depth that ended without a RETURN, such as after an error.
    void profile_unwind(lua_State *p_state, int p_depth);
    Error ensure_native_class(const StringName &p_name);
    Error load_chunk(const String &p_source, const String &p_path, lua_State *p_state = nullptr);
    Dictionary module_aliases(const String &p_from) const;
    String resolve_module(const String &p_requested, const String &p_from) const;
    String get_name() const override { return "Luau"; }
    String get_type() const override { return "LuauScript"; }
    String get_extension() const override { return "luau"; }
    bool handles_global_class_type(const String &p_type) const override { return p_type == "LuauScript"; }
    String get_global_class_name(const String &p_path, String *r_base_type = nullptr, String *r_icon_path = nullptr, bool *r_is_abstract = nullptr, bool *r_is_tool = nullptr) const override;
    void init() override;
    void finish() override;
    Vector<String> get_reserved_words() const override;
    bool is_control_flow_keyword(const String &p_word) const override { return p_word == "if" || p_word == "then" || p_word == "else" || p_word == "elseif" || p_word == "for" || p_word == "while" || p_word == "repeat" || p_word == "until" || p_word == "return" || p_word == "break" || p_word == "continue"; }
    Vector<String> get_comment_delimiters() const override { return {"--", "--[[ ]]"}; }
    Vector<String> get_doc_comment_delimiters() const override { return {"---"}; }
    Vector<String> get_string_delimiters() const override { return {"\" \"", "' '", "[[ ]]", "` `"}; }
    bool validate(const String &p_script, const String &p_path, List<String> *r_functions, List<ScriptError> *r_errors, List<Warning> *r_warnings, HashSet<int> *r_safe_lines) const override;
    Error complete_code(const String &p_code, const String &p_path, Object *p_owner, List<CodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) override;
    bool supports_builtin_mode() const override { return false; }
    ScriptNameCasing preferred_file_name_casing() const override { return SCRIPT_NAME_CASING_PASCAL_CASE; }
    Ref<Script> make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const override;
    int find_function(const String &p_function, const String &p_code) const override;
    String make_function(const String &p_class, const String &p_name, const PackedStringArray &p_args) const override;
    void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const override {}
    void add_global_constant(const StringName &p_variable, const Variant &p_value) override;
    void reload_all_scripts() override;
    void reload_scripts(const Array &p_scripts, bool p_soft_reload) override;
    void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) override { p_script->reload(p_soft_reload); }
    void get_recognized_extensions(List<String> *r_extensions) const override { r_extensions->push_back("luau"); }
    void get_public_functions(List<MethodInfo> *r_functions) const override {}
    void get_public_constants(List<Pair<String, Variant>> *r_constants) const override {}
    void get_public_annotations(List<MethodInfo> *r_annotations) const override {}
    String debug_get_error() const override { return last_error; }
    int debug_get_stack_level_count() const override;
    int debug_get_stack_level_line(int p_level) const override;
    String debug_get_stack_level_function(int p_level) const override;
    String debug_get_stack_level_source(int p_level) const override;
    void debug_get_stack_level_locals(int p_level, List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) override;
    void debug_get_stack_level_members(int p_level, List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) override;
    ScriptInstance *debug_get_stack_level_instance(int p_level) override;
    bool debug_evaluate_expression(int p_level, const String &p_expression, Variant &r_value, String &r_error) override;
    void debug_get_globals(List<String> *r_names, List<Variant> *r_values, int p_max_subitems, int p_max_depth) override;
    String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems, int p_max_depth) override;
    void profiling_start() override;
    void profiling_stop() override;
    void profiling_set_save_native_calls(bool p_enable) override;
    int profiling_get_accumulated_data(ProfilingInfo *p_info, int p_max) override;
    int profiling_get_frame_data(ProfilingInfo *p_info, int p_max) override;
    void frame() override;
    LuauLanguage();
    ~LuauLanguage() override { singleton = nullptr; }
};
class ResourceFormatLoaderLuau : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderLuau, ResourceFormatLoader);
public:
    Ref<Resource> load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) override;
    void get_recognized_extensions(List<String> *r_extensions) const override { r_extensions->push_back("luau"); }
    bool handles_type(const String &p_type) const override { return p_type == "Script" || p_type == "LuauScript"; }
    String get_resource_type(const String &p_path) const override { return p_path.get_extension() == "luau" ? "LuauScript" : ""; }
};
class ResourceFormatSaverLuau : public ResourceFormatSaver {
    GDCLASS(ResourceFormatSaverLuau, ResourceFormatSaver);
public:
    Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
    bool recognize(const Ref<Resource> &p_resource) const override { return Object::cast_to<LuauScript>(p_resource.ptr()) != nullptr; }
    void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *r_extensions) const override { if (recognize(p_resource)) r_extensions->push_back("luau"); }
};
