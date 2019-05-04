#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  Uniform Uniform::from_json_item (ShaderProgram const& program, JSONItem& item) {
    static const auto extract_value = [] (u8_t value_type, JSONItem& value_item, void* out) -> void {
      switch (value_type) {
        case UniformType::Bool: {
          *reinterpret_cast<bool*>(out) = value_item.get_boolean();
        } break;

        case UniformType::F32: {
          *reinterpret_cast<f32_t*>(out) = value_item.get_number();
        } break;

        case UniformType::F64: {
          *reinterpret_cast<f64_t*>(out) = value_item.get_number();
        } break;

        case UniformType::S32: {
          *reinterpret_cast<s32_t*>(out) = value_item.get_number();
        } break;

        case UniformType::U32: {
          *reinterpret_cast<u32_t*>(out) = value_item.get_number();
        } break;


        case UniformType::Vector2f: {
          for (size_t i = 0; i < 2; i ++) {
            reinterpret_cast<::mod::Vector2f*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;
        
        case UniformType::Vector2d: {
          for (size_t i = 0; i < 2; i ++) {
            reinterpret_cast<::mod::Vector2d*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector2s: {
          for (size_t i = 0; i < 2; i ++) {
            reinterpret_cast<::mod::Vector2s*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector2u: {
          for (size_t i = 0; i < 2; i ++) {
            reinterpret_cast<::mod::Vector2u*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;


        case UniformType::Vector3f: {
          for (size_t i = 0; i < 3; i ++) {
            reinterpret_cast<::mod::Vector3f*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;
        
        case UniformType::Vector3d: {
          for (size_t i = 0; i < 3; i ++) {
            reinterpret_cast<::mod::Vector3d*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector3s: {
          for (size_t i = 0; i < 3; i ++) {
            reinterpret_cast<::mod::Vector3s*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector3u: {
          for (size_t i = 0; i < 3; i ++) {
            reinterpret_cast<::mod::Vector3u*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;


        case UniformType::Vector4f: {
          for (size_t i = 0; i < 4; i ++) {
            reinterpret_cast<::mod::Vector4f*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;
        
        case UniformType::Vector4d: {
          for (size_t i = 0; i < 4; i ++) {
            reinterpret_cast<::mod::Vector4d*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector4s: {
          for (size_t i = 0; i < 4; i ++) {
            reinterpret_cast<::mod::Vector4s*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Vector4u: {
          for (size_t i = 0; i < 4; i ++) {
            reinterpret_cast<::mod::Vector4u*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;
        

        case UniformType::Matrix3: {
          for (size_t i = 0; i < 9; i ++) {
            reinterpret_cast<::mod::Matrix3*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;

        case UniformType::Matrix4: {
          for (size_t i = 0; i < 16; i ++) {
            reinterpret_cast<::mod::Matrix4*>(out)->elements[i] = value_item.get_array_number(i);
          }
        } break;


        case UniformType::Texture: {
          *reinterpret_cast<TextureSlot*>(out) = value_item.get_number();
        } break;


        default: value_item.asset_error("Expected a single value UniformType not %s", UniformType::name(value_type));
      }
    };
    

    String& type_name = item.get_object_string("type");

    u8_t type = UniformType::from_name(type_name.value);

    if (type == UniformType::Invalid) {
      item.get_object_item("type")->asset_error(
        "'%s' is not a valid UniformType name. Valid values are:\n%s\n",
        type_name.value,
        UniformType::valid_values
      );
    }

    
    JSONItem* location_item = item.get_object_item("location");

    s32_t location;

    if (location_item != NULL) {
      location = location_item->get_number();
    } else {
      String& name = item.get_object_string("name");

      location = program.get_uniform_location(name.value);

      if (location == -1) {
        item.get_object_item("name")->asset_error(
          "The designated ShaderProgram (with origin '%s') has no active uniform named '%s'",
          program.origin, name.value
        );
      }
    }

    
    JSONItem* value_item = item.get_object_item("value");

    item.asset_assert(value_item != NULL, "Uniforms require a default value");


    u8_t element_value [sizeof(Matrix4)];

    if (UniformType::validate_array(type)) {
      JSONArray& value_arr = value_item->get_array();
      u8_t element_type = UniformType::element_type(type);
      s64_t element_size = UniformType::size(element_type);

      Array<u8_t> data = { element_size * value_arr.count };

      for (size_t i = 0; i < value_arr.count; i ++) {
        extract_value(element_type, value_arr[i], element_value);

        data.append_multiple(element_value, element_size);
      }

      switch (type) {
        case UniformType::BoolArray: return Uniform::from_ex(location, Array<bool>  { reinterpret_cast<bool*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::F32Array:  return Uniform::from_ex(location, Array<f32_t> { reinterpret_cast<f32_t*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::F64Array:  return Uniform::from_ex(location, Array<f64_t> { reinterpret_cast<f64_t*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::S32Array:  return Uniform::from_ex(location, Array<s32_t> { reinterpret_cast<s32_t*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::U32Array:  return Uniform::from_ex(location, Array<u32_t> { reinterpret_cast<u32_t*>(data.elements), data.count / element_size, data.capacity / element_size });

        case UniformType::Vector2fArray: return Uniform::from_ex(location, Array<::mod::Vector2f> { reinterpret_cast<::mod::Vector2f*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector2dArray: return Uniform::from_ex(location, Array<::mod::Vector2d> { reinterpret_cast<::mod::Vector2d*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector2sArray: return Uniform::from_ex(location, Array<::mod::Vector2s> { reinterpret_cast<::mod::Vector2s*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector2uArray: return Uniform::from_ex(location, Array<::mod::Vector2u> { reinterpret_cast<::mod::Vector2u*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector3fArray: return Uniform::from_ex(location, Array<::mod::Vector3f> { reinterpret_cast<::mod::Vector3f*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector3dArray: return Uniform::from_ex(location, Array<::mod::Vector3d> { reinterpret_cast<::mod::Vector3d*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector3sArray: return Uniform::from_ex(location, Array<::mod::Vector3s> { reinterpret_cast<::mod::Vector3s*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector3uArray: return Uniform::from_ex(location, Array<::mod::Vector3u> { reinterpret_cast<::mod::Vector3u*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector4fArray: return Uniform::from_ex(location, Array<::mod::Vector4f> { reinterpret_cast<::mod::Vector4f*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector4dArray: return Uniform::from_ex(location, Array<::mod::Vector4d> { reinterpret_cast<::mod::Vector4d*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector4sArray: return Uniform::from_ex(location, Array<::mod::Vector4s> { reinterpret_cast<::mod::Vector4s*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Vector4uArray: return Uniform::from_ex(location, Array<::mod::Vector4u> { reinterpret_cast<::mod::Vector4u*>(data.elements), data.count / element_size, data.capacity / element_size });

        case UniformType::Matrix3Array: return Uniform::from_ex(location, Array<::mod::Matrix3> { reinterpret_cast<::mod::Matrix3*>(data.elements), data.count / element_size, data.capacity / element_size });
        case UniformType::Matrix4Array: return Uniform::from_ex(location, Array<::mod::Matrix4> { reinterpret_cast<::mod::Matrix4*>(data.elements), data.count / element_size, data.capacity / element_size });

        default: {
          data.destroy();
          value_item->asset_error("Unknown error occurred with element type");
        }
      }
    } else {
      extract_value(type, *value_item, element_value);

      switch (type) {
        case UniformType::Bool: return Uniform { location, *reinterpret_cast<bool*>(element_value) };
        case UniformType::F32:  return Uniform { location, *reinterpret_cast<f32_t*>(element_value) };
        case UniformType::F64:  return Uniform { location, *reinterpret_cast<f64_t*>(element_value) };
        case UniformType::S32:  return Uniform { location, *reinterpret_cast<s32_t*>(element_value) };
        case UniformType::U32:  return Uniform { location, *reinterpret_cast<u32_t*>(element_value) };

        case UniformType::Vector2f: return Uniform { location, *reinterpret_cast<::mod::Vector2f*>(element_value) };
        case UniformType::Vector2d: return Uniform { location, *reinterpret_cast<::mod::Vector2d*>(element_value) };
        case UniformType::Vector2s: return Uniform { location, *reinterpret_cast<::mod::Vector2s*>(element_value) };
        case UniformType::Vector2u: return Uniform { location, *reinterpret_cast<::mod::Vector2u*>(element_value) };
        case UniformType::Vector3f: return Uniform { location, *reinterpret_cast<::mod::Vector3f*>(element_value) };
        case UniformType::Vector3d: return Uniform { location, *reinterpret_cast<::mod::Vector3d*>(element_value) };
        case UniformType::Vector3s: return Uniform { location, *reinterpret_cast<::mod::Vector3s*>(element_value) };
        case UniformType::Vector3u: return Uniform { location, *reinterpret_cast<::mod::Vector3u*>(element_value) };
        case UniformType::Vector4f: return Uniform { location, *reinterpret_cast<::mod::Vector4f*>(element_value) };
        case UniformType::Vector4d: return Uniform { location, *reinterpret_cast<::mod::Vector4d*>(element_value) };
        case UniformType::Vector4s: return Uniform { location, *reinterpret_cast<::mod::Vector4s*>(element_value) };
        case UniformType::Vector4u: return Uniform { location, *reinterpret_cast<::mod::Vector4u*>(element_value) };

        case UniformType::Matrix3: return Uniform { location, *reinterpret_cast<::mod::Matrix3*>(element_value) };
        case UniformType::Matrix4: return Uniform { location, *reinterpret_cast<::mod::Matrix4*>(element_value) };

        case UniformType::Texture: return Uniform { location, *reinterpret_cast<TextureSlot*>(element_value) };

        default: value_item->asset_error("Unknown error occurred with element type");
      }
    }
  }



  TextureUnit TextureUnit::from_json_item (JSONItem& item) {
    s32_t location = item.get_object_number("location");

    String& name = item.get_object_string("name");

    Texture* texture = AssetManager.get_pointer_from_name<Texture>(name.value);

    if (texture == NULL) {
      item.get_object_item("name")->asset_error(
        "No Texture named '%s' has been loaded",
        name.value
      );
    }

    return TextureUnit { location, texture };
  }



  

  void FaceCullingSetting::apply (Material const& material) const {
    material.asset_assert(validate(), "Cannot apply invalid FaceCullingSetting");

    if (enabled) {
      glEnable(GL_CULL_FACE);
      glCullFace(FaceCullingSide::to_gl(side));
      glFrontFace(VertexWinding::to_gl(winding));
    } else {
      glDisable(GL_CULL_FACE);
    }
  }


  void AlphaBlendingSetting::apply (Material const& material) const {
    material.asset_assert(validate(), "Cannot apply invalid AlphaBlendingSetting");

    if (enabled) {
      glEnable(GL_BLEND);
      glBlendFunc(AlphaBlend::to_gl(source_factor), AlphaBlend::to_gl(destination_factor));
    } else {
      glDisable(GL_BLEND);
    }
  }


  void DepthSetting::apply (Material const& material) const {
    material.asset_assert(validate(), "Cannot apply invalid DepthSetting");

    if (enabled) {
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(DepthFactor::to_gl(factor));
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }



  Material::Material (
    char const* in_origin,
    ShaderProgramHandle in_shader_program,
    FaceCullingSetting const& in_face_culling,
    AlphaBlendingSetting const& in_alpha_blending,
    DepthSetting const& in_depth,
    bool in_enable_skinning,
    bool in_enable_wireframe
  )
  : origin(str_clone(in_origin))
  , shader_program(in_shader_program)
  , face_culling(in_face_culling)
  , alpha_blending(in_alpha_blending)
  , depth(in_depth)
  , enable_skinning(in_enable_skinning)
  , enable_wireframe(in_enable_wireframe)
  {
    asset_assert_terminal(shader_program.valid, "Invalid ShaderProgram");
    asset_assert_terminal(face_culling.validate(), "Invalid FaceCullingSetting");
    asset_assert_terminal(alpha_blending.validate(), "Invalid AlphaBlendingSetting");
    asset_assert_terminal(depth.validate(), "Invalid DepthSetting");
  }


  Material Material::from_json_item (char const* origin, JSONItem const& json) {
    String& shader_program_name = json.get_object_string("shader_program");

    ShaderProgram* shader_program = AssetManager.get_pointer_from_name<ShaderProgram>(shader_program_name.value);

    if (shader_program == NULL) {
      json.get_object_item("shader_program")->asset_error(
        "No ShaderProgram with the name '%s' has been loaded",
        shader_program_name.value
      );
    }


    FaceCullingSetting face_culling;

    JSONItem* face_culling_item = json.get_object_item("face_culling");
    if (face_culling_item != NULL) {
      face_culling.enabled = true;

      String& side_s = face_culling_item->get_object_string("side");
      face_culling.side = FaceCullingSide::from_name(side_s.value);

      if (!FaceCullingSide::validate(face_culling.side)) {
        face_culling_item->get_object_item("side")->asset_error(
          "'%s' is not a valid FaceCullingSide value. Valid values are:\n%s",
          side_s.value, FaceCullingSide::valid_values
        );
      }

      String& winding_s = face_culling_item->get_object_string("winding");
      face_culling.winding = VertexWinding::from_name(winding_s.value);

      if (!VertexWinding::validate(face_culling.winding)) {
        face_culling_item->get_object_item("winding")->asset_error(
          "'%s' is not a valid VertexWinding value. Valid values are:\n%s",
          winding_s.value, VertexWinding::valid_values
        );
      }
    } else {
      face_culling.enabled = false;
    }


    AlphaBlendingSetting alpha_blending;

    JSONItem* alpha_blending_item = json.get_object_item("alpha_blending");
    if (alpha_blending_item != NULL) {
      alpha_blending.enabled = true;

      String& source_factor_s = alpha_blending_item->get_object_string("source_factor");
      alpha_blending.source_factor = AlphaBlend::from_name(source_factor_s.value);

      if (!AlphaBlend::validate(alpha_blending.source_factor)) {
        alpha_blending_item->get_object_item("source_factor")->asset_error(
          "'%s' is not a valid AlphaBlend value. Valid values are:\n%s",
          source_factor_s.value, AlphaBlend::valid_values
        );
      }

      String& destination_factor_s = alpha_blending_item->get_object_string("destination_factor");
      alpha_blending.destination_factor = AlphaBlend::from_name(destination_factor_s.value);

      if (!AlphaBlend::validate(alpha_blending.destination_factor)) {
        alpha_blending_item->get_object_item("destination_factor")->asset_error(
          "'%s' is not a valid AlphaBlend value. Valid values are:\n%s",
          destination_factor_s.value, AlphaBlend::valid_values
        );
      }
    } else {
      alpha_blending.enabled = false;
    }


    DepthSetting depth;

    JSONItem* depth_item = json.get_object_item("depth");
    if (depth_item != NULL) {
      depth.enabled = true;

      String& factor_s = depth_item->get_string();
      depth.factor = DepthFactor::from_name(factor_s.value);

      if (!DepthFactor::validate(depth.factor)) {
        depth_item->asset_error(
          "'%s' is not a valid DepthFactor value. Valid values are:\n%s",
          factor_s.value, DepthFactor::valid_values
        );
      }
    } else {
      depth.enabled = false;
    }


    bool enable_skinning;

    JSONItem* enable_skinning_item = json.get_object_item("enable_skinning");

    if (enable_skinning_item != NULL) {
      enable_skinning = enable_skinning_item->get_boolean();
    } else {
      enable_skinning = false;
    }


    bool enable_wireframe;

    JSONItem* enable_wireframe_item = json.get_object_item("enable_wireframe");

    if (enable_wireframe_item != NULL) {
      enable_wireframe = enable_wireframe_item->get_boolean();
    } else {
      enable_wireframe = false;
    }


    Material material = {
      origin,
      shader_program,
      face_culling,
      alpha_blending,
      depth,
      enable_skinning,
      enable_wireframe
    };


    try {
      JSONItem* uniforms_item = json.get_object_item("uniforms");

      if (uniforms_item != NULL) {
        for (auto [ i, uniform_item ] : uniforms_item->get_array()) {
          Uniform uniform = Uniform::from_json_item(*shader_program, uniform_item);
          material.uniforms.append(uniform);
        }
      }


      JSONItem* textures_item = json.get_object_item("textures");

      if (textures_item != NULL) {
        for (auto [ i, texture_item ] : textures_item->get_array()) {
          TextureUnit texture = TextureUnit::from_json_item(texture_item);
          material.textures.append(texture);
        }
      }
    } catch (Exception& exception) {
      material.destroy();
      throw exception;
    }


    return material;
  }

  Material Material::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    Material material;

    try {
      material = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return material;
  }

  Material Material::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load Material: Unable to read file"
    );

    Material material;

    try {
      material = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);

    return material;
  }


  void Material::destroy () {
    for (auto [ i, uniform ] : uniforms) uniform.destroy();
    uniforms.destroy();
    textures.destroy();
  }



  void Material::apply_settings () const {
    face_culling.apply(*this);
    alpha_blending.apply(*this);
    depth.apply(*this);

    if (enable_wireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  void Material::apply_uniforms (ShaderProgram const& program_ref) const {
    for (auto [ i, uniform ] : uniforms) uniform.apply(program_ref);
  }
  
  void Material::apply_textures () const {
    for (auto [ i, texture ] : textures) texture.apply();
  }

  void Material::use () const {
    ShaderProgram const& program_ref = *shader_program;

    program_ref.use();

    apply_settings();
    apply_uniforms(program_ref);
    apply_textures();
  }



  s64_t Material::get_uniform_index (s32_t location) const {
    for (auto [ i, uniform ] : uniforms) {
      if (uniform.location == location) return i;
    }

    return -1;
  }

  s64_t Material::get_uniform_index (char const* name) const {
    ShaderProgram const& program_ref = *shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_index(location);
    else return -1;
  }


  Uniform* Material::get_uniform_pointer (s32_t location) const {
    for (auto [ i, uniform ] : uniforms) {
      if (uniform.location == location) return &uniform;
    }

    return NULL;
  }

  Uniform* Material::get_uniform_pointer (char const* name) const {
    ShaderProgram const& program_ref = *shader_program;

    s32_t location = program_ref.get_uniform_location(name);
    
    if (location != -1) return get_uniform_pointer(location);
    else return NULL;
  }

  Uniform& Material::get_uniform (s32_t location) const {
    Uniform* ptr = get_uniform_pointer(location);

    m_assert(ptr != NULL, "There is no Uniform bound to location %" PRId32, location);

    return *ptr;
  }

  Uniform& Material::get_uniform (char const* name) const {
    Uniform* ptr = get_uniform_pointer(name);

    m_assert(ptr != NULL, "There is no Uniform bound to name %s" PRId32, name);

    return *ptr;
  }

  

  s64_t Material::get_texture_index (s32_t location) const {
    for (auto [ i, texture ] : textures) {
      if (texture.location == location) return i;
    }

    return -1;
  }


  TextureUnit* Material::get_texture_pointer (s32_t location) const {
    for (auto [ i, texture ] : textures) {
      if (texture.location == location) return &texture;
    }

    return NULL;
  }

  TextureUnit& Material::get_texture (s32_t location) const {
    TextureUnit* ptr = get_texture_pointer(location);

    m_assert(ptr != NULL, "There is no Uniform bound to location %" PRId32, location);

    return *ptr;
  }

  void Material::set_texture (s32_t location, TextureHandle const& value) {
    TextureUnit* existing_texture_unit = get_texture_pointer(location);

    if (existing_texture_unit) {
      existing_texture_unit->texture = value;
    } else {
      textures.append({ location, value });
    }
  }

  void Material::unset_texture (s32_t location) {
    s64_t index = get_texture_index(location);

    if (index != -1) textures.remove(index);
  }
}