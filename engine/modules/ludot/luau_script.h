#pragma once
#include "core/object/script_language.h"
#include "core/object/script_instance.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/object/ref_counted.h"
#include "core/os/mutex.h"
#include "lua.h"
#include "Luau/LudotNaming.h"
#include "Luau/LudotEngineEnums.h"

// Registers the task library. Defined in luau_task.cpp.
void ludot_open_task(lua_State *L);
// The Signal library and its Signal and Connection objects. Defined in luau_signal.cpp.
void ludot_open_signal(lua_State *L);
// Pushes the Signal object for an engine signal, such as Node.tree_exiting.
void ludot_push_engine_signal(lua_State *L, Object *p_object, const StringName &p_name);
// The engine Signal value of a Signal object, or false for a Luau-only signal.
bool ludot_signal_variant(lua_State *L, int p_index, Variant &r_value);
// Binds each Signal field of a script object to the engine signal of the same name, so
// Luau, GDScript, and the editor share its handlers. Connections made earlier move over.
void ludot_bind_signal_fields(lua_State *L, int p_object, Object *p_owner, const HashMap<StringName, MethodInfo> &p_signals);
// A Luau value carried through an engine signal, so tables and plain class objects keep
// their identity between Luau handlers. Defined in luau_signal.cpp.
class LuauValue : public RefCounted {
    GDCLASS(LuauValue, RefCounted);
    int reference = LUA_NOREF;
    uint64_t generation = 0;
protected:
    static void _bind_methods();
public:
    void store(lua_State *L, int p_index);
    // Pushes the value, or nil after the VM restarted.
    void push(lua_State *L) const;
    Variant to_variant() const;
    ~LuauValue();
};
// Every exposed, non-editor engine enum, named for the Enum global. Built on first use.
const Luau::EngineEnumCatalog &ludot_engine_enum_catalog();

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
    // This class declares onready properties, so its own __onready runs before _ready.
    bool has_onready = false;
    List<PropertyInfo> properties;
    HashMap<StringName, Variant> defaults;
    HashMap<StringName, String> property_types;
	HashMap<StringName, Dictionary> property_enums;
	HashMap<StringName, MethodInfo> methods;
    HashSet<LuauScriptInstance *> instances;
    // Signal-typed fields, which the engine lists and emits as script signals.
    HashMap<StringName, MethodInfo> signals;
protected:
    static void _bind_methods();
public:
    bool can_instantiate() const override;
	Variant new_instance(const Variant **p_args, int p_count, Callable::CallError &r_error);
	const HashMap<StringName, MethodInfo> &get_signals() const { return signals; }
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
	Dictionary get_enum_property(const StringName &name) const { return property_enums.has(name) ? property_enums[name] : Dictionary(); }
	StringName resolve_method_name(const StringName &p_method) const;
    Array get_lint_warnings(const String &p_code) const;
    PackedStringArray get_completions(const String &p_code) const;
    Dictionary get_symbol_help(const String &p_code, const String &p_symbol, Object *p_owner = nullptr) const;
    // Tooling queries for editor integrations such as the MCP addon. Editor builds only.
    // The engine API as Luau spells it, with Godot's documentation.
    static Dictionary get_api_docs(const String &p_symbol);
    // Checker and syntax errors, and lint and Selene warnings with suppressions applied,
    // for source that belongs at p_path. Nothing runs.
    static Array check_code(const String &p_code, const String &p_path);
    static Array lint_code(const String &p_code, const String &p_path);
    // Hover help for the identifier at a 1-based line and column, and completions there.
    static Dictionary inspect_code(const String &p_code, const String &p_path, int p_line, int p_column);
    // Engine enums whose name matches p_query; an exact name lists its items.
    static Array find_enums(const String &p_query);
    // The file an import path resolves to from p_from, or "" when it does not resolve.
    static String resolve_module_path(const String &p_requested, const String &p_from);
    // StyLua with the user's settings and the Ludot syntax protections.
    static Dictionary format_code(const String &p_code);
    // The built-in templates for p_base_class and its ancestors, and their expansion.
    static Array get_templates(const String &p_base_class);
    static String create_from_template(const String &p_template, const String &p_class_name, const String &p_base_class);
#ifdef TOOLS_ENABLED
    StringName get_doc_class_name() const override { return class_name; }
	Vector<DocData::ClassDoc> get_documentation() const override;
	String get_class_icon_path() const override { return ""; }
#endif
    bool has_method(const StringName &p_method) const override;
    MethodInfo get_method_info(const StringName &p_method) const override;
	MethodInfo get_constructor_info() const override { return get_method_info("__init"); }
	bool is_tool() const override { return false; }
	bool is_valid() const override { return valid; }
    bool is_abstract() const override { return false; }
    ScriptLanguage *get_language() const override;
    bool has_script_signal(const StringName &p_signal) const override { return signals.has(p_signal); }
    void get_script_signal_list(List<MethodInfo> *r_signals) const override {
        for (const KeyValue<StringName, MethodInfo> &signal : signals) {
            r_signals->push_back(signal.value);
        }
    }
    bool get_property_default_value(const StringName &p_property, Variant &r_value) const override;
    void get_script_method_list(List<MethodInfo> *r_methods) const override;
	void get_script_property_list(List<PropertyInfo> *r_properties) const override {
		for (const PropertyInfo &property : properties) {
			r_properties->push_back(property);
		}
	}
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
	LuauScriptInstance(Object *p_owner, const Ref<LuauScript> &p_script, const Variant **p_args = nullptr, int p_count = 0);
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
		enum Kind { PROPERTY,
			SIGNAL,
			METHOD,
			CONSTANT };
		Kind kind = PROPERTY;
        StringName name;
        Variant::Type type = Variant::NIL;
        int method_ref = LUA_NOREF;
        // Catalog index when the property or constant is a non-bitfield engine enum, else -1.
        int enumeration = -1;
    };
    // Static members of native classes and builtin types, keyed by class or type name and then Luau name.
    HashMap<StringName, HashMap<String, NativeMember>> native_members;
    HashMap<StringName, HashSet<StringName>> native_callbacks;
    String last_error;
    static LuauLanguage *get_singleton() { return singleton; }
    Luau::ApiCase method_casing = Luau::ApiCase::Camel;
    Luau::ApiCase property_casing = Luau::ApiCase::Pascal;
    static String callback_name(const StringName &p_base, const StringName &p_name);
    static String api_name(const String &p_name, bool p_property = false, bool p_constant = false);
    static void push_variant(lua_State *L, const Variant &p_value);
    // Engine enum values. Each item is one shared userdata, so identity equality holds.
    int enum_items_ref = LUA_NOREF;
    int enum_names_ref = LUA_NOREF;
    int enum_by_name_ref = LUA_NOREF;
    int enum_spaces_ref = LUA_NOREF;
    HashMap<StringName, int> enum_by_source;
    void ensure_enums(lua_State *L);
    // Catalog index for a PropertyInfo of a non-bitfield enum type, else -1.
    int enum_index(const PropertyInfo &p_info);
    // Pushes the EnumItem for p_value, or the plain value when no item matches.
    void push_enum(lua_State *L, int p_enumeration, const Variant &p_value);
    void push_typed(lua_State *L, const Variant &p_value, const PropertyInfo &p_info);
    // Many engine properties declare a plain int; their getter carries the enum type.
    static PropertyInfo enum_property(const StringName &p_class, const PropertyInfo &p_property);
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
    // Task scheduler. One per VM, stepped from SceneTree::process_frame. Engine calls
    // into Luau start in resumable threads, so task.wait can yield from callbacks.
    struct Task {
        // SIGNAL waits for a Signal to fire. It never resumes on a timer.
        enum Kind { DEFER, WAIT, DELAY, SIGNAL };
        int ref = LUA_NOREF;
        Kind kind = DEFER;
        ObjectID owner;
        double duration = 0.0;
        double elapsed = 0.0;
        // Values already on the thread stack for its next resume. Waits push the elapsed time instead.
        int arguments = 0;
        uint64_t order = 0;
    };
    HashMap<lua_State *, Task> tasks;
    uint64_t task_order = 0;
    int call_depth = 0;
    bool running_deferred = false;
    LocalVector<ObjectID> task_owners;
    LocalVector<int> idle_threads;
    HashSet<lua_State *> running_threads;
    ObjectID task_tree;
    ObjectID current_task_owner() const { return task_owners.is_empty() ? ObjectID() : task_owners[task_owners.size() - 1]; }
    // Calls the function below p_count arguments on the main state in a resumable thread and
    // pushes its first result, or nil when it yielded or failed. Returns false after an error,
    // which is reported with a stack trace prefixed by p_where.
    bool invoke(int p_count, ObjectID p_owner, const String &p_where = String());
    int resume_thread(lua_State *p_thread, lua_State *p_from, int p_arguments, ObjectID p_owner, const String &p_where = String());
    void report_thread_error(lua_State *p_thread, const String &p_where);
    void schedule_task(lua_State *p_thread, Task::Kind p_kind, double p_duration, int p_arguments);
    // Removes a pending schedule and its queued arguments, keeping the thread itself.
    void unschedule_task(lua_State *p_thread);
    void drop_task(lua_State *p_thread);
    void drop_owner_tasks(ObjectID p_owner);
    void resume_task(lua_State *p_thread);
    void run_deferred();
    void step_tasks();
    // Lists scheduled threads with where each one is suspended, for the debugger globals.
    void debug_get_tasks(List<String> *r_names, List<Variant> *r_values);
    Error ensure_native_class(const StringName &p_name);
    Error load_chunk(const String &p_source, const String &p_path, lua_State *p_state = nullptr);
    Dictionary module_aliases(const String &p_from) const;
    String resolve_module(const String &p_requested, const String &p_from, const String &p_extension = "luau") const;
    String get_name() const override { return "Luau"; }
    String get_type() const override { return "LuauScript"; }
    String get_extension() const override { return "luau"; }
    bool handles_global_class_type(const String &p_type) const override { return p_type == "LuauScript"; }
    String get_global_class_name(const String &p_path, String *r_base_type = nullptr, String *r_icon_path = nullptr, bool *r_is_abstract = nullptr, bool *r_is_tool = nullptr) const override;
    void init() override;
    void finish() override;
    Vector<String> get_reserved_words() const override;
	bool is_control_flow_keyword(const String &p_word) const override { return p_word == "if" || p_word == "then" || p_word == "else" || p_word == "elseif" || p_word == "for" || p_word == "while" || p_word == "repeat" || p_word == "until" || p_word == "return" || p_word == "break" || p_word == "continue" || p_word == "match" || p_word == "with" || p_word == "case" || p_word == "otherwise"; }
	Vector<String> get_comment_delimiters() const override { return {"--", "--[[ ]]"}; }
    Vector<String> get_doc_comment_delimiters() const override { return {"---"}; }
    Vector<String> get_string_delimiters() const override { return {"\" \"", "' '", "[[ ]]", "` `"}; }
    bool validate(const String &p_script, const String &p_path, List<String> *r_functions, List<ScriptError> *r_errors, List<Warning> *r_warnings, HashSet<int> *r_safe_lines) const override;
    Error complete_code(const String &p_code, const String &p_path, Object *p_owner, List<CodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) override;
    bool supports_builtin_mode() const override { return false; }
	bool supports_documentation() const override { return true; }
    ScriptNameCasing preferred_file_name_casing() const override { return SCRIPT_NAME_CASING_PASCAL_CASE; }
    bool is_using_templates() override { return true; }
    Vector<ScriptTemplate> get_built_in_templates(const StringName &p_object) override;
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
