#include "../main.hh"
#include "../examples/gui/main_menu_ex.cc"
#include "../examples/gui/vendor_ex.cc"



MODULE_API void module_init () {
  using namespace mod;
  using namespace ImGui;


  Application.init();

  AssetManager.load_database_from_file("./assets/asset_db.json");


  draw_debug.init();


  ECS& ecs = *new ECS;


  /* XML TEST */
  RenderMesh3D dae_mesh; {
    XML xml = XML::from_file("./assets/meshes/thinmatrix_cowboy.dae");

    XMLItem collada = xml.first_named("COLLADA");

    XMLItem geometries = collada.first_named("library_geometries");

    XMLItem geometry = geometries.first_named("geometry");

    if (geometries.count_of_name("geometry") > 1) printf(
      "Warning: Collada loader currently only supports single geometries, only the first (named '%s') will be loaded\n",
      geometry.get_attribute("name").value.value
    );

    XMLItem mesh = geometry.first_named("mesh");


    // gather source data //

    size_t sources_count = mesh.count_of_name("source");

    struct Source {
      String id;
      Array<f64_t> floats;
    };

    Array<Source> sources;

    struct Accessor {
      String id;
      String source_id;
      size_t count;
      size_t offset;
      size_t stride;
      size_t elements;
    };

    Array<Accessor> accessors;

    for (size_t i = 0; i < sources_count; i ++) {
      XMLItem source = mesh.nth_named(i, "source");

      XMLItem* float_array = source.first_named_pointer("float_array");

      if (float_array != NULL) {
        String id = float_array->get_attribute("id").value; // borrowing string here, destroyed by XML

        size_t float_count = strtoumax(float_array->get_attribute("count").value.value, NULL, 10);
        Array<f64_t> floats { float_count };

        char* base = float_array->get_text().value;

        for (size_t i = 0; i < float_count; i ++) {
          floats.append(strtod(base, &base));
        }

        sources.append({ id, floats });
      }

      XMLItem accessor = source.first_named("technique_common").first_named("accessor");

      XMLAttribute* offset = accessor.get_attribute_pointer("offset");

      accessors.append({
        source.get_attribute("id").value, // borrowing string here, destroyed by XML
        accessor.get_attribute("source").value, // borrowing string here, destroyed by XML
        strtoumax(accessor.get_attribute("count").value.value, NULL, 10),
        offset != NULL? strtoumax(offset->value.value, NULL, 10) : 0,
        strtoumax(accessor.get_attribute("stride").value.value, NULL, 10),
        accessor.count_of_name("param")
      });
    }



    
    // Gather inputs //

    struct VertexBinding {
      String id;
      String source_id;
    };

    size_t vertex_binding_count = mesh.count_of_name("vertices");

    Array<VertexBinding> vertex_bindings { vertex_binding_count };

    for (size_t i = 0; i < vertex_binding_count; i ++) {
      XMLItem& vertices = mesh.nth_named(i, "vertices");
      XMLItem& input = vertices.first_named("input");

      vertex_bindings.append({
        vertices.get_attribute("id").value, // borrowing string here, destroyed by XML
        input.get_attribute("source").value // borrowing string here, destroyed by XML
      });
    }

    const auto get_vertex_binding = [&] (String const& id) -> VertexBinding& {
      for (auto [ k, binding ] : vertex_bindings) {
        if (binding.id == id.value + 1) return binding;
      }

      mesh.asset_error("Could not find VertexBinding '%s'", id);
    };


    struct Input {
      String semantic;
      String source_id;
      size_t offset;
      size_t set;
    };

    struct Polylist {
      XMLItem& origin;
      size_t count;
      Array<Input> inputs;
      Array<u32_t> indices;
    };

    size_t polylist_count = mesh.count_of_name("polylist");

    Array<Polylist> polylists { polylist_count };

    for (size_t i = 0; i < polylist_count; i ++) {
      XMLItem& polylist = mesh.nth_named(i, "polylist");

      size_t input_count = polylist.count_of_name("input");

      Array<Input> inputs = { input_count };

      for (size_t j = 0; j < input_count; j ++) {
        XMLItem& input = polylist.nth_named(j, "input");

        String semantic = input.get_attribute("semantic").value; // borrowing string here, destroyed by XML
        String source_id = input.get_attribute("source").value; // borrowing string here, destroyed by XML

        if (semantic == "VERTEX") {
          source_id = get_vertex_binding(source_id).source_id;
        }

        XMLAttribute* set = input.get_attribute_pointer("set");

        inputs.append({
          semantic,
          source_id,
          strtoumax(input.get_attribute("offset").value.value, NULL, 10),
          set != NULL? strtoumax(set->value.value, NULL, 10) : 0
        });
      }


      size_t count = strtoumax(polylist.get_attribute("count").value.value, NULL, 10);
      

      size_t indices_count = count * inputs.count * 3;

      Array<u32_t> indices = { indices_count };
      
      XMLItem& p = polylist.first_named("p");
      char* base = p.get_text().value;
      char* end = NULL;

      for (size_t k = 0; k < indices_count; k ++) {
        u32_t index = strtoul(base, &end, 10);
        p.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at index %zu (expected %zu indices)", k, indices_count);
        base = end;
        indices.append(index);
      }


      polylists.append({
        polylist,
        count,
        inputs,
        indices
      });
    }



    const auto get_accessor = [&] (String const& id) -> Accessor& {
      for (auto [ i, accessor ] : accessors) {
        if (accessor.id == id.value + 1) return accessor;
      }

      mesh.asset_error("Could not find Accessor '%s'", id.value);
    };

    const auto get_source = [&] (String const& id) -> Source& {
      for (auto [ i, source ] : sources) {
        if (source.id == id.value + 1) return source;
      }

      mesh.asset_error("Could not find Source '%s'", id.value);
    };

    const auto get_input = [&] (Polylist const& polylist, char const* semantic) -> Input& {
      for (auto [ i, input ] : polylist.inputs) {
        if (input.semantic == semantic) return input;
      }

      polylist.origin.asset_error("Could not find semantic Input '%s'", semantic);
    };

    const auto get_input_pointer = [&] (Polylist const& polylist, char const* semantic) -> Input* {
      for (auto [ i, input ] : polylist.inputs) {
        if (input.semantic == semantic) return &input;
      }

      return NULL;
    };



    // /*
      // print //

      // for (auto [ i, polylist ] : polylists) {
      //   auto& [ _p, count, inputs, indices ] = polylist;

      //   printf("Polylist %zu\n- Count: %zu\n- Inputs: %zu\n- Indices: %zu\n", i, count, inputs.count, indices.count);

      //   printf("  Indices\n  - [ ");
      //   for (size_t j = 0; j < indices.count; j += inputs.count) {
      //     printf("[ ");
      //     for (size_t k = j; k < j + inputs.count; k ++) {
      //       printf("%" PRIu32, indices[k]);
      //       if (k < j + inputs.count - 1) printf(", ");
      //     }
      //     printf(" ], ");

      //     if (j > 4 * inputs.count) {
      //       printf("...");
      //       break;
      //     }
      //   }
      //   printf(" ]\n");

      //   for (auto [ j, input ] : inputs) {
      //     auto& [ semantic, source_id, offset, set ] = input;
      //     printf("  Input %zu\n  - Semantic: %s\n  - Source ID: %s\n  - Offset: %zu\n  - Set: %zu\n", j, semantic.value, source_id.value, offset, set);

      //     auto& [ id, asource_id, acount, aoffset, stride, elements ] = get_accessor(source_id);
      //     printf("    Accessor\n    - Source ID: %s\n    - Count: %zu\n    - Offset: %zu\n    - Stride: %zu\n    - Elements: %zu\n", asource_id.value, acount, aoffset, stride, elements);

      //     auto& [ sid, floats ] = get_source(asource_id);
      //     printf("      Source\n      - Count: %zu\n      - Floats: [ ", floats.count);

      //     for (size_t k = aoffset; k < acount * stride; k += stride) {
      //       printf("[ ");
      //       for (size_t l = k; l < k + elements; l ++) {
      //         printf("%lf", floats[l]);
      //         if (l < k + elements - 1) printf(", ");
      //       }
      //       printf(" ], ");

      //       if (k > 3 * 3) {
      //         printf("...");
      //         break;
      //       }
      //     }
      //     printf(" ]\n");
      //   }

      //   printf("\n");
      // }
    // */



    static constexpr size_t max_attributes = 8;
    struct IVertex {
      Polylist& polylist;

      bool set;

      size_t index;

      s64_t duplicate;

      u32_t position;

      s64_t normal;
      s64_t uv;
      s64_t color;


      void set_attributes (s64_t in_normal, s64_t in_uv, s64_t in_color) {
        normal = in_normal;
        uv = in_uv;
        color = in_color;
        
        set = true;
      }

      bool has_same_attributes (s64_t test_normal, s64_t test_uv, s64_t test_color) const {
        return normal == test_normal
            && uv == test_uv
            && color == test_color;
      }
    };

    Array<MaterialInfo> material_config_data;
    Array<IVertex> i_vertices;
    Array<u32_t> i_indices;

    size_t max_inputs = 0;

    for (auto [ i, polylist ] : polylists) {
      auto& [ _p, face_count, inputs, indices ] = polylist;
      
      max_inputs = num::max(inputs.count, max_inputs);

      MaterialInfo mat_info = { i, indices.count / 3, 0 };


      Input& position_input = get_input(polylist, "VERTEX");

      for (size_t j = 0; j < indices.count; j += inputs.count) {
        size_t iv_index = j / inputs.count;

        u32_t position = indices[j + position_input.offset];

        IVertex iv = { polylist, false, iv_index, -1 };
        iv.position = position;
        i_vertices.append(iv);
      }


      Input* normal_input = get_input_pointer(polylist, "NORMAL");
      Input* uv_input = get_input_pointer(polylist, "TEXCOORD");
      Input* color_input = get_input_pointer(polylist, "COLOR");

      bool have_normal = normal_input != NULL;
      bool have_uv = uv_input != NULL;
      bool have_color = color_input != NULL;

      if (have_normal || have_uv || have_color) {
        for (size_t j = 0; j < indices.count; j += inputs.count) {
          size_t iv_index = j / inputs.count;

          s64_t normal = have_normal? indices[j + normal_input->offset] : -1;
          s64_t uv = have_uv? indices[j + uv_input->offset] : -1;
          s64_t color = have_color? indices[j + color_input->offset] : -1;

          IVertex* existing_vertex = &i_vertices[iv_index];

          if (!existing_vertex->set) {
            existing_vertex->set_attributes(normal, uv, color);
            i_indices.append(iv_index);
          } else if (existing_vertex->has_same_attributes(normal, uv, color)) {
            i_indices.append(iv_index);
          } else {
            bool found_existing = false;

            while (existing_vertex->duplicate != -1) {
              existing_vertex = &i_vertices[existing_vertex->duplicate];

              if (existing_vertex->has_same_attributes(normal, uv, color)) {
                i_indices.append(existing_vertex->index);
                found_existing = true;
                break;
              }
            }

            if (!found_existing) {
              size_t new_iv_index = i_vertices.count;

              IVertex new_iv = { polylist, false, new_iv_index, -1 };
              new_iv.set_attributes(normal, uv, color);
              i_vertices.append(new_iv);

              existing_vertex->duplicate = new_iv_index;

              i_indices.append(new_iv_index);
            }
          }
        }
      }
      

      mat_info.length = indices.count / 3 - mat_info.start_index;

      material_config_data.append(mat_info);
    }


    mesh.asset_assert(i_indices.count % 3 == 0, "Final indices count was not cleanly divisible by 3, make sure your mesh is triangulated");


    Array<Vector3f> final_positions { i_vertices.count };
    Array<Vector3f> final_normals { i_vertices.count };
    Array<Vector2f> final_uvs { i_vertices.count };
    Array<Vector3f> final_colors { i_vertices.count };
    Array<Vector3u> final_faces = { i_indices.count / 3 };


    for (size_t i = 0; i < i_indices.count; i += 3) {
      final_faces.append({ i_indices[i], i_indices[i + 2], i_indices[i + 1] });
    }


    MaterialConfig final_material_config;

    if (material_config_data.count != 1) {
      final_material_config = MaterialConfig::from_ex(material_config_data);
    } else {
      material_config_data.destroy();
    }


    bool incomplete_normals = false;

    for (auto [ i, iv ] : i_vertices) {
      Input& position_input = get_input(iv.polylist, "VERTEX");
      Accessor& position_accessor = get_accessor(position_input.source_id);
      Source& position_source = get_source(position_accessor.source_id);

      Vector3f pos;

      for (size_t j = 0; j < 3; j ++) {
        pos.elements[j] = position_source.floats[position_accessor.offset + iv.position * position_accessor.stride + j];
      }

      final_positions.append(pos);


      if (!incomplete_normals) {
        if (iv.normal != -1) {
          Vector3f norm = { 0, 0, 0 };

          Input& normal_input = get_input(iv.polylist, "NORMAL");
          Accessor& normal_accessor = get_accessor(normal_input.source_id);
          Source& normal_source = get_source(normal_accessor.source_id);

          for (size_t j = 0; j < 3; j ++) {
            norm.elements[j] = normal_source.floats[normal_accessor.offset + iv.normal * normal_accessor.stride + j];
          }

          final_normals.append(norm);
        } else {
          incomplete_normals = true;
          final_normals.destroy();
        }
      }


      Vector2f uv = { 0, 0 };
      
      if (iv.uv != -1) {
        Input& uv_input = get_input(iv.polylist, "TEXCOORD");
        Accessor& uv_accessor = get_accessor(uv_input.source_id);
        Source& uv_source = get_source(uv_accessor.source_id);

        for (size_t j = 0; j < 2; j ++) {
          uv.elements[j] = uv_source.floats[uv_accessor.offset + iv.uv * uv_accessor.stride + j];
        }
      }

      final_uvs.append(uv);


      Vector3f color = { 1, 1, 1 };
      
      if (iv.color != -1) {
        Input& color_input = get_input(iv.polylist, "COLOR");
        Accessor& color_accessor = get_accessor(color_input.source_id);
        Source& color_source = get_source(color_accessor.source_id);

        for (size_t j = 0; j < 3; j ++) {
          color.elements[j] = color_source.floats[color_accessor.offset + iv.color * color_accessor.stride + j];
        }
      }

      final_colors.append(color);
    }


    dae_mesh = RenderMesh3D::from_ex(
      "collada!",

      true,

      final_positions.count,
      final_positions.elements,
      incomplete_normals || final_normals.count == 0? NULL : final_normals.elements,
      final_uvs.count == 0? NULL : final_uvs.elements,
      final_colors.count == 0? NULL : final_colors.elements,

      final_faces.count,
      final_faces.elements,

      final_material_config
    );



    // cleanup //

    for (auto [ i, source ] : sources) {
      source.floats.destroy();
    }

    sources.destroy();


    accessors.destroy();


    vertex_bindings.destroy();


    for (auto [ i, polylist ] : polylists) {
      polylist.inputs.destroy();
      polylist.indices.destroy();
    }

    polylists.destroy();

    i_vertices.destroy();

    i_indices.destroy();

    xml.destroy();
  }





  
  struct BasicInput {
    bool enabled;

    f32_t movement_rate;
  };

  struct PointLight {
    Vector3f color;
    f32_t brightness;
  };
  
  MaterialHandle directional_light_mat = AssetManager.get<Material>("PointLight");
  MaterialHandle unlit_color_mat = AssetManager.get<Material>("UnlitColor");
  RenderMesh3DHandle test_cube_mesh = AssetManager.get<RenderMesh3D>("Test Cube");

  ecs.create_component_type<Transform3D>();
  ecs.create_component_type<MaterialHandle>();
  ecs.create_component_type<RenderMesh3DHandle>();
  ecs.create_component_type<BasicInput>();
  ecs.create_component_type<PointLight>();


  EntityHandle cube; {
    cube = ecs.create_entity();
    cube.add_component(Transform3D {
      0,
      Constants::Quaternion::identity,
      100
    });
    cube.add_component(directional_light_mat);
    cube.add_component(BasicInput { true, 64 });
    cube.add_component(RenderMesh3DHandle { &dae_mesh });
  }

  EntityHandle plane; {
    plane = ecs.create_entity();
    plane.add_component(Transform3D {
      { 0, 0, -50 },
      Constants::Quaternion::identity,
      200
    });
    plane.add_component(directional_light_mat);
    plane.add_component(AssetManager.get<RenderMesh3D>("Test Quad 3D"));
  }

  EntityHandle light; {
    light = ecs.create_entity();
    light.add_component(Transform3D {
      { 200, 200, 200 },
      Constants::Quaternion::identity,
      10
    });
    light.add_component(test_cube_mesh);
    light.add_component(unlit_color_mat);
    light.add_component(PointLight { { 1, 1, 1 }, 1 });
  };


  ecs.create_system("MovementInput", true, { ecs.get_component_type_by_instance_type<BasicInput>().id, ecs.get_component_type_by_instance_type<Transform3D>().id }, [&] (ECS* ecs, uint32_t index) {
    BasicInput& input = ecs->get_component<BasicInput>(index);
    if (input.enabled) {
      Vector3f movement = { 0, 0, 0 };

      if (Input["Forward"]) movement.y -= 1;
      if (Input["Backward"]) movement.y += 1;

      if (Input["Left"]) movement.x -= 1;
      if (Input["Right"]) movement.x += 1;

      if (Input["Down"]) movement.z -= 1;
      if (Input["Up"]) movement.z += 1;

      movement = movement.normalize() * (input.movement_rate / Application.frame_delta);

      ecs->get_component<Transform3D>(index).position += movement;
    }
  });


  bool light_orbit = true;
  f32_t light_orbit_size = 200;
  f32_t light_orbit_time = 0;
  f32_t light_orbit_speed = M_PI * .125;

  f32_t specular_strength = 0.5f;
  f32_t specular_power = 2.0f;
  
  f32_t camera_rot_base = (M_PI * 2) * .25;
  f32_t camera_rot = (M_PI * 2) * .125;
  f32_t camera_dist = 500;
  f32_t camera_height = camera_dist;
  f32_t camera_zoom = 1;

  f32_t camera_rot_rate = M_PI;
  f32_t camera_roll_rate = 512;
  f32_t camera_zoom_rate = 1;
  f32_t camera_max_zoom = 3;
  f32_t camera_min_zoom = .25;

  Vector2f camera_drag_start;
  f32_t camera_rot_start;
  f32_t camera_height_start;
  bool camera_drag = false;

  f32_t camera_near = 0.01f;
  f32_t camera_far = 2500.0f;


  Matrix4 view_matrix;
  Matrix4 projection_matrix;
  Matrix4 camera_matrix;
  

  ecs.create_system("Render", [&] (ECS* ecs) {
    if (light_orbit) {
      light_orbit_time += light_orbit_speed / Application.frame_delta;
      Transform3D& light_tran = light.get_component<Transform3D>();
      light_tran.position = {
        cosf(light_orbit_time) * light_orbit_size,
        sinf(light_orbit_time) * light_orbit_size,
        light_tran.position.z
      };
    }
    Vector2f camera_drag_delta = { 0, 0 };
    if (Input["Primary Action"]) {
      if (!camera_drag) { // drag start
        camera_drag_start = Input.mouse_position_unit;
        camera_rot_start = camera_rot;
        camera_height_start = camera_height;
        camera_drag = true;
      } else { // drag continue
        camera_drag_delta = Input.mouse_position_unit - camera_drag_start;

        camera_rot = camera_rot_start + camera_drag_delta.x * camera_rot_rate;
        camera_height = camera_height_start + -camera_drag_delta.y * camera_roll_rate;
      }
    } else if (camera_drag) { // drag end
      camera_drag = false;
    }

    if (Input["Zoom In"]) camera_zoom = num::max(camera_min_zoom, camera_zoom - (camera_zoom_rate / Application.frame_delta));
    else if (Input["Zoom Out"]) camera_zoom = num::min(camera_max_zoom, camera_zoom + (camera_zoom_rate / Application.frame_delta));

    Vector3f camera_position = { cosf(camera_rot + camera_rot_base) * camera_dist, sinf(camera_rot + camera_rot_base) * camera_dist, camera_height };
    
    SetNextWindowPos({ 10, 10 }, ImGuiCond_Appearing, { 0, 0 });
    SetNextWindowCollapsed(true, ImGuiCond_Appearing);
    Begin("Info", NULL); {
      Text("Mouse PX: %dx%d", Input.mouse_position_px.x, Input.mouse_position_px.y);
      Text("Mouse NDC: %.3fx%.3f", Input.mouse_position_unit.x, Input.mouse_position_unit.y);
      Text("Camera Position: x %f, y %f, z %f", camera_position.x, camera_position.y, camera_position.z);
      Text("- Rotation: %f", camera_rot);
      Text("- Height: %f", camera_height);
      Text("- Zoom: %f", camera_zoom);
      if (Button("Reset", { 0, 0 })) {
        camera_rot = 0;
        camera_height = 100;
        camera_zoom = 1;
      }

      ColorPicker3("Light Color", light.get_component<PointLight>().color.elements);
      SliderFloat("Light Brightness", &light.get_component<PointLight>().brightness, 0.01f, 5.0f);
      if (Button("Light Orbit")) light_orbit = !light_orbit;
      SameLine(); Text("(%s)", light_orbit? "Enabled" : "Disabled");
      SliderFloat("Light Height", &light.get_component<Transform3D>().position.z, 0.0f, 400.0f);
      SliderFloat("Light Orbit Size", &light_orbit_size, 10.0f, 400.0f);
      SliderFloat("Light Orbit Speed", &light_orbit_speed, 0.001f, M_PI * .25);

      SliderFloat("Specular Strength", &specular_strength, 0.0f, 10.0f);
      SliderFloat("Specular Power", &specular_power, 0.0f, 255.0f);
      directional_light_mat->set_uniform("specular_strength", specular_strength);
      directional_light_mat->set_uniform("specular_power", specular_power);
    } End();

    view_matrix = Matrix4::from_look(camera_position, { 0, 0, 0 }, Constants::Vector3f::down, true).inverse();

    Vector2f half_screen = Vector2f { Application.ig_io->DisplaySize } / 2.0f;

    Vector2f half_screen_zoom = half_screen * camera_zoom;
    
    projection_matrix = Matrix4::from_orthographic(
      -half_screen_zoom.x, half_screen_zoom.x,
      -half_screen_zoom.y, half_screen_zoom.y,
      camera_near, camera_far
    );

    camera_matrix = projection_matrix * view_matrix;

    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<MaterialHandle>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs->entity_count; i ++) {
      EntityHandle entity = ecs->get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 model_matrix = Matrix4::compose(transform);

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();
        MaterialHandle& material = entity.get_component<MaterialHandle>();
        Material& ref = *material;

        ref.set_uniform("m_model", model_matrix);
        ref.set_uniform("m_view", view_matrix);
        ref.set_uniform("m_projection", projection_matrix);
        ref.set_uniform("m_normal", Matrix3::normal(view_matrix * model_matrix));

        if (ref.supports_uniform("light_pos")) {
          ref.set_uniform("light_pos", light.get_component<Transform3D>().position);
          ref.set_uniform("light_color", light.get_component<PointLight>().color * light.get_component<PointLight>().brightness);
        }

        mesh.draw_with_material(material);
      }
    }
  });


  ecs.create_system("Face Normal Debugger", [&] (ECS* ecs) {
    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs->entity_count; i ++) {
      EntityHandle entity = ecs->get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = Matrix4::compose(transform);

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();

        for (auto [ i, face ] : mesh.faces) {
          Triangle tri = { mesh.positions[face.x], mesh.positions[face.y], mesh.positions[face.z] };

          Vector3f normal = tri.normal();
          Vector3f center = tri.center();

          draw_debug.line3(Line3 { center, center + normal * .1 }.apply_matrix(transform_matrix), { 1, 0, 1 });
        }
      }
    }
  });

  ecs.create_system("Vertex Normal Debugger", [&] (ECS* ecs) {
    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };


    for (u32_t i = 0; i < ecs->entity_count; i ++) {
      EntityHandle entity = ecs->get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = Matrix4::compose(transform);

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();

        for (auto [ i, normal ] : mesh.normals) {
          Vector3f& pos = mesh.positions[i];
          
          draw_debug.line3(Line3 { pos, pos + normal * .1 }.apply_matrix(transform_matrix), { 0, 1, 1 });
        }
      }
    }
  });

  
  struct Hit {
    f32_t distance;
    EntityHandle entity;
  };

  Array<Hit> hits;

  ecs.create_system("Object Picker", [&] (ECS* ecs) {
    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    Vector3f origin_n = { Input.mouse_position_unit.x, Input.mouse_position_unit.y, -.99 };
    Vector3f end_n = { Input.mouse_position_unit.x, Input.mouse_position_unit.y, .9 };

    Matrix4 inverse_projection_matrix = projection_matrix.inverse();
    Matrix4 inverse_view_matrix = view_matrix.inverse();

    Vector3f origin = origin_n.unproject(inverse_view_matrix, inverse_projection_matrix);

    Vector3f direction = (end_n.unproject(inverse_view_matrix, inverse_projection_matrix) - origin).normalize();

    Ray3 ray = { origin, direction };

    hits.clear();

    for (u32_t i = 0; i < ecs->entity_count; i ++) {
      EntityHandle entity = ecs->get_handle(i);
      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& tran = entity.get_component<Transform3D>();
        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();
        Matrix4 world_matrix = Matrix4::compose(tran);
        AABB3 mesh_bounds = mesh.get_aabb().apply_matrix(world_matrix);
        
        if (auto aabb_res = Intersects::ray3_aabb3(ray, mesh_bounds); aabb_res.a) {
          // hits.append({ res.b, entity });

          for (auto [ i, face ] : mesh.faces) {
            Triangle world_tri = Triangle {
              mesh.positions[face[0]],
              mesh.positions[face[1]],
              mesh.positions[face[2]]
            }.apply_matrix(world_matrix);

            if (auto tri_res = Intersects::ray3_triangle(ray, world_tri, false); tri_res.a) {
              hits.append({ tri_res.b, entity });
            }
          }
        }
      }
    }

    draw_debug.cube(AABB3::from_center_and_size(ray.origin, { 10 }), { 1, 1, 0 });

    Begin("Ray", NULL);
    Text("Origin: %.3fx%.3fx%.3f", ray.origin.x, ray.origin.y, ray.origin.z);
    if (hits.count > 0) {
      hits.sort_in_place([&] (Hit const& a, Hit const& b) -> bool {
        return a.distance < b.distance;
      });

      auto [ distance, entity ] = hits[0];

      Vector3f intersect = ray.vector_at_offset(distance);

      Transform3D& t = entity.get_component<Transform3D>();
      RenderMesh3D& m = *entity.get_component<RenderMesh3DHandle>();
      AABB3 b = m.get_aabb().apply_matrix(Matrix4::compose(t));

      draw_debug.cube(AABB3::from_center_and_size(intersect, { 15 }), { 0, 1, 1 });

      Text("Intersect: %.3fx%.3fx%.3f", intersect.x, intersect.y, intersect.z);
      Text("Entity:");
      Text("- ID: %u", entity->id);
      Text("- Position: %.3fx%.3fx%.3f", t.position.x, t.position.y, t.position.z);
      Text("- Mesh Origin: %s", m.origin);

      draw_debug.cube(AABB3::from_center_and_size(b.min, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size(b.max, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.min.x, b.min.y, b.max.z }, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.min.x, b.max.y, b.min.z }, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.min.x, b.max.y, b.max.z }, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.max.x, b.min.y, b.min.z }, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.max.x, b.min.y, b.max.z }, { 10 }), { 1, 0, 1 });
      draw_debug.cube(AABB3::from_center_and_size({ b.max.x, b.max.y, b.min.z }, { 10 }), { 1, 0, 1 });
    }
    End();
  });
  

  Array<WatchedFileReport> update_reports;
  
  while (true) {
    if (!Application.begin_frame()) break;


    update_reports.clear();

    AssetManager.update_watched_files(&update_reports);

    if (update_reports.count > 0) {
      printf("Updated assets:\n");
      for (auto [ i, report ] : update_reports) {
        printf("'%s'\n", report.path.value);
        
        if (report.have_error) {
          printf("%s\n", report.error.value);
        }
      }
    }


    draw_debug.begin_frame();


    ecs.update();
    

    main_menu_ex();
    // vendor_ex();


    draw_debug.end_frame(camera_matrix, Application.resolution);


    Application.end_frame();
  }
  

  update_reports.destroy();
  ecs.destroy();
  draw_debug.destroy();
  Application.destroy();
}