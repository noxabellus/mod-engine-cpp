#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  inline void init_gl_data (RenderMesh3D* mesh) {
    s32_t draw_arg = mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    // TODO switch to DSA style (See dsa_example below)

    glGenVertexArrays(1, &mesh->gl_vao);
    glBindVertexArray(mesh->gl_vao);

    glGenBuffers(4, mesh->gl_vbos);


    // Setup positions buffer & attrib, copy any data to gl, enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*) 0);
    glBufferData(GL_ARRAY_BUFFER, mesh->positions.count * sizeof(Vector3f), mesh->positions.elements, draw_arg);
    glEnableVertexAttribArray(0);

    // Setup UVs attrib, do not copy data or enable (not sure binding is needed)
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), (void*) 0);

    // Setup colors attrib, do not copy data or enable
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*) 0);

    // Setup faces and copy data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_vbos[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faces.count * sizeof(Vector3u), mesh->faces.elements, draw_arg);

    // Clear binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void init_gl_uvs (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, mesh->uvs.count * sizeof(Vector2f), mesh->uvs.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void init_gl_colors (RenderMesh3D* mesh) {
    // TODO switch to DSA style
    
    glBindVertexArray(mesh->gl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, mesh->colors.count * sizeof(Vector3f), mesh->colors.elements, mesh->dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline void disable_gl_uvs (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
  }

  inline void disable_gl_colors (RenderMesh3D* mesh) {
    // TODO switch to DSA style

    glBindVertexArray(mesh->gl_vao);
    glDisableVertexAttribArray(2);
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
    Vector2f const* in_uvs,
    Vector3f const* in_colors,

    size_t face_count,
    Vector3u const* in_faces,

    MaterialConfig const& in_material_config
  )
  : origin(str_clone(in_origin))
  , dynamic(in_dynamic)
  {
    positions.append_multiple(in_positions, vertex_count);
    faces.append_multiple(in_faces, face_count);

    init_gl_data(this);

    if (in_uvs != NULL) enable_uvs(in_uvs);
    if (in_colors != NULL) enable_colors(in_colors);

    material_config = in_material_config;
  }


  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    size_t vertex_count,
    Vector3f* positions,
    Vector2f* uvs,
    Vector3f* colors,
    
    size_t face_count,
    Vector3u* faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;

    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = Array<Vector3f>::from_ex(positions, vertex_count);
    mesh.faces = Array<Vector3u>::from_ex(faces, face_count);

    init_gl_data(&mesh);

    if (uvs != NULL) mesh.enable_uvs_ex(uvs);
    if (colors != NULL) mesh.enable_colors_ex(colors);

    mesh.material_config = material_config;

    return mesh;
  }


  RenderMesh3D RenderMesh3D::from_ex (
    char const* origin,

    bool dynamic,
    
    Array<Vector3f> const& positions,
    Array<Vector2f> const& uvs,
    Array<Vector3f> const& colors,
    
    Array<Vector3u> const& faces,

    MaterialConfig const& material_config
  ) {
    RenderMesh3D mesh;
    
    mesh.origin = str_clone(origin);

    mesh.dynamic = dynamic;

    mesh.positions = positions;
    mesh.faces = faces;

    init_gl_data(&mesh);
    
    mesh.enable_uvs_ex(uvs);
    mesh.enable_colors_ex(colors);

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
    mesh.faces = faces;

    init_gl_data(&mesh);

    mesh.material_config = material_config;
    
    return mesh;
  }



  RenderMesh3D RenderMesh3D::from_json_item (const char* origin, JSONItem const& json) {
    Array<f32_t> positions;
    Array<f32_t> uvs;
    Array<f32_t> colors;
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
      faces.destroy();
      uvs.destroy();
      colors.destroy();
      material_config.destroy();
      throw exception;
    }

    return from_ex(
      origin,

      dynamic,

      positions.count / 3,
      (Vector3f*) positions.elements,
      (Vector2f*) uvs.elements,
      (Vector3f*) colors.elements,

      faces.count / 3,
      (Vector3u*) faces.elements,

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
      mesh = from_str(origin, (char*) source);
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);

    return mesh;
  }



  
  void RenderMesh3D::clear () {
    positions.clear();
    faces.clear();

    needs_update.set_multiple(0, 3);

    if (uvs.elements != NULL) {
      uvs.clear();
      needs_update.set(1);
    }

    if (colors.elements != NULL) {
      colors.clear();
      needs_update.set(2);
    }

    material_config.clear();
  }


  void RenderMesh3D::destroy () {
    if (origin != NULL) {
      free(origin);
      origin = NULL;
    }

    positions.destroy();
    faces.destroy();
    disable_uvs();
    disable_colors();

    material_config.destroy();

    needs_update.clear();

    glDeleteBuffers(4, gl_vbos);
    glDeleteVertexArrays(1, &gl_vao);
  }



  void RenderMesh3D::update () {
    // TODO switch to DSA style

    if (needs_update.match_any()) {
      s32_t draw_arg = dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

      glBindVertexArray(gl_vao);

      if (needs_update.match_index(0)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, positions.count * sizeof(Vector3f), positions.elements, draw_arg);
      }

      if (needs_update.match_index(1)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, uvs.count * sizeof(Vector2f), uvs.elements, draw_arg);
      }

      if (needs_update.match_index(2)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbos[2]);
        glBufferData(GL_ARRAY_BUFFER, colors.count * sizeof(Vector3f), colors.elements, draw_arg);
      }

      if (needs_update.match_index(3)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_vbos[3]);
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
    
    glDrawElements(GL_TRIANGLES, faces.count * 3, GL_UNSIGNED_INT, (void*) 0);
  }

  void RenderMesh3D::draw_section_with_active_shader (size_t section_index) {
    use();

    MaterialInfo const& info = material_config[section_index];

    glDrawElements(GL_TRIANGLES, info.length * 3, GL_UNSIGNED_INT, (void*) (info.start_index * 3 * sizeof(uint32_t)));
  }

  void RenderMesh3D::draw_with_material (MaterialHandle const& material) {
    material->use();

    draw_with_active_shader();
  }

  void RenderMesh3D::draw_section_with_material (size_t section_index, MaterialHandle const& material) {
    material->use();

    draw_section_with_active_shader(section_index);
  }

  void RenderMesh3D::draw_with_material_set (MaterialSetHandle const& material_set) {
    MaterialSet const& ref = *material_set;

    use();
    
    if (material_config.multi_material) {
      for (auto [ i, info ] : material_config.materials) {
        ref[info.material_index]->use();
        
        glDrawElements(GL_TRIANGLES, info.length * 3, GL_UNSIGNED_INT, (void*) (info.start_index * 3 * sizeof(uint32_t)));
      }
    } else {
      ref[material_config.material_index]->use();
    
      glDrawElements(GL_TRIANGLES, faces.count * 3, GL_UNSIGNED_INT, (void*) 0);
    }
  }



  void RenderMesh3D::enable_uvs (Vector2f const* data) {
    m_assert(uvs.elements == NULL, "Cannot enable uvs, data already exists");

    uvs.append_multiple(data, positions.count);

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

    needs_update.unset(1);
  }



  void RenderMesh3D::enable_colors (Vector3f const* data) {
    m_assert(colors.elements == NULL, "Cannot enable colors, data already exists");

    colors.append_multiple(data, positions.count);

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

    needs_update.unset(2);
  }


  tri_t<Vector3f*, Vector2f*, Vector3f*> RenderMesh3D::get_vertex (size_t index) const {
    return {
      &positions[index],
      uvs.elements != NULL? &uvs[index] : NULL,
      colors.elements != NULL? &colors[index] : NULL
    };
  }

  void RenderMesh3D::set_vertex (size_t index, Vector3f const& position) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions[index] = position;
    needs_update.set(0);
  }

  void RenderMesh3D::set_vertex (size_t index, Vector3f const& position, Vector2f const& uv) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions[index] = position;
    uvs[index] = uv;
    needs_update.set_multiple(0, 1);
  }

  void RenderMesh3D::set_vertex (size_t index, Vector3f const& position, Vector3f const& color) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions[index] = position;
    colors[index] = color;
    needs_update.set_multiple(0, 2);
  }

  void RenderMesh3D::set_vertex (size_t index, Vector3f const& position, Vector2f const& uv, Vector3f const& color) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions[index] = position;
    uvs[index] = uv;
    colors[index] = color;
    needs_update.set_multiple(0, 1, 2);
  }


  void RenderMesh3D::append_vertex (Vector3f const& position) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions.append(position);
    needs_update.set(0);
  }

  void RenderMesh3D::append_vertex (Vector3f const& position, Vector2f const& uv) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions.append(position);
    uvs.append(uv);
    needs_update.set_multiple(0, 1);
  }

  void RenderMesh3D::append_vertex (Vector3f const& position, Vector3f const& color) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions.append(position);
    colors.append(color);
    needs_update.set_multiple(0, 2);
  }

  void RenderMesh3D::append_vertex (Vector3f const& position, Vector2f const& uv, Vector3f const& color) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions.append(position);
    uvs.append(uv);
    colors.append(color);
    needs_update.set_multiple(0, 1, 2);
  }


  void RenderMesh3D::insert_vertex (size_t index, Vector3f const& position) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions.insert(index, position);
    needs_update.set(0);
  }

  void RenderMesh3D::insert_vertex (size_t index, Vector3f const& position, Vector2f const& uv) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements == NULL, "Expected a color attribute");
    positions.insert(index, position);
    uvs.insert(index, uv);
    needs_update.set_multiple(0, 1);
  }

  void RenderMesh3D::insert_vertex (size_t index, Vector3f const& position, Vector3f const& color) {
    m_assert(uvs.elements == NULL, "Expected a uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions.insert(index, position);
    colors.insert(index, color);
    needs_update.set_multiple(0, 2);
  }

  void RenderMesh3D::insert_vertex (size_t index, Vector3f const& position, Vector2f const& uv, Vector3f const& color) {
    m_assert(uvs.elements != NULL, "Unexpected uv attribute");
    m_assert(colors.elements != NULL, "Unexpected color attribute");
    positions.insert(index, position);
    uvs.insert(index, uv);
    colors.insert(index, color);
    needs_update.set_multiple(0, 1, 2);
  }


  void RenderMesh3D::remove_vertex (size_t index) {
    positions.remove(index);
    needs_update.set(0);

    if (uvs.elements != NULL) {
      uvs.remove(index);
      needs_update.set(1);
    }

    if (colors.elements != NULL) {
      colors.remove(index);
      needs_update.set(2);
    }
  }





  Vector3u& RenderMesh3D::get_face (size_t index) const {
    return faces[index];
  }

  void RenderMesh3D::set_face (size_t index, Vector3u const& face) {
    faces[index] = face;
    needs_update.set(3);
  }

  void RenderMesh3D::append_face (Vector3u const& face) {
    faces.append(face);
    needs_update.set(3);
  }

  void RenderMesh3D::append_face (size_t index, Vector3u const& face) {
    faces.insert(index, face);
    needs_update.set(3);
  }

  void RenderMesh3D::remove_face (size_t index) {
    faces.remove(index);
    needs_update.set(3);
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