#include "../main.hh"
#include "../examples/gui/main_menu_ex.cc"
#include "../examples/gui/vendor_ex.cc"



MODULE_API void module_init () {
  using namespace mod;


  Application.default_controls.append({ "Mod Control", { { }, { Keycode::X }, { } } });
  Application.create();


  AssetManager.create();
  AssetManager.load_database_from_file("./assets/asset_db.json");


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

  ecs.create_system("Render", [&] (ECS* ecs) {
    using namespace ImGui;
    Vector3f camera_position = { cosf(camera_rot + camera_rot_base) * camera_dist, sinf(camera_rot + camera_rot_base) * camera_dist, camera_height };
    
    SetNextWindowPos({ 10, 10 }, ImGuiCond_Always, { 0, 0 });
    Begin("Camera Info", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
      Text("Position: x %f, y %f, z %f", camera_position.x, camera_position.y, camera_position.z);
      Text("- Rotation: %f", camera_rot);
      Text("- Height: %f", camera_height);
      Text("- Distance: %f", camera_dist);
      if (Button("Reset", { 0, 0 })) {
        camera_rot = 0;
        camera_dist = 100;
        camera_height = 100;
      }
    } End();

    Matrix4 camera_look_matrix = Matrix4::from_look(camera_position, { 0, 0, 0 }, Constants::Vector3f::down, true).inverse();

    Vector2f half_screen = Vector2f { Application.ig_io->DisplaySize } / 2.0f;
    
    Matrix4 screen_matrix = Matrix4::from_orthographic(
      -half_screen.x, half_screen.x,
      -half_screen.y, half_screen.y,
      0.01f, 10000.0f
    );

    Matrix4 camera_matrix = screen_matrix * camera_look_matrix;

    ComponentMask mask = ComponentMask {
      ecs->get_component_type_by_instance_type<Transform3D>().id,
      ecs->get_component_type_by_instance_type<MaterialHandle>().id,
      ecs->get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs->entity_count; i ++ ) {
      EntityHandle entity =  ecs->get_handle(i);

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


    ecs.update();
    

    main_menu_ex();
    vendor_ex();


    Application.end_frame();
  }
  

  update_reports.destroy();


  ecs.destroy();


  AssetManager.destroy();
  Application.destroy();
}