#ifndef TEXTURE_H
#define TEXTURE_H

#include "../cstd.hh"
#include "../util.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "../AssetHandle.hh"



namespace mod {
  namespace TextureWrap {
    enum: s8_t {
      Invalid = -1,
      Repeat = 0,
      Mirror,
      ClampEdge,
      ClampBorder
    };

    /* Get the name of a TextureWrap as a str */
    static constexpr char const* name (u8_t wrap) {
      switch (wrap) {
        case Repeat: return "Repeat";
        case Mirror: return "Mirror";
        case ClampEdge: return "ClampEdge";
        case ClampBorder: return "ClampBorder";
        default: return "Invalid";
      }
    }

    static constexpr char const* valid_values = (
      "Repeat,\n"
      "Mirror,\n"
      "ClampEdge,\n"
      "ClampBorder\n"
    );

    /* Get a TextureWrap from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(name, "Repeat", max_length) == 0) return Repeat;
      else if (str_cmp_caseless(name, "Mirror", max_length) == 0) return Mirror;
      else if (str_cmp_caseless(name, "ClampEdge", max_length) == 0) return ClampEdge;
      else if (str_cmp_caseless(name, "ClampBorder", max_length) == 0) return ClampBorder;
      else return Invalid;
    }

    /* Convert an internal TextureWrap to an OpenGL enum.
     * Returns GL_INVALID_ENUM if the wrap was invalid */
    static constexpr s32_t to_gl (u8_t wrap) {
      switch (wrap) {
        case Repeat: return GL_REPEAT;
        case Mirror: return GL_MIRRORED_REPEAT;
        case ClampEdge: return GL_CLAMP_TO_EDGE;
        case ClampBorder: return GL_CLAMP_TO_BORDER;
        default: return GL_INVALID_ENUM;
      }
    }

    /* Convert an OpenGL enum to a TextureWrap */
    static constexpr s8_t from_gl (s32_t gl_enum) {
      switch (gl_enum) {
        case GL_REPEAT: return Repeat;
        case GL_MIRRORED_REPEAT: return Mirror;
        case GL_CLAMP_TO_EDGE: return ClampEdge;
        case GL_CLAMP_TO_BORDER: return ClampBorder;
        default: return Invalid;
      }
    }

    /* Determine if a value is a valid TextureWrap */
    static constexpr bool validate (u8_t wrap) {
      return wrap >= Repeat
          && wrap <= ClampBorder;
    }
  }


  namespace TextureFilter {
    enum: s8_t {
      Invalid = -1,

      Nearest = 0,
      Linear,

      NearestNearest,
      LinearLinear,
      NearestLinear,
      LinearNearest
    };

    /* Get the name of a TextureFilter as a str */
    static constexpr char const* name (u8_t filter) {
      switch (filter) {
        case Nearest: return "Nearest";
        case Linear: return "Linear";
        case NearestNearest: return "NearestNearest";
        case LinearLinear: return "LinearLinear";
        case NearestLinear: return "NearestLinear";
        case LinearNearest: return "LinearNearest";
        default: return "Invalid";
      }
    }

    static constexpr char const* valid_min_values = (
      "Nearest,\n"
      "Linear,\n"
      "NearestNearest,\n"
      "LinearLinear,\n"
      "NearestLinear,\n"
      "LinearNearest\n"
    );

    static constexpr char const* valid_mag_values = (
      "Nearest,\n"
      "Linear\n"
    );

    /* Get a TextureFilter from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(name, "Nearest", max_length) == 0) return Nearest;
      else if (str_cmp_caseless(name, "Linear", max_length) == 0) return Linear;
      else if (str_cmp_caseless(name, "NearestNearest", max_length) == 0) return NearestNearest;
      else if (str_cmp_caseless(name, "LinearLinear", max_length) == 0) return LinearLinear;
      else if (str_cmp_caseless(name, "NearestLinear", max_length) == 0) return NearestLinear;
      else if (str_cmp_caseless(name, "LinearNearest", max_length) == 0) return LinearNearest;
      else return Invalid;
    }

    /* Convert an internal TextureFilter to an OpenGL enum.
     * Returns GL_INVALID_ENUM if the filter was invalid */
    static constexpr s32_t to_gl (u8_t filter) {
      switch (filter) {
        case Nearest: return GL_NEAREST;
        case Linear: return GL_LINEAR;
        case NearestNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case LinearLinear: return GL_LINEAR_MIPMAP_LINEAR;
        case NearestLinear: return GL_NEAREST_MIPMAP_LINEAR;
        case LinearNearest: return GL_LINEAR_MIPMAP_NEAREST;
        default: return GL_INVALID_ENUM;
      }
    }

    /* Convert an OpenGL enum to a TextureFilter */
    static constexpr s8_t from_gl (s32_t gl_enum) {
      switch (gl_enum) {
        case GL_NEAREST: return Nearest;
        case GL_LINEAR: return Linear;
        case GL_NEAREST_MIPMAP_NEAREST: return NearestNearest;
        case GL_LINEAR_MIPMAP_LINEAR: return LinearLinear;
        case GL_NEAREST_MIPMAP_LINEAR: return NearestLinear;
        case GL_LINEAR_MIPMAP_NEAREST: return LinearNearest;
        default: return Invalid;
      }
    }

    /* Determine if a value is a valid TextureFilter */
    static constexpr bool validate (u8_t filter) {
      return filter >= Nearest
          && filter <= LinearNearest;
    }

    /* Determine if a value is a valid TextureFilter for use in the mag filter slot */
    static constexpr bool validate_mag (u8_t filter) {
      return filter == Nearest
          || filter == Linear;
    }

    /* Determine if a TextureFilter uses a mipmap */
    static constexpr bool uses_mipmap (u8_t filter) {
      return filter >= NearestNearest
          && filter <= LinearNearest;
    }
  }


  struct Texture {
    char* origin;
    u32_t asset_id = 0;

    u32_t gl_id;


    /* Create a new uninitialized Texture */
    Texture () { }

    /* Create a new Texture from a FreeImage bitmap and optionally initialize its wrap and filter parameters (Defaults to TextureWrap::Repeat for both wrap axis and TextureFilter::Nearest for both filters) */
    ENGINE_API Texture (
      char const* origin,
      FIBITMAP* image,
      u8_t h_wrap = TextureWrap::Repeat,
      u8_t v_wrap = TextureWrap::Repeat,
      u8_t min_filter = TextureFilter::Nearest,
      u8_t mag_filter = TextureFilter::Nearest
    );



    /* Create a new Texture from a JSONItem */
    ENGINE_API static Texture from_json_item (char const* origin, JSONItem const& json);

    /* Create a new Texture from JSON */
    static Texture from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new Texture from a source str */
    ENGINE_API static Texture from_str (char const* origin, char const* source);

    /* Create a new Texture from a source file */
    ENGINE_API static Texture from_file (char const* origin);


    /* Clean up a Texture's heap allocation and delete its OpenGL version */
    ENGINE_API void destroy ();


    /* Upload a new FreeImage bitmap to OpenGL for a Texture, and optionally change its origin (Defaults to the existing origin) */
    ENGINE_API void update (FIBITMAP* new_image, char const* new_origin = NULL);

    /* Download a new FreeImage bitmap from OpenGL with a Texture's data from an optional mipmap level (Defaults to 0) */
    ENGINE_API FIBITMAP* read (s32_t level = 0) const;


    /* Bind a Texture's gl_id to the OpenGL context, at an optional texture unit location (Defaults to 0) */
    void use (s32_t location = 0) const {
      glActiveTexture(GL_TEXTURE0 + location);
      glBindTexture(GL_TEXTURE_2D, gl_id);
    }


    /* Set an arbitrary OpenGL parameter for a Texture */
    template <typename T> void set_parameter (s32_t location, T const& value) const {
      // TODO this function needs error checking (see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTexParameter.xhtml)

      static_assert(
           std::is_same<T, s32_t>::value
        || std::is_same<T, f32_t>::value
        || std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value
        || std::is_same<T, Vector2u>::value || std::is_same<T, Vector3u>::value || std::is_same<T, Vector4u>::value
        || std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value,
        "Texture::set_parameter may only be used with "
        "s32_t, "
        "f32_t, "
        "Vector2s, "
        "Vector3s, "
        "Vector4s, "
        "Vector2u, "
        "Vector3u, "
        "Vector4u, "
        "Vector2f, "
        "Vector3f, or "
        "Vector4f"
      );
      
      if constexpr (std::is_same<T, s32_t>::value) glTextureParameteri(gl_id, location, value);
      else if constexpr (std::is_same<T, f32_t>::value) glTextureParameterf(gl_id, location, value);
      
      else if constexpr (std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value) glTextureParameterIiv(gl_id, location, value.elements);
      else if constexpr (std::is_same<T, Vector2u>::value || std::is_same<T, Vector3u>::value || std::is_same<T, Vector4u>::value) glTextureParameterIuiv(gl_id, location, value.elements);
      else if constexpr (std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value) glTextureParameterfv(gl_id, location, value.elements);
    }

    /* Get the value of an arbitrary OpenGL parameter for a Texture */
    template <typename T> T get_parameter (s32_t location) const {
      // TODO this function needs error checking (see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTexParameter.xhtml)

      static_assert(
           std::is_same<T, s32_t>::value
        || std::is_same<T, u32_t>::value
        || std::is_same<T, f32_t>::value
        || std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value
        || std::is_same<T, Vector2u>::value || std::is_same<T, Vector3u>::value || std::is_same<T, Vector4u>::value
        || std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value,
        "Texture::get_parameter may only be used with "
        "s32_t, "
        "u32_t, "
        "f32_t, "
        "Vector2s, "
        "Vector3s, "
        "Vector4s, "
        "Vector2u, "
        "Vector3u, "
        "Vector4u, "
        "Vector2f, "
        "Vector3f, or "
        "Vector4f"
      );

      T out;

      if constexpr (std::is_same<T, s32_t>::value) glGetTextureParameterIiv(gl_id, location, &out);
      else if constexpr (std::is_same<T, u32_t>::value) glGetTextureParameterIuiv(gl_id, location, &out);
      else if constexpr (std::is_same<T, f32_t>::value) glGetTextureParameterfv(gl_id, location, &out);
      
      else if constexpr (std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value) glGetTextureParameterIiv(gl_id, location, out.elements);
      else if constexpr (std::is_same<T, Vector2u>::value || std::is_same<T, Vector3u>::value || std::is_same<T, Vector4u>::value) glGetTextureParameterIuiv(gl_id, location, out.elements);
      else if constexpr (std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value) glGetTextureParameterfv(gl_id, location, out.elements);

      return out;
    }

    /* Get the value of an arbitrary OpenGL parameter for a level of a Texture */
    template <typename T> T get_level_parameter (s32_t level, s32_t location) const {
      // TODO this function needs error checking (see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTexLevelParameter.xhtml)

      static_assert(
           std::is_same<T, s32_t>::value
        || std::is_same<T, f32_t>::value
        || std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value
        || std::is_same<T, Vector2u>::value || std::is_same<T, Vector3u>::value || std::is_same<T, Vector4u>::value
        || std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value,
        "Texture::get_level_parameter may only be used with "
        "s32_t, "
        "f32_t, "
        "Vector2s, "
        "Vector3s, "
        "Vector4s, "
        "Vector2u, "
        "Vector3u, "
        "Vector4u, "
        "Vector2f, "
        "Vector3f, or "
        "Vector4f"
      );

      T out;

      if constexpr (std::is_same<T, s32_t>::value) glGetTextureLevelParameteriv(gl_id, level, location, &out);
      else if constexpr (std::is_same<T, f32_t>::value) glGetTextureLevelParameterfv(gl_id, level, location, &out);
      
      else if constexpr (std::is_same<T, Vector2s>::value || std::is_same<T, Vector3s>::value || std::is_same<T, Vector4s>::value) glGetTextureLevelParameteriv(gl_id, level, location, out.elements);
      else if constexpr (std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value || std::is_same<T, Vector4f>::value) glGetTextureLevelParameterfv(gl_id, level, location, out.elements);

      return out;
    }

    

    /* Set the wrap parameters for a Texture */
    ENGINE_API void set_wrap (u8_t h_wrap, u8_t v_wrap) const;

    /* Set the filter parameters for a Texture */
    ENGINE_API void set_filter (u8_t min_filter, u8_t mag_filter) const;
    

    /* Get the wrap parameters for a Texture */
    pair_t<u8_t> get_wrap () const {
      return {
        (u8_t) TextureWrap::from_gl(get_parameter<s32_t>(GL_TEXTURE_WRAP_S)),
        (u8_t) TextureWrap::from_gl(get_parameter<s32_t>(GL_TEXTURE_WRAP_T))
      };
    }
    
    /* Get the filter parameters for a Texture */
    pair_t<u8_t> get_filter () const {
      return {
        (u8_t) TextureFilter::from_gl(get_parameter<s32_t>(GL_TEXTURE_MIN_FILTER)),
        (u8_t) TextureFilter::from_gl(get_parameter<s32_t>(GL_TEXTURE_MAG_FILTER))
      };
    }

    /* Get the size of a Texture at an optional mipmap level (Defaults to 0, the actual size) */
    Vector2s get_size (s32_t level = 0) const {
      return {
        get_level_parameter<s32_t>(level, GL_TEXTURE_WIDTH),
        get_level_parameter<s32_t>(level, GL_TEXTURE_HEIGHT)
      };
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


  using TextureHandle = AssetHandle<Texture>;
}

#endif