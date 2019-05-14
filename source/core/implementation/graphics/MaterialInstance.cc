#include "../../include/graphics/lib.hh"



namespace mod {
  void MaterialInstance::apply_settings (Material const& mat) const {
    if (face_culling.valid) face_culling->apply(mat);
    else mat.face_culling.apply(mat);

    if (alpha_blending.valid) alpha_blending->apply(mat);
    else mat.alpha_blending.apply(mat);

    if (depth.valid) depth->apply(mat);
    else mat.depth.apply(mat);

    bool ew = enable_wireframe.valid? *enable_wireframe : mat.enable_wireframe;
    
    if (ew) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  void MaterialInstance::apply_uniforms (Material const& mat, ShaderProgram const& program_ref) const {
    for (auto [ i, uniform ] : mat.uniforms) {
      if (get_uniform_index_local(uniform.location) == -1) uniform.apply(program_ref);
    }

    for (auto [ i, uniform_override ] : uniform_overrides) uniform_override.apply(program_ref);
  }
  
  void MaterialInstance::apply_textures (Material const& mat) const {
    for (auto [ i, texture ] : mat.textures) {
      if (get_texture_index_local(texture.location) == -1) texture.apply();
    }

    for (auto [ i, texture_override ] : texture_overrides) texture_override.apply();
  }

  void MaterialInstance::use () const {
    Material const& mat = *base;
    ShaderProgram const& program_ref = *mat.shader_program;

    program_ref.use();

    apply_settings(mat);
    apply_uniforms(mat, program_ref);
    apply_textures(mat);
  }



  s64_t MaterialInstance::get_uniform_index_local (s32_t location) const {
    for (auto [ i, uniform_override ] : uniform_overrides) {
      if (uniform_override.location == location) return i;
    }

    return -1;
  }

  s64_t MaterialInstance::get_uniform_index_local (char const* name) const {
    ShaderProgram const& program_ref = *base->shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_index_local(location);
    else return -1;
  }

  s64_t MaterialInstance::get_uniform_index_base (s32_t location) const {
    for (auto [ i, uniform ] : base->uniforms) {
      if (uniform.location == location) return i;
    }

    return -1;
  }

  s64_t MaterialInstance::get_uniform_index_base (char const* name) const {
    ShaderProgram const& program_ref = *base->shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_index_base(location);
    else return -1;
  }

  pair_t<bool, s64_t> MaterialInstance::get_uniform_index (s32_t location) const {
    s64_t local_index = get_uniform_index_local(location);
    
    if (local_index != -1) return { true, local_index };
    else return { false, get_uniform_index_base(location) };
  }

  pair_t<bool, s64_t> MaterialInstance::get_uniform_index (char const* name) const {
    ShaderProgram const& program_ref = *base->shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_index(location);
    else return { false, -1 };
  }


  Uniform* MaterialInstance::get_uniform_pointer (s32_t location) {
    for (auto [ i, uniform_override ] : uniform_overrides) {
      if (uniform_override.location == location) return &uniform_override;
    }

    for (auto [ i, uniform ] : base->uniforms) {
      if (uniform.location == location) {
        uniform_overrides.append(Uniform { uniform });
        return &uniform_overrides.last();
      }
    }

    return NULL;
  }

  Uniform* MaterialInstance::get_uniform_pointer (char const* name) {
    ShaderProgram const& program_ref = *base->shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_pointer(location);
    else return NULL;
  }

  Uniform& MaterialInstance::get_uniform (s32_t location) {
    Uniform* ptr = get_uniform_pointer(location);

    m_assert(ptr != NULL, "There is no Uniform bound to location %" PRId32, location);

    return *ptr;
  }

  Uniform& MaterialInstance::get_uniform (char const* name) {
    Uniform* ptr = get_uniform_pointer(name);

    m_assert(ptr != NULL, "There is no Uniform bound to name %s" PRId32, name);

    return *ptr;
  }

  void MaterialInstance::unset_uniform (s32_t location) {
    s64_t index = get_uniform_index_local(location);

    if (index != -1) {
      uniform_overrides[index].destroy();
      uniform_overrides.remove(index);
    }
  }

  

  s64_t MaterialInstance::get_texture_index_local (s32_t location) const {
    for (auto [ i, texture_override ] : texture_overrides) {
      if (texture_override.location == location) return i;
    }

    return -1;
  }

  s64_t MaterialInstance::get_texture_index_base (s32_t location) const {
    for (auto [ i, texture ] : base->textures) {
      if (texture.location == location) return i;
    }

    return -1;
  }

  pair_t<bool, s64_t> MaterialInstance::get_texture_index (s32_t location) const {
    s64_t local_index = get_texture_index_local(location);

    if (local_index != -1) return { true, local_index };
    else return { false, get_texture_index_base(location) };
  }


  TextureUnit* MaterialInstance::get_texture_pointer (s32_t location) {
    for (auto [ i, texture_override ] : texture_overrides) {
      if (texture_override.location == location) return &texture_override;
    }

    for (auto [ i, texture ] : base->textures) {
      if (texture.location == location) {
        texture_overrides.append(texture);
        return &texture_overrides.last();
      }
    }

    return NULL;
  }

  TextureUnit& MaterialInstance::get_texture (s32_t location) {
    TextureUnit* ptr = get_texture_pointer(location);

    m_assert(ptr != NULL, "There is no Uniform bound to location %" PRId32, location);

    return *ptr;
  }

  void MaterialInstance::set_texture (s32_t location, TextureHandle const& value) {
    TextureUnit* existing_texture_unit = get_texture_pointer(location);

    if (existing_texture_unit) {
      existing_texture_unit->texture = value;
    } else {
      texture_overrides.append({ location, value });
    }
  }

  void MaterialInstance::unset_texture (s32_t location) {
    s64_t index = get_texture_index_local(location);

    if (index != -1) texture_overrides.remove(index);
  }
}