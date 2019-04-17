#include "..\main.hh"
#include "..\..\source\extern\imgui\implementation\main.cpp"
#include "..\..\source\extern\ImGuiColorTextEdit\main.cpp"

using namespace mod;



MODULE_API void module_init () {
  ECS& ecs = *new ECS;

  struct Transform {
    Vector2f position;
    Vector2f velocity;
  };

  struct Sprite {
    u32_t id;
  };

  struct Printable { bool print; };

  ComponentType::ID TransformID = ecs.create_component_type<Transform>();
  ComponentType::ID SpriteID = ecs.create_component_type<Sprite>();
  ComponentType::ID PrintableID = ecs.create_component_type<Printable>();

  num::seed_random();

  for (size_t i = 0; i < 64000; i ++) {
    auto e = ecs.create_entity();

    e.create_component<Transform>(
      Vector2f {
        num::random<f32_t>(),
        num::random<f32_t>()
      },
      Vector2f {
        num::random_range<f32_t>(-1.0f, 1.0f),
        num::random_range<f32_t>(-1.0f, 1.0f)
      }
    );

    e.create_component<Sprite>(num::random_range<u32_t>(0, 3));

    if (num::random<f32_t>() > 0.8f) {
      auto& c = *(Printable*) e.create_component_by_name("Printable");
      c.print = true;
    }
  }

  ecs.create_system("Transform", true, { TransformID }, [=] (ECS* ecs, u32_t index) {
    Transform& t = ecs->get_component<Transform>(index);
    t.position += t.velocity;
  });

  ecs.create_system_before("Transform", "Render", [=] (ECS* ecs) {
    static Bitmask filter = { TransformID, SpriteID, 9 };

    for (u32_t i = 0; i < ecs->entity_count; i += 1000) {
      Entity& entity = ecs->entities[i];

      if (entity.enabled_components.match_subset(filter)) {
        printf("Entity %u: Sprite %u", i, ecs->get_component<Sprite>(i).id);

        if (entity.enabled_components.match_index(PrintableID)) {
          Transform& t = ecs->get_component<Transform>(i);
          printf(" Position: %g, %g", t.position.x, t.position.y);
        }
        putchar('\n');
      }
    }
  });

  if (ig_demo_init()) {
    text_editor_demo_init();

    while (ig_demo_loop_start()) {
      ig_demo_loop_body();
      text_editor_demo_loop();
      ig_demo_loop_end();
      
      ecs.update();
    }

    ig_demo_end();
  }

  ecs.destroy();
}