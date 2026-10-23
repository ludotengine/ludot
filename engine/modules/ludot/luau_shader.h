#pragma once
#include "scene/resources/shader.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/templates/hash_set.h"

class LuauShader : public Shader {
    GDCLASS(LuauShader, Shader);
    String source;
    String diagnostics;
    // Libraries ("shader include") produce no shader code of their own.
    bool library = false;
    // Kept loaded so editor reloads reach them, which then recompile this shader.
    Vector<Ref<LuauShader>> includes;
    // The resource path is assigned after loading, so the loader provides it for import resolution.
    String load_path;
    static HashSet<LuauShader *> live_shaders;
protected:
    static void _bind_methods();
public:
    Error compile_source(const String &p_source);
    void set_load_path(const String &p_path) { load_path = p_path; }
    bool is_library() const { return library; }
    LuauShader() { live_shaders.insert(this); }
    ~LuauShader() override { live_shaders.erase(this); }
    void set_source(const String &p_source) { compile_source(p_source); }
    String get_source() const { return source; }
    String get_diagnostics() const { return diagnostics; }
    bool is_text_shader() const override { return false; }
    void reload_from_file() override;
};
class ResourceFormatLoaderLuauShader : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderLuauShader, ResourceFormatLoader);
public:
    Ref<Resource> load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) override;
    void get_recognized_extensions(List<String> *r_extensions) const override { r_extensions->push_back("lushader"); }
    bool handles_type(const String &p_type) const override { return p_type == "Shader" || p_type == "LuauShader"; }
    String get_resource_type(const String &p_path) const override { return p_path.get_extension() == "lushader" ? "LuauShader" : ""; }
};
class ResourceFormatSaverLuauShader : public ResourceFormatSaver {
    GDCLASS(ResourceFormatSaverLuauShader, ResourceFormatSaver);
public:
    Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
    bool recognize(const Ref<Resource> &p_resource) const override { return Object::cast_to<LuauShader>(p_resource.ptr()) != nullptr; }
    void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *r_extensions) const override { if (recognize(p_resource)) r_extensions->push_back("lushader"); }
};
