#include "../include/util.hh"


namespace mod {
  pair_t<void*, size_t> load_file (char const* path) {
    if (path == NULL) return { NULL, 0 };

    FILE* f = fopen(path, "rb");

    if (f == NULL) return { NULL, 0 };

    auto data = (u8_t*) malloc(FILE_READ_CHUNK_SIZE + 1);

    m_assert(data != NULL, "Out of memory or other null pointer error while allocating memory for load_file with path '%s' at base chunk size %zu", path, FILE_READ_CHUNK_SIZE + 1);

    size_t offset = 0;
    size_t i = 1;
    size_t n;

    do {
      size_t total_size = FILE_READ_CHUNK_SIZE * i + 1;

      data = (u8_t*) realloc(data, total_size);

      m_assert(data != NULL, "Out of memory or other null pointer error while reallocating memory for load_file with path '%s' with buffer size %zu", path, total_size);

      n = fread(data + offset, 1, FILE_READ_CHUNK_SIZE, f);
      offset += n;
      ++ i;
    } while (n == FILE_READ_CHUNK_SIZE);

    fclose(f);

    size_t final_size = offset + 1;

    data = (u8_t*) realloc(data, final_size);

    m_assert(data != NULL, "Out of memory or other null pointer error while performing final reallocation of memory for load_file with path '%s' with buffer size %zu", path, final_size);

    // if (is_text) data[offset] = 0;

    return { data, offset };
  }


  bool save_file (char const* path, void const* data, size_t size) {
    if (path == NULL || data == NULL || size == 0) return false;
    
    FILE* f = fopen(path, "wb");

    if (f == NULL) return false;

    size_t n = fwrite(data, 1, size, f);
    m_assert(n == size, "Failed to write full data to file, had size of %zu, but wrote %zu", size, n);

    fclose(f);

    return true;
  }


  bool file_exists (char const* path) {
    struct stat file_stats;
    return (stat(path, &file_stats) == 0);
  }


  s64_t str_dir_parent_length (char const* path, size_t max_len) {
    if (max_len == 0) max_len = SIZE_MAX;

    s64_t last_sep = -1;
    size_t i = 0;

    while (path[i] != '\0' && i < max_len) {
      if (path[i] == '\\' || path[i] == '/') last_sep = i;
      ++ i;
    }

    return last_sep;
  }

  s64_t str_dir_traverse_back (char const* path, size_t back, size_t max_len) {
    s64_t parent_len = max_len;

    for (size_t i = 0; i < back; i ++) {
      parent_len = str_dir_parent_length(path, parent_len);
      if (parent_len == -1) break;
    }

    return parent_len;
  }

  size_t str_dir_count_back_paths (char const* path, size_t max_len) {
    if (max_len == 0) max_len = SIZE_MAX;

    size_t i = 0;
    size_t count = 0;

    while (path[i] != '\0' && i < max_len) {
      if (path[i] == '.' && path[i + 1] == '.' && (path[i + 2] == '\\' || path[i + 2] == '/')) {
        i += 3;
        ++ count;
      } else {
        break;
      }
    }

    return count;
  }

  bool str_dir_relativize_path (char const* base_path, char const* relative_path, char* out, size_t max_length) {
    size_t back = str_dir_count_back_paths(relative_path, 0);
    size_t rel_total_len = strlen(relative_path);
    size_t back_len = back * 3;

    s64_t base_back_len = str_dir_traverse_back(base_path, back + 1, 0);

    if (base_back_len == -1) return false;

    ++ base_back_len;

    if (back == 0
    && (str_starts_with(relative_path, "./", 2) || str_starts_with(relative_path, ".\\",  2))) {
      back_len = 2;
    }

    size_t rel_len = rel_total_len - back_len;
    size_t t_len = base_back_len + rel_len;

    if (t_len > max_length) return false;

    memcpy(out, base_path, base_back_len);
    memcpy(out + base_back_len, relative_path + back_len, rel_len + 1);

    return true;
  }


  s64_t str_file_extension (char const* str, size_t max_length) {
    if (max_length == 0) max_length = SIZE_MAX;

    s64_t dot = -1;
    size_t i = 0;
    
    while (str[i] != '\0' && i < max_length) {
      if (str[i] == '.') dot = i;
      else if (str[i] == '\\' || str[i] == '/') dot = -1;
      
      ++ i;
    }

    return dot;
  }


  bool str_starts_with (char const* str, char const* start, size_t max_length) {
    if (max_length == 0) max_length = SIZE_MAX;

    size_t i = 0;
    while (start[i] != '\0' && i < max_length) {
      if (str[i] != start[i]) return false;
      ++ i;
    }

    return true;
  }

  bool str_ends_with (char const* str, char const* end, size_t max_length) {
    if (max_length == 0) max_length = strlen(end);

    size_t str_len = strlen(str);

    if (max_length > str_len) return false;

    for (size_t i = 0; i < max_length; i ++) {
      if (str[str_len - max_length + i] != end[i]) return false;
    }

    return true;
  }

  s32_t str_cmp_caseless (char const* str1, char const* str2, size_t num) {
    s32_t ret_code = -9999;

    size_t i = 0;

    if (str1 == NULL || str2== NULL) goto done;

    while ((*str1 || *str2) && (i < num)) {
      ret_code = tolower((s32_t)(*str1)) - tolower((s32_t)(*str2));

      if (ret_code != 0) break;

      ++ i;
      
      ++ str1;
      ++ str2;
    }

    done: return ret_code;
  }
}



char const* glGetTypeName (u32_t type) {
  switch (type) {
    case GL_FLOAT: return	"float";
    case GL_FLOAT_VEC2: return	"vec2";
    case GL_FLOAT_VEC3: return	"vec3";
    case GL_FLOAT_VEC4: return	"vec4";
    case GL_DOUBLE: return	"double";
    case GL_DOUBLE_VEC2: return	"dvec2";
    case GL_DOUBLE_VEC3: return	"dvec3";
    case GL_DOUBLE_VEC4: return	"dvec4";
    case GL_INT: return	"int";
    case GL_INT_VEC2: return	"ivec2";
    case GL_INT_VEC3: return	"ivec3";
    case GL_INT_VEC4: return	"ivec4";
    case GL_UNSIGNED_INT: return	"unsigned int";
    case GL_UNSIGNED_INT_VEC2: return	"uvec2";
    case GL_UNSIGNED_INT_VEC3: return	"uvec3";
    case GL_UNSIGNED_INT_VEC4: return	"uvec4";
    case GL_BOOL: return	"bool";
    case GL_BOOL_VEC2: return	"bvec2";
    case GL_BOOL_VEC3: return	"bvec3";
    case GL_BOOL_VEC4: return	"bvec4";
    case GL_FLOAT_MAT2: return	"mat2";
    case GL_FLOAT_MAT3: return	"mat3";
    case GL_FLOAT_MAT4: return	"mat4";
    case GL_FLOAT_MAT2x3: return	"mat2x3";
    case GL_FLOAT_MAT2x4: return	"mat2x4";
    case GL_FLOAT_MAT3x2: return	"mat3x2";
    case GL_FLOAT_MAT3x4: return	"mat3x4";
    case GL_FLOAT_MAT4x2: return	"mat4x2";
    case GL_FLOAT_MAT4x3: return	"mat4x3";
    case GL_DOUBLE_MAT2: return	"dmat2";
    case GL_DOUBLE_MAT3: return	"dmat3";
    case GL_DOUBLE_MAT4: return	"dmat4";
    case GL_DOUBLE_MAT2x3: return	"dmat2x3";
    case GL_DOUBLE_MAT2x4: return	"dmat2x4";
    case GL_DOUBLE_MAT3x2: return	"dmat3x2";
    case GL_DOUBLE_MAT3x4: return	"dmat3x4";
    case GL_DOUBLE_MAT4x2: return	"dmat4x2";
    case GL_DOUBLE_MAT4x3: return	"dmat4x3";
    case GL_SAMPLER_1D: return	"sampler1D";
    case GL_SAMPLER_2D: return	"sampler2D";
    case GL_SAMPLER_3D: return	"sampler3D";
    case GL_SAMPLER_CUBE: return	"samplerCube";
    case GL_SAMPLER_1D_SHADOW: return	"sampler1DShadow";
    case GL_SAMPLER_2D_SHADOW: return	"sampler2DShadow";
    case GL_SAMPLER_1D_ARRAY: return	"sampler1DArray";
    case GL_SAMPLER_2D_ARRAY: return	"sampler2DArray";
    case GL_SAMPLER_1D_ARRAY_SHADOW: return	"sampler1DArrayShadow";
    case GL_SAMPLER_2D_ARRAY_SHADOW: return	"sampler2DArrayShadow";
    case GL_SAMPLER_2D_MULTISAMPLE: return	"sampler2DMS";
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return	"sampler2DMSArray";
    case GL_SAMPLER_CUBE_SHADOW: return	"samplerCubeShadow";
    case GL_SAMPLER_BUFFER: return	"samplerBuffer";
    case GL_SAMPLER_2D_RECT: return	"sampler2DRect";
    case GL_SAMPLER_2D_RECT_SHADOW: return	"sampler2DRectShadow";
    case GL_INT_SAMPLER_1D: return	"isampler1D";
    case GL_INT_SAMPLER_2D: return	"isampler2D";
    case GL_INT_SAMPLER_3D: return	"isampler3D";
    case GL_INT_SAMPLER_CUBE: return	"isamplerCube";
    case GL_INT_SAMPLER_1D_ARRAY: return	"isampler1DArray";
    case GL_INT_SAMPLER_2D_ARRAY: return	"isampler2DArray";
    case GL_INT_SAMPLER_2D_MULTISAMPLE: return	"isampler2DMS";
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return	"isampler2DMSArray";
    case GL_INT_SAMPLER_BUFFER: return	"isamplerBuffer";
    case GL_INT_SAMPLER_2D_RECT: return	"isampler2DRect";
    case GL_UNSIGNED_INT_SAMPLER_1D: return	"usampler1D";
    case GL_UNSIGNED_INT_SAMPLER_2D: return	"usampler2D";
    case GL_UNSIGNED_INT_SAMPLER_3D: return	"usampler3D";
    case GL_UNSIGNED_INT_SAMPLER_CUBE: return	"usamplerCube";
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return	"usampler2DArray";
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return	"usampler2DArray";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return	"usampler2DMS";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return	"usampler2DMSArray";
    case GL_UNSIGNED_INT_SAMPLER_BUFFER: return	"usamplerBuffer";
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return	"usampler2DRect";
    case GL_IMAGE_1D: return	"image1D";
    case GL_IMAGE_2D: return	"image2D";
    case GL_IMAGE_3D: return	"image3D";
    case GL_IMAGE_2D_RECT: return	"image2DRect";
    case GL_IMAGE_CUBE: return	"imageCube";
    case GL_IMAGE_BUFFER: return	"imageBuffer";
    case GL_IMAGE_1D_ARRAY: return	"image1DArray";
    case GL_IMAGE_2D_ARRAY: return	"image2DArray";
    case GL_IMAGE_2D_MULTISAMPLE: return	"image2DMS";
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return	"image2DMSArray";
    case GL_INT_IMAGE_1D: return	"iimage1D";
    case GL_INT_IMAGE_2D: return	"iimage2D";
    case GL_INT_IMAGE_3D: return	"iimage3D";
    case GL_INT_IMAGE_2D_RECT: return	"iimage2DRect";
    case GL_INT_IMAGE_CUBE: return	"iimageCube";
    case GL_INT_IMAGE_BUFFER: return	"iimageBuffer";
    case GL_INT_IMAGE_1D_ARRAY: return	"iimage1DArray";
    case GL_INT_IMAGE_2D_ARRAY: return	"iimage2DArray";
    case GL_INT_IMAGE_2D_MULTISAMPLE: return	"iimage2DMS";
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return	"iimage2DMSArray";
    case GL_UNSIGNED_INT_IMAGE_1D: return	"uimage1D";
    case GL_UNSIGNED_INT_IMAGE_2D: return	"uimage2D";
    case GL_UNSIGNED_INT_IMAGE_3D: return	"uimage3D";
    case GL_UNSIGNED_INT_IMAGE_2D_RECT: return	"uimage2DRect";
    case GL_UNSIGNED_INT_IMAGE_CUBE: return	"uimageCube";
    case GL_UNSIGNED_INT_IMAGE_BUFFER: return	"uimageBuffer";
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return	"uimage1DArray";
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return	"uimage2DArray";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return	"uimage2DMS";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return	"uimage2DMSArray";
    case GL_UNSIGNED_INT_ATOMIC_COUNTER: return	"atomic_uint";
    default: return "invalid";
  }
}



s64_t glBlendFactorFromStr (char const* value) {
  if (strcmp("ZERO", value) == 0 || strcmp("zero", value) == 0) return GL_ZERO;
  else if (strcmp("ONE", value) == 0 || strcmp("one", value) == 0) return GL_ONE;
  else if (strcmp("SRC_COLOR", value) == 0 || strcmp("src_color", value) == 0) return GL_SRC_COLOR;
  else if (strcmp("ONE_MINUS_SRC_COLOR", value) == 0 || strcmp("one_minus_src_color", value) == 0) return GL_ONE_MINUS_SRC_COLOR;
  else if (strcmp("DST_COLOR", value) == 0 || strcmp("dst_color", value) == 0) return GL_DST_COLOR;
  else if (strcmp("ONE_MINUS_DST_COLOR", value) == 0 || strcmp("one_minus_dst_color", value) == 0) return GL_ONE_MINUS_DST_COLOR;
  else if (strcmp("SRC_ALPHA", value) == 0 || strcmp("src_alpha", value) == 0) return GL_SRC_ALPHA;
  else if (strcmp("ONE_MINUS_SRC_ALPHA", value) == 0 || strcmp("one_minus_src_alpha", value) == 0) return GL_ONE_MINUS_SRC_ALPHA;
  else if (strcmp("DST_ALPHA", value) == 0 || strcmp("dst_alpha", value) == 0) return GL_DST_ALPHA;
  else if (strcmp("ONE_MINUS_DST_ALPHA", value) == 0 || strcmp("one_minus_dst_alpha", value) == 0) return GL_ONE_MINUS_DST_ALPHA;
  else if (strcmp("CONSTANT_COLOR", value) == 0 || strcmp("constant_color", value) == 0) return GL_CONSTANT_COLOR;
  else if (strcmp("ONE_MINUS_CONSTANT_COLOR", value) == 0 || strcmp("one_minus_constant_color", value) == 0) return GL_ONE_MINUS_CONSTANT_COLOR;
  else if (strcmp("CONSTANT_ALPHA", value) == 0 || strcmp("constant_alpha", value) == 0) return GL_CONSTANT_ALPHA;
  else if (strcmp("ONE_MINUS_CONSTANT_ALPHA", value) == 0 || strcmp("one_minus_constant_alpha", value) == 0) return GL_ONE_MINUS_CONSTANT_ALPHA;
  else if (strcmp("SRC_ALPHA_SATURATE", value) == 0 || strcmp("src_alpha_sature", value) == 0) return GL_SRC_ALPHA_SATURATE;
  else return -1;
}

s64_t glDepthFactorFromStr (char const* value) {
  if (strcmp("NEVER", value) == 0 || strcmp("never", value) == 0) return GL_NEVER;
  else if (strcmp("LESS", value) == 0 || strcmp("less", value) == 0) return GL_LESS;
  else if (strcmp("EQUAL", value) == 0 || strcmp("equal", value) == 0) return GL_EQUAL;
  else if (strcmp("LEQUAL", value) == 0 || strcmp("lequal", value) == 0) return GL_LEQUAL;
  else if (strcmp("GREATER", value) == 0 || strcmp("greater", value) == 0) return GL_GREATER;
  else if (strcmp("NOTEQUAL", value) == 0 || strcmp("notequal", value) == 0) return GL_NOTEQUAL;
  else if (strcmp("GEQUAL", value) == 0 || strcmp("gequal", value) == 0) return GL_GEQUAL;
  else if (strcmp("ALWAYS", value) == 0 || strcmp("always", value) == 0) return GL_ALWAYS;
  else return -1;
}

s64_t glFaceFromStr (char const* value) {
  if (strcmp("FRONT", value) == 0 || strcmp("front", value) == 0) return GL_FRONT;
  else if (strcmp("BACK", value) == 0 || strcmp("back", value) == 0) return GL_BACK;
  else if (strcmp("FRONT_AND_BACK", value) == 0 || strcmp("front_and_back", value) == 0) return GL_FRONT_AND_BACK;
  else return -1;
}

s64_t glWindFromStr (char const* value) {
  if (strcmp("CW", value) == 0 || strcmp("cw", value) == 0) return GL_CW;
  else if (strcmp("CCW", value) == 0 || strcmp("ccw", value) == 0) return GL_CCW;
  else return -1;
}

s64_t glWrapFromStr (char const* value) {
  if (strcmp("CLAMP_TO_EDGE", value) == 0 || strcmp("clamp_to_edge", value) == 0) return GL_CLAMP_TO_EDGE;
  else if (strcmp("MIRRORED_REPEAT", value) == 0 || strcmp("mirrored_repeat", value) == 0) return GL_MIRRORED_REPEAT;
  else if (strcmp("REPEAT", value) == 0 || strcmp("repeat", value) == 0) return GL_REPEAT;
  else return -1;
}

s64_t glFilterFromStr (char const* value) {
  if (strcmp("NEAREST", value) == 0 || strcmp("nearest", value) == 0) return GL_NEAREST;
  else if (strcmp("LINEAR", value) == 0 || strcmp("linear", value) == 0) return GL_LINEAR;
  else if (strcmp("NEAREST_MIPMAP_NEAREST", value) == 0 || strcmp("nearest_mipmap_nearest", value) == 0) return GL_NEAREST_MIPMAP_NEAREST;
  else if (strcmp("LINEAR_MIPMAP_NEAREST", value) == 0 || strcmp("linear_mipmap_nearest", value) == 0) return GL_LINEAR_MIPMAP_NEAREST;
  else if (strcmp("NEAREST_MIPMAP_LINEAR", value) == 0 || strcmp("nearest_mipmap_linear", value) == 0) return GL_NEAREST_MIPMAP_LINEAR;
  else if (strcmp("LINEAR_MIPMAP_LINEAR", value) == 0 || strcmp("linear_mipmap_linear", value) == 0) return GL_LINEAR_MIPMAP_LINEAR;
  else return -1;
}