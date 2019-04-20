#ifndef SHADER_H
#define SHADER_H

#include "../cstd.hh"
#include "../util.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "../AssetHandle.hh"


namespace mod {
  namespace ShaderType {
    enum: s8_t {
      Invalid = -1,
      Vertex = 0,
      Fragment,
      TesselationControl,
      TesselationEvaluation,
      Geometry,
      Compute
    };

    /* Get the name of a ShaderType as a str */
    static constexpr char const* name (u8_t type) {
      switch (type) {
        case Vertex: return "Vertex";
        case Fragment: return "Fragment";
        case TesselationControl: return "TesselationControl";
        case TesselationEvaluation: return "TesselationEvaluation";
        case Geometry: return "Geometry";
        case Compute: return "Compute";
        default: return "Invalid";
      }
    }

    /* Get a ShaderType from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(name, "Vertex", max_length) == 0) return Vertex;
      else if (str_cmp_caseless(name, "Fragment", max_length) == 0) return Fragment;
      else if (str_cmp_caseless(name, "TesselationControl", max_length) == 0) return TesselationControl;
      else if (str_cmp_caseless(name, "TesselationEvaluation", max_length) == 0) return TesselationEvaluation;
      else if (str_cmp_caseless(name, "Geometry", max_length) == 0) return Geometry;
      else if (str_cmp_caseless(name, "Compute", max_length) == 0) return Compute;
      else return Invalid;
    }

    /* Get a ShaderType from a file extension.
     * Accepts whole path or extension only */
    static s8_t from_file_ext (char const* path, size_t max_length = SIZE_MAX) {
      s64_t ext_offset = str_file_extension(path, max_length);

      static const auto match = [] (char const* str, size_t max) -> s8_t {
        if (str_cmp_caseless(str, "vert", max) == 0) return Vertex;
        else if (str_cmp_caseless(str, "frag", max) == 0) return Fragment;
        else if (str_cmp_caseless(str, "tesc", max) == 0) return TesselationControl;
        else if (str_cmp_caseless(str, "tese", max) == 0) return TesselationEvaluation;
        else if (str_cmp_caseless(str, "geom", max) == 0) return Geometry;
        else if (str_cmp_caseless(str, "comp", max) == 0) return Compute;
        else return Invalid;
      };

      if (ext_offset == -1) {
        return match(path, max_length);
      } else {
        return match(path + ext_offset + 1, max_length - ext_offset - 1);
      }
    }

    /* A list of accepted file formats */
    static char const* known_file_exts =
      "Vertex: .vert\n"
      "Fragment: .frag\n"
      "TesselationControl: .tesc\n"
      "TesselationEvaluation: .tese\n"
      "Geometry: .geom\n"
      "Compute: .comp";

    /* Convert an internal ShaderType to an OpenGL enum.
     * Returns GL_INVALID_ENUM if the type was invalid */
    static constexpr s32_t to_gl (u8_t type) {
      switch (type) {
        case Vertex: return GL_VERTEX_SHADER;
        case Fragment: return GL_FRAGMENT_SHADER;
        case TesselationControl: return GL_TESS_CONTROL_SHADER;
        case TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
        case Geometry: return GL_GEOMETRY_SHADER;
        case Compute: return GL_COMPUTE_SHADER;
        default: return GL_INVALID_ENUM;
      }
    }

    /* Determine if a value is a valid ShaderType */
    static constexpr bool validate (u8_t type) {
      return type >= Vertex
          && type <= Compute;
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