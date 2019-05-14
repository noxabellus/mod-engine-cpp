#ifndef MATERIAL_INSTANCE_H
#define MATERIAL_INSTANCE_H

#include "Material.hh"
#include "../Optional.hh"


namespace mod {
  struct MaterialInstance {
    MaterialHandle base;

    Optional<FaceCullingSetting> face_culling;
    Optional<AlphaBlendingSetting> alpha_blending;
    Optional<DepthSetting> depth;

    Optional<bool> enable_skinning;
    Optional<bool> enable_wireframe;

    Array<Uniform> uniform_overrides;
    Array<TextureUnit> texture_overrides;


    /* Create a new uninitialized MaterialInstance */
    MaterialInstance () { }

    /* Create a new MaterialInstance referencing a Material, optionally initializing its render settings */
    MaterialInstance (
      MaterialHandle in_base,
      Optional<FaceCullingSetting> in_face_culling = { },
      Optional<AlphaBlendingSetting> in_alpha_blending = { },
      Optional<DepthSetting> in_depth = { },
      Optional<bool> in_enable_skinning = { },
      Optional<bool> in_enable_wireframe = { }
    )
    : base(in_base)
    , face_culling(in_face_culling)
    , alpha_blending(in_alpha_blending)
    , depth(in_depth)
    , enable_skinning(in_enable_skinning)
    , enable_wireframe(in_enable_wireframe)
    { }

    /* Create a new MaterialInstance and initialize its uniform and texture override data by copying from existing arrays, optionally initializing its render settings */
    MaterialInstance (
      MaterialHandle in_base,
      Array<Uniform> const& in_uniform_overrides,
      Array<TextureUnit> const& in_texture_overrides,
      Optional<FaceCullingSetting> in_face_culling = { },
      Optional<AlphaBlendingSetting> in_alpha_blending = { },
      Optional<DepthSetting> in_depth = { },
      Optional<bool> in_enable_skinning = { },
      Optional<bool> in_enable_wireframe = { }
    )
    : MaterialInstance(in_base, in_face_culling, in_alpha_blending, in_depth, in_enable_skinning, in_enable_wireframe)
    { 
      uniform_overrides.copy(in_uniform_overrides);
      texture_overrides.copy(in_texture_overrides);
    }

    /* Create a new MaterialInstance and initialize its uniform and texture override data by taking ownership of existing arrays, optionally initializing its render settings */
    static MaterialInstance from_ex (
      MaterialHandle base,
      Array<Uniform> const& uniform_overrides,
      Array<TextureUnit> const& texture_overrides,
      Optional<FaceCullingSetting> face_culling = { },
      Optional<AlphaBlendingSetting> alpha_blending = { },
      Optional<DepthSetting> depth = { },
      Optional<bool> enable_skinning = { },
      Optional<bool> enable_wireframe = { }
    ) {
      MaterialInstance instance { base, face_culling, alpha_blending, depth, enable_skinning, enable_wireframe };

      instance.uniform_overrides = uniform_overrides;
      instance.texture_overrides = texture_overrides;

      return instance;
    }


    /* Clean up a MaterialInstance's heap allocations */
    void destroy () {
      for (auto [ i, uniform_override ] : uniform_overrides) uniform_override.destroy();
      uniform_overrides.destroy();
      texture_overrides.destroy();
    }


    /* Determine whether a MaterialInstance has a value for a Uniform, by location */
    bool has_uniform (s32_t location) const {
      return get_uniform_index(location).b != -1;
    }

    /* Determine whether a MaterialInstance has a value for a Uniform, by name */
    bool has_uniform (char const* name) const {
      return get_uniform_index(name).b != -1;
    }

    /* Determine whether a MaterialInstance's ShaderProgram can support a Uniform at the given location */
    bool supports_uniform (s32_t location) const {
      return base->supports_uniform(location);
    }

    /* Determine whether a MaterialInstance's ShaderProgram can support a Uniform with the given name */
    bool supports_uniform (char const* name) const {
      return base->supports_uniform(name);
    }


    /* Get the index of a MaterialInstance's Uniform inside its Uniform array, by location.
     * Returns -1 if no Uniform is bound to the given location in the MaterialInstance */
    ENGINE_API s64_t get_uniform_index_local (s32_t location) const;

    /* Get the index of a MaterialInstance's Uniform inside its Uniform array, by name.
     * Returns -1 if no Uniform is bound to the given name in the MaterialInstance */
    ENGINE_API s64_t get_uniform_index_local (char const* name) const;

    /* Get the index of a MaterialInstance's Uniform inside its base Material's Uniform array, by location.
     * Returns -1 if no Uniform is bound to the given location in the MaterialInstance */
    ENGINE_API s64_t get_uniform_index_base (s32_t location) const;

    /* Get the index of a MaterialInstance's Uniform inside its base Material's Uniform array, by name.
     * Returns -1 if no Uniform is bound to the given name in the MaterialInstance */
    ENGINE_API s64_t get_uniform_index_base (char const* name) const;

    /* Get the index of a MaterialInstance's Uniform inside its Uniform array or that of its base, by location.
     * The pair returned has a boolean that is true if the index is local, or false if it is in the base.
     * Returns -1 if no Uniform is bound to the given location in the MaterialInstance */
    ENGINE_API pair_t<bool, s64_t> get_uniform_index (s32_t location) const;

    /* Get the index of a MaterialInstance's Uniform inside its Uniform array or that of its base, by name.
     * The pair returned has a boolean that is true if the index is local, or false if it is in the base.
     * Returns -1 if no Uniform is bound to the given name in the MaterialInstance */
    ENGINE_API pair_t<bool, s64_t> get_uniform_index (char const* name) const;


    /* Get a pointer to a MaterialInstance's Uniform by location.
     * Returns NULL if no Uniform is bound to the given location in the MaterialInstance or its base.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    ENGINE_API Uniform* get_uniform_pointer (s32_t location);

    /* Get a MaterialInstance's Uniform by name.
     * Returns NULL if no Uniform is found with the given name,
     * or if no Uniform is bound to location associated with the name, in the MaterialInstance or its base.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    ENGINE_API Uniform* get_uniform_pointer (char const* name);


    /* Get a MaterialInstance's Uniform by location.
     * Panics if no Uniform is bound to the given location in the MaterialInstance or its base.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    ENGINE_API Uniform& get_uniform (s32_t location);

    /* Get a MaterialInstance's Uniform by name.
     * Panics if no Uniform is found with the given name,
     * or no Uniform is bound to location associated with the name, in the MaterialInstance or its base.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    ENGINE_API Uniform& get_uniform (char const* name);



    /* Apply a MaterialInstance's base and own settings to its base's ShaderProgram */
    ENGINE_API void apply_settings (Material const& mat) const;

    /* Apply a MaterialInstance's base and own uniforms to its base's ShaderProgram */
    ENGINE_API void apply_uniforms (Material const& mat, ShaderProgram const& program_ref) const;

    /* Apply a MaterialInstance's base and own textures to its base's ShaderProgram */
    ENGINE_API void apply_textures (Material const& mat) const;


    /* Apply a MaterialInstance's base and own settings and data to its base's ShaderProgram,
     * and bind it to the OpenGL context */
    ENGINE_API void use () const;


    /* Get a MaterialInstance's Uniform value by location.
     * Panics if no Uniform is bound to the given location in the MaterialInstance or its base,
     * or if the Uniform is not a value type, or on type mismatch.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    template <typename T> T& get_uniform_value (s32_t location) const {
      return get_uniform(location).get<T>();
    }
    
    /* Get a MaterialInstance's Uniform value by name.
     * Panics if no Uniform is bound to the given name in the MaterialInstance or its base,
     * or if the Uniform is not a value type, or on type mismatch.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    template <typename T> T& get_uniform_value (char const* name) const {
      return get_uniform(name).get<T>();
    }

    
    /* Get a MaterialInstance's Uniform array by location.
     * Panics if no Uniform is bound to the given location in the MaterialInstance or its base,
     * or if the Uniform is not a array type, or on type mismatch.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    template <typename T> T& get_uniform_array (s32_t location) const {
      return get_uniform(location).get_array<T>();
    }
    
    /* Get a MaterialInstance's Uniform array by name.
     * Panics if no Uniform is bound to the given name in the MaterialInstance or its base,
     * or if the Uniform is not a array type, or on type mismatch.
     * Creates a copy of the base's Uniform if one exists in the base but not the instance */
    template <typename T> T& get_uniform_array (char const* name) const {
      return get_uniform(name).get_array<T>();
    }


    /* Set a MaterialInstance's value Uniform by location.
     * Creates a new Uniform in the MaterialInstance if one does not exist.
     * Panics if the type is mismatched to an existing Uniform,
     * or if the MaterialInstance's base's ShaderProgram doesn't have a Uniform with the given location. */
    template <typename T> void set_uniform (s32_t location, T const& value) {
      static constexpr u8_t uniform_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(uniform_type) && !UniformType::validate_array(uniform_type), "MaterialInstance::set_uniform requires a value with a valid Uniform value type (Use set_uniform_array for array types)");

      Uniform* existing_uniform = get_uniform_pointer(location);

      if (existing_uniform != NULL) {
        existing_uniform->set(value);
      } else {
        m_assert(supports_uniform(location), "The ShaderProgram (with origin '%s') does not have a uniform at the given location %" PRId32, base->shader_program->origin, location);
        uniform_overrides.append({ location, value });
      }
    }

    /* Set a MaterialInstance's value Uniform by location.
     * Creates a new Uniform in the MaterialInstance if one does not exist.
     * Panics if the type is mismatched to an existing Uniform,
     * or if the MaterialInstance's base's ShaderProgram does not have a Uniform at the given name */
    template <typename T> void set_uniform (char const* name, T const& value) {
      ShaderProgram const& program_ref = *base->shader_program;

      s32_t location = program_ref.get_uniform_location(name);

      m_assert(
        location != -1,
        "The ShaderProgram (with origin '%s') does not have an active Uniform with the name '%s'",
        program_ref.origin, name
      );

      return set_uniform(location, value);
    }
    

    /* Copy new values into a MaterialInstance's array Uniform by location.
     * Overwrites existing values in the array, if one exists.
     * Creates a new Uniform in the MaterialInstance if one does not exist.
     * Panics if the element type is mismatched to an existing Uniform,
     * or if the MaterialInstance's base's ShaderProgram doesn't have a Uniform with the given location */
    template <typename T> void set_uniform_array (s32_t location, Array<T> const& arr) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "MaterialInstance::set_uniform_array requires a value with a valid Uniform array type (Use set_uniform for value types)");

      Uniform* existing_uniform = get_uniform_pointer(location);

      if (existing_uniform != NULL) {
        existing_uniform->set_array(arr);
      } else {
        m_assert(supports_uniform(location), "The ShaderProgram (with origin '%s') does not have a uniform at the given location %" PRId32, base->shader_program->origin, location);
        uniforms.append({ location, arr });
      }
    }
    

    /* Copy new values into a MaterialInstance's array Uniform by name.
     * Overwrites existing values in the array, if one exists.
     * Creates a new Uniform in the MaterialInstance if one does not exist.
     * Panics if the element type is mismatched to an existing Uniform,
     * or if the MaterialInstance's ShaderProgram does not have a Uniform at the given name */
    template <typename T> void set_uniform_array (char const* name, Array<T> const& arr) {
      ShaderProgram const& program_ref = *base->shader_program;

      s32_t location = program_ref.get_uniform_location(name);

      m_assert(
        location != -1,
        "The ShaderProgram (with origin '%s') does not have an active Uniform with the name '%s'",
        program_ref.origin, name
      );

      return set_uniform_array(location, arr);
    }


    /* Overwrite an element of a MaterialInstance's array Uniform
     * Panics if no array Uniform exists at the given location inside the instance or its base,
     * or if the element index is out of range,
     * or on type mismatch.
     * Copies the base's array first if a copy does not exist in the instance */
    template <typename T> void set_uniform_element (s32_t location, size_t index, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "MaterialInstance::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).set_element(index, value);
    }

    /* Append an element to the end of a MaterialInstance's array Uniform
     * Panics if no array Uniform exists at the given location inside the instance or its base,
     * or on type mismatch.
     * Copies the base's array first if a copy does not exist in the instance */
    template <typename T> void append_uniform_element (s32_t location, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "MaterialInstance::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).append_element(value);
    }

    /* Insert an element into a MaterialInstance's array Uniform at the given index
     * Panics if no array Uniform exists at the given location inside the instance or its base,
     * or on type mismatch.
     * Copies the base's array first if a copy does not exist in the instance */
    template <typename T> void insert_uniform_element (s32_t location, size_t index, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "MaterialInstance::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).insert_element(index, value);
    }

    /* Remove an element from a MaterialInstance's array Uniform at the given index
     * Panics if no array Uniform exists at the given location inside the instance or its base,
     * or on type mismatch.
     * Copies the base's array first if a copy does not exist in the instance */
    template <typename T> void remove_uniform_element (s32_t location, size_t index) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "MaterialInstance::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).remove_element<T>(index);
    }


    /* Remove a Uniform from a MaterialInstance by location.
     * Not recommended unless it's base has a value */
    ENGINE_API void unset_uniform (s32_t location);



    /* Get the index of a MaterialInstance's TextureUnit inside its texture overrides array, by location.
     * Returns -1 if no TextureUnit is bound to the given location in the MaterialInstance */
    ENGINE_API s64_t get_texture_index_local (s32_t location) const;

    /* Get the index of a MaterialInstance's TextureUnit inside its base's texture units array, by location.
     * Returns -1 if no TextureUnit is bound to the given location in the MaterialInstance's base */
    ENGINE_API s64_t get_texture_index_base (s32_t location) const;

    /* Get the index of a MaterialInstance's TextureUnit inside its texture overrides array or its base's texture units array, by location.
     * The pair returned has a boolean that is true if the index is local, or false if it is in the base.
     * Returns -1 if no TextureUnit is bound to the given location in the MaterialInstance or its base */
    ENGINE_API pair_t<bool, s64_t> get_texture_index (s32_t location) const;

    /* Get a pointer to a MaterialInstance's TextureUnit by location.
     * Returns NULL if no TextureUnit is bound to the given location in the MaterialInstance or its base.
     * Creates a copy of the base's TextureUnit if one is bound in the base but not the instance */
    ENGINE_API TextureUnit* get_texture_pointer (s32_t location);

    /* Get a MaterialInstance's TextureUnit by location.
     * Panics if no TextureUnit is bound to the given location in the MaterialInstance or its base.
     * Creates a copy of the base's TextureUnit if one is bound in the base but not the instance */
    ENGINE_API TextureUnit& get_texture (s32_t location);

    /* Get a MaterialInstance's TextureUnit value by location.
     * Panics if no TextureUnit is bound to the given location in the MaterialInstance or its base.
     * Creates a copy of the base's TextureUnit if one is bound in the base but not the instance */
    TextureHandle& get_texture_value (s32_t location) {
      return get_texture(location).texture;
    }

    /* Set a MaterialInstance's TextureUnit value by location.
     * Creates a new TextureUnit if none is bound to the given location in the MaterialInstance,
     * or overwrites the existing value */
    ENGINE_API void set_texture (s32_t location, TextureHandle const& value);

    /* Remove a TextureUnit from a MaterialInstance by location.
     * Not recommended unless it's base has a value */
    ENGINE_API void unset_texture (s32_t location);
  };
}

#endif