#include "..\main.hh"
#include "..\..\source\extern\imgui\implementation\main.cpp"
#include "..\..\source\extern\ImGuiColorTextEdit\main.cpp"

using namespace mod;



MODULE_API void module_init () {
  AssetManager.create();
  Application.create();
  ECS& ecs = *new ECS;

  AssetManager.load_database_from_file("./assets/asset_db.json");
  
  MaterialHandle ma = AssetManager.get<Material>("Minimal3DA");

  RenderMesh3DHandle c = AssetManager.get<RenderMesh3D>("Test Cube");
  RenderMesh3DHandle q = AssetManager.get<RenderMesh3D>("Test Quad 3D");


  ecs.create_component_type<Transform3D>();
  ecs.create_component_type<MaterialHandle>();
  ecs.create_component_type<RenderMesh3DHandle>();

  EntityHandle cube; {
    cube = ecs.create_entity();
    cube.add_component(Transform3D {
      { 0, 0, 0 },
      Constants::Quaternion::identity,
      { 100, 100, 100 }
    });
    cube.add_component(ma);
    cube.add_component(c);
  }

  EntityHandle plane; {
    plane = ecs.create_entity();
    plane.add_component(Transform3D {
      { 0, 0, -50 },
      Constants::Quaternion::identity,
      { 200, 200, 1 }
    });
    plane.add_component(ma);
    plane.add_component(q);
  }


  f32_t camera_rot_base = (M_PI * 2) * .25;
  f32_t camera_rot = (M_PI * 2) * .125;
  f32_t camera_dist = 100;
  f32_t camera_height = 100;


  ecs.create_system("Render", [&] (ECS* ecs) {
    Vector3f camera_position = { cosf(camera_rot + camera_rot_base) * camera_dist, sinf(camera_rot + camera_rot_base) * camera_dist, camera_height };

    ImGui::SetNextWindowPos({ 10, 10 }, ImGuiCond_Always, { 0, 0 });
    ImGui::Begin("Camera Info", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
      ImGui::Text("Position: x %f, y %f, z %f", camera_position.x, camera_position.y, camera_position.z);
      ImGui::Text("- Rotation: %f", camera_rot);
      ImGui::Text("- Height: %f", camera_height);
      ImGui::Text("- Distance: %f", camera_dist);
      if (ImGui::Button("Reset", { 0, 0 })) {
        camera_rot = 0;
        camera_dist = 100;
        camera_height = 100;
      }
    } ImGui::End();

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
  
  text_editor_demo_init();
  
  Array<WatchedFileReport> update_reports;
  
  while (true) {
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

    SDL_Event event;

    bool quit = false;

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type) {
        case SDL_QUIT: {
          quit = true;
        } break;

        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym) {
            case 'w': camera_dist -= 10.0f; break;
            case 's': camera_dist += 10.0f; break;
            case 'a': camera_rot -= 0.1f; break;
            case 'd': camera_rot += 0.1f; break;
            case 'c': camera_height -= 10.0f; break;
            case ' ': camera_height += 10.0f; break;
            case 'i': cube.get_component<Transform3D>().position.y -= 1.0f; break;
            case 'k': cube.get_component<Transform3D>().position.y += 1.0f; break;
            case 'j': cube.get_component<Transform3D>().position.x -= 1.0f; break;
            case 'l': cube.get_component<Transform3D>().position.x += 1.0f; break;
            case '[': cube.get_component<Transform3D>().position.z += 1.0f; break;
            case '\'': cube.get_component<Transform3D>().position.z -= 1.0f; break;
          }
        } break;
        
        default: break;
      }
    }

    if (quit) break;

    Application.begin_frame();

    ecs.update();
    
    ImGui::PushFont(Application.fonts[1]);
    ig_demo_loop_body();
    text_editor_demo_loop();
    ImGui::PopFont();

    Application.end_frame();
  }
  
  ecs.destroy();
  Application.destroy();
  AssetManager.destroy();
}