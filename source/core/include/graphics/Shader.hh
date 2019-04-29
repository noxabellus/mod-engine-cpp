#ifndef SHADER_H
#define SHADER_H

#include "../cstd.hh"
#include "../util.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "../AssetHandle.hh"


namespace mod {
  namespace ShaderType {
    enum: u8_t {
      Vertex,
      Fragment,
      TesselationControl,
      TesselationEvaluation,
      Geometry,
      Compute,

      total_type_count,

      Invalid = (u8_t) -1,
    };

    static constexpr char const* names [total_type_count] = {
      "Vertex",
      "Fragment",
      "TesselationControl",
      "TesselationEvaluation",
      "Geometry",
      "Compute"
    };

    /* Get the name of a ShaderType as a str */
    static constexpr char const* name (u8_t type) {
      if (type < total_type_count) return names[type];
      else return "Invalid";
    }

    /* Get a ShaderType from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t type = 0; type < total_type_count; type ++) {
        if ((max_length == SIZE_MAX || strlen(names[type]) == max_length) && str_cmp_caseless(name, names[type], max_length) == 0) return type;
      }

      return Invalid;
    }

    static constexpr char const* extensions [total_type_count] = {
      "vert",
      "frag",
      "tesc",
      "tese",
      "geom",
      "comp"
    };

    /* Get a ShaderType from a file extension.
     * Accepts whole path or extension only */
    static constexpr u8_t from_file_ext (char const* path, size_t max_length = SIZE_MAX) {
      s64_t ext_offset = str_file_extension(path, max_length);

      char const* offset_path = ext_offset == -1? path : path + ext_offset + 1;
      size_t offset_max = ext_offset == -1? max_length : max_length - ext_offset - 1;

      for (u8_t type = 0; type < total_type_count; type ++) {
        if ((max_length == SIZE_MAX || strlen(extensions[type]) == max_length) && str_cmp_caseless(offset_path, extensions[type], offset_max) == 0) return type;
      }

      return Invalid;
    }

    /* Get a file extension from a ShaderType.
     * Returns NULL if the ShaderType was invalid */
    static constexpr char const* to_file_ext (u8_t type) {
      if (type < total_type_count) return extensions[type];
      else return NULL;
    }

    /* A list of accepted file formats */
    static constexpr char const* known_file_exts = (
      "Vertex: .vert\n"
      "Fragment: .frag\n"
      "TesselationControl: .tesc\n"
      "TesselationEvaluation: .tese\n"
      "Geometry: .geom\n"
      "Compute: .comp"
    );

    static constexpr s32_t gl_versions [total_type_count] = {
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
      GL_TESS_CONTROL_SHADER,
      GL_TESS_EVALUATION_SHADER,
      GL_GEOMETRY_SHADER,
      GL_COMPUTE_SHADER
    };

    /* Convert an internal ShaderType to an OpenGL enum.
     * Returns GL_INVALID_ENUM if the type was invalid */
    static constexpr s32_t to_gl (u8_t type) {
      if (type < total_type_count) return gl_versions[type];
      else return GL_INVALID_ENUM;
    }

    /* Convert an OpenGL enum to an internal ShaderType */
    static constexpr u8_t from_gl (s32_t gl_type) {
      for (u8_t type = 0; type < total_type_count; type ++) {
        if (gl_versions[type] == gl_type) return type;
      }

      return Invalid;
    }

    /* Determine if a value is a valid ShaderType */
    static constexpr bool validate (u8_t type) {
      return type < total_type_count;
    }
  }


  struct Shader {
    char* origin;
    u32_t asset_id = 0;

    u32_t gl_id;
    u8_t type;


    /* Create a new uninitialized Shader */
    Shader () { }

    /* Create a new Shader and initialize its data.
     * Compiles the source in OpenGL (Source must not be null) */
    ENGINE_API Shader (char const* in_origin, u8_t in_type, char const* source);

    /* Create a new Shader from a source file */
    ENGINE_API static Shader from_file (char const* origin);


    /* Clean up a Shader's origin heap allocation and delete it's compiled OpenGL version */
    ENGINE_API void destroy ();

    

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


  using ShaderHandle = AssetHandle<Shader>;
}

#endif