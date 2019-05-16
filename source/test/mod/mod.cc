#include "../main.hh"
#include "../examples/gui/main_menu_ex.cc"
#include "../examples/gui/vendor_ex.cc"

#include "DAE.cc"

MODULE_API void module_init ();

void module_init () {
  using namespace mod;
  using namespace ImGui;

  Application.init();

  AssetManager.load_database_from_file("./assets/asset_db.json");


  draw_debug.init();


  ECS& ecs = *new ECS;


  /* XML TEST */
  Matrix4 dae_tran = Transform3D { 0, Quaternion::from_euler(Euler { Vector3f { 0, 0, num::deg_to_rad(180) } }), 1 }.compose();

  DAE dae = DAE::from_file("./assets/meshes/animated_character_underwear.dae", dae_tran);

  RenderMesh3D dae_mesh = dae.load_mesh();

  Skeleton dae_skel = dae.load_skeleton();

  SkeletalAnimation dae_idle_anim = dae.load_animation("idle_attention");
  SkeletalAnimation dae_walk_anim = dae.load_animation("walk");
  SkeletalAnimation dae_run_anim = dae.load_animation("run");
  

  // source animation data is corrupted on first frames
  dae_idle_anim.keyframes[0].destroy();
  dae_idle_anim.keyframes.remove(0);
  dae_walk_anim.keyframes[0].destroy();
  dae_walk_anim.keyframes.remove(0);
  dae_run_anim.keyframes[0].destroy();
  dae_run_anim.keyframes.remove(0);
  
  
  dae_idle_anim.time_scale = 1400.f;
  dae_walk_anim.time_scale = 1400.f;
  dae_run_anim.time_scale = 1400.f;
  

  dae.destroy();



  struct BasicInput {
    bool enabled;

    f32_t movement_rate;
  };

  struct PointLight {
    Vector3f color;
    f32_t brightness;
  };
  
  MaterialHandle weight_check_mat = AssetManager.get<Material>("WeightCheck");
  MaterialHandle directional_light_mat = AssetManager.get<Material>("PointLight");
  MaterialHandle unlit_color_mat = AssetManager.get<Material>("UnlitColor");

  MaterialHandle unlit_color_uniform_mat = AssetManager.get<Material>("UnlitColorUniform");
  
  // MaterialSet unlit_colors = MaterialSet::from_ex("UnlitColors", Array<MaterialSetEntry>::from_elements(unlit_color_a, unlit_color_b));

  MaterialHandle unlit_texture_mat = AssetManager.get<Material>("UnlitTexture");
  RenderMesh3DHandle test_cube_mesh = AssetManager.get<RenderMesh3D>("Test Cube");

  ecs.create_component_type<Transform3D>();
  ecs.create_component_type<MaterialHandle>();
  ecs.create_component_type<MaterialInstance>();
  ecs.create_component_type<MaterialSetHandle>();
  ecs.create_component_type<MaterialSet>();
  ecs.create_component_type<RenderMesh3DHandle>();
  ecs.create_component_type<BasicInput>();
  ecs.create_component_type<PointLight>();
  ecs.create_component_type<SkeletonHandle>();
  ecs.create_component_type<SkeletalAnimationHandle>();
  ecs.create_component_type<SkeletalAnimationState>();
  ecs.create_component_type<SkeletonState>();


  EntityHandle character; {
    character = ecs.create_entity();
    character.add_component(Transform3D {
      0,
      Constants::Quaternion::identity,
      1
    });
    // character.add_component(weight_check_mat);
    character.add_component(BasicInput { true, 64 });
    character.add_component(RenderMesh3DHandle { &dae_mesh });
    // character.add_component(SkeletonHandle { &dae_skel });
    // character.add_component(SkeletalAnimationHandle { &dae_anim });
    // character.add_component(SkeletalAnimationState {
    //   &dae_anim,
    //   1.0f,
    //   1400.f
    // });
    character.add_component(SkeletonState::from_ex(
     &dae_skel,
     Array<SkeletalAnimationState>::from_elements(
       SkeletalAnimationState {
         &dae_idle_anim,
         1.f,
         1.f
       },
       SkeletalAnimationState {
         &dae_walk_anim,
         0.f,
         1.f
       },
       SkeletalAnimationState {
         &dae_run_anim,
         0.f,
         1.f
       }
     )
    ));

    MaterialInstance unlit_color_a = { unlit_color_uniform_mat };
    MaterialInstance unlit_color_b = { unlit_color_uniform_mat };
    unlit_color_a.set_uniform("color", Vector3f { 1, 0, 0 });
    unlit_color_b.set_uniform("color", Vector3f { 0, 0, 1 });
    character.add_component(MaterialSet::from_ex("UnlitColors", Array<MaterialSetEntry>::from_elements(unlit_color_a, unlit_color_b)));
  }

  EntityHandle plane; {
    plane = ecs.create_entity();
    plane.add_component(Transform3D {
      0,
      Constants::Quaternion::identity,
      100
    });
    plane.add_component(directional_light_mat);
    plane.add_component(AssetManager.get<RenderMesh3D>("Test Quad 3D"));
  }

  EntityHandle light; {
    light = ecs.create_entity();
    light.add_component(Transform3D {
      200,
      Constants::Quaternion::identity,
      10
    });
    light.add_component(test_cube_mesh);
    light.add_component(unlit_color_mat);
    light.add_component(PointLight { { 1, 1, 1 }, 1 });
  }


  ecs.create_system("Skeletal Animator Debug Controller", false, { ecs.get_component_type_by_instance_type<SkeletonState>().id }, [&] (ECS*, u32_t index) {
    SkeletonState& state = ecs.get_component<SkeletonState>(index);
    Begin("Skeletal Animator Controls");
    char el_name [128];
    for (auto [ i, animation_state ] : state.animation_states) {
      Text("Animation %zu", i);
      
      f32_t nba = animation_state.blend_alpha;
      snprintf(el_name, 128, "blend_alpha %zu", i);
      SliderFloat(el_name, &nba, 0.0f, 1.0f, "%.3f");

      snprintf(el_name, 128, "time_offset %zu", i);
      SliderFloat(el_name, &animation_state.time_offset, -1.0f, 1.0f, "%.3f");

      
      snprintf(el_name, 128, "time_scale %zu", i);
      SliderFloat(el_name, &animation_state.time_scale, 0.1f, 2.0f, "%.3f");

      if (num::flt_not_equal(nba, animation_state.blend_alpha)) {
        f32_t diff = nba - animation_state.blend_alpha;
        animation_state.blend_alpha = nba;
        if (diff > 0.0f) {
          for (auto [ j, animation_state_2 ] : state.animation_states) {
            if (i != j && animation_state_2.blend_alpha > 0.0f) animation_state_2.blend_alpha -= diff;
          }
        }
      }

      snprintf(el_name, 128, "normalized_blend_alpha %zu", i);
      SliderFloat(el_name, &animation_state.normalized_blend_alpha, -2.0f, 2.0f, "%.3f");
      snprintf(el_name, 128, "playing %zu", i);
      Checkbox(el_name, &animation_state.playing);
      snprintf(el_name, 128, "active %zu", i);
      Checkbox(el_name, &animation_state.active);
      Text("local_time %zu: %.3f", i, animation_state.local_time);
    }
    End();
  });


  ecs.create_system("Skeletal Animator", false, { ecs.get_component_type_by_instance_type<SkeletonState>().id }, [&] (ECS*, u32_t index) {
    ecs.get_component<SkeletonState>(index).update_pose(Application.frame_delta);
  });

  ecs.create_system("Skeletal Animator Debugger", false, { ecs.get_component_type_by_instance_type<SkeletonState>().id }, [&] (ECS*, u32_t index) {
    SkeletonState& state = ecs.get_component<SkeletonState>(index);

    Skeleton& skel = *state.skeleton;

    Array<Matrix4> const& pose = state.pose;

    for (auto [ i, bone ] : skel) {
      Matrix4 tran = pose[i] * bone.bind_matrix;

      Vector3f bone_pos = Vector3f{}.apply_matrix(tran);
      draw_debug.cube(false, AABB3::from_center_and_size(bone_pos, 1), { 1, 0, 1 });

      if (bone.parent_index != -1) {
        Bone& parent_bone = *skel.get_parent(bone);

        Matrix4 parent_tran = pose[bone.parent_index] * parent_bone.bind_matrix;

        Vector3f parent_pos = Vector3f{}.apply_matrix(parent_tran);

        draw_debug.line3(false, { bone_pos, parent_pos }, { 1, 1, 0 });
      }
    }
  });


  // ecs.create_system("Skeletal Animation Debugger", false, { ecs.get_component_type_by_instance_type<SkeletonHandle>().id, ecs.get_component_type_by_instance_type<SkeletalAnimationState>().id }, [&] (ECS*, u32_t index) {
  //   static Array<SkeletalKeyframeChannel> intermediate_transforms = Array<SkeletalKeyframeChannel> { 0, true };
  //   static Array<Matrix4> pose = Array<Matrix4> { 0, true };

  //   Skeleton& skel = *ecs.get_component<SkeletonHandle>(index);
  //   SkeletalAnimationState& animation_state = ecs.get_component<SkeletalAnimationState>(index);
    
  //   // if (!animation_state.active) return;

  //   if (animation_state.playing) animation_state.local_time += Application.frame_delta;

  //   // if (animation_state.blend_alpha > 0.0f) {
  //     animation_state.animation->get_pose_slerp(
  //       (animation_state.local_time + animation_state.time_offset) / animation_state.time_scale,
  //       intermediate_transforms
  //     );
  //   // }

  //   pose.clear();

  //   for (auto [ i, intermediate ] : intermediate_transforms) {
  //     pose.append(intermediate.transform.compose() * skel.bones[intermediate.target_index].inverse_bind_matrix);
  //   }

  //   for (auto [ i, bone ] : skel) {
  //     Matrix4 tran = pose[i] * bone.bind_matrix;

  //     Vector3f bone_pos = Vector3f{}.apply_matrix(tran);
  //     draw_debug.cube(false, AABB3::from_center_and_size(bone_pos, 1), { 1, 0, 1 });

  //     if (bone.parent_index != -1) {
  //       Bone& parent_bone = *skel.get_parent(bone);

  //       Matrix4 parent_tran = pose[bone.parent_index] * parent_bone.bind_matrix;

  //       Vector3f parent_pos = Vector3f{}.apply_matrix(parent_tran);

  //       draw_debug.line3(false, { bone_pos, parent_pos }, { 1, 1, 0 });
  //     }
  //   }
  // });



  ecs.create_system("MovementInput", true, { ecs.get_component_type_by_instance_type<BasicInput>().id, ecs.get_component_type_by_instance_type<Transform3D>().id }, [&] (ECS*, u32_t index) {
    BasicInput& input = ecs.get_component<BasicInput>(index);
    if (input.enabled) {
      Vector3f movement = { 0, 0, 0 };

      if (Input["Forward"]) movement.y -= 1;
      if (Input["Backward"]) movement.y += 1;

      if (Input["Left"]) movement.x -= 1;
      if (Input["Right"]) movement.x += 1;

      if (Input["Down"]) movement.z -= 1;
      if (Input["Up"]) movement.z += 1;

      movement = movement.normalize() * (input.movement_rate / Application.frame_delta);

      ecs.get_component<Transform3D>(index).position += movement;
    }
  });



  bool light_orbit = true;
  f32_t light_orbit_size = 200;
  f32_t light_orbit_time = 0;
  f32_t light_orbit_speed = M_PI * .125;

  f32_t specular_strength = 0.5f;
  f32_t specular_power = 2.0f;
  
  f32_t camera_rot_base = (M_PI * 2) * .25;
  f32_t camera_rot = 0;//(M_PI * 2) * .125;
  f32_t camera_dist = 500;
  f32_t camera_height = camera_dist;
  Vector3f camera_target = { 0, 0, 100 };
  f32_t camera_zoom = .01;

  f32_t camera_rot_rate = M_PI;
  f32_t camera_roll_rate = 512;
  f32_t camera_zoom_rate = .25;
  f32_t camera_max_zoom = 3;
  f32_t camera_min_zoom = .01;

  Vector2f camera_drag_start;
  f32_t camera_rot_start;
  f32_t camera_height_start;
  bool camera_drag = false;

  f32_t camera_near = 0.01f;
  f32_t camera_far = 2500.0f;


  Matrix4 view_matrix;
  Matrix4 projection_matrix;
  Matrix4 camera_matrix;

  ecs.create_system("Render", [&] (ECS*) {
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

    view_matrix = Matrix4::from_look(camera_position, camera_target, Constants::Vector3f::down, true).inverse();

    Vector2f half_screen = Vector2f { Application.ig_io->DisplaySize } / 2.0f;

    Vector2f half_screen_zoom = half_screen * camera_zoom;
    
    projection_matrix = Matrix4::from_orthographic(
      -half_screen_zoom.x, half_screen_zoom.x,
      -half_screen_zoom.y, half_screen_zoom.y,
      camera_near, camera_far
    );

    camera_matrix = projection_matrix * view_matrix;

    ComponentMask mask = ComponentMask {
      ecs.get_component_type_by_instance_type<Transform3D>().id,
      ecs.get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    ComponentType::ID single_mat_h_id = ecs.get_component_type_by_instance_type<MaterialHandle>().id;
    ComponentType::ID single_mat_i_id = ecs.get_component_type_by_instance_type<MaterialInstance>().id;
    ComponentType::ID mat_set_h_id = ecs.get_component_type_by_instance_type<MaterialSetHandle>().id;
    ComponentType::ID mat_set_id = ecs.get_component_type_by_instance_type<MaterialSet>().id;

    ComponentType::ID skel_state_id = ecs.get_component_type_by_instance_type<SkeletonState>().id;


    for (u32_t i = 0; i < ecs.entity_count; i ++) {
      EntityHandle entity = ecs.get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();
        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();

        Matrix4 model_matrix = transform.compose();
        
        Matrix3 normal_matrix = Matrix3::normal(view_matrix * model_matrix);

        bool has_skel_state = entity->enabled_components.match_index(skel_state_id);

        auto const update_mat_uniforms = [&] (Material& mat) {
          mat.set_uniform("m_model", model_matrix);
          mat.set_uniform("m_view", view_matrix);
          mat.set_uniform("m_projection", projection_matrix);
          mat.set_uniform("m_normal", normal_matrix);

          if (mat.enable_skinning && has_skel_state) {
            mat.set_uniform_array("bone_transforms", entity.get_component<SkeletonState>().pose);
          }

          if (mat.supports_uniform("light_pos")) {
            mat.set_uniform("light_pos", light.get_component<Transform3D>().position);
            mat.set_uniform("light_color", light.get_component<PointLight>().color * light.get_component<PointLight>().brightness);
          }
        };

        if (entity->enabled_components.match_index(single_mat_h_id)) {
          MaterialHandle& material = entity.get_component<MaterialHandle>();

          update_mat_uniforms(*material);

          mesh.draw_with_material(material);
        } else if (entity->enabled_components.match_index(single_mat_i_id)) {
          MaterialInstance& material_instance = entity.get_component<MaterialInstance>();

          update_mat_uniforms(*material_instance.base);

          mesh.draw_with_material_instance(material_instance);
        } else if (entity->enabled_components.match_index(mat_set_h_id)) {
          MaterialSetHandle& material_set = entity.get_component<MaterialSetHandle>();

          for (auto [ i, mat ] : *material_set) {
            update_mat_uniforms(mat.is_instance? *mat.instance.base : *mat.handle);
          }

          mesh.draw_with_material_set(material_set);
        } else if (entity->enabled_components.match_index(mat_set_id)) {
          MaterialSet& material_set = entity.get_component<MaterialSet>();

          for (auto [ i, mat ] : material_set) {
            update_mat_uniforms(mat.is_instance? *mat.instance.base : *mat.handle);
          }

          mesh.draw_with_material_set(&material_set);
        }
      }
    }
  });



  ecs.create_system("Face Normal Debugger", [&] (ECS*) {
    ComponentMask mask = ComponentMask {
      ecs.get_component_type_by_instance_type<Transform3D>().id,
      ecs.get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs.entity_count; i ++) {
      EntityHandle entity = ecs.get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = transform.compose();

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();

        for (auto [ i, face ] : mesh.faces) {
          Triangle tri = { mesh.positions[face.x], mesh.positions[face.y], mesh.positions[face.z] };

          Vector3f normal = tri.normal();
          Vector3f center = tri.center().apply_matrix(transform_matrix);

          draw_debug.line3(Line3 { center, center + normal * 5 }, { 1, 0, 1 });
        }
      }
    }
  });



  ecs.create_system("Vertex Normal Debugger", [&] (ECS*) {
    ComponentMask mask = ComponentMask {
      ecs.get_component_type_by_instance_type<Transform3D>().id,
      ecs.get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };


    for (u32_t i = 0; i < ecs.entity_count; i ++) {
      EntityHandle entity = ecs.get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = transform.compose();

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();

        for (auto [ i, normal ] : mesh.normals) {
          Vector3f pos = mesh.positions[i].apply_matrix(transform_matrix);
          
          draw_debug.line3(Line3 { pos, pos + normal * 5 }, { 0, 1, 1 });
        }
      }
    }
  });



  ecs.create_system("Face Edge Debugger", [&] (ECS*) {
    ComponentMask mask = ComponentMask {
      ecs.get_component_type_by_instance_type<Transform3D>().id,
      ecs.get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    for (u32_t i = 0; i < ecs.entity_count; i ++) {
      EntityHandle entity = ecs.get_handle(i);

      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& transform = entity.get_component<Transform3D>();

        Matrix4 transform_matrix = transform.compose();

        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();
        
        for (auto [ i, face ] : mesh.faces) {
          Triangle tri = { mesh.positions[face.x], mesh.positions[face.y], mesh.positions[face.z] };

          draw_debug.line3(Line3 { tri.a, tri.b }.apply_matrix(transform_matrix), { 1, 1, 0 });
          draw_debug.line3(Line3 { tri.b, tri.c }.apply_matrix(transform_matrix), { 1, 1, 0 });
          draw_debug.line3(Line3 { tri.c, tri.a }.apply_matrix(transform_matrix), { 1, 1, 0 });
        }
      }
    }
  });



  // ecs.create_system("Skeleton Debugger", [&] (ECS*) {
  //   ComponentMask mask = ComponentMask {
  //     ecs.get_component_type_by_instance_type<Transform3D>().id,
  //     ecs.get_component_type_by_instance_type<SkeletonHandle>().id
  //   };

  //   ComponentType::ID animation_type = ecs.get_component_type_by_instance_type<SkeletalAnimationHandle>().id;

  //   static Array<pair_t<bool, s32_t>> settings = Array<pair_t<bool, s32_t>> { 0, true };

  //   static auto const get_settings = [&] (size_t index) -> auto& {
  //     while (settings.count <= index) settings.append({ true, 1 });
  //     return settings[index];
  //   };

  //   Begin("Bone Transforms");
  //   for (u32_t i = 0; i < ecs.entity_count; i ++) {
  //     EntityHandle entity = ecs.get_handle(i);

  //     if (!entity->enabled_components.match_subset(mask)) continue;

  //     Transform3D& transform = entity.get_component<Transform3D>();

  //     Matrix4 transform_matrix = transform.compose();

  //     Skeleton& skeleton = *entity.get_component<SkeletonHandle>();

  //     char entity_ident [128];
  //     snprintf(entity_ident, 128, "Entity %u", entity.id);
  //     bool do_menu = CollapsingHeader(entity_ident);

  //     SkeletalKeyframe keyframe;
  //     auto& anim_settings = get_settings(i);
  //     if (do_menu) Checkbox("Enable animation", &anim_settings.a);
  //     bool do_anim = anim_settings.a && entity->enabled_components[animation_type];
  //     s32_t& selection = anim_settings.b;

  //     if (do_anim) {
  //       SkeletalAnimation& animation = *entity.get_component<SkeletalAnimationHandle>();
    
  //       if (do_menu) SliderInt("Keyframe", &selection, 1, animation.keyframes.count - 1);

  //       keyframe = animation.keyframes[selection];
  //     }
      
  //     if (do_menu) Text("%zu total bones", skeleton.bones.count);

  //     for (auto [ j, bone ] : skeleton) {
  //       bool selected = false;
        
  //       if (do_menu) {
  //         Text("%zu - %s", j, bone.name.value);
  //         selected = IsItemHovered();
  //         if (selected) SetMouseCursor(ImGuiMouseCursor_Hand);
  //       }


  //       Transform3D* anim_tran_ptr = do_anim? keyframe.get_bone_transform(j) : NULL;
  //       Transform3D anim_tran = anim_tran_ptr != NULL? *anim_tran_ptr : bone.bind_matrix.decompose();
  //       Matrix4 anim_matrix = anim_tran.compose();
  //       Matrix4 f_matrix = transform_matrix * anim_matrix;
  //       Vector3f bone_pos = Vector3f{}.apply_matrix(f_matrix);

  //       draw_debug.cube(false, AABB3::from_center_and_size(bone_pos, selected? 10 : 1), selected? Vector3f { 0, 1, 1  } : Vector3f { 1, 0, 1 });

  //       Bone* parent_bone = skeleton.get_parent(bone);
  //       if (parent_bone != NULL) {
  //         Transform3D* parent_anim_tran_ptr = do_anim? keyframe.get_bone_transform(bone.parent_index) : NULL;
  //         Transform3D parent_anim_tran = parent_anim_tran_ptr != NULL? *parent_anim_tran_ptr : parent_bone->bind_matrix.decompose();
  //         Matrix4 parent_anim_matrix = parent_anim_tran.compose();
  //         Matrix4 parent_fmatrix = transform_matrix * parent_anim_matrix;
  //         Vector3f parent_pos = Vector3f{}.apply_matrix(parent_fmatrix);
  //         draw_debug.line3(false, Line3 { parent_pos, bone_pos }, { 1, 1, 0 });
  //       }

  //       if (do_menu) {
  //         Transform3D f_tran = f_matrix.decompose();
          
  //         Text("- Position: %.3f, %.3f, %.3f", f_tran.position.x, f_tran.position.y, f_tran.position.z);
  //         Text("- Rotation: %.3f, %.3f, %.3f, %.3f", f_tran.rotation.x, f_tran.rotation.y, f_tran.rotation.z, f_tran.rotation.w);
  //         Text("- Scale: %.3f, %.3f, %.3f", f_tran.scale.x, f_tran.scale.y, f_tran.scale.z);
  //       }
  //     }
  //   }
  //   End();
  // });


  
  struct Hit {
    f32_t distance;
    EntityHandle entity;
  };

  Array<Hit> hits;

  ecs.create_system("Object Picker", [&] (ECS*) {
    ComponentMask mask = ComponentMask {
      ecs.get_component_type_by_instance_type<Transform3D>().id,
      ecs.get_component_type_by_instance_type<RenderMesh3DHandle>().id
    };

    Vector3f origin_n = { Input.mouse_position_unit.x, Input.mouse_position_unit.y, -.99 };
    Vector3f end_n = { Input.mouse_position_unit.x, Input.mouse_position_unit.y, .9 };

    Matrix4 inverse_projection_matrix = projection_matrix.inverse();
    Matrix4 inverse_view_matrix = view_matrix.inverse();

    Vector3f origin = origin_n.unproject(inverse_view_matrix, inverse_projection_matrix);

    Vector3f direction = (end_n.unproject(inverse_view_matrix, inverse_projection_matrix) - origin).normalize();

    Ray3 ray = { origin, direction };

    hits.clear();

    for (u32_t i = 0; i < ecs.entity_count; i ++) {
      EntityHandle entity = ecs.get_handle(i);
      if (entity->enabled_components.match_subset(mask)) {
        Transform3D& tran = entity.get_component<Transform3D>();
        RenderMesh3D& mesh = *entity.get_component<RenderMesh3DHandle>();
        Matrix4 world_matrix = tran.compose();
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
      AABB3 b = m.get_aabb().apply_matrix(t.compose());

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

  ecs.get_system_by_name("Face Normal Debugger").enabled = false;
  ecs.get_system_by_name("Vertex Normal Debugger").enabled = false;
  ecs.get_system_by_name("Face Edge Debugger").enabled = false;
  ecs.get_system_by_name("Object Picker").enabled = false;
  

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

    Begin("Debuggers");
    Checkbox("Face Normals", &ecs.get_system_by_name("Face Normal Debugger").enabled);
    Checkbox("Vertex Normals", &ecs.get_system_by_name("Vertex Normal Debugger").enabled);
    Checkbox("Face Edges", &ecs.get_system_by_name("Face Edge Debugger").enabled);
    Checkbox("Object Picker", &ecs.get_system_by_name("Object Picker").enabled);
    Checkbox("Animator Controls", &ecs.get_system_by_name("Skeletal Animator Debug Controller").enabled);
    Checkbox("Animated Skeleton", &ecs.get_system_by_name("Skeletal Animator Debugger").enabled);

    // MaterialHandle material_options [] = {
    //   weight_check_mat,
    //   directional_light_mat,
    //   unlit_texture_mat
    // };
    
    // if (BeginCombo("Mesh Material", AssetManager.get_name_from_id<Material>(character.get_component<MaterialHandle>().get_id()))) {
    //   for (u8_t i = 0; i < m_array_length(material_options); i ++) {
    //     bool is_selected = character.get_component<MaterialHandle>().get_id() == material_options[i].get_id();

    //     if (Selectable(AssetManager.get_name_from_id<Material>(material_options[i].get_id()), is_selected)) {
    //       character.get_component<MaterialHandle>() = material_options[i];
    //     }

    //     if (is_selected) SetItemDefaultFocus();
    //   }

    //   EndCombo();
    // }
    End();

    

    main_menu_ex();
    // vendor_ex();


    draw_debug.end_frame(camera_matrix, Application.resolution);


    Application.end_frame();
  }

  dae_mesh.destroy();
  dae_skel.destroy();
  dae_idle_anim.destroy();
  dae_walk_anim.destroy();
  dae_run_anim.destroy();

  update_reports.destroy();
  ecs.destroy();
  draw_debug.destroy();
  Application.destroy();
}