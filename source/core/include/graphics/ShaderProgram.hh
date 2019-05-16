#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "../cstd.hh"
#include "../util.hh"
#include "../Array.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "Shader.hh"
#include "../AssetHandle.hh"

#include "../math/lib.hh"


namespace mod {
  struct UniformInfo {
    #ifndef CUSTOM_UNIFORM_INFO_MAX_NAME_LENGTH
      static constexpr size_t max_name_length = 64;
    #else
      static constexpr size_t max_name_length = CUSTOM_UNIFORM_INFO_MAX_NAME_LENGTH;
    #endif

    char name [max_name_length] = { 0 };
    s32_t size;
    u32_t type;
    s32_t location;
  };

  struct ShaderProgram {
    char* origin;
    u32_t asset_id = 0;

    u32_t gl_id;
    
    ShaderHandle vertex_shader;
    ShaderHandle fragment_shader;
    ShaderHandle tesselation_control_shader;
    ShaderHandle tesselation_evaluation_shader;
    ShaderHandle geometry_shader;
    ShaderHandle compute_shader;

    Array<UniformInfo> uniform_info;


    /* Create a new uninitialized ShaderProgram */
    ShaderProgram () { }

    /* Create a new ShaderProgram from a set of Shaders */
    ENGINE_API ShaderProgram (
      char const* in_origin,

      ShaderHandle vertex = { },
      ShaderHandle fragment = { },
      ShaderHandle tesselation_control = { },
      ShaderHandle tesselation_evaluation = { },
      ShaderHandle geometry = { },
      ShaderHandle compute = { }
    );


    /* Create a new ShaderProgram from a subitem of JSON */
    ENGINE_API static ShaderProgram from_json_item (char const* origin, JSONItem const& json_item);

    /* Create a new ShaderProgram from JSON */
    static ShaderProgram from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new ShaderProgram from a source str */
    ENGINE_API static ShaderProgram from_str (char const* origin, char const* source);

    /* Create a new ShaderProgram from a source file */
    ENGINE_API static ShaderProgram from_file (char const* origin);

    /* Clean up a ShaderProgram's heap allocation and delete its compiled OpenGL version */
    ENGINE_API void destroy ();


    /* Get an array of information about a ShaderProgram's uniforms */
    ENGINE_API Array<UniformInfo> const& get_uniform_info ();

    /* Print information about a ShaderProgram's uniforms */
    ENGINE_API void dump_uniform_info ();


    /* Determine if a ShaderProgram has a uniform at the given location */
    bool has_uniform (s32_t location) {
      for (auto [ i, info ] : get_uniform_info()) {
        if (info.location == location) return true;
      }

      return false;
    }

    /* Determine if a ShaderProgram has a uniform at the given name */
    bool has_uniform (char const* name) {
      return get_uniform_location(name) != -1;
    }


    /* Bind a ShaderProgram's gl_id to the OpenGL context */
    void use () const {
      glUseProgram(gl_id);
    }


    /* Get the location id of a ShaderProgram's uniform by name.
     * Returns -1 if no active uniform was found */
    s32_t get_uniform_location (char const* name) const {
      return glGetUniformLocation(gl_id, name);
    }


    /* Set the value of a uniform in a ShaderProgram */
    template <typename T> void set_uniform (s32_t location, T const& value) const {
      static_assert(
           std::is_same<T, bool>::value
        || std::is_same<T, s32_t>::value
        || std::is_same<T, u32_t>::value
        || std::is_same<T, f32_t>::value
        || std::is_same<T, f64_t>::value
        || std::is_same<T, Vector2s>::value
        || std::is_same<T, Vector2u>::value
        || std::is_same<T, Vector2f>::value
        || std::is_same<T, Vector2d>::value
        || std::is_same<T, Vector3s>::value
        || std::is_same<T, Vector3u>::value
        || std::is_same<T, Vector3f>::value
        || std::is_same<T, Vector3d>::value
        || std::is_same<T, Vector4s>::value
        || std::is_same<T, Vector4u>::value
        || std::is_same<T, Vector4f>::value
        || std::is_same<T, Vector4d>::value
        || std::is_same<T, Matrix3>::value
        || std::is_same<T, Matrix4>::value,
        "ShaderProgram::set_uniform may only be used with values of type "
        "bool, "
        "s32_t, "
        "u32_t, "
        "f32_t, "
        "f64_t, "
        "Vector2s, "
        "Vector2u, "
        "Vector2f, "
        "Vector2d, "
        "Vector3s, "
        "Vector3u, "
        "Vector3f, "
        "Vector3d, "
        "Vector4s, "
        "Vector4u, "
        "Vector4f, "
        "Vector4d, "
        "Matrix3, or "
        "Matrix4, or arrays of these types"
      );

      if constexpr (std::is_same<T, bool>::value) return glProgramUniform1i(gl_id, location, static_cast<s32_t>(value));

      else if constexpr (std::is_same<T, s32_t>::value) return glProgramUniform1i(gl_id, location, value);
      else if constexpr (std::is_same<T, u32_t>::value) return glProgramUniform1ui(gl_id, location, value);
      else if constexpr (std::is_same<T, f32_t>::value) return glProgramUniform1f(gl_id, location, value);
      else if constexpr (std::is_same<T, f64_t>::value) return glProgramUniform1d(gl_id, location, value);

      else if constexpr (std::is_same<T, Vector2s>::value) return glProgramUniform2i(gl_id, location, value.x, value.y);
      else if constexpr (std::is_same<T, Vector2u>::value) return glProgramUniform2ui(gl_id, location, value.x, value.y);
      else if constexpr (std::is_same<T, Vector2f>::value) return glProgramUniform2f(gl_id, location, value.x, value.y);
      else if constexpr (std::is_same<T, Vector2d>::value) return glProgramUniform2d(gl_id, location, value.x, value.y);

      else if constexpr (std::is_same<T, Vector3s>::value) return glProgramUniform3i(gl_id, location, value.x, value.y, value.z);
      else if constexpr (std::is_same<T, Vector3u>::value) return glProgramUniform3ui(gl_id, location, value.x, value.y, value.z);
      else if constexpr (std::is_same<T, Vector3f>::value) return glProgramUniform3f(gl_id, location, value.x, value.y, value.z);
      else if constexpr (std::is_same<T, Vector3d>::value) return glProgramUniform3d(gl_id, location, value.x, value.y, value.z);

      else if constexpr (std::is_same<T, Vector4s>::value) return glProgramUniform4i(gl_id, location, value.x, value.y, value.z, value.w);
      else if constexpr (std::is_same<T, Vector4u>::value) return glProgramUniform4ui(gl_id, location, value.x, value.y, value.z, value.w);
      else if constexpr (std::is_same<T, Vector4f>::value) return glProgramUniform4f(gl_id, location, value.x, value.y, value.z, value.w);
      else if constexpr (std::is_same<T, Vector4d>::value) return glProgramUniform4d(gl_id, location, value.x, value.y, value.z, value.w);

      else if constexpr (std::is_same<T, Matrix3>::value) return glProgramUniformMatrix3fv(gl_id, location, 1, GL_FALSE, value.elements);
      else if constexpr (std::is_same<T, Matrix4>::value) return glProgramUniformMatrix4fv(gl_id, location, 1, GL_FALSE, value.elements);
    }

    /* Set the array of a uniform inside a ShaderProgram, from a buffer */
    template <typename T> void set_uniform (s32_t location, T const* values, size_t value_count) const {
      static_assert(
           std::is_same<T, bool>::value
        || std::is_same<T, s32_t>::value
        || std::is_same<T, u32_t>::value
        || std::is_same<T, f32_t>::value
        || std::is_same<T, f64_t>::value
        || std::is_same<T, Vector2s>::value
        || std::is_same<T, Vector2u>::value
        || std::is_same<T, Vector2f>::value
        || std::is_same<T, Vector2d>::value
        || std::is_same<T, Vector3s>::value
        || std::is_same<T, Vector3u>::value
        || std::is_same<T, Vector3f>::value
        || std::is_same<T, Vector3d>::value
        || std::is_same<T, Vector4s>::value
        || std::is_same<T, Vector4u>::value
        || std::is_same<T, Vector4f>::value
        || std::is_same<T, Vector4d>::value
        || std::is_same<T, Matrix3>::value
        || std::is_same<T, Matrix4>::value,
        "ShaderProgram::set_uniform may only be used with values of type "
        "bool, "
        "s32_t, "
        "u32_t, "
        "f32_t, "
        "f64_t, "
        "Vector2s, "
        "Vector2u, "
        "Vector2f, "
        "Vector2d, "
        "Vector3s, "
        "Vector3u, "
        "Vector3f, "
        "Vector3d, "
        "Vector4s, "
        "Vector4u, "
        "Vector4f, "
        "Vector4d, "
        "Matrix3, or "
        "Matrix4, or arrays of these types"
      );

      static Array<s32_t> intermediate_bool_array = Array<s32_t> { 0, true };

      if constexpr (std::is_same<T, bool>::value) {
        intermediate_bool_array.clear();
        
        for (size_t i = 0; i < value_count; i ++) {
          intermediate_bool_array.append(static_cast<s32_t>(values[i]));
        }

        return set_uniform<s32_t>(location, intermediate_bool_array.elements, value_count);
      }

      else if constexpr (std::is_same<T, s32_t>::value) return glProgramUniform1iv(gl_id, location, value_count, values);
      else if constexpr (std::is_same<T, u32_t>::value) return glProgramUniform1uiv(gl_id, location, value_count, values);
      else if constexpr (std::is_same<T, f32_t>::value) return glProgramUniform1fv(gl_id, location, value_count, values);
      else if constexpr (std::is_same<T, f64_t>::value) return glProgramUniform1dv(gl_id, location, value_count, values);

      else if constexpr (std::is_same<T, Vector2s>::value) return glProgramUniform2iv(gl_id, location, value_count, reinterpret_cast<s32_t const*>(values));
      else if constexpr (std::is_same<T, Vector2u>::value) return glProgramUniform2uiv(gl_id, location, value_count, reinterpret_cast<u32_t const*>(values));
      else if constexpr (std::is_same<T, Vector2f>::value) return glProgramUniform2fv(gl_id, location, value_count, reinterpret_cast<f32_t const*>(values));
      else if constexpr (std::is_same<T, Vector2d>::value) return glProgramUniform2dv(gl_id, location, value_count, reinterpret_cast<f64_t const*>(values));

      else if constexpr (std::is_same<T, Vector3s>::value) return glProgramUniform3iv(gl_id, location, value_count, reinterpret_cast<s32_t const*>(values));
      else if constexpr (std::is_same<T, Vector3u>::value) return glProgramUniform3uiv(gl_id, location, value_count, reinterpret_cast<u32_t const*>(values));
      else if constexpr (std::is_same<T, Vector3f>::value) return glProgramUniform3fv(gl_id, location, value_count, reinterpret_cast<f32_t const*>(values));
      else if constexpr (std::is_same<T, Vector3d>::value) return glProgramUniform3dv(gl_id, location, value_count, reinterpret_cast<f64_t const*>(values));

      else if constexpr (std::is_same<T, Vector4s>::value) return glProgramUniform4iv(gl_id, location, value_count, reinterpret_cast<s32_t const*>(values));
      else if constexpr (std::is_same<T, Vector4u>::value) return glProgramUniform4uiv(gl_id, location, value_count, reinterpret_cast<u32_t const*>(values));
      else if constexpr (std::is_same<T, Vector4f>::value) return glProgramUniform4fv(gl_id, location, value_count, reinterpret_cast<f32_t const*>(values));
      else if constexpr (std::is_same<T, Vector4d>::value) return glProgramUniform4dv(gl_id, location, value_count, reinterpret_cast<f64_t const*>(values));

      else if constexpr (std::is_same<T, Matrix3>::value) return glProgramUniformMatrix3fv(gl_id, location, value_count, GL_FALSE, reinterpret_cast<f32_t const*>(values));
      else if constexpr (std::is_same<T, Matrix4>::value) return glProgramUniformMatrix4fv(gl_id, location, value_count, GL_FALSE, reinterpret_cast<f32_t const*>(values));
    }

    /* Set the array of a uniform inside a ShaderProgram */
    template <typename T> void set_uniform (s32_t location, Array<T> const& arr) const {
      return set_uniform(location, arr.elements, arr.count);
    }


    /* Set the value of a uniform inside a ShaderProgram, by name */
    template <typename T> void set_uniform (char const* name, T const& value) const {
      s32_t location = get_uniform_location(name);

      asset_assert(location != -1, "Cannot find uniform with name '%s' to set its value", name);

      return set_uniform(location, value);
    }

    /* Set the array of a uniform inside a ShaderProgram, from a buffer, by name */
    template <typename T> void set_uniform (char const* name, T const* values, size_t value_count) const {
      s32_t location = get_uniform_location(name);

      asset_assert(location != -1, "Cannot find uniform with name '%s' to set its array", name);

      return set_uniform(location, values, value_count);
    }

    /* Set the array of a uniform inside a ShaderProgram, by name */
    template <typename T> void set_uniform (char const* name, Array<T> const& arr) const {
      return set_uniform(name, arr.elements, arr.count);
    }

    

    /* Throw an exception using the origin of this Asset, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (s32_t line, char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, char const* fmt, A ... args) {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, s32_t line, char const* fmt, A ... args) {
      if (!cond) asset_error(line, fmt, args...);
    }


    /* Throw an exception using the origin of this Asset */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number */
    template <typename ... A> NORETURN void asset_error (s32_t line, char const* fmt, A ... args) const {
      m_asset_error(origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, s32_t line, char const* fmt, A ... args) const {
      if (!cond) asset_error(line, fmt, args...);
    }
  };


  using ShaderProgramHandle = AssetHandle<ShaderProgram>;
}

#endif