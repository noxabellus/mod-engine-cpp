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
    cube.add_component(test_cube_mesh);
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