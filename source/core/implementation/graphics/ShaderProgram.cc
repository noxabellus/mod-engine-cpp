#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  ShaderProgram::ShaderProgram (
    char const* origin,

    ShaderHandle vertex,
    ShaderHandle fragment,
    ShaderHandle tesselation_control,
    ShaderHandle tesselation_evaluation,
    ShaderHandle geometry,
    ShaderHandle compute
  )
  : origin(str_clone(origin))
  {
    gl_id = glCreateProgram();

    if (vertex.valid) {
      Shader& ref = *vertex;

      asset_assert_terminal(
        ref.type == ShaderType::Vertex,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a Vertex Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      vertex_shader = vertex;
    }

    if (fragment.valid) {
      Shader& ref = *fragment;

      asset_assert_terminal(
        ref.type == ShaderType::Fragment,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a Fragment Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      fragment_shader = fragment;
    }

    if (tesselation_control.valid) {
      Shader& ref = *tesselation_control;

      asset_assert_terminal(
        ref.type == ShaderType::TesselationControl,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a TesselationControl Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      tesselation_control_shader = tesselation_control;
    }

    if (tesselation_evaluation.valid) {
      Shader& ref = *tesselation_evaluation;

      asset_assert_terminal(
        ref.type == ShaderType::TesselationEvaluation,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a TesselationEvaluation Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      tesselation_evaluation_shader = tesselation_evaluation;
    }

    if (geometry.valid) {
      Shader& ref = *geometry;

      asset_assert_terminal(
        ref.type == ShaderType::Geometry,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a Geometry Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      geometry_shader = geometry;
    }

    if (compute.valid) {
      Shader& ref = *compute;

      asset_assert_terminal(
        ref.type == ShaderType::Compute,
        "Cannot attach Shader with origin '%s' to ShaderProgram as a Compute Shader, it is a %s Shader",
        ref.origin, ShaderType::name(ref.type)
      );

      glAttachShader(gl_id, ref.gl_id);

      compute_shader = compute;
    }

    glLinkProgram(gl_id);

    s32_t success;
    char info_log [1024];
    glGetProgramiv(gl_id, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(gl_id, 1024, NULL, info_log);
      asset_error_terminal("Failed to link ShaderProgram\nGL Program info log:\n%s", info_log);
    }
  }


  ShaderProgram ShaderProgram::from_json_item (char const* origin, JSONItem const& json) {
    JSONItem* vertex_item = json.get_object_item("vertex");
    JSONItem* fragment_item = json.get_object_item("fragment");
    JSONItem* tesselation_control_item = json.get_object_item("tesselation_control");
    JSONItem* tesselation_evaluation_item = json.get_object_item("tesselation_evaluation");
    JSONItem* geometry_item = json.get_object_item("geometry");
    JSONItem* compute_item = json.get_object_item("compute");

    return {
      origin,
      
      vertex_item != NULL? vertex_item->get_string().value : NULL,
      fragment_item != NULL? fragment_item->get_string().value : NULL,
      tesselation_control_item != NULL? tesselation_control_item->get_string().value : NULL,
      tesselation_evaluation_item != NULL? tesselation_evaluation_item->get_string().value : NULL,
      geometry_item != NULL? geometry_item->get_string().value : NULL,
      compute_item != NULL? compute_item->get_string().value : NULL
    };
  }

  ShaderProgram ShaderProgram::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    ShaderProgram program;

    try {
      program = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return program;
  }

  ShaderProgram ShaderProgram::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load ShaderProgram: Unable to read file"
    );

    ShaderProgram program;

    try {
      program = from_str(origin, (char*) source);
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);

    return program;
  }


  void ShaderProgram::destroy () {
    if (origin != NULL) {
      free(origin);
      origin = NULL;
    }
    
    if (gl_id != 0) {
      glDeleteProgram(gl_id);
      gl_id = 0;
    }

    uniform_info.destroy();
  }


  Array<UniformInfo> const& ShaderProgram::get_uniform_info () {
    if (uniform_info.elements == NULL) {
      uniform_info = { 16 };

      s32_t count;
      glGetProgramiv(gl_id, GL_ACTIVE_UNIFORMS, &count);

      for (s32_t i = 0; i < count; i ++) {
        UniformInfo info;

        glGetActiveUniform(gl_id, i, UniformInfo::max_name_length, NULL, &info.size, &info.type, info.name);

        info.location = get_uniform_location(info.name);

        uniform_info.append(info);
      }
    }

    return uniform_info;
  }

  void ShaderProgram::dump_uniform_info () {
    Array<UniformInfo> info = get_uniform_info();

    printf(
      "ShaderProgram Info:\n"
      "Origin: %s\n"
      "Active uniform count: %zu\n"
      "Active uniforms:\n",
      origin, info.count
    );

    for (auto [ i, uniform ] : info) {
      printf(
        "'%s' @ %" PRId32 " : %s x %" PRId32 "\n",
        uniform.name,
        uniform.location,
        glGetTypeName(uniform.type),
        uniform.size
      );
    }

    info.destroy();
  }
}