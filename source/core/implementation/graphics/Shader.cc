#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  Shader::Shader (char const* in_origin, u8_t in_type, char const* source)
  : origin(str_clone(in_origin))
  , type(in_type)
  {
    asset_assert_terminal(
      ShaderType::validate(type),
      "Cannot create Shader with invalid type %" PRIu8,
      type
    );

    gl_id = glCreateShader(ShaderType::to_gl(type));

    glShaderSource(gl_id, 1, const_cast<char const* const*>(&source), NULL);

    glCompileShader(gl_id);

    s32_t success;
    char info_log [1024];
    glGetShaderiv(gl_id, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(gl_id, 1024, NULL, info_log);
      asset_error_terminal(
        "Failed to compile %s Shader source\nGL Shader info log:\n%s",
        ShaderType::name(type), info_log
      );
    }
  }

  Shader Shader::from_file (char const* origin) {
    u8_t type = ShaderType::from_file_ext(origin);

    m_asset_assert(
      ShaderType::validate(type),
      origin,
      "Failed to load Shader source: File extension not recognized. Known file extensions are:\n%s",
      ShaderType::known_file_exts
    );

    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load %s Shader: Unable to read file",
      ShaderType::name(type)
    );

    Shader shader;

    try {
      shader = { origin, type, static_cast<char*>(source) };
    } catch (Exception& exception) {
      memory::deallocate(source);
      throw exception;
    }

    memory::deallocate(source);

    return shader;
  }


  void Shader::destroy () {
    if (origin != NULL) memory::deallocate(origin);
    
    if (gl_id != 0) {
      glDeleteShader(gl_id);
      gl_id = 0;
    }
  }
}