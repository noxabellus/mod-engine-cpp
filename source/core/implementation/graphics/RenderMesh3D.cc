#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  inline void init_gl_data (RenderMesh3D* mesh) {
    s32_t draw_arg = mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    // TODO switch to DSA style (See dsa_example below)

    using namespace Mesh3DAttribute;

    glGenVertexArrays(1, &mesh->gl_vao);
    glBindVertexArray(mesh->gl_vao);

    glGenBuffers(total_attribute_count, mesh->gl_vbos);


    // Setup positions buffer & attrib, copy any data to gl, enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Position]);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), reinterpret_cast<void*>(0));
    glBufferData(GL_ARRAY_BUFFER, mesh->positions.count * sizeof(Vector3f), mesh->positions.elements, draw_arg);
    glEnableVertexAttribArray(Position);

    
    // Setup normals buffer & attrib, copy any data to gl, enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Normal]);
    glVertexAttribPointer(Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), reinterpret_cast<void*>(0));
    glBufferData(GL_ARRAY_BUFFER, mesh->normals.count * sizeof(Vector3f), mesh->normals.elements, draw_arg);
    glEnableVertexAttribArray(Normal);

    // Setup UVs attrib, do not copy data or enable (not sure binding is needed)
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[UV]);
    glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), reinterpret_cast<void*>(0));

    // Setup colors attrib, do not copy data or enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Color]);
    glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), reinterpret_cast<void*>(0));

    // Setup skin indices attrib, do not copy data or enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[SkinIndices]);
    glVertexAttribIPointer(SkinIndices, 4, GL_UNSIGNED_INT, sizeof(Vector4u), reinterpret_cast<void*>(0));

    // Setup skin weights attrib, do not copy data or enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[SkinWeights]);
    glVertexAttribPointer(SkinWeights, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), reinterpret_cast<void*>(0));


    // Setup faces and copy data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_vbos[Face]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faces.count * sizeof(Vector3u), mesh->faces.elements, draw_arg);

    // Clear binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void init_gl_uvs (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Mesh3DAttribute::UV]);
    glBufferData(GL_ARRAY_BUFFER, mesh->uvs.count * sizeof(Vector2f), mesh->uvs.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(Mesh3DAttribute::UV);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void init_gl_colors (RenderMesh3D* mesh) {
    // TODO switch to DSA style
    
    glBindVertexArray(mesh->gl_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Mesh3DAttribute::Color]);
    glBufferData(GL_ARRAY_BUFFER, mesh->colors.count * sizeof(Vector3f), mesh->colors.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(Mesh3DAttribute::Color);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void init_gl_skinning (RenderMesh3D* mesh) {
    // TODO switch to DSA style
    
    glBindVertexArray(mesh->gl_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Mesh3DAttribute::SkinIndices]);
    glBufferData(GL_ARRAY_BUFFER, mesh->skin_indices.count * sizeof(Vector4u), mesh->skin_indices.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(Mesh3DAttribute::SkinIndices);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[Mesh3DAttribute::SkinWeights]);
    glBufferData(GL_ARRAY_BUFFER, mesh->skin_weights.count * sizeof(Vector4f), mesh->skin_weights.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(Mesh3DAttribute::SkinWeights);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void disable_gl_uvs (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);

    glDisableVertexAttribArray(Mesh3DAttribute::UV);

    glBindVertexArray(0);
  }

  inline void disable_gl_colors (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);

    glDisableVertexAttribArray(Mesh3DAttribute::Color);

    glBindVertexArray(0);
  }

  inline void disable_gl_skinning (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);

    glDisableVertexAttribArray(Mesh3DAttribute::SkinIndices);
    glDisableVertexAttribArray(Mesh3DAttribute::SkinWeights);

    glBindVertexArray(0);
  }

  // void dsa_example () {
  //   ///////////////////// OLD //////////////////////
  //   glGenBuffers(1, &buffer);
  //   glBindBuffer(GL_ARRAY_BUFFER, buffer);
  //   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //   glGenVertexArrays(1, &vao);
  //   glBindVertexArray(vao);
  //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  //   glEnableVertexAttribArray(0);
  //   glBindBuffer(GL_ARRAY_BUFFER, 0);
  //   glBindVertexArray(0);

  //   ///////////////////// NEW //////////////////////
  //   glCreateBuffers(1, &buffer);
  //   glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);

  //   glCreateVertexArrays(1, &vao);
  //   glEnableVertexArrayAttrib(vao, 0);
  //   glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

  //   glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(GLfloat) * 3);
  //   glVertexArrayAttribBinding(vao, 0, 0);
  // }


  RenderMesh3D::RenderMesh3D (char const* in_origin, bool in_dynamic)
  : origin(str_clone(in_origin))
  , dynamic(in_dynamic)
  {
    init_gl_data(this);
  }


  RenderMesh3D::RenderMesh3D (
    char const* in_origin,

    bool in_dynamic,

    size_t vertex_count,
    Vector3f const* in_positions,
    Vector3f const* in_normals,

    Vector2f const* in_uvs,
    Vector3f const* in_colors,

    Vector4u const* in_skin_indices,
    Vector4f const* in_skin_weights,

    size_t face_count,
    Vector3u const* in_faces,

    MaterialConfig const& in_material_config
  )
  : origin(str_clone(in_origin))
  , dynamic(in_dynamic)
  {
    positions.append_multiple(in_positions, vertex_count);
    
    recalculate_bounds();

    faces.append_multiple(in_faces, face_count);    
    
    if (in_normals != NULL) normals.append_multiple(in_normals, vertex_count);
    else calculate_normals();

    init_gl_data(this);

    if (in_uvs != NULL) enable_uvs(in_uvs);
    if (in_colors != NULL) enable_colors(in_colors);
    if (in_skin_indices != NULL || in_skin_weights != NULL) enable_skinning(in_skin_indices, in_skin_weights);

    material_config = in_material_config;
  }


  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    size_t vertex_count,
    Vector3f* positions,
    Vector3f* normals,

    Vector2f* uvs,
    Vector3f* colors,
    
    Vector4u* skin_indices,
    Vector4f* skin_weights,
    
    size_t face_count,
    Vector3u* faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;

    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = Array<Vector3f>::from_ex(positions, vertex_count);

    mesh.recalculate_bounds();

    mesh.faces = Array<Vector3u>::from_ex(faces, face_count);

    if (normals != NULL) mesh.normals = Array<Vector3f>::from_ex(normals, vertex_count);
    else mesh.calculate_normals();

    init_gl_data(&mesh);

    if (uvs != NULL) mesh.enable_uvs_ex(uvs);
    if (colors != NULL) mesh.enable_colors_ex(colors);
    if (skin_indices != NULL || skin_weights != NULL) mesh.enable_skinning_ex(skin_indices, skin_weights);

    mesh.material_config = material_config;

    return mesh;
  }


  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    Array<Vector3f> const& positions,
    Array<Vector3f> const& normals,

    Array<Vector2f> const& uvs,
    Array<Vector3f> const& colors,

    Array<Vector4u> const& skin_indices,
    Array<Vector4f> const& skin_weights,
    
    Array<Vector3u> const& faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;
    
    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = positions;
    mesh.recalculate_bounds();
    mesh.normals = normals;
    mesh.faces = faces;

    init_gl_data(&mesh);
    
    if (uvs.elements != NULL) mesh.enable_uvs_ex(uvs);
    if (colors.elements != NULL) mesh.enable_colors_ex(colors);

    if (skin_indices.elements != NULL || skin_weights.elements != NULL) mesh.enable_skinning_ex(skin_indices, skin_weights);

    mesh.material_config = material_config;
    
    return mesh;
  }


  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    Array<Vector3f> const& positions,
    Array<Vector3f> const& normals,
    
    Array<Vector3u> const& faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;
    
    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = positions;
    mesh.recalculate_bounds();
    mesh.normals = normals;
    mesh.faces = faces;

    init_gl_data(&mesh);

    mesh.material_config = material_config;
    
    return mesh;
  }



  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    Array<Vector3f> const& positions,
    
    Array<Vector3u> const& faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;
    
    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = positions;
    mesh.recalculate_bounds();
    mesh.faces = faces;
    mesh.calculate_normals();

    init_gl_data(&mesh);

    mesh.material_config = material_config;
    
    return mesh;
  }


  RenderMesh3D RenderMesh3D::from_json_item (const char* origin, JSONItem const& json) {
    Array<f32_t> positions;
    Array<f32_t> normals;

    Array<f32_t> uvs;
    Array<f32_t> colors;

    Array<u32_t> skin_indices;
    Array<f32_t> skin_weights;

    Array<u32_t> faces;

    bool dynamic;

    MaterialConfig material_config;


    try {
      /* Positions */ {
        JSONItem* pos_item = json.get_object_item("positions");

        json.asset_assert(pos_item != NULL, "Expected an array 'positions'");

        JSONArray& pos_arr = pos_item->get_array();

        pos_item->asset_assert(pos_arr.count % 3 == 0, "Number of positions elements must be cleanly divisible by 3");

        for (auto [ i, value ] : pos_arr) positions.append(value.get_number());
      }
    
      /* Faces */ {
        JSONItem* face_item = json.get_object_item("faces");

        json.asset_assert(face_item != NULL, "Expected an array 'faces'");

        JSONArray& face_arr = face_item->get_array();

        face_item->asset_assert(face_arr.count % 3 == 0, "Number of faces elements must be cleanly divisible by 3");
        
        size_t vertex_count = positions.count / 3;

        for (auto [ i, value ] : face_arr) {
          uint32_t index = value.get_number();

          value.asset_assert(
            index < vertex_count,
            "Vertex index %" PRIu32 " is invalid, vertex count is %zu",
            index, vertex_count
          );

          faces.append(index);
        }
      }

      /* Normals */ {
        JSONItem* norm_item = json.get_object_item("normals");

        if (norm_item != NULL) {
          JSONArray& norm_arr = norm_item->get_array();

          norm_item->asset_assert(norm_arr.count == positions.count, "Number of normals elements must be equal to number of positions elements");

          for (auto [ i, value ] : norm_arr) normals.append(value.get_number());
        }
      }

      /* UVs */ {
        JSONItem* uv_item = json.get_object_item("uvs");

        if (uv_item != NULL) {
          JSONArray& uv_arr = uv_item->get_array();

          uv_item->asset_assert(uv_arr.count % 2 == 0, "Number of uvs elements must be cleanly divisible by 2");
          uv_item->asset_assert(uv_arr.count / 2 == positions.count / 3, "Number of uvs elements divided by 2 must be the same as positions elements divided by 3");

          for (auto [ i, value ] : uv_arr) uvs.append(value.get_number());
        }
      }

      /* Colors */ {
        JSONItem* color_item = json.get_object_item("colors");

        if (color_item != NULL) {
          JSONArray& color_arr = color_item->get_array();
          
          color_item->asset_assert(color_arr.count == positions.count, "Number of colors elements must be the same as positions elements");

          for (auto [ i, value ] : color_arr) colors.append(value.get_number());
        }
      }

      /* Skin */ {
        JSONItem* skin_indices_item = json.get_object_item("skin_indices");
        JSONItem* skin_weights_item = json.get_object_item("skin_weights");

        if (skin_indices_item != NULL || skin_weights_item != NULL) {
          json.asset_assert(skin_indices_item != NULL && skin_weights_item != NULL, "If either skin_indices or skin_weights are present, the other must be as well");

          JSONArray& skin_indices_arr = skin_indices_item->get_array();
          JSONArray& skin_weights_arr = skin_weights_item->get_array();

          skin_indices_item->asset_assert(skin_indices_arr.count % 4 == 0, "Number of skin_indices elements must be cleanly divisible by 4");
          skin_indices_item->asset_assert(skin_indices_arr.count / 4 == positions.count / 3, "Number of skin_indices elements divided by 4 must be the same as positions elements divided by 3");
          skin_indices_item->asset_assert(skin_weights_arr.count == skin_indices_arr.count, "Number of skin_weights elements must be the same as skin_indices elements");

          for (auto [ i, index ] : skin_indices_arr) {
            skin_indices.append(index.get_number());
            skin_weights.append(skin_weights_arr[i].get_number());
          }
        }
      }

      /* MaterialConfig */ {
        JSONItem* material_config_item = json.get_object_item("material_config");

        if (material_config_item != NULL) {
          material_config = MaterialConfig::from_json_item(faces.count / 3, *material_config_item);
        }
      }

      /* Dynamic */ {
        JSONItem* dynamic_item = json.get_object_item("dynamic");

        if (dynamic_item != NULL) {
          dynamic = dynamic_item->get_boolean();
        } else {
          dynamic = false;
        }
      }
    } catch (Exception& exception) {
      positions.destroy();
      normals.destroy();
      
      uvs.destroy();
      colors.destroy();

      skin_indices.destroy();
      skin_weights.destroy();

      faces.destroy();

      material_config.destroy();

      throw exception;
    }

    return from_ex(
      origin,

      dynamic,

      positions.count / 3,
      reinterpret_cast<Vector3f*>(positions.elements),
      reinterpret_cast<Vector3f*>(normals.elements),

      reinterpret_cast<Vector2f*>(uvs.elements),
      reinterpret_cast<Vector3f*>(colors.elements),
      
      reinterpret_cast<Vector4u*>(skin_indices.elements),
      reinterpret_cast<Vector4f*>(skin_weights.elements),

      faces.count / 3,
      reinterpret_cast<Vector3u*>(faces.elements),

      material_config
    );
  }


  RenderMesh3D RenderMesh3D::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    RenderMesh3D mesh;

    try {
      mesh = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return mesh;
  }

  RenderMesh3D RenderMesh3D::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load RenderMesh3D: Unable to read file"
    );

    RenderMesh3D mesh;

    try {
      mesh = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      memory::deallocate(source);
      throw exception;
    }

    memory::deallocate(source);

    return mesh;
  }


  void RenderMesh3D::recalculate_bounds () {
    bounds = AABB3::from_vector_list(positions.elements, positions.count);
    needs_update.unset(bounds_flag);
  }

  AABB3 const& RenderMesh3D::get_aabb () {
    if (needs_update.match_index(bounds_flag)) recalculate_bounds();
    return bounds;
  }

  
  void RenderMesh3D::clear () {
    using namespace Mesh3DAttribute;

    positions.clear();
    normals.clear();
    faces.clear();

    needs_update.set_multiple(Position, bounds_flag, Normal, Face);

    if (uvs.elements != NULL) {
      uvs.clear();
      needs_update.set(UV);
    }

    if (colors.elements != NULL) {
      colors.clear();
      needs_update.set(Color);
    }
    
    if (skin_indices.elements != NULL) {
      skin_indices.clear();
      needs_update.set(SkinIndices);
    }
    
    if (skin_weights.elements != NULL) {
      skin_weights.clear();
      needs_update.set(SkinWeights);
    }

    material_config.clear();
  }


  void RenderMesh3D::destroy () {
    if (origin != NULL)  memory::deallocate(origin);
    
    positions.destroy();
    normals.destroy();
    
    disable_uvs();
    disable_colors();

    disable_skinning();

    faces.destroy();

    material_config.destroy();

    needs_update.clear();

    glDeleteBuffers(Mesh3DAttribute::total_attribute_count, gl_vbos);
    glDeleteVertexArrays(1, &gl_vao);
  }


  void RenderMesh3D::calculate_normals () {
    normals.reallocate(positions.count);

    normals.count = 0;

    for (size_t i = 0; i < positions.count; i ++) normals.append({ 0.0f });

    for (auto [ i, face ] : faces) {
      Vector3f norm = 
            (positions[face[2]] - positions[face[0]])
      .cross(positions[face[1]] - positions[face[0]]);

      normals[face[0]] += norm;
      normals[face[1]] += norm;
      normals[face[2]] += norm;
    }

    for (auto [ i, normal ] : normals) normal = normal.normalize();
  }

  void RenderMesh3D::calculate_face_normals () {
    normals.reallocate(positions.count);

    normals.count = positions.count;

    for (auto [ i, face ] : faces) {
      Vector3f norm =
            (positions[face[2]] - positions[face[0]])
      .cross(positions[face[1]] - positions[face[0]])
      .normalize();

      normals[face[0]] = norm;
      normals[face[1]] = norm;
      normals[face[2]] = norm;
    }
  }


  void RenderMesh3D::update () {
    // TODO switch to DSA style

    using namespace Mesh3DAttribute;

    if (needs_update.match_index(bounds_flag)) {
      recalculate_bounds();
    }

    if (needs_update.any_bits()) {
      s32_t draw_arg = dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

      glBindVertexArray(gl_vao);

      if (needs_update.match_index(Position)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[Position]);
        glBufferData(GL_ARRAY_BUFFER, positions.count * sizeof(Vector3f), positions.elements, draw_arg);
      }

      if (needs_update.match_index(Normal)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[Normal]);
        glBufferData(GL_ARRAY_BUFFER, normals.count * sizeof(Vector3f), normals.elements, draw_arg);
      }

      if (needs_update.match_index(UV)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[UV]);
        glBufferData(GL_ARRAY_BUFFER, uvs.count * sizeof(Vector2f), uvs.elements, draw_arg);
      }

      if (needs_update.match_index(Color)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[Color]);
        glBufferData(GL_ARRAY_BUFFER, colors.count * sizeof(Vector3f), colors.elements, draw_arg);
      }

      if (needs_update.match_index(SkinIndices)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[SkinIndices]);
        glBufferData(GL_ARRAY_BUFFER, skin_indices.count * sizeof(Vector4u), skin_indices.elements, draw_arg);
      }

      if (needs_update.match_index(SkinWeights)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[SkinWeights]);
        glBufferData(GL_ARRAY_BUFFER, skin_weights.count * sizeof(Vector4f), skin_weights.elements, draw_arg);
      }

      if (needs_update.match_index(Face)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_vbos[Face]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.count * sizeof(Vector3u), faces.elements, draw_arg);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);

      needs_update.clear();
    }
  }


  void RenderMesh3D::use () {
    update();

    glBindVertexArray(gl_vao);
  }
  


  void RenderMesh3D::draw_with_active_shader () {
    use();
    
    glDrawElements(GL_TRIANGLES, faces.count * 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
  }

  void RenderMesh3D::draw_section_with_active_shader (size_t section_index) {
    use();

    MaterialInfo const& info = material_config[section_index];

    glDrawElements(GL_TRIANGLES, info.length * 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(info.start_index * 3 * sizeof(uint32_t)));
  }

  void RenderMesh3D::draw_with_material (MaterialHandle const& material) {
    material->use();

    draw_with_active_shader();
  }

  void RenderMesh3D::draw_with_material_instance (MaterialInstance const& material_instance) {
    material_instance.use();

    draw_with_active_shader();
  }

  void RenderMesh3D::draw_section_with_material (size_t section_index, MaterialHandle const& material) {
    material->use();

    draw_section_with_active_shader(section_index);
  }

  void RenderMesh3D::draw_section_with_material_instance (size_t section_index, MaterialInstance const& material_instance) {
    material_instance.use();

    draw_section_with_active_shader(section_index);
  }

  void RenderMesh3D::draw_with_material_set (MaterialSetHandle const& material_set) {
    MaterialSet const& ref = *material_set;

    use();
    
    if (material_config.multi_material) {
      for (auto [ i, info ] : material_config.materials) {
        ref[info.material_index].use();
        
        glDrawElements(GL_TRIANGLES, info.length * 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(info.start_index * 3 * sizeof(uint32_t)));
      }
    } else {
      ref[material_config.material_index].use();
    
      glDrawElements(GL_TRIANGLES, faces.count * 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
    }
  }



  void RenderMesh3D::enable_uvs (Vector2f const* data) {
    m_assert(uvs.elements == NULL, "Cannot enable uvs, data already exists");

    if (data != NULL) uvs.append_multiple(data, positions.count);
    else uvs.grow_allocation();

    init_gl_uvs(this);
  }

  
  void RenderMesh3D::enable_uvs (Array<Vector2f> const& arr) {
    m_assert(uvs.elements == NULL, "Cannot enable uvs, data already exists");
    m_assert(arr.count == positions.count, "Cannot enable colors, copied array has %zu entries, but there are %zu vertices in the mesh", arr.count, positions.count);

    uvs.append_multiple(arr.elements, positions.count);

    init_gl_uvs(this);
  }

  void RenderMesh3D::enable_uvs_ex (Vector2f* data) {
    m_assert(uvs.elements == NULL, "Cannot enable uvs, data already exists");

    uvs = Array<Vector2f>::from_ex(data, positions.count);

    init_gl_uvs(this);
  }

  void RenderMesh3D::enable_uvs_ex (Array<Vector2f> const& arr) {
    m_assert(uvs.elements == NULL, "Cannot enable uvs, data already exists");
    m_assert(arr.count == positions.count, "Cannot enable colors, new array has %zu entries, but there are %zu vertices in the mesh", arr.count, positions.count);

    uvs = arr;

    init_gl_uvs(this);
  }

  void RenderMesh3D::disable_uvs () {
    if (uvs.elements == NULL) return;

    uvs.destroy();

    disable_gl_uvs(this);

    needs_update.unset(Mesh3DAttribute::UV);
  }



  void RenderMesh3D::enable_colors (Vector3f const* data) {
    m_assert(colors.elements == NULL, "Cannot enable colors, data already exists");

    if (data != NULL) colors.append_multiple(data, positions.count);
    else colors.grow_allocation();

    init_gl_colors(this);
  }

  
  void RenderMesh3D::enable_colors (Array<Vector3f> const& arr) {
    m_assert(colors.elements == NULL, "Cannot enable colors, data already exists");
    m_assert(arr.count == positions.count, "Cannot enable colors, copied array has %zu entries, but there are %zu vertices in the mesh", colors.count, positions.count);

    colors.append_multiple(arr.elements, positions.count);

    init_gl_colors(this);
  }

  void RenderMesh3D::enable_colors_ex (Vector3f* data) {
    m_assert(colors.elements == NULL, "Cannot enable colors, data already exists");

    colors = Array<Vector3f>::from_ex(data, positions.count);

    init_gl_colors(this);
  }

  void RenderMesh3D::enable_colors_ex (Array<Vector3f> const& arr) {
    m_assert(colors.elements == NULL, "Cannot enable colors, data already exists");
    m_assert(arr.count == positions.count, "Cannot enable colors, new array has %zu entries, but there are %zu vertices in the mesh", colors.count, positions.count);

    colors = arr;

    init_gl_colors(this);
  }

  void RenderMesh3D::disable_colors () {
    if (colors.elements == NULL) return;

    colors.destroy();

    disable_gl_colors(this);

    needs_update.unset(Mesh3DAttribute::Color);
  }



  void RenderMesh3D::enable_skinning (Vector4u const* in_skin_indices, Vector4f const* in_skin_weights) {
    m_assert(skin_indices.elements == NULL && skin_weights.elements == NULL, "Cannot enable skinning, data already exists");

    if (in_skin_indices != NULL) skin_indices.append_multiple(in_skin_indices, positions.count);
    else skin_indices.grow_allocation();

    if (in_skin_weights != NULL) skin_weights.append_multiple(in_skin_weights, positions.count);
    else skin_weights.grow_allocation();

    init_gl_skinning(this);
  }

  
  void RenderMesh3D::enable_skinning (Array<Vector4u> const& in_skin_indices, Array<Vector4f> const& in_skin_weights) {
    m_assert(skin_indices.elements == NULL && skin_weights.elements == NULL, "Cannot enable skinning, data already exists");
    m_assert(in_skin_indices.count == positions.count, "Cannot enable skinning, copied skin indices array has %zu entries, but there are %zu vertices in the mesh", in_skin_indices.count, positions.count);
    m_assert(in_skin_weights.count == positions.count, "Cannot enable skinning, copied skin weights array has %zu entries, but there are %zu vertices in the mesh", in_skin_weights.count, positions.count);

    skin_indices.append_multiple(in_skin_indices.elements, positions.count);
    skin_weights.append_multiple(in_skin_weights.elements, positions.count);

    init_gl_skinning(this);
  }

  void RenderMesh3D::enable_skinning_ex (Vector4u* in_skin_indices, Vector4f* in_skin_weights) {
    m_assert(skin_indices.elements == NULL && skin_weights.elements == NULL, "Cannot enable skinning, data already exists");

    skin_indices = Array<Vector4u>::from_ex(in_skin_indices, positions.count);
    skin_weights = Array<Vector4f>::from_ex(in_skin_weights, positions.count);

    init_gl_skinning(this);
  }

  void RenderMesh3D::enable_skinning_ex (Array<Vector4u> const& in_skin_indices, Array<Vector4f> const& in_skin_weights) {
    m_assert(skin_indices.elements == NULL && skin_weights.elements == NULL, "Cannot enable skinning, data already exists");
    m_assert(in_skin_indices.count == positions.count, "Cannot enable skinning, new skin indices array has %zu entries, but there are %zu vertices in the mesh", in_skin_indices.count, positions.count);
    m_assert(in_skin_weights.count == positions.count, "Cannot enable skinning, new skin weights array has %zu entries, but there are %zu vertices in the mesh", in_skin_weights.count, positions.count);

    skin_indices = in_skin_indices;
    skin_weights = in_skin_weights;

    init_gl_skinning(this);
  }

  void RenderMesh3D::disable_skinning () {
    if (skin_indices.elements == NULL && skin_weights.elements == NULL) return;

    skin_indices.destroy();
    skin_weights.destroy();

    disable_gl_skinning(this);

    needs_update.unset(Mesh3DAttribute::SkinIndices);
    needs_update.unset(Mesh3DAttribute::SkinWeights);
  }


  VertexRef3D RenderMesh3D::get_vertex (size_t index) const {
    return {
      positions[index],
      normals[index],

      uvs.elements != NULL? Optional<Vector2f&> { uvs[index] } : Optional<Vector2f&> { },
      colors.elements != NULL? Optional<Vector3f&> { colors[index] } : Optional<Vector3f&> { },

      skin_indices.elements != NULL? Optional<Vector4u&> { skin_indices[index] } : Optional<Vector4u&> { },
      skin_weights.elements != NULL? Optional<Vector4f&> { skin_weights[index] } : Optional<Vector4f&> { }
    };
  }



  void RenderMesh3D::set_vertex (size_t index, VertexData3D const& data) {
    using namespace Mesh3DAttribute;

    positions[index] = data.position;
    normals[index] = data.normal;
    needs_update.set_multiple(Position, bounds_flag, Normal);

    if (uvs.elements != NULL) {
      uvs[index] = data.uv;
      needs_update.set(UV);
    } else m_assert(!data.uv.valid, "Unexpected UV attribute data");

    if (colors.elements != NULL) {
      colors[index] = data.color;
      needs_update.set(Color);
    } else m_assert(!data.color.valid, "Unexpected Color attribute data");

    if (skin_indices.elements != NULL) {
      skin_indices[index] = data.skin_indices;
      needs_update.set(SkinIndices);
    } else m_assert(!data.skin_indices.valid, "Unexpected SkinIndices attribute data");

    if (skin_weights.elements != NULL) {
      skin_weights[index] = data.skin_weights;
      needs_update.set(SkinWeights);
    } else m_assert(!data.skin_weights.valid, "Unexpected SkinWeights attribute data");
  }


  void RenderMesh3D::append_vertex (VertexData3D const& data) {
    using namespace Mesh3DAttribute;

    positions.append(data.position);
    normals.append(data.normal);
    needs_update.set_multiple(Position, bounds_flag, Normal);

    if (uvs.elements != NULL) {
      uvs.append(data.uv);
      needs_update.set(UV);
    } else m_assert(!data.uv.valid, "Unexpected UV attribute data");

    if (colors.elements != NULL) {
      colors.append(data.color);
      needs_update.set(Color);
    } else m_assert(!data.color.valid, "Unexpected Color attribute data");

    if (skin_indices.elements != NULL) {
      skin_indices.append(data.skin_indices);
      needs_update.set(SkinIndices);
    } else m_assert(!data.skin_indices.valid, "Unexpected SkinIndices attribute data");

    if (skin_weights.elements != NULL) {
      skin_weights.append(data.skin_weights);
      needs_update.set(SkinWeights);
    } else m_assert(!data.skin_weights.valid, "Unexpected SkinWeights attribute data");
  }


  void RenderMesh3D::insert_vertex (size_t index, VertexData3D const& data) {
    using namespace Mesh3DAttribute;

    positions.insert(index,data.position);
    normals.insert(index, data.normal);
    needs_update.set_multiple(Position, bounds_flag, Normal);

    if (uvs.elements != NULL) {
      uvs.insert(index, data.uv);
      needs_update.set(UV);
    } else m_assert(!data.uv.valid, "Unexpected UV attribute data");

    if (colors.elements != NULL) {
      colors.insert(index, data.color);
      needs_update.set(Color);
    } else m_assert(!data.color.valid, "Unexpected Color attribute data");

    if (skin_indices.elements != NULL) {
      skin_indices.insert(index, data.skin_indices);
      needs_update.set(SkinIndices);
    } else m_assert(!data.skin_indices.valid, "Unexpected SkinIndices attribute data");

    if (skin_weights.elements != NULL) {
      skin_weights.insert(index, data.skin_weights);
      needs_update.set(SkinWeights);
    } else m_assert(!data.skin_weights.valid, "Unexpected SkinWeights attribute data");
  }


  void RenderMesh3D::remove_vertex (size_t index) {
    using namespace Mesh3DAttribute;

    positions.remove(index);
    normals.remove(index);
    needs_update.set_multiple(Position, bounds_flag, Normal);

    if (uvs.elements != NULL) {
      uvs.remove(index);
      needs_update.set(UV);
    }

    if (colors.elements != NULL) {
      colors.remove(index);
      needs_update.set(Color);
    }

    if (skin_indices.elements != NULL) {
      skin_indices.remove(index);
      needs_update.set(SkinIndices);
    }

    if (skin_weights.elements != NULL) {
      skin_weights.remove(index);
      needs_update.set(SkinWeights);
    }
  }





  Vector3u& RenderMesh3D::get_face (size_t index) const {
    return faces[index];
  }

  void RenderMesh3D::set_face (size_t index, Vector3u const& face) {
    faces[index] = face;

    needs_update.set(Mesh3DAttribute::Face);
  }

  void RenderMesh3D::append_face (Vector3u const& face) {
    faces.append(face);

    needs_update.set(Mesh3DAttribute::Face);
  }

  void RenderMesh3D::append_face (size_t index, Vector3u const& face) {
    faces.insert(index, face);

    needs_update.set(Mesh3DAttribute::Face);
  }

  void RenderMesh3D::remove_face (size_t index) {
    faces.remove(index);
    
    needs_update.set(Mesh3DAttribute::Face);
  }



  void RenderMesh3D::set_material (size_t material_index, bool cast_shadow) {
    if (material_config.multi_material) material_config.materials.destroy();
    material_config = { material_index, cast_shadow };
  }

  void RenderMesh3D::set_multi_material (Array<MaterialInfo> const& materials) {
    if (material_config.multi_material) {
      material_config.materials.clear();
      material_config.materials.copy(materials);
    } else {
      material_config = { materials };
    }
  }

  void RenderMesh3D::set_multi_material_ex (Array<MaterialInfo> const& materials) {
    if (material_config.multi_material) material_config.materials.destroy();
    material_config = MaterialConfig::from_ex(materials);
  }


  MaterialInfo& RenderMesh3D::get_material_element (size_t index) const {
    return material_config[index];
  }

  void RenderMesh3D::set_material_element (size_t index, MaterialInfo const& value) {
    material_config[index] = value;
  }

  void RenderMesh3D::append_material_element (MaterialInfo const& value) {
    material_config.append(value);
  }

  void RenderMesh3D::insert_material_element (size_t index, MaterialInfo const& value) {
    material_config.insert(index, value);
  }

  void RenderMesh3D::remove_material_element (size_t index) {
    material_config.remove(index);
  }
}