#include "../main.hh"
#include "../examples/gui/main_menu_ex.cc"
#include "../examples/gui/vendor_ex.cc"



MODULE_API void module_init () {
  using namespace mod;


  Application.default_controls.append({ "Mod Control", { { }, { Keycode::X }, { } } });
  Application.create();


  AssetManager.create();
  AssetManager.load_database_from_file("./assets/asset_db.json");


  draw_debug_2d.create();
  draw_debug_3d.create();


  ECS& ecs = *new ECS;

  
  struct BasicInput {
    bool enabled;

    f32_t movement_rate;
  };
  
  MaterialHandle ma = AssetManager.get<Material>("Minimal3DA");


  ecs.create_component_type<Transform3D>();
  ecs.create_component_type<MaterialHandle>();
  ecs.create_component_type<RenderMesh3DHandle>();
  ecs.create_component_type<BasicInput>();


  EntityHandle cube; {
    cube = ecs.create_entity();
    cube.add_component(Transform3D {
      0,
      Constants::Quaternion::identity,
      100
    });
    cube.add_component(ma);
    cube.add_component(AssetManager.get<RenderMesh3D>("Test Cube"));
    cube.add_component(BasicInput { true, 64 });
  }

  EntityHandle plane; {
    plane = ecs.create_entity();
    plane.add_component(Transform3D {
      { 0, 0, -50 },
      Constants::Quaternion::identity,
      200
    });
    plane.add_component(ma);
    plane.add_component(AssetManager.get<RenderMesh3D>("Test Quad 3D"));
  }


  ecs.create_system("MovementInput", true, { ecs.get_component_type_by_instance_type<BasicInput>().id, ecs.get_component_type_by_instance_type<Transform3D>().id }, [&] (ECS* ecs, uint32_t index) {
    BasicInput& input = ecs->get_component<BasicInput>(index);
    if (input.enabled) {
      Vector3f movement = { 0, 0, 0 };

      if (Application.input["Forward"]) movement.y -= 1;
      if (Application.input["Backward"]) movement.y += 1;

      if (Application.input["Left"]) movement.x -= 1;
      if (Application.input["Right"]) movement.x += 1;

      if (Application.input["Down"]) movement.z -= 1;
      if (Application.input["Up"]) movement.z += 1;

      movement = movement.normalize() * (input.movement_rate / Application.frame_delta);

      ecs->get_component<Transform3D>(index).position += movement;
    }
  });

  
  f32_t camera_rot_base = (M_PI * 2) * .25;
  f32_t camera_rot = (M_PI * 2) * .125;
  f32_t camera_dist = 100;
  f32_t camera_height = 100;
  f32_t camera_zoom = 1;

  f32_t camera_rot_rate = M_PI;
  f32_t camera_roll_rate = 256;
  f32_t camera_zoom_rate = 1;
  f32_t camera_max_zoom = 3;
  f32_t camera_min_zoom = .25;

  Vector2f camera_drag_start;
  f32_t camera_rot_start;
  f32_t camera_height_start;
  bool camera_drag = false;

  Matrix4 camera_matrix;
  

  ecs.create_system("Render", [&] (ECS* ecs) {
    Vector2f camera_drag_delta = { 0, 0 };
    if (Application.input["Primary Action"]) {
      if (!camera_drag) { // drag start
        camera_drag_start = Application.input.mouse_position_unit;
        camera_rot_start = camera_rot;
        camera_height_start = camera_height;
        camera_drag = true;
      } else { // drag continue
        camera_drag_delta = Application.input.mouse_position_unit - camera_drag_start;

        camera_rot = camera_rot_start + camera_drag_delta.x * camera_rot_rate;
        camera_height = camera_height_start + camera_drag_delta.y * camera_roll_rate;
      }
    } else if (camera_drag) { // drag end
      camera_drag = false;
    }

    if (Application.input["Zoom In"]) camera_zoom = num::max(camera_min_zoom, camera_zoom - (camera_zoom_rate / Application.frame_delta));
    else if (Application.input["Zoom Out"]) camera_zoom = num::min(camera_max_zoom, camera_zoom + (camera_zoom_rate / Application.frame_delta));

    Vector3f camera_position = { cosf(camera_rot + camera_rot_base) * camera_dist, sinf(camera_rot + camera_rot_base) * camera_dist, camera_height };
    
    using namespace ImGui;
    SetNextWindowPos({ 10, 10 }, ImGuiCond_Always, { 0, 0 });
    Begin("Camera Info", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
      Text("Position: x %f, y %f, z %f", camera_position.x, camera_position.y, camera_position.z);
      Text("- Rotation: %f", camera_rot);
      Text("- Height: %f", camera_height);
      Text("- Zoom: %f", camera_zoom);
      if (Button("Reset", { 0, 0 })) {
        camera_rot = 0;
        camera_height = 100;
        camera_zoom = 1;
      }
    } End();

    Matrix4 camera_look_matrix = Matrix4::from_look(camera_position, { 0, 0, 0 }, Constants::Vector3f::down, true).inverse();

    Vector2f half_screen = Vector2f { Application.ig_io->DisplaySize } / 2.0f;

    Vector2f half_screen_zoom = half_screen * camera_zoom;
    
    Matrix4 screen_matrix = Matrix4::from_orthographic(
      -half_screen_zoom.x, half_screen_zoom.x,
      -half_screen_zoom.y, half_screen_zoom.y,
      0.01f, 10000.0f
    );

    camera_matrix = screen_matrix * camera_look_matrix;

    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<MaterialHandle>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs->entity_count; i ++) {
      EntityHandle entity = ecs->get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = Matrix4::compose(transform);

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();
        MaterialHandle& material = entity.get_component<MaterialHandle>();
        Material& ref = *material;

        ref.set_uniform("transform", transform_matrix);
        ref.set_uniform("view", camera_matrix);

        mesh.draw_with_material(material);
      }
    }
  });


  ecs.create_system("Normal Debugger", [&] (ECS* ecs) {
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

          draw_debug_3d.line(Line3 { center, center - normal }.apply_matrix(transform_matrix), { 1, 0, 1 });
        }
      }
    }
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


    draw_debug_2d.begin_frame();
    draw_debug_3d.begin_frame();


    ecs.update();
    

    ImGui::Begin("Mouse", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Usable: %s", Application.input.mouse_usable? "true":"false");
    ImGui::Text("Pixel: %dx%d", Application.input.mouse_position_px.x, Application.input.mouse_position_px.y);
    ImGui::Text("Unit:  %fx%f", Application.input.mouse_position_unit.x, Application.input.mouse_position_unit.y);
    ImGui::End();

    ImGui::Begin("Resolution", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Internal: %dx%d", Application.resolution.x, Application.resolution.y);
    ImGui::Text("ImGui: %fx%f", Application.ig_io->DisplaySize.x, Application.ig_io->DisplaySize.y);
    ImGui::End();


    main_menu_ex();
    vendor_ex();


    draw_debug_3d.end_frame(camera_matrix);
    draw_debug_2d.end_frame(Application.resolution);


    Application.end_frame();
  }
  

  update_reports.destroy();


  ecs.destroy();
  

  draw_debug_2d.destroy();
  AssetManager.destroy();
  Application.destroy();
}