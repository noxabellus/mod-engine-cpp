#ifndef MATERIAL_H
#define MATERIAL_H

#include "../cstd.hh"
#include "../util.hh"
#include "../JSON.hh"
#include "../Array.hh"
#include "../Exception.hh"

#include "../math/lib.hh"

#include "../AssetHandle.hh"

#include "ShaderProgram.hh"
#include "Texture.hh"



namespace mod {
  struct TextureSlot { // Wrapper for proper type info
    s32_t value = 0;

    TextureSlot () { }
    TextureSlot (s32_t const& in_value)
    : value(in_value)
    { }
    operator s32_t () const {
      return value;
    }
  };

  namespace UniformType {
    enum: u8_t {
      Bool,
      F32,
      F64,
      S32,
      U32,

      Vector2f,
      Vector2d,
      Vector2s,
      Vector2u,
      Vector3f,
      Vector3d,
      Vector3s,
      Vector3u,
      Vector4f,
      Vector4d,
      Vector4s,
      Vector4u,
      Matrix3,
      Matrix4,

      Texture,

      BoolArray,
      F32Array,
      F64Array,
      S32Array,
      U32Array,

      Vector2fArray,
      Vector2dArray,
      Vector2sArray,
      Vector2uArray,
      Vector3fArray,
      Vector3dArray,
      Vector3sArray,
      Vector3uArray,
      Vector4fArray,
      Vector4dArray,
      Vector4sArray,
      Vector4uArray,

      Matrix3Array,
      Matrix4Array,

      total_type_count,

      Invalid = -1
    };


    static constexpr char const* names [total_type_count] = {
      "Bool",
      "F32",
      "F64",
      "S32",
      "U32",

      "Vector2f",
      "Vector2d",
      "Vector2s",
      "Vector2u",
      "Vector3f",
      "Vector3d",
      "Vector3s",
      "Vector3u",
      "Vector4f",
      "Vector4d",
      "Vector4s",
      "Vector4u",

      "Matrix3",
      "Matrix4",

      "Texture",

      "BoolArray",
      "F32Array",
      "F64Array",
      "S32Array",
      "U32Array",

      "Vector2fArray",
      "Vector2dArray",
      "Vector2sArray",
      "Vector2uArray",
      "Vector3fArray",
      "Vector3dArray",
      "Vector3sArray",
      "Vector3uArray",
      "Vector4fArray",
      "Vector4dArray",
      "Vector4sArray",
      "Vector4uArray",

      "Matrix3Array",
      "Matrix4Array"
    };

    /* Get the name of a UniformType as a str */
    static constexpr char const* name (u8_t type) {
      if (type < total_type_count) return names[type];
      else return "Invalid";
    }

    static constexpr char const* valid_values = (
      "Bool,\n"
      "F32,\n"
      "F64,\n"
      "S32,\n"
      "U32,\n"
      "Vector2f,\n"
      "Vector2d,\n"
      "Vector2s,\n"
      "Vector2u,\n"
      "Vector3f,\n"
      "Vector3d,\n"
      "Vector3s,\n"
      "Vector3u,\n"
      "Vector4f,\n"
      "Vector4d,\n"
      "Vector4s,\n"
      "Vector4u,\n"
      "Matrix3,\n"
      "Matrix4,\n"
      "Texture,\n"
      "BoolArray,\n"
      "F32Array,\n"
      "F64Array,\n"
      "S32Array,\n"
      "U32Array,\n"
      "Vector2fArray,\n"
      "Vector2dArray,\n"
      "Vector2sArray,\n"
      "Vector2uArray,\n"
      "Vector3fArray,\n"
      "Vector3dArray,\n"
      "Vector3sArray,\n"
      "Vector3uArray,\n"
      "Vector4fArray,\n"
      "Vector4dArray,\n"
      "Vector4sArray,\n"
      "Vector4uArray,\n"
      "Matrix3Array,\n"
      "Matrix4Array\n"
    );

    /* Get a UniformType from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t type = 0; type < total_type_count; type ++) {
        if ((max_length == SIZE_MAX || strlen(names[type]) == max_length) && str_cmp_caseless(name, names[type], max_length) == 0) return type;
      }

      return Invalid;
    }

    static constexpr size_t sizes [BoolArray] = {
      sizeof(bool),
      sizeof(f32_t),
      sizeof(f64_t),
      sizeof(s32_t),
      sizeof(u32_t),
      sizeof(Vector2f),
      sizeof(Vector2d),
      sizeof(Vector2s),
      sizeof(Vector2u),
      sizeof(Vector3f),
      sizeof(Vector3d),
      sizeof(Vector3s),
      sizeof(Vector3u),
      sizeof(Vector4f),
      sizeof(Vector4d),
      sizeof(Vector4s),
      sizeof(Vector4u),
      sizeof(Matrix3),
      sizeof(Matrix4),
      sizeof(TextureSlot)
    };

    /* Get the size in bytes of a UniformType
     * Returns -1 for invalid or array types */
    static constexpr s64_t size (u8_t type) {
      if (type < BoolArray) return sizes[type];
      else return -1;
    }

    static constexpr u8_t element_types [total_type_count - BoolArray] = {
      Bool,
      F32,
      F64,
      S32,
      U32,

      Vector2f,
      Vector2d,
      Vector2s,
      Vector2u,
      Vector3f,
      Vector3d,
      Vector3s,
      Vector3u,
      Vector4f,
      Vector4d,
      Vector4s,
      Vector4u,

      Matrix3,
      Matrix4
    };

    /* Get the value UniformType associated with an array UniformType */
    static constexpr u8_t element_type (u8_t type) {
      if (type > Texture && type < total_type_count) return element_types[type - BoolArray];
      else return Invalid;
    }

    /* Determine if a value is a valid UniformType */
    static constexpr bool validate (u8_t type) {
      return type < total_type_count;
    }

    /* Determine if a value is a valid value UniformType */
    static constexpr bool validate_value (u8_t type) {
      return type < BoolArray;
    }

    /* Determine if a value is a valid array UniformType */
    static constexpr bool validate_array (u8_t type) {
      return type > Texture
          && type < total_type_count;
    }

    /* Get a UniformType from a real type */
    template <typename T> static constexpr u8_t from_type () {
      if constexpr (std::is_same<T, bool>::value) return Bool;
      else if constexpr (std::is_same<T, f32_t>::value) return F32;
      else if constexpr (std::is_same<T, f64_t>::value) return F64;
      else if constexpr (std::is_same<T, s32_t>::value) return S32;
      else if constexpr (std::is_same<T, u32_t>::value) return U32;

      else if constexpr (std::is_same<T, ::mod::Vector2f>::value) return Vector2f;
      else if constexpr (std::is_same<T, ::mod::Vector2d>::value) return Vector2d;
      else if constexpr (std::is_same<T, ::mod::Vector2s>::value) return Vector2s;
      else if constexpr (std::is_same<T, ::mod::Vector2u>::value) return Vector2u;
      else if constexpr (std::is_same<T, ::mod::Vector3f>::value) return Vector3f;
      else if constexpr (std::is_same<T, ::mod::Vector3d>::value) return Vector3d;
      else if constexpr (std::is_same<T, ::mod::Vector3s>::value) return Vector3s;
      else if constexpr (std::is_same<T, ::mod::Vector3u>::value) return Vector3u;
      else if constexpr (std::is_same<T, ::mod::Vector4f>::value) return Vector4f;
      else if constexpr (std::is_same<T, ::mod::Vector4d>::value) return Vector4d;
      else if constexpr (std::is_same<T, ::mod::Vector4s>::value) return Vector4s;
      else if constexpr (std::is_same<T, ::mod::Vector4u>::value) return Vector4u;

      else if constexpr (std::is_same<T, ::mod::Matrix3>::value) return Matrix3;
      else if constexpr (std::is_same<T, ::mod::Matrix4>::value) return Matrix4;

      else if constexpr (std::is_same<T, ::mod::TextureSlot>::value) return Texture;

      else if constexpr (std::is_same<T, Array<bool> >::value) return BoolArray;
      else if constexpr (std::is_same<T, Array<f32_t> >::value) return F32Array;
      else if constexpr (std::is_same<T, Array<f64_t> >::value) return F64Array;
      else if constexpr (std::is_same<T, Array<s32_t> >::value) return S32Array;
      else if constexpr (std::is_same<T, Array<u32_t> >::value) return U32Array;

      else if constexpr (std::is_same<T, Array<::mod::Vector2f> >::value) return Vector2fArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector2d> >::value) return Vector2dArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector2s> >::value) return Vector2sArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector2u> >::value) return Vector2uArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector3f> >::value) return Vector3fArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector3d> >::value) return Vector3dArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector3s> >::value) return Vector3sArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector3u> >::value) return Vector3uArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector4f> >::value) return Vector4fArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector4d> >::value) return Vector4dArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector4s> >::value) return Vector4sArray;
      else if constexpr (std::is_same<T, Array<::mod::Vector4u> >::value) return Vector4uArray;

      else if constexpr (std::is_same<T, Array<::mod::Matrix3> >::value) return Matrix3Array;
      else if constexpr (std::is_same<T, Array<::mod::Matrix4> >::value) return Matrix4Array;
      
      else return Invalid;
    }
  }


  namespace FaceCullingSide {
    enum: u8_t {
      Back,
      Front,

      total_side_count,

      Invalid = -1
    };

    static constexpr char const* names [total_side_count] = {
      "Back", "Front"
    };

    /* Get the name of a FaceCullingSide as a str */
    static constexpr char const* name (u8_t side) {
      if (side < total_side_count) return names[side];
      else return "Invalid";
    }

    static constexpr char const* valid_values = (
      "Back,\n"
      "Front\n"
    );

    /* Get a FaceCullingSide from its name in str form */
    static u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t side = 0; side < total_side_count; side ++) {
        if ((max_length == SIZE_MAX || strlen(names[side]) == max_length) && str_cmp_caseless(name, names[side], max_length) == 0) return side;
      }

      return Invalid;
    }

    static constexpr s32_t gl_versions [total_side_count] = {
      GL_BACK,
      GL_FRONT
    };

    /* Get an OpenGL enum from a FaceCullingSide.
     * Returns GL_INVALID_ENUM if the FaceCullingSide was invalid */
    static constexpr s32_t to_gl (u8_t side) {
      if (side < total_side_count) return gl_versions[side];
      else return GL_INVALID_ENUM;
    }

    /* Get a FaceCullingSide from an OpenGL enum */
    static constexpr u8_t from_gl (s32_t gl_side) {
      for (u8_t side = 0; side < total_side_count; side ++) {
        if (gl_versions[side] == gl_side) return side;
      }

      return Invalid;
    }

    /* Determine if a value is a valid FaceCullingSide */
    static constexpr bool validate (u8_t side) {
      return side < total_side_count;
    }
  }

  namespace VertexWinding {
    enum: u8_t {
      Clockwise,
      CounterClockwise,

      total_wind_count,

      Invalid = -1
    };

    static constexpr char const* names [total_wind_count] = {
      "Clockwise",
      "CounterClockwise"
    };

    /* Get the name of a VertexWinding as a str */
    static constexpr char const* name (u8_t wind) {
      if (wind < total_wind_count) return names[wind];
      else return "Invalid";
    }

    static constexpr char const* valid_values = (
      "Clockwise,\n"
      "CounterClockwise"
    );

    /* Get a VertexWinding from its name in str form */
    static u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t wind = 0; wind < total_wind_count; wind ++) {
        if ((max_length == SIZE_MAX || strlen(names[wind]) == max_length) && str_cmp_caseless(name, names[wind], max_length) == 0) return wind;
      }
      return Invalid;
    }

    static constexpr s32_t gl_versions [total_wind_count] = {
      GL_CW,
      GL_CCW
    };

    /* Get an OpenGL enum from a VertexWinding.
     * Returns GL_INVALID_ENUM if the VertexWinding was invalid */
    static constexpr s32_t to_gl (u8_t wind) {
      if (wind < total_wind_count) return gl_versions[wind];
      else return GL_INVALID_ENUM;
    }

    /* Get a VertexWinding from an OpenGL enum */
    static constexpr u8_t from_gl (s32_t gl_wind) {
      for (u8_t wind = 0; wind < total_wind_count; wind ++) {
        if (gl_versions[wind] == gl_wind) return wind;
      }

      return Invalid;
    }

    /* Determine if a value is a valid VertexWinding */
    static constexpr bool validate (u8_t wind) {
      return wind < total_wind_count;
    }
  }

  namespace AlphaBlend {
    enum: u8_t {
      Zero,
      One,
      SourceColor,
      OneMinusSourceColor,
      DestinationColor,
      OneMinusDestinationColor,
      SourceAlpha,
      OneMinusSourceAlpha,
      DestinationAlpha,
      OneMinusDestinationAlpha,
      ConstantColor,
      OneMinusConstantColor,
      ConstantAlpha,
      OneMinusConstantAlpha,
      SourceAlphaSaturate,

      total_blend_count,

      Invalid = -1
    };

    static constexpr char const* names [total_blend_count] = {
      "Zero",
      "One",
      "SourceColor",
      "OneMinusSourceColor",
      "DestinationColor",
      "OneMinusDestinationColor",
      "SourceAlpha",
      "OneMinusSourceAlpha",
      "DestinationAlpha",
      "OneMinusDestinationAlpha",
      "ConstantColor",
      "OneMinusConstantColor",
      "ConstantAlpha",
      "OneMinusConstantAlpha",
      "SourceAlphaSaturate"
    };

    /* Get the name of an AlphaBlend as a str */
    static constexpr char const* name (u8_t blend) {
      if (blend < total_blend_count) return names[blend];
      else return "Invalid";
    }

    static constexpr char const* valid_values = (
      "Zero,\n"
      "One,\n"
      "SourceColor,\n"
      "OneMinusSourceColor,\n"
      "DestinationColor,\n"
      "OneMinusDestinationColor,\n"
      "SourceAlpha,\n"
      "OneMinusSourceAlpha,\n"
      "DestinationAlpha,\n"
      "OneMinusDestinationAlpha,\n"
      "ConstantColor,\n"
      "OneMinusConstantColor,\n"
      "ConstantAlpha,\n"
      "OneMinusConstantAlpha,\n"
      "SourceAlphaSaturate\n"
    );

    /* Get an AlphaBlend from its name in str form */
    static u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t blend = 0; blend < total_blend_count; blend ++) {
        if ((max_length == SIZE_MAX || strlen(names[blend]) == max_length) && str_cmp_caseless(name, names[blend], max_length) == 0) return blend;
      }

      return Invalid;
    }

    static constexpr s32_t gl_versions [total_blend_count] = {
      GL_ZERO,
      GL_ONE,
      GL_SRC_COLOR,
      GL_ONE_MINUS_SRC_COLOR,
      GL_DST_COLOR,
      GL_ONE_MINUS_DST_COLOR,
      GL_SRC_ALPHA,
      GL_ONE_MINUS_SRC_ALPHA,
      GL_DST_ALPHA,
      GL_ONE_MINUS_DST_ALPHA,
      GL_CONSTANT_COLOR,
      GL_ONE_MINUS_CONSTANT_COLOR,
      GL_CONSTANT_ALPHA,
      GL_ONE_MINUS_CONSTANT_ALPHA,
      GL_SRC_ALPHA_SATURATE
    };

    /* Get an OpenGL enum from an AlphaBlend.
     * Returns GL_INVALID_ENUM if the AlphaBlend was invalid */
    static constexpr s32_t to_gl (u8_t blend) {
      if (blend < total_blend_count) return gl_versions[blend];
      else return GL_INVALID_ENUM;
    }

    /* Get an AlphaBlend from an OpenGL enum */
    static constexpr u8_t from_gl (s32_t gl_blend) {
      for (u8_t blend = 0; blend < total_blend_count; blend ++) {
        if (gl_versions[blend] == gl_blend) return blend;
      }

      return Invalid;
    }

    /* Determine if a value is a valid AlphaBlend */
    static constexpr bool validate (u8_t blend) {
      return blend < total_blend_count;
    }
  }


  namespace DepthFactor {
    enum: u8_t {
      Always,
      Never,
      Equal,
      NotEqual,
      Lesser,
      LesserOrEqual,
      Greater,
      GreaterOrEqual,

      total_factor_count,

      Invalid = -1
    };

    static constexpr char const* names [total_factor_count] = {
      "Always",
      "Never",
      "Equal",
      "NotEqual",
      "Lesser",
      "LesserOrEqual",
      "Greater",
      "GreaterOrEqual"
    };

    /* Get the name of a DepthFactor as a str */
    static constexpr char const* name (u8_t factor) {
      if (factor < total_factor_count) return names[factor];
      else return "Invalid";
    }

    static constexpr char const* valid_values = (
      "Always,\n"
      "Never,\n"
      "Equal,\n"
      "NotEqual,\n"
      "Lesser,\n"
      "LesserOrEqual,\n"
      "Greater,\n"
      "GreaterOrEqual\n"
    );

    /* Get a DepthFactor from its name in str form */
    static u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t factor = 0; factor < total_factor_count; factor ++) {
        if ((max_length == SIZE_MAX || strlen(names[factor]) == max_length) && str_cmp_caseless(name, names[factor], max_length) == 0) return factor;
      }

      return Invalid;
    }

    static constexpr s32_t gl_versions [total_factor_count] = {
      GL_ALWAYS,
      GL_NEVER,
      GL_EQUAL,
      GL_NOTEQUAL,
      GL_LESS,
      GL_LEQUAL,
      GL_GREATER,
      GL_GEQUAL
    };

    /* Get an OpenGL enum from a DepthFactor.
     * Returns GL_INVALID_ENUM if the DepthFactor was invalid */
    static constexpr s32_t to_gl (u8_t factor) {
      if (factor < total_factor_count) return gl_versions[factor];
      else return GL_INVALID_ENUM;
    }

    /* Get a DepthFactor from an OpenGL enum */
    static constexpr u8_t from_gl (s32_t gl_factor) {
      for (u8_t factor = 0; factor < total_factor_count; factor ++) {
        if (gl_versions[factor] == gl_factor) return factor;
      }

      return Invalid;
    }

    /* Determine if a value is a valid DepthFactor */
    static constexpr bool validate (u8_t factor) {
      return factor < total_factor_count;
    }
  }










  struct Uniform {
    s32_t location = 0;
    u8_t type = UniformType::Invalid;
    union {
      bool Bool;
      f32_t F32;
      f64_t F64;
      s32_t S32;
      u32_t U32;

      ::mod::Vector2f Vector2f;
      ::mod::Vector2d Vector2d;
      ::mod::Vector2s Vector2s;
      ::mod::Vector2u Vector2u;
      ::mod::Vector3f Vector3f;
      ::mod::Vector3d Vector3d;
      ::mod::Vector3s Vector3s;
      ::mod::Vector3u Vector3u;
      ::mod::Vector4f Vector4f;
      ::mod::Vector4d Vector4d;
      ::mod::Vector4s Vector4s;
      ::mod::Vector4u Vector4u;

      ::mod::Matrix3 Matrix3;
      ::mod::Matrix4 Matrix4;

      TextureSlot Texture;
      
      Array<bool> BoolArray;
      Array<f32_t> F32Array;
      Array<f64_t> F64Array;
      Array<s32_t> S32Array;
      Array<u32_t> U32Array;

      Array<::mod::Vector2f> Vector2fArray;
      Array<::mod::Vector2d> Vector2dArray;
      Array<::mod::Vector2s> Vector2sArray;
      Array<::mod::Vector2u> Vector2uArray;
      Array<::mod::Vector3f> Vector3fArray;
      Array<::mod::Vector3d> Vector3dArray;
      Array<::mod::Vector3s> Vector3sArray;
      Array<::mod::Vector3u> Vector3uArray;
      Array<::mod::Vector4f> Vector4fArray;
      Array<::mod::Vector4d> Vector4dArray;
      Array<::mod::Vector4s> Vector4sArray;
      Array<::mod::Vector4u> Vector4uArray;

      Array<::mod::Matrix3> Matrix3Array;
      Array<::mod::Matrix4> Matrix4Array;
    };

    
    /* Create a new zero-initialized Uniform */
    Uniform ()
    : Matrix4 { }
    { }

    /* Create a new Uniform and initialize its location and value
     * If the value is an array, creates a copy (Does not take ownership) */
    template <typename T> Uniform (s32_t in_location, T const& value)
    : location(in_location)
    {
      static constexpr u8_t t_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(t_type), "Cannot use Uniform functions with invalid types");

      type = t_type;

      if constexpr (UniformType::validate_array(t_type)) get_array<T>().copy(value);
      else get<T>() = value;
    }

    /* Create a new Uniform and initialize its location and value
     * If the value is an array, takes ownership */
    template <typename T> static Uniform from_ex (s32_t in_location, Array<T> value) {
      static constexpr u8_t t_type = UniformType::from_type<T>();

      Uniform uniform;

      static_assert(UniformType::validate(t_type), "Cannot use Uniform functions with invalid types");

      uniform.location = in_location;
      uniform.type = t_type;
      
      uniform.get_array<T>() = value;

      return uniform;
    }
    
    /* Create a new Uniform from a JSONItem */
    ENGINE_API static Uniform from_json_item (ShaderProgram const& program, JSONItem& item);

    /* Get the value of a value Uniform */
    template <typename T> T& get () const {
      static constexpr u8_t t_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(t_type), "Cannot use Uniform functions with invalid types");

      m_assert(
            t_type == type
        || (t_type == UniformType::S32 && type == UniformType::Texture)
        || (t_type == UniformType::Texture && type == UniformType::S32),
        "Cannot get Uniform of type %s as %s",
        UniformType::name(type), UniformType::name(t_type)
      );

      if constexpr (t_type == UniformType::Bool) return const_cast<T&>(Bool);
      else if constexpr (t_type == UniformType::F32) return const_cast<T&>(F32);
      else if constexpr (t_type == UniformType::F64) return const_cast<T&>(F64);
      else if constexpr (t_type == UniformType::S32) return const_cast<T&>(S32);
      else if constexpr (t_type == UniformType::U32) return const_cast<T&>(U32);

      else if constexpr (t_type == UniformType::Vector2f) return const_cast<T&>(Vector2f);
      else if constexpr (t_type == UniformType::Vector2d) return const_cast<T&>(Vector2d);
      else if constexpr (t_type == UniformType::Vector2s) return const_cast<T&>(Vector2s);
      else if constexpr (t_type == UniformType::Vector2u) return const_cast<T&>(Vector2u);
      else if constexpr (t_type == UniformType::Vector3f) return const_cast<T&>(Vector3f);
      else if constexpr (t_type == UniformType::Vector3d) return const_cast<T&>(Vector3d);
      else if constexpr (t_type == UniformType::Vector3s) return const_cast<T&>(Vector3s);
      else if constexpr (t_type == UniformType::Vector3u) return const_cast<T&>(Vector3u);
      else if constexpr (t_type == UniformType::Vector4f) return const_cast<T&>(Vector4f);
      else if constexpr (t_type == UniformType::Vector4d) return const_cast<T&>(Vector4d);
      else if constexpr (t_type == UniformType::Vector4s) return const_cast<T&>(Vector4s);
      else if constexpr (t_type == UniformType::Vector4u) return const_cast<T&>(Vector4u);

      else if constexpr (t_type == UniformType::Matrix3) return const_cast<T&>(Matrix3);
      else if constexpr (t_type == UniformType::Matrix4) return const_cast<T&>(Matrix4);

      else if constexpr (t_type == UniformType::Texture) return const_cast<T&>(Texture);
      else m_error("Unknown error occurred");
    }

    /* Clean up a Uniform's heap allocation if it has one */
    void destroy () {
      // this works because all array element pointers are at the same offset
      if (UniformType::validate_array(type)) BoolArray.destroy();
    }

    /* Set a value Uniform */
    template <typename T> void set (T const& value) {
      get<T>() = value;
    }

    /* Get the array of an array Uniform */
    template <typename T> Array<T>& get_array () const {
      static constexpr u8_t t_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(t_type), "Cannot use Uniform functions with invalid types");

      m_assert(UniformType::validate_array(type), "Cannot use get_element on non-array UniformType %s", UniformType::name(type));

      u8_t element_type = UniformType::element_type(type);

      m_assert(
        t_type == element_type,
        "Cannot get Uniform element type %s as %s",
        UniformType::name(element_type), UniformType::name(t_type)
      );

      if constexpr (t_type == UniformType::BoolArray) return const_cast<Array<T>&>(BoolArray);
      else if constexpr (t_type == UniformType::F32Array) return const_cast<Array<T>&>(F32Array);
      else if constexpr (t_type == UniformType::F64Array) return const_cast<Array<T>&>(F64Array);
      else if constexpr (t_type == UniformType::S32Array) return const_cast<Array<T>&>(S32Array);
      else if constexpr (t_type == UniformType::U32Array) return const_cast<Array<T>&>(U32Array);

      else if constexpr (t_type == UniformType::Vector2fArray) return const_cast<Array<T>&>(Vector2fArray);
      else if constexpr (t_type == UniformType::Vector2dArray) return const_cast<Array<T>&>(Vector2dArray);
      else if constexpr (t_type == UniformType::Vector2sArray) return const_cast<Array<T>&>(Vector2sArray);
      else if constexpr (t_type == UniformType::Vector2uArray) return const_cast<Array<T>&>(Vector2uArray);
      else if constexpr (t_type == UniformType::Vector3fArray) return const_cast<Array<T>&>(Vector3fArray);
      else if constexpr (t_type == UniformType::Vector3dArray) return const_cast<Array<T>&>(Vector3dArray);
      else if constexpr (t_type == UniformType::Vector3sArray) return const_cast<Array<T>&>(Vector3sArray);
      else if constexpr (t_type == UniformType::Vector3uArray) return const_cast<Array<T>&>(Vector3uArray);
      else if constexpr (t_type == UniformType::Vector4fArray) return const_cast<Array<T>&>(Vector4fArray);
      else if constexpr (t_type == UniformType::Vector4dArray) return const_cast<Array<T>&>(Vector4dArray);
      else if constexpr (t_type == UniformType::Vector4sArray) return const_cast<Array<T>&>(Vector4sArray);
      else if constexpr (t_type == UniformType::Vector4uArray) return const_cast<Array<T>&>(Vector4uArray);

      else if constexpr (t_type == UniformType::Matrix3Array) return const_cast<Array<T>&>(Matrix3Array);
      else if constexpr (t_type == UniformType::Matrix4Array) return const_cast<Array<T>&>(Matrix4Array);
      else m_error("Unknown error occurred");
    }

    /* Copy the elements of some array into an array Uniform, overwriting the existing elements */
    template <typename T> void set_array (Array<T> const& arr) {
      get_array<T>().copy(arr);
    }

    /* Get an element of an array Uniform */
    template <typename T> T& get_element (size_t index) const {
      return get_array<T>()[index];
    }

    /* Set an element of an array Uniform */
    template <typename T> void set_element (size_t index, T const& value) {
      get_array<T>()[index] = value;
    }

    /* Append an element to an array Uniform */
    template <typename T> void append_element (T const& value) {
      get_array<T>().append(value);
    }

    /* Insert an element at an index in an array Uniform */
    template <typename T> void insert_element (size_t index, T const& value) {
      get_array<T>().insert(index, value);
    }

    /* Remove an element at an index in an array Uniform */
    template <typename T> void remove_element (size_t index) {
      get_array<T>().remove(index);
    }

    /* Clear an array Uniform */
    template <typename T> void clear_array () {
      get_array<T>().clear();
    }

    /* Apply a Uniform to a ShaderProgram */
    void apply (ShaderProgram const& shader_program) const {
      switch (type) {
        case UniformType::Bool: return shader_program.set_uniform(location, Bool);
        case UniformType::F32: return shader_program.set_uniform(location, F32);
        case UniformType::F64: return shader_program.set_uniform(location, F64);
        case UniformType::S32: return shader_program.set_uniform(location, S32);
        case UniformType::U32: return shader_program.set_uniform(location, U32);

        case UniformType::Vector2f: return shader_program.set_uniform(location, Vector2f);
        case UniformType::Vector2d: return shader_program.set_uniform(location, Vector2d);
        case UniformType::Vector2s: return shader_program.set_uniform(location, Vector2s);
        case UniformType::Vector2u: return shader_program.set_uniform(location, Vector2u);
        case UniformType::Vector3f: return shader_program.set_uniform(location, Vector3f);
        case UniformType::Vector3d: return shader_program.set_uniform(location, Vector3d);
        case UniformType::Vector3s: return shader_program.set_uniform(location, Vector3s);
        case UniformType::Vector3u: return shader_program.set_uniform(location, Vector3u);
        case UniformType::Vector4f: return shader_program.set_uniform(location, Vector4f);
        case UniformType::Vector4d: return shader_program.set_uniform(location, Vector4d);
        case UniformType::Vector4s: return shader_program.set_uniform(location, Vector4s);
        case UniformType::Vector4u: return shader_program.set_uniform(location, Vector4u);

        case UniformType::Matrix3: return shader_program.set_uniform(location, Matrix3);
        case UniformType::Matrix4: return shader_program.set_uniform(location, Matrix4);

        case UniformType::Texture: return shader_program.set_uniform<s32_t>(location, Texture);

        case UniformType::BoolArray: return shader_program.set_uniform(location, BoolArray);
        case UniformType::F32Array: return shader_program.set_uniform(location, F32Array);
        case UniformType::F64Array: return shader_program.set_uniform(location, F64Array);
        case UniformType::S32Array: return shader_program.set_uniform(location, S32Array);
        case UniformType::U32Array: return shader_program.set_uniform(location, U32Array);

        case UniformType::Vector2fArray: return shader_program.set_uniform(location, Vector2fArray);
        case UniformType::Vector2dArray: return shader_program.set_uniform(location, Vector2dArray);
        case UniformType::Vector2sArray: return shader_program.set_uniform(location, Vector2sArray);
        case UniformType::Vector2uArray: return shader_program.set_uniform(location, Vector2uArray);
        case UniformType::Vector3fArray: return shader_program.set_uniform(location, Vector3fArray);
        case UniformType::Vector3dArray: return shader_program.set_uniform(location, Vector3dArray);
        case UniformType::Vector3sArray: return shader_program.set_uniform(location, Vector3sArray);
        case UniformType::Vector3uArray: return shader_program.set_uniform(location, Vector3uArray);
        case UniformType::Vector4fArray: return shader_program.set_uniform(location, Vector4fArray);
        case UniformType::Vector4dArray: return shader_program.set_uniform(location, Vector4dArray);
        case UniformType::Vector4sArray: return shader_program.set_uniform(location, Vector4sArray);
        case UniformType::Vector4uArray: return shader_program.set_uniform(location, Vector4uArray);

        case UniformType::Matrix3Array: return shader_program.set_uniform(location, Matrix3Array);
        case UniformType::Matrix4Array: return shader_program.set_uniform(location, Matrix4Array);

        default: m_error("Cannot set Uniform: Invalid type %" PRIu8, type);
      }
    }
  };






  struct TextureUnit {
    s32_t location;
    TextureHandle texture;

    /* Create a new uinitialized TextureUnit */
    TextureUnit () { }

    /* Create a new TextureUnit and initialize its values */
    TextureUnit (s32_t in_location, Texture* in_texture)
    : location(in_location)
    {
      texture = in_texture;
    }

    /* Create a new TextureUnit and initialize its values */
    TextureUnit (s32_t in_location, TextureHandle const& in_texture)
    : location(in_location)
    , texture(in_texture)
    { }

    /* Create a new TextureUnit from a JSONItem */
    ENGINE_API static TextureUnit from_json_item (JSONItem& item);

    /* Apply a TextureUnit to the OpenGL context */
    void apply () const {
      texture->use(location);
    }
  };






  struct FaceCullingSetting {
    bool enabled = true;
    u8_t side = FaceCullingSide::Back;
    u8_t winding = VertexWinding::CounterClockwise;

    bool validate () const {
      return !enabled
          || (FaceCullingSide::validate(side) && VertexWinding::validate(winding));
    }

    ENGINE_API void apply (Material const& material) const;
  };

  struct AlphaBlendingSetting {
    bool enabled = false;
    u8_t source_factor = AlphaBlend::SourceAlpha;
    u8_t destination_factor = AlphaBlend::OneMinusSourceAlpha;

    bool validate () const {
      return !enabled
          || (AlphaBlend::validate(source_factor) && AlphaBlend::validate(destination_factor));
    }

    ENGINE_API void apply (Material const& material) const;
  };

  struct DepthSetting {
    bool enabled = true;
    u8_t factor = DepthFactor::Lesser;

    bool validate () const {
      return !enabled
          || DepthFactor::validate(factor);
    }

    ENGINE_API void apply (Material const& material) const;
  };





  struct Material {
    char* origin;
    u32_t asset_id = 0;

    ShaderProgramHandle shader_program;

    FaceCullingSetting face_culling;
    AlphaBlendingSetting alpha_blending;
    DepthSetting depth;

    bool enable_skinning = false;
    bool enable_wireframe = false;

    Array<Uniform> uniforms;
    Array<TextureUnit> textures;

    
    /* Create a new uninitialized Material */
    Material () { }

    /* Create a new Material and initialize its data */
    ENGINE_API Material (
      char const* in_origin,
      ShaderProgramHandle in_shader_program,
      FaceCullingSetting const& in_face_culling = { },
      AlphaBlendingSetting const& in_alpha_blending = { },
      DepthSetting const& in_depth = { },
      bool in_enable_skinning = false,
      bool in_enable_wireframe = false
    );


    /* Create a new Material from a JSONItem */
    ENGINE_API static Material from_json_item (char const* origin, JSONItem const& json);

    /* Create a new Material from JSON */
    static Material from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new Material from a source str */
    ENGINE_API static Material from_str (char const* origin, char const* source);

    /* Create a new Material from a source file */
    ENGINE_API static Material from_file (char const* origin);


    /* Clean up a Material's heap allocations */
    ENGINE_API void destroy ();


    /* Determine whether a Material has a value for a Uniform, by location */
    bool has_uniform (s32_t location) const {
      return get_uniform_index(location) != -1;
    }

    /* Determine whether a Material has a value for a Uniform, by name */
    bool has_uniform (char const* name) const {
      return get_uniform_index(name) != -1;
    }

    /* Determine whether a Material's ShaderProgram can support a Uniform at the given location */
    bool supports_uniform (s32_t location) const {
      return shader_program->has_uniform(location);
    }

    /* Determine whether a Material's ShaderProgram can support a Uniform with the given name */
    bool supports_uniform (char const* name) const {
      return shader_program->has_uniform(name);
    }

    /* Get the index of a Material's Uniform inside its Uniform array, by location.
     * Returns -1 if no Uniform is bound to the given location in the Material */
    ENGINE_API s64_t get_uniform_index (s32_t location) const;

    /* Get the index of a Material's Uniform inside its Uniform array, by name.
     * Returns -1 if no Uniform is bound to the given name in the Material */
    ENGINE_API s64_t get_uniform_index (char const* name) const;

    /* Get a pointer to a Material's Uniform by location.
     * Returns NULL if no Uniform is bound to the given location in the Material */
    ENGINE_API Uniform* get_uniform_pointer (s32_t location) const;

    /* Get a Material's Uniform by name.
     * Returns NULL if no Uniform is found with the given name,
     * or if no Uniform is bound to location associated with the name, in the Material */
    ENGINE_API Uniform* get_uniform_pointer (char const* name) const;


    /* Get a Material's Uniform by location.
     * Panics if no Uniform is bound to the given location in the Material */
    ENGINE_API Uniform& get_uniform (s32_t location) const;

    /* Get a Material's Uniform by name.
     * Panics if no Uniform is found with the given name,
     * or no Uniform is bound to location associated with the name, in the Material */
    ENGINE_API Uniform& get_uniform (char const* name) const;



    /* Apply a Material's settings to its ShaderProgram */
    ENGINE_API void apply_settings () const;

    /* Apply a Material's uniforms to its ShaderProgram */
    ENGINE_API void apply_uniforms (ShaderProgram const& program_ref) const;

    /* Apply a Material's textures to its ShaderProgram */
    ENGINE_API void apply_textures () const;


    /* Apply a Material's settings and data to its ShaderProgram,
     * and bind it to the OpenGL context */
    ENGINE_API void use () const;


    /* Get a Material's Uniform value by location.
     * Panics if no Uniform is bound to the given location in the Material,
     * or if the Uniform is not a value type, or on type mismatch */
    template <typename T> T& get_uniform_value (s32_t location) const {
      return get_uniform(location).get<T>();
    }
    
    /* Get a Material's Uniform value by name.
     * Panics if no Uniform is bound to the given name in the Material,
     * or if the Uniform is not a value type, or on type mismatch */
    template <typename T> T& get_uniform_value (char const* name) const {
      return get_uniform(name).get<T>();
    }

    
    /* Get a Material's Uniform array by location.
     * Panics if no Uniform is bound to the given location in the Material,
     * or if the Uniform is not a array type, or on type mismatch */
    template <typename T> T& get_uniform_array (s32_t location) const {
      return get_uniform(location).get_array<T>();
    }
    
    /* Get a Material's Uniform array by name.
     * Panics if no Uniform is bound to the given name in the Material,
     * or if the Uniform is not a array type, or on type mismatch */
    template <typename T> T& get_uniform_array (char const* name) const {
      return get_uniform(name).get_array<T>();
    }


    /* Set a Material's value Uniform by location.
     * Creates a new Uniform in the Material if one does not exist.
     * Panics if the type is mismatched to an existing Uniform,
     * or if the Material's ShaderProgram doesn't have a Uniform with the given location */
    template <typename T> void set_uniform (s32_t location, T const& value) {
      static constexpr u8_t uniform_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(uniform_type) && !UniformType::validate_array(uniform_type), "Material::set_uniform requires a value with a valid Uniform value type (Use set_uniform_array for array types)");

      Uniform* existing_uniform = get_uniform_pointer(location);

      if (existing_uniform != NULL) {
        existing_uniform->set(value);
      } else {
        m_assert(supports_uniform(location), "The ShaderProgram (with origin '%s') does not have a uniform at the given location %" PRId32, shader_program->origin, location);
        uniforms.append({ location, value });
      }
    }

    /* Set a Material's value Uniform by location.
     * Creates a new Uniform in the Material if one does not exist.
     * Panics if the type is mismatched to an existing Uniform,
     * or if the Material's ShaderProgram does not have a Uniform at the given name */
    template <typename T> void set_uniform (char const* name, T const& value) {
      ShaderProgram const& program_ref = *shader_program;

      s32_t location = program_ref.get_uniform_location(name);

      m_assert(
        location != -1,
        "The ShaderProgram (with origin '%s') does not have an active Uniform with the name '%s'",
        program_ref.origin, name
      );

      return set_uniform(location, value);
    }
    

    /* Copy new values into a Material's array Uniform by location.
     * Overwrites existing values in the array, if one exists.
     * Creates a new Uniform in the Material if one does not exist.
     * Panics if the element type is mismatched to an existing Uniform,
     * or if the Material's ShaderProgram doesn't have a Uniform with the given location */
    template <typename T> void set_uniform_array (s32_t location, Array<T> const& arr) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "Material::set_uniform_array requires a value with a valid Uniform array type (Use set_uniform for value types)");

      Uniform* existing_uniform = get_uniform_pointer(location);

      if (existing_uniform != NULL) {
        existing_uniform->set_array(arr);
      } else {
        m_assert(supports_uniform(location), "The ShaderProgram (with origin '%s') does not have a uniform at the given location %" PRId32, shader_program->origin, location);
        uniforms.append({ uniform })
      }
    }
    

    /* Copy new values into a Material's array Uniform by name.
     * Overwrites existing values in the array, if one exists.
     * Creates a new Uniform in the Material if one does not exist.
     * Panics if the element type is mismatched to an existing Uniform,
     * or if the Material's ShaderProgram does not have a Uniform at the given name */
    template <typename T> void set_uniform_array (char const* name, Array<T> const& arr) {
      ShaderProgram const& program_ref = *shader_program;

      s32_t location = program_ref.get_uniform_location(name);

      m_assert(
        location != -1,
        "The ShaderProgram (with origin '%s') does not have an active Uniform with the name '%s'",
        program_ref.origin, name
      );

      return set_uniform_array(location, arr);
    }


    /* Overwrite an element of a Material's array Uniform
     * Panics if no array Uniform exists at the given location,
     * or if the element index is out of range,
     * or on type mismatch */
    template <typename T> void set_uniform_element (s32_t location, size_t index, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "Material::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).set_element(index, value);
    }

    /* Append an element to the end of a Material's array Uniform
     * Panics if no array Uniform exists at the given location,
     * or on type mismatch */
    template <typename T> void append_uniform_element (s32_t location, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "Material::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).append_element(value);
    }

    /* Insert an element into a Material's array Uniform at the given index
     * Panics if no array Uniform exists at the given location,
     * or on type mismatch */
    template <typename T> void insert_uniform_element (s32_t location, size_t index, T const& value) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "Material::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).insert_element(index, value);
    }

    /* Remove an element from a Material's array Uniform at the given index
     * Panics if no array Uniform exists at the given location,
     * or on type mismatch */
    template <typename T> void remove_uniform_element (s32_t location, size_t index) {
      static constexpr u8_t element_type = UniformType::from_type<T>();

      static_assert(UniformType::validate(element_type) && !UniformType::validate_array(element_type), "Material::set_uniform_element requires a value with a valid Uniform array type (Use set_uniform for value types)");

      get_uniform(location).remove_element<T>(index);
    }


    /* Remove a Uniform from a Material by location.
     * Not recommended under most circumstances */
    void unset_uniform (s32_t location) {
      s64_t index = get_uniform_index(location);

      if (index != -1) {
        uniforms[index].destroy();
        uniforms.remove(index);
      }
    }



    /* Get the index of a Material's TextureUnit inside its TextureUnit array, by location.
     * Returns -1 if no TextureUnit is bound to the given location in the Material */
    ENGINE_API s64_t get_texture_index (s32_t location) const;

    /* Get a pointer to a Material's TextureUnit by location.
     * Returns NULL if no TextureUnit is bound to the given location in the Material */
    ENGINE_API TextureUnit* get_texture_pointer (s32_t location) const;

    /* Get a Material's TextureUnit by location.
     * Panics if no TextureUnit is bound to the given location in the Material */
    ENGINE_API TextureUnit& get_texture (s32_t location) const;

    /* Get a Material's TextureUnit value by location.
     * Panics if no TextureUnit is bound to the given location in the Material */
    TextureHandle& get_texture_value (s32_t location) const {
      return get_texture(location).texture;
    }

    /* Set a Material's TextureUnit value by location.
     * Creates a new TextureUnit if none is bound to the given location in the Material,
     * or overwrites the existing value */
    ENGINE_API void set_texture (s32_t location, TextureHandle const& value);

    /* Remove a TextureUnit from a Material by location.
     * Not recommended under most circumstances */
    ENGINE_API void unset_texture (s32_t location);



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


  using MaterialHandle = AssetHandle<Material>;
}

#endif