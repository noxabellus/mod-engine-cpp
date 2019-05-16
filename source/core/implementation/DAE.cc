#include "../include/DAE.hh"



namespace mod {
  void DAEIVertex::set_attributes (s64_t in_normal, s64_t in_uv, s64_t in_color) {
    normal = in_normal;
    uv = in_uv;
    color = in_color;
    
    set = true;
  }

  bool DAEIVertex::has_same_attributes (s64_t test_normal, s64_t test_uv, s64_t test_color) const {
    return normal == test_normal
        && uv == test_uv
        && color == test_color;
  }

  void DAEIBone::add_child (DAEIBone const& bone) {
    children.append(bone);
    DAEIBone& new_bone = children.last();
    new_bone.parent = this;
    for (auto [ i, child ] : new_bone.children) child.parent = &new_bone;
  }

  void DAEIBone::remove_child (DAEIBone& bone) {
    s64_t index = children.get_index(bone);
    m_asset_assert(index != -1, name.value, "Cannot get index of child bone %s to remove it", bone.name.value);
    children.remove(index);
  }

  void DAEIBone::calculate_bind (Matrix4 const& transform) {
    if (parent == NULL) bind_matrix = transform * base_matrix;
    else bind_matrix = parent->bind_matrix * base_matrix;

    inverse_bind_matrix = bind_matrix.inverse();

    for (auto [ i, child ] : children) child.calculate_bind(transform);
  }

  Transform3D DAEIBone::final_transform () const {
    if (parent != NULL) return (parent->inverse_bind_matrix * bind_matrix).decompose();
    else return bind_matrix.decompose();
  }

  DAEIBone DAEIBone::process (DAE const& dae, XMLItem& joint) {
    DAEIBone bone { 256 };
    
    bone.sid = joint.get_attribute("sid").value;
    bone.id = joint.get_attribute("id").value;

    s64_t origin_index = dae.get_bone_index(bone.sid);
    
    joint.asset_assert(origin_index != -1, "Cannot find index for bone '%s'", joint.get_attribute("sid").value.value);

    bone.origin_index = origin_index;

    bone.name = joint.get_attribute("name").value.clone();

    // printf("Binding bone %s to origin_index %u\n", bone.name.value, bone.origin_index);

    Matrix4 matrix;

    XMLItem& matrix_item = joint.first_named("matrix");
    String& matrix_text = matrix_item.get_text();

    char* base = matrix_text.value;
    char* end = NULL;
    for (size_t j = 0; j < 16; j ++) {
      matrix[j] = strtof(base, &end);
      matrix_item.asset_assert(end != NULL && base != end, "Not enough elements in matrix");
      base = end;
    }
    
    bone.base_matrix = matrix.transpose();

    for (size_t i = 0; i < joint.count_of_name("node"); i ++) {
      XMLItem& child_joint = joint.nth_named(i, "node");
      XMLAttribute* type_attribute = child_joint.get_attribute_pointer("type");

      if (type_attribute != NULL && type_attribute->value == "JOINT") {
        bone.add_child(process(dae, child_joint));
      }
    }

    return bone;
  }

  bool DAEIBone::filter (std::function<bool (DAEIBone const&)> filter) {
    if (parent != NULL) {
      if (filter(*this)) {
        Array<DAEIBone> my_children = children;
        DAEIBone* my_parent = parent;
        String my_name = name;
        my_parent->remove_child(*this);
        for (auto [ i, child ] : my_children) {
          my_parent->add_child(child);
        }
        my_name.destroy();
        my_children.destroy();
        return true;
      } else {
        for (auto [ i, child ] : children) if (child.filter(filter)) return true;
      }
    } else {
      if (children.count > 1) {
        for (auto [ i, child ] : children) if (child.filter(filter)) return true;
      } else if (filter(*this)) {
        DAEIBone child = children[0];
        origin_index = child.origin_index;
        sid = child.sid;
        id = child.id;
        name.destroy();
        name = child.name;
        children.destroy();
        children = child.children;
        for (auto [ i, new_child ] : children) new_child.parent = this;
        base_matrix = child.base_matrix;
        bind_matrix = child.bind_matrix;
        inverse_bind_matrix = child.inverse_bind_matrix;
      }
    }

    return false;
  }

  void DAEIBone::collapse (Array<DAEBoneBinding>& out_bones, s32_t parent_index) const {
    size_t index = out_bones.count;

    out_bones.append(DAEBoneBinding {
      origin_index,
      id,
      sid,

      name,
      parent_index,
      final_transform()
    });
    
    for (auto [ i, child ] : children) child.collapse(out_bones, index);
  }

  bool DAEIBone::traverse_cond (std::function<bool (DAEIBone const&)> callback) const {
    if (callback(*this)) {
      for (auto [ i, child ] : children) {
        if (!child.traverse_cond(callback)) return false;
      }

      return true;
    } else return false;
  }
  
  void DAEIBone::traverse (std::function<void (DAEIBone const&)> callback) const {
    callback(*this);

    for (auto [ i, child ] : children) child.traverse(callback);
  }

  void DAEIBone::destroy () {
    for (auto [ i, child ] : children) child.destroy();
    children.destroy();
  }


  

  DAEIChannel& DAEIKeyframe::get_channel (u32_t target_index) const {
    for (auto [ i, channel ] : channels) {
      if (channel.target_index == target_index) return channel;
    }

    dae->xml.asset_error("Cannot find DAEIChannel with target index %" PRIu32, target_index);
  }



  DAEIKeyframe& DAEIAnimation::get_keyframe_for_time (f32_t time) {
    for (auto [ i, keyframe ] : keyframes) {
      if (num::almost_equal(keyframe.time, time)) return keyframe;
    }

    keyframes.append({ dae, time, { } });

    return keyframes.last();
  }

  DAEIAnimation DAEIAnimation::process (DAE const* dae, DAEIBone const& root_ibone, DAEAnimClip& clip) {
    DAEIAnimation animation { clip.name, dae, clip.end - clip.start, { } };

    for (auto [ i, channel_id ] : clip.channel_ids) {
      DAEAnimChannel& channel = dae->get_anim_channel(channel_id);

      s64_t bone_index = DAE::get_origin_bone_index_from_node_id(root_ibone, channel.target_id);
      
      if (bone_index == -1) {
        // printf("Warning: Failed to find bone index for animation target id '%s'\n", channel.target_id.value);
        continue;
      }

      DAEAnimSampler& sampler = dae->get_anim_sampler(channel.sampler_id);

      DAEInput& input = dae->get_sampler_input(sampler, "INPUT");
      DAEInput& output = dae->get_sampler_input(sampler, "OUTPUT");

      DAEAccessor& input_accessor = dae->get_accessor(input.source_id);
      DAEAccessor& output_accessor = dae->get_accessor(output.source_id);

      sampler.origin->asset_assert(
        input_accessor.count == output_accessor.count,
        "Cannot process sampler sources, source accessors are incongruent (input count %zu, output count %zu)",
        input_accessor.count, output_accessor.count
      );

      sampler.origin->asset_assert(
        input_accessor.stride == 1,
        "Cannot process sampler input, source accessor does not have a stride of 1 (%zu)",
        input_accessor.stride
      );

      sampler.origin->asset_assert(
        output_accessor.stride == 16,
        "Cannot process sampler output, source accessor does not have a stride of 16 (%zu)",
        output_accessor.stride
      );

      DAESource& input_source = dae->get_source(input_accessor.source_id);
      DAESource& output_source = dae->get_source(output_accessor.source_id);

      for (size_t i = 0; i < input_accessor.count; i ++) {
        f32_t time = input_source.floats[input_accessor.offset + i * input_accessor.stride];

        Matrix4 matrix;

        for (auto [ j, e ] : matrix) {
          e = output_source.floats[output_accessor.offset + i * output_accessor.stride + j];
        }

        DAEIKeyframe& keyframe = animation.get_keyframe_for_time(time);

        keyframe.channels.append(DAEIChannel {
          static_cast<u32_t>(bone_index),
          matrix.transpose()
        });
      }
    }

    return animation;
  }
  


  void DAEIAnimation::calculate_bind (DAEIBone const& root_ibone, Matrix4 const& transform) const {
    for (pair_t<size_t, DAEIKeyframe&> pair : keyframes) {
      DAEIKeyframe& keyframe = pair.b;

      root_ibone.traverse([&] (DAEIBone const& ibone) {
        DAEIChannel& channel = keyframe.get_channel(ibone.origin_index);

        if (ibone.parent == NULL) channel.bind_matrix = transform * channel.base_matrix;
        else {
          DAEIChannel& parent_channel = keyframe.get_channel(ibone.parent->origin_index);

          channel.bind_matrix = parent_channel.bind_matrix * channel.base_matrix;
        }
      });
    }
  }




  void DAEIAnimation::filter (DAEBoneBindingList const& binding_list) const {
    auto const inner_filter = [&] (DAEIKeyframe& keyframe) -> bool {
      for (auto [ j, channel ]: keyframe.channels) {
        if (channel.filtered) continue;
        
        bool found = false;
        
        for (auto [ k, binding ] : binding_list) {
          if (binding.origin_index == channel.target_index) {
            channel.target_index = k;
            channel.filtered = true;
            found = true;
            break;
          }
        }

        if (!found) {
          keyframe.channels.remove(j);
          return true;
        }
      }

      return false;
    };

    for (auto [ i, keyframe ] : keyframes) {
      bool cont = false;
      do cont = inner_filter(keyframe);
      while (cont);
    }
  }

  void DAEIAnimation::collapse (DAEBoneBindingList const& binding_list, Array<SkeletalKeyframe>& out_array) const {
    for (auto [ i, keyframe ] : keyframes) {
      Array<SkeletalKeyframeChannel> out_transforms;

      for (auto [ j, channel ] : keyframe.channels) {
        out_transforms.append({
          channel.target_index,
          channel.final_transform(keyframe, binding_list)
        });
      }

      out_array.append(SkeletalKeyframe {
        keyframe.time,
        out_transforms
      });
    }
  }


  
  Transform3D DAEIChannel::final_transform (DAEIKeyframe const& owner, DAEBoneBindingList const& binding_list) const {
    DAEBoneBinding& binding = binding_list[target_index];

    if (binding.parent_index != -1) {
      DAEIChannel& parent_channel = owner.get_channel(binding.parent_index);
      return (parent_channel.bind_matrix.inverse() * bind_matrix).decompose();
    } else return bind_matrix.decompose();
  }


  void DAEIAnimation::destroy () {
    for (auto [ i, keyframe ] : keyframes) keyframe.channels.destroy();
    keyframes.destroy();
  }





  bool DAEBoneBindingList::traverse_cond (std::function<bool (size_t, DAEBoneBinding const&)> callback, size_t index) const {
    if (callback(index, bindings[index])) {
      for (auto [ i, binding ] : bindings) {
        if (binding.parent_index == index) {
          if (!traverse_cond(callback, i)) return false;
        }
      }

      return true;
    } else return false;
  }

  void DAEBoneBindingList::traverse (std::function<void (size_t, DAEBoneBinding const&)> callback, size_t index) const {
    callback(index, bindings[index]);

    for (auto [ i, binding ] : bindings) {
      if (binding.parent_index == index) traverse(callback, i);
    }
  }

  void DAEBoneBindingList::collapse (Array<Bone>& out_array) const {
    for (auto [ i, binding ] : bindings) {
      out_array.append({
        binding.name,
        binding.parent_index,
        binding.base_transform
      });
    }
  }





  std::function<bool (DAEIBone const&)> DAE::std_bone_filter = [] (DAEIBone const& bone) { return bone.name.starts_with("IK") || bone.name.starts_with("TK"); };
  

  /* Create a new DAE from an XML, taking ownership of the XML */
  DAE::DAE (XML const& in_xml, Matrix4 const& in_transform, bool apply_bone_filter, std::function<bool (DAEIBone const&)> bone_filter)
  : xml(in_xml)
  , root(in_xml.first_named("COLLADA"))
  , transform(in_transform)
  {
    try {
      XMLItem& geometries = root.first_named("library_geometries");

      XMLItem& geometry = geometries.first_named("geometry");

      if (geometries.count_of_name("geometry") > 1) printf(
        "Warning: DAE loader currently only supports single geometries, only the first (named '%s') will be loaded\n",
        geometry.get_attribute("name").value.value
      );


      {
        XMLItem& in_mesh = geometry.first_named("mesh");

        if (geometry.count_of_name("mesh") > 1) printf(
          "Warning: DAE loader currently only supports single mesh, only the first will be loaded\n"
        );

        mesh = &in_mesh;
        
        gather_base_data(in_mesh);
      }


      {
        XMLItem& controllers = root.first_named("library_controllers");

        XMLItem& controller = controllers.first_named("controller");

        XMLItem& in_skin = controller.first_named("skin");

        if (controllers.count_of_name("controller") > 1) printf(
          "Warning: DAE loader currently only supports single controllers, only the first (named '%s') will be loaded\n",
          controller.get_attribute("name").value.value
        );

        if (controller.count_of_name("skin") > 1) printf(
          "Warning: DAE loader currently only supports single skin, only the first will be loaded\n"
        );

        in_skin.asset_assert(
          geometry.get_attribute("id").value == in_skin.get_attribute("source").value.value + 1,
          "The skin found does not have a source attribute ('%s') matching the geometry id ('%s')",
          geometry.get_attribute("id").value.value,
          in_skin.get_attribute("source").value.value
        );


        skin = &in_skin;

        gather_base_data(in_skin);
      }


      {
        size_t vertex_binding_count = mesh->count_of_name("vertices");

        vertex_bindings = { vertex_binding_count };

        for (size_t i = 0; i < vertex_binding_count; i ++) {
          XMLItem& vertices = mesh->nth_named(i, "vertices");
          XMLItem& input = vertices.first_named("input");

          vertex_bindings.append({
            vertices.get_attribute("id").value, // borrowing string here, destroyed by XML
            input.get_attribute("source").value // borrowing string here, destroyed by XML
          });
       }
      }

      {
        size_t triangles_count = mesh->count_of_name("triangles");

        bool polylist_mode;

        if (triangles_count == 0) {
          polylist_mode = true;
          triangles_count = mesh->count_of_name("polylist");
          mesh->asset_assert(triangles_count != 0, "Expected at least one element 'triangles' or 'polylist'");
        } else {
          polylist_mode = false;
        }

        triangles_list = { triangles_count };

        for (size_t i = 0; i < triangles_count; i ++) {
          XMLItem& triangles = polylist_mode? mesh->nth_named(i, "polylist") : mesh->nth_named(i, "triangles");

          Array<DAEInput> inputs = gather_inputs(triangles);

          size_t count = strtoumax(triangles.get_attribute("count").value.value, NULL, 10);
          
          if (polylist_mode) {
            XMLItem& vcount = triangles.first_named("vcount");

            char* base = vcount.get_text().value;
            char* end = NULL;

            for (size_t k = 0; k < count; k ++) {
              u32_t x = strtoul(base, &end, 10);
              vcount.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at vcount %zu (expected %zu vcounts)", k, count);
              base = end;
              vcount.asset_assert(x == 3, "DAE parser only supports polylists in triangles mode, but found vcount != 3 (%" PRIu32 ")", x);
            }
          }
          
          size_t advance = inputs.reduce(static_cast<size_t>(0), [] (size_t& acc, DAEInput const& input) {
            acc = num::max(acc, input.offset);
          }) + 1;

          size_t indices_count = count * advance * 3;

          Array<u32_t> indices = { indices_count };
          
          XMLItem& p = triangles.first_named("p");
          char* base = p.get_text().value;
          char* end = NULL;

          for (size_t k = 0; k < indices_count; k ++) {
            u32_t index = strtoul(base, &end, 10);
            p.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at index %zu (expected %zu indices)", k, indices_count);
            base = end;
            indices.append(index);
          }

          triangles_list.append({
            &triangles,
            advance,
            inputs,
            indices
          });
        }
      }


      {
        XMLItem& joints = skin->first_named("joints");
        joint_data = { &joints };

        joint_data.inputs = gather_inputs(joints);
      }

      {
        XMLItem& weights = skin->first_named("vertex_weights");
        weight_data = { &weights };

        weight_data.inputs = gather_inputs(weights);
        weight_data.count = strtoumax(weights.get_attribute("count").value.value, NULL, 10);

        {
          XMLItem& vcount = weights.first_named("vcount");
          char* base = vcount.get_text().value;
          char* end = NULL;

          for (size_t i = 0; i < weight_data.count; i ++) {
            u32_t num = strtoul(base, &end, 10);
            vcount.asset_assert(end != NULL && end != base, "Less values than expected or other parsing error at index %zu (expected %zu values)", i, weight_data.count);
            weight_data.vcount.append(num);
            base = end;
          }
        }

        {
          XMLItem v = weights.first_named("v");
          char* base = v.get_text().value;
          char* end = NULL;

          while (*base != '<') { // TODO fix this, < is not gonna come up
            u32_t index = strtoul(base, &end, 10);

            if (base == end || end == NULL) break;

            base = end;
            
            weight_data.indices.append(index);
          }
        }
      }

      DAEIBone root_ibone; {
        XMLItem& lib_vis_scenes = root.first_named("library_visual_scenes");

        XMLItem& scene = lib_vis_scenes.first_named("visual_scene");

        lib_vis_scenes.asset_assert(lib_vis_scenes.count_of_name("visual_scene") == 1, "Expected exactly one visual scene");

        XMLItem* armature_ptr = scene.find_pointer_by_attribute_value("node", "id", "Armature");

        if (armature_ptr == NULL) return;

        XMLItem& root_joint = armature_ptr->find_by_attribute_value("node", "type", "JOINT");

        root_ibone = DAEIBone::process(*this, root_joint);

        root_ibone.calculate_bind(transform);
      }


      {
        XMLItem* lib_anims_ptr = root.first_named_pointer("library_animations");

        if (lib_anims_ptr != NULL) {
          size_t anim_channel_count = lib_anims_ptr->count_of_name("animation");

          for (size_t i = 0; i < anim_channel_count; i ++) {
            XMLItem& anim = lib_anims_ptr->nth_named(i, "animation");

            gather_base_data(anim);

            XMLItem& sampler = anim.first_named("sampler");

            anim_samplers.append({
              &sampler,
              sampler.get_attribute("id").value,
              gather_inputs(sampler)
            });

            XMLItem& channel = anim.first_named("channel");

            String const& ctid = channel.get_attribute("target").value;

            String target_id = { ctid.value, ctid.length - str_length("/transform") };

            anim_channels.append({
              anim.get_attribute("id").value,
              target_id,
              channel.get_attribute("source").value
            });
          }
        }
      }


      {
        XMLItem* lib_clips_ptr = root.first_named_pointer("library_animation_clips");

        if (lib_clips_ptr != NULL) {
          XMLItem& lib_clips = *lib_clips_ptr;

          size_t anim_clip_count = lib_clips.count_of_name("animation_clip");

          for (size_t i = 0; i < anim_clip_count; i ++) {
            XMLItem& clip = lib_clips.nth_named(i, "animation_clip");

            size_t channel_count = clip.count_of_name("instance_animation");

            Array<String> channel_ids { channel_count };

            for (size_t j = 0; j < channel_count; j ++) {
              XMLItem& channel = clip.nth_named(j, "instance_animation");

              channel_ids.append(channel.get_attribute("url").value);
            }

            anim_clips.append({
              clip.get_attribute("name").value,
              strtof(clip.get_attribute("start").value.value, NULL),
              strtof(clip.get_attribute("end").value.value, NULL),
              channel_ids
            });
          }
        }
      }


      {
        if (anim_clips.count <= 1) {
          if (anim_clips.count == 0 && anim_channels.count > 0) {
            DAEAnimClip clip { { }, 0.0f, 1.0f, { } }; // No name here to prevent memory leak

            for (auto [ i, channel ] : anim_channels) {
              clip.channel_ids.append(channel.id);
            }

            DAEIAnimation i_anim = DAEIAnimation::process(this, root_ibone, clip);

            clip.channel_ids.destroy();
            
            i_anim.calculate_bind(root_ibone, transform);

            animations.append(i_anim);
          } else if (anim_clips.count == 1) {
            DAEIAnimation i_anim = DAEIAnimation::process(this, root_ibone, anim_clips[0]);
            
            i_anim.calculate_bind(root_ibone, transform);

            animations.append(i_anim);
          }
        } else {
          for (auto [ i, clip ] : anim_clips) {
            DAEIAnimation i_anim = DAEIAnimation::process(this, root_ibone, clip);

            i_anim.calculate_bind(root_ibone, transform);

            animations.append(i_anim);
          }
        }
      }


      if (apply_bone_filter) {
        bool cont;
        do cont = root_ibone.filter(bone_filter);
        while (cont);
      }

      root_ibone.collapse(bone_bindings_list.bindings);

      if (apply_bone_filter) {
        for (auto [ i, anim ] : animations) {
          anim.filter(bone_bindings_list);
        }
      }

      root_ibone.destroy();
    } catch (Exception& exception) {
      destroy();
      throw exception;
    }
  }

  void DAE::destroy () {
    for (auto [ i, source ] : sources) {
      if (source.float_array) source.floats.destroy();
      else source.names.destroy();
    }

    sources.destroy();

    accessors.destroy();

    vertex_bindings.destroy();

    for (auto [ i, triangles ] : triangles_list) {
      triangles.inputs.destroy();
      triangles.indices.destroy();
    }

    triangles_list.destroy();

    joint_data.inputs.destroy();

    weight_data.inputs.destroy();
    weight_data.vcount.destroy();
    weight_data.indices.destroy();

    bone_bindings_list.destroy();

    for (auto [ i, sampler ] : anim_samplers) sampler.inputs.destroy();
    anim_samplers.destroy();

    for (auto [ i, channel ] : anim_channels) channel.target_id.destroy();
    anim_channels.destroy();

    for (auto [ i, clip ] : anim_clips) clip.channel_ids.destroy();
    anim_clips.destroy();

    for (auto [ i, anim ] : animations) anim.destroy();
    animations.destroy();
    
    xml.destroy();
  }

    

  DAEAccessor& DAE::get_accessor (String const& id) const {
    for (auto [ i, accessor ] : accessors) {
      if (accessor.id == id.value + 1) return accessor;
    }

    root.asset_error("Could not find Accessor '%s'", id.value);
  }

  DAESource& DAE::get_source (String const& id) const {
    for (auto [ i, source ] : sources) {
      if (source.id == id.value + 1) return source;
    }

    root.asset_error("Could not find Source '%s'", id.value);
  }

  DAEInput& DAE::get_poly_input (DAETriangles const& triangles, char const* semantic) {
    for (auto [ i, input ] : triangles.inputs) {
      if (input.semantic == semantic) return input;
    }

    triangles.origin->asset_error("Could not find semantic Input '%s'", semantic);
  }

  DAEInput* DAE::get_poly_input_pointer (DAETriangles const& triangles, char const* semantic) {
    for (auto [ i, input ] : triangles.inputs) {
      if (input.semantic == semantic) return &input;
    }

    return NULL;
  }

  DAEInput& DAE::get_wd_input (char const* semantic) const {
    for (auto [ i, input ] : weight_data.inputs) {
      if (input.semantic == semantic) return input;
    }

    weight_data.origin->asset_error("Could not find semantic Input '%s'", semantic);
  }

  DAEInput* DAE::get_wd_input_pointer (char const* semantic) const {
    for (auto [ i, input ] : weight_data.inputs) {
      if (input.semantic == semantic) return &input;
    }

    return NULL;
  }

  DAEInput& DAE::get_joint_input (char const* semantic) const {
    for (auto [ i, input ] : joint_data.inputs) {
      if (input.semantic == semantic) return input;
    }

    joint_data.origin->asset_error("Could not find semantic Input '%s'", semantic);
  }

  DAEInput* DAE::get_joint_input_pointer (char const* semantic) const {
    for (auto [ i, input ] : joint_data.inputs) {
      if (input.semantic == semantic) return &input;
    }

    return NULL;
  }

  s64_t DAE::get_bone_index (String& id) const {
    DAEInput& joint_input = get_joint_input("JOINT");

    DAEAccessor& joint_accessor = get_accessor(joint_input.source_id);

    DAESource& joint_source = get_source(joint_accessor.source_id);

    for (size_t i = 0; i < joint_accessor.count; i ++) {
      pair_t joint_name = joint_source.names[joint_accessor.offset + i * joint_accessor.stride];
      if (joint_name.b == id.length && strncmp(id.value, joint_name.a, joint_name.b) == 0) return i;
    }

    return -1;
  }
  
  s64_t DAE::get_origin_bone_index_from_node_id (DAEIBone const& root_ibone, String& id) {
    s64_t result = -1;

    root_ibone.traverse_cond([&] (DAEIBone const& ibone) -> bool {
      if (ibone.id == id) {
        result = ibone.origin_index;
        return false;
      } else return true;
    });

    return result;
  }

  s64_t DAE::get_filtered_bone_index_from_node_id (String& id) const {
    for (auto [ i, binding ] : bone_bindings_list) {
      if (binding.id == id) return binding.origin_index;
    }

    return -1;
  }

  s64_t DAE::get_filtered_bone_index (u32_t origin_index) const {
    if (origin_index != static_cast<u32_t>(-1)) {
      for (auto [ i, binding ] : bone_bindings_list) {
        if (binding.origin_index == origin_index) return i;
      }
    }
    return -1;
  }

  DAEAnimClip& DAE::get_anim_clip (char const* name) const {
    for (auto [ i, clip ] : anim_clips) {
      if (clip.name == name) return clip;
    }

    xml.asset_error("Could not find an Animation named '%s'", name);
  }

  DAEAnimChannel& DAE::get_anim_channel (String& id) const {
    char const* value = id.value;

    if (*value == '#') ++ value;

    for (auto [ i, channel ] : anim_channels) {
      if (channel.id == value) return channel;
    }

    xml.asset_error("Could not find an Animation Channel with id '%s'", id.value);
  }

  DAEAnimSampler& DAE::get_anim_sampler (String& id) const {
    for (auto [ i, sampler ] : anim_samplers) {
      if (sampler.id == id.value + 1) return sampler;
    }

    xml.asset_error("Could not find an Animation Sampler with id '%s'", id.value);
  }

  DAEInput& DAE::get_sampler_input (DAEAnimSampler const& sampler, char const* semantic) {
    for (auto [ i, input ] : sampler.inputs) {
      if (input.semantic == semantic) return input;
    }

    sampler.origin->asset_error("Could not find semantic input '%s'", semantic);
  }


  DAEIAnimation& DAE::get_animation (char const* name) const {
    for (auto [ i, anim ] : animations) {
      if (anim.name == name) return anim;
    }

    xml.asset_error("Could not find an Animation with name '%s'", name);
  }





  RenderMesh3D DAE::load_mesh () const {
    // needs a try catch wrapper!!

    Array<DAEIJoint> i_joints;

    DAEInput& weight_input = get_wd_input("WEIGHT");
    DAEAccessor& weight_accessor = get_accessor(weight_input.source_id);
    DAESource& weight_source = get_source(weight_accessor.source_id);

    {
      auto& [ wd_origin, inputs, count, vcount, indices ] = const_cast<DAEWeightData&>(weight_data); // Bug fix: const structures are improperly decomposed in c++17

      size_t j = 0;
      for (auto [ i, c ] : vcount) {
        wd_origin->asset_assert(c < DAEIJoint::max_vcount, "VCount %" PRIu32 " (at index %zu) exceeds maximum", c, i);

        DAEIJoint idata = { c };

        for (size_t k = 0; k < c; k ++) {
          size_t l = j * 2 + k * 2;
          idata.joints[k] = indices[l];
          idata.weights[k] = indices[l + 1];
        }

        i_joints.append(idata);
          
        j += c;
      }
    }


    Array<MaterialInfo> material_config_data;
    Array<DAEIVertex> i_vertices;
    Array<u32_t> i_indices;
    
    {
      Array<DAEIVertex> local_i_vertices;
      Array<u32_t> local_i_indices;
      for (auto [ i, triangles ] : triangles_list) {
        auto& [ _p, face_count, inputs, indices ] = triangles;

        MaterialInfo mat_info = { i, i_indices.count / 3, 0 };


        DAEInput& position_input = get_poly_input(triangles, "VERTEX");

        for (size_t j = 0; j < indices.count; j += triangles.advance) {
          size_t iv_index = j / triangles.advance;

          u32_t position = indices[j + position_input.offset];

          DAEIVertex iv = { triangles, iv_index };
          iv.position = position;
          local_i_vertices.append(iv);
        }


        DAEInput* normal_input = get_poly_input_pointer(triangles, "NORMAL");
        DAEInput* uv_input = get_poly_input_pointer(triangles, "TEXCOORD");
        DAEInput* color_input = get_poly_input_pointer(triangles, "COLOR");

        bool have_normal = normal_input != NULL;
        bool have_uv = uv_input != NULL;
        bool have_color = color_input != NULL;

        if (have_normal || have_uv || have_color) {
          for (size_t j = 0; j < indices.count; j += triangles.advance) {
            size_t iv_index = j / triangles.advance;

            s64_t normal = have_normal? static_cast<s64_t>(indices[j + normal_input->offset]) : -1;
            s64_t uv = have_uv? static_cast<s64_t>(indices[j + uv_input->offset]) : -1;
            s64_t color = have_color? static_cast<s64_t>(indices[j + color_input->offset]) : -1;

            DAEIVertex* existing_vertex = &local_i_vertices[iv_index];

            if (!existing_vertex->set) {
              existing_vertex->set_attributes(normal, uv, color);
              local_i_indices.append(iv_index);
            } else if (existing_vertex->has_same_attributes(normal, uv, color)) {
              local_i_indices.append(iv_index);
            } else {
              bool found_existing = false;

              while (existing_vertex->duplicate != -1) {
                existing_vertex = &local_i_vertices[existing_vertex->duplicate];

                if (existing_vertex->has_same_attributes(normal, uv, color)) {
                  local_i_indices.append(existing_vertex->index);
                  found_existing = true;
                  break;
                }
              }

              if (!found_existing) {
                size_t new_iv_index = local_i_vertices.count;

                DAEIVertex new_iv = { triangles, new_iv_index };
                new_iv.set_attributes(normal, uv, color);
                local_i_vertices.append(new_iv);

                existing_vertex->duplicate = new_iv_index;

                local_i_indices.append(new_iv_index);
              }
            }
          }
        }

        size_t iv_offset = i_vertices.count;
        for (auto [ i, liv ] : local_i_vertices) {
          liv.index += iv_offset;
          if (liv.duplicate != -1) liv.duplicate += iv_offset;
          i_vertices.append(liv);
        }

        for (auto [ i, lii ] : local_i_indices) {
          i_indices.append(lii + iv_offset);
        }

        local_i_vertices.clear();
        local_i_indices.clear();
        

        mat_info.length = (i_indices.count / 3) - mat_info.start_index;

        material_config_data.append(mat_info);
      }

      local_i_vertices.destroy();
      local_i_indices.destroy();

      mesh->asset_assert(i_indices.count % 3 == 0, "Final indices count was not cleanly divisible by 3, make sure your mesh is triangulated");
    }

    Array<Vector3f> final_positions { i_vertices.count };
    Array<Vector3f> final_normals { i_vertices.count };
    Array<Vector2f> final_uvs { i_vertices.count };
    Array<Vector3f> final_colors { i_vertices.count };
    Array<Vector3u> final_faces = { i_indices.count / 3 };
    Array<Vector4u> final_joints = { i_vertices.count };
    Array<Vector4f> final_weights = { i_vertices.count };


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
      DAEInput& position_input = get_poly_input(iv.triangles, "VERTEX");
      DAEAccessor& position_accessor = get_accessor(position_input.source_id);
      DAESource& position_source = get_source(position_accessor.source_id);

      Vector3f pos;

      for (size_t j = 0; j < 3; j ++) {
        pos.elements[j] = position_source.floats[position_accessor.offset + iv.position * position_accessor.stride + j];
      }

      final_positions.append(pos.apply_matrix(transform));


      if (!incomplete_normals) {
        if (iv.normal != -1) {
          Vector3f norm = { 0, 0, 0 };

          DAEInput& normal_input = get_poly_input(iv.triangles, "NORMAL");
          DAEAccessor& normal_accessor = get_accessor(normal_input.source_id);
          DAESource& normal_source = get_source(normal_accessor.source_id);

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
        DAEInput& uv_input = get_poly_input(iv.triangles, "TEXCOORD");
        DAEAccessor& uv_accessor = get_accessor(uv_input.source_id);
        DAESource& uv_source = get_source(uv_accessor.source_id);

        for (size_t j = 0; j < 2; j ++) {
          uv.elements[j] = uv_source.floats[uv_accessor.offset + iv.uv * uv_accessor.stride + j];
        }
      }

      final_uvs.append(uv);

      Vector3f color = { 1, 1, 1 };
      
      if (iv.color != -1) {
        DAEInput& color_input = get_poly_input(iv.triangles, "COLOR");
        DAEAccessor& color_accessor = get_accessor(color_input.source_id);
        DAESource& color_source = get_source(color_accessor.source_id);

        for (size_t j = 0; j < 3; j ++) {
          color.elements[j] = color_source.floats[color_accessor.offset + iv.color * color_accessor.stride + j];
        }
      }

      final_colors.append(color);


      Vector4u joints = { static_cast<u32_t>(-1), static_cast<u32_t>(-1), static_cast<u32_t>(-1), static_cast<u32_t>(-1) };
      Vector4f weights = { 0, 0, 0, 0 };
      DAEIJoint& ij = i_joints[iv.position];

      if (ij.count <= 4) {
        for (size_t j = 0; j < ij.count; j ++) {
          joints[j] = ij.joints[j];
          weights[j] = weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride];
        }
      } else {
        pair_t<u32_t, f32_t> sorted [DAEIJoint::max_vcount];

        for (size_t j = 0; j < ij.count; j ++) {
          sorted[j].a = ij.joints[j];
          sorted[j].b = weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride];
        }

        quick_sort(sorted, 0, ij.count - 1, [&] (pair_t<u32_t, f32_t> const& x, pair_t<u32_t, f32_t> const& y) {
          return x.b > y.b;
        });

        for (size_t j = 0; j < 4; j ++) {
          joints[j] = sorted[j].a;
          weights[j] = sorted[j].b;
        }
      }

      weights.normalize();

      final_joints.append(joints);
      final_weights.append(weights);
    }
    

    for (auto [ i, joint ] : final_joints) {
      Vector4f& weight = final_weights[i];

      for (auto [ j, index ] : joint) {
        s64_t filtered_index = get_filtered_bone_index(index);

        if (filtered_index == -1) {
          if (weight[j] != 0.0f) {
            weight[j] = 0.0f;
            printf("Warning: Original bone %u has non-zero weight but is no longer present in filtered bones\n", index);
            weight = weight.normalize();
          }
          index = 0;
        } else {
          index = filtered_index;
        }
      }
    }


    RenderMesh3D dae_mesh = RenderMesh3D::from_ex(
      "collada!",

      true,

      final_positions.count,
      final_positions.elements,
      incomplete_normals || final_normals.count == 0? NULL : final_normals.elements,
      
      final_uvs.count == 0? NULL : final_uvs.elements,
      final_colors.count == 0? NULL : final_colors.elements,

      final_joints.count == 0? NULL : final_joints.elements,
      final_weights.count == 0? NULL : final_weights.elements,

      final_faces.count,
      final_faces.elements,

      final_material_config
    );


    // dae_mesh.use();

    // u32_t buffers [2];
    // glGenBuffers(2, buffers);

    // glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    // glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, sizeof(Vector4u), reinterpret_cast<void*>(0));
    // glBufferData(GL_ARRAY_BUFFER, final_joints.count * sizeof(Vector4u), final_joints.elements, GL_DYNAMIC_DRAW);
    // glEnableVertexAttribArray(4);

    // glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), reinterpret_cast<void*>(0));
    // glBufferData(GL_ARRAY_BUFFER, final_weights.count * sizeof(Vector4f), final_weights.elements, GL_DYNAMIC_DRAW);
    // glEnableVertexAttribArray(5);


    i_joints.destroy();
    i_vertices.destroy();

    i_indices.destroy();

    // REMOVE THIS when skinned meshes are implemented
    // final_joints.destroy();
    // final_weights.destroy();


    return dae_mesh;
  }










  Skeleton DAE::load_skeleton () const {
    Array<Bone> final_bones;

    bone_bindings_list.collapse(final_bones);

    Skeleton skeleton = Skeleton::from_ex(
      "collada!",
      final_bones,
      0
    );

    return skeleton;
  }


    




  SkeletalAnimation DAE::load_animation (char const* name) const {
    if (name == NULL) xml.asset_assert(animations.count == 1, "Animation name is required if there is more than 1 animation in the source");

    DAEIAnimation& i_anim = name != NULL? get_animation(name) : animations[0];

    Array<SkeletalKeyframe> final_keyframes;
    i_anim.collapse(bone_bindings_list, final_keyframes);

    SkeletalAnimation anim = SkeletalAnimation::from_ex(
      "collada!",
      final_keyframes,
      1.0f,
      i_anim.length
    );

    return anim;
  }







  DAEVertexBinding& DAE::get_vertex_binding (String const& id) const {
    for (auto [ k, binding ] : vertex_bindings) {
      if (binding.id == id.value + 1) return binding;
    }

    mesh->asset_error("Could not find VertexBinding '%s'", id.value);
  }

  Array<DAEInput> DAE::gather_inputs (XMLItem& origin) const {
    size_t input_count = origin.count_of_name("input");

    Array<DAEInput> inputs = { input_count };

    for (size_t j = 0; j < input_count; j ++) {
      XMLItem& input = origin.nth_named(j, "input");

      String semantic = input.get_attribute("semantic").value; // borrowing string here, destroyed by XML
      String source_id = input.get_attribute("source").value; // borrowing string here, destroyed by XML

      if (semantic == "VERTEX") {
        source_id = get_vertex_binding(source_id).source_id;
      }

      XMLAttribute* set = input.get_attribute_pointer("set");
      XMLAttribute* offset = input.get_attribute_pointer("offset");
      inputs.append({
        semantic,
        source_id,
        offset != NULL? strtoumax(offset->value.value, NULL, 10) : 0,
        set != NULL? strtoumax(set->value.value, NULL, 10) : 0
      });
    }

    return inputs;
  }

  void DAE::gather_base_data (XMLItem& section) {
    size_t sources_count = section.count_of_name("source");

    for (size_t i = 0; i < sources_count; i ++) {
      XMLItem source = section.nth_named(i, "source");

      XMLItem* float_array = source.first_named_pointer("float_array");
      XMLItem* name_array = source.first_named_pointer("Name_array");

      if (float_array != NULL) {
        String id = float_array->get_attribute("id").value; // borrowing string here, destroyed by XML

        size_t float_count = strtoumax(float_array->get_attribute("count").value.value, NULL, 10);
        Array<f64_t> floats { float_count };

        char* base = float_array->get_text().value;
        char* end = NULL;
        for (size_t j = 0; j < float_count; j ++) {
          floats.append(strtod(base, &end));
          float_array->asset_assert(end != base, "Failed to parse float at index %zu", j);
          base = end;
        }

        sources.append({ id, floats });
      } else if (name_array != NULL) {
        String id = name_array->get_attribute("id").value;

        size_t name_count = strtoumax(name_array->get_attribute("count").value.value, NULL, 10);
        Array < pair_t<char*, size_t> > names;

        char* base = name_array->get_text().value;
        char* end = NULL;
        for (size_t j = 0; j < name_count; j ++) {
          while (char_is_whitespace(*base)) ++ base;
          end = base;
          while (!char_is_whitespace(*end) && *end != '\0') ++ end;
          names.append({ base, static_cast<size_t>(end - base) });
          base = end;
        }

        sources.append({ id, names });
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
  }
}