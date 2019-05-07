#include "../../include/graphics/Skeleton.hh"
#include "../../include/draw_debug.hh"



namespace mod {
  Skeleton::Skeleton (char const* in_origin, Array<Bone> const& in_bones, u32_t in_root)
  : origin(str_clone(in_origin))
  , root_index(in_root)
  {
    bones.copy(in_bones);
    asset_assert_terminal(get_bone_pointer(root_index) != NULL, "Root bone index was out of range");
    calculate_bind_matrices();
  }


  Skeleton Skeleton::from_ex (char const* origin, Array<Bone> const& bones, u32_t root) {
    Skeleton skeleton;
    skeleton.origin = str_clone(origin);
    skeleton.bones = bones;
    skeleton.root_index = root;
    skeleton.asset_assert_terminal(skeleton.get_bone_pointer(skeleton.root_index) != NULL, "Root bone index was out of range"); 
    skeleton.calculate_bind_matrices();
    return skeleton;
  }


  Skeleton Skeleton::from_json_item (char const* origin, JSONItem const& json) {
    Array<Bone> bones;

    u32_t root_index = 0;

    try {
      JSONItem* bone_item = json.get_object_item("bones");
      json.asset_assert(bone_item != NULL, "Expected a key/value pair 'bones'");

      JSONArray& bone_arr = json.get_object_array("bones");
      bone_item->asset_assert(bone_arr.count > 0, "Expected at least one Bone");

      for (auto [ i, bone_item ] : bone_arr) {
        String name = { bone_item.get_object_string("name").value };


        JSONItem* parent_item = bone_item.get_object_item("parent");
        s32_t parent = parent_item != NULL? static_cast<s32_t>(parent_item->get_number()) : -1;

        if (parent != -1) parent_item->asset_assert(parent < static_cast<s32_t>(bone_arr.count), "Parent index (%d) out of range (%zu)", parent, bone_arr.count);


        JSONItem* transform_item = bone_item.get_object_item("transform");
        bone_item.asset_assert(transform_item != NULL, "Expected a key/value pair 'transform'");

        Transform3D transform;

        if (transform_item->type == JSONType::Array) {
          Matrix4 mat4;
          for (auto [ i, e ] : mat4) e = transform_item->get_array_number(i);
          transform = mat4.decompose();
        } else {
          JSONItem* position_item = transform_item->get_object_item("position");
          JSONItem* rotation_item = transform_item->get_object_item("rotation");
          JSONItem* scale_item = transform_item->get_object_item("scale");

          transform_item->asset_assert(position_item != NULL, "Expected a key/value pair 'position'");
          transform_item->asset_assert(rotation_item != NULL, "Expected a key/value pair 'rotation'");
          transform_item->asset_assert(scale_item != NULL, "Expected a key/value pair 'scale'");

          for (size_t i = 0; i < 4; i ++) {
            if (i < 3) {
              transform.position[i] = position_item->get_array_number(i);
              transform.scale[i] = scale_item->get_array_number(i);
            }

            transform.rotation[i] = rotation_item->get_array_number(i);
          }
        }

        bones.append({ name, parent, transform });
      }


      JSONItem* root_index_item = json.get_object_item("root_index");

      if (root_index_item != NULL)  root_index = root_index_item->get_number();
    } catch (Exception& exception) {
      bones.destroy();
      throw exception;
    }
    
    return from_ex(origin, bones, root_index);
  }


  Skeleton Skeleton::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    Skeleton skeleton;

    try {
      skeleton = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return skeleton;
  }


  Skeleton Skeleton::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(source != NULL, origin, "Failed to load Skeleton: unable to read file");

    Skeleton skeleton;

    try {
      skeleton = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);

    return skeleton;
  }
  

  void Skeleton::destroy () {
    if (origin != NULL) {
      free(origin);
      origin = NULL;
    }

    for (auto [ i, bone ] : bones) bone.destroy();

    bones.destroy();
  }




  Bone* Skeleton::get_bone_pointer (size_t index) const {
    return bones.get_element(index);
  }

  Bone* Skeleton::get_bone_pointer (char const* name) const {
    for (auto [ i, bone ] : bones) {
      if (bone.name == name) return &bone;
    }

    return NULL;
  }


  Bone& Skeleton::get_bone (size_t index) const {
    asset_assert(index < bones.count, "Out of range access: index %zu, count %zu", index, bones.count);
    return bones.elements[index];
  }
  
  Bone& Skeleton::get_bone (char const* name) const {
    Bone* ptr = get_bone_pointer(name);
    asset_assert(ptr != NULL, "No Bone with name '%s' found", name);
    return *ptr;
  }

  size_t Skeleton::get_child_count (Bone const& parent_bone) const {
    s64_t parent_index = get_index(parent_bone);

    asset_assert(parent_index != -1, "Cannot get number of children for a Bone not owned by this Skeleton");

    size_t count = 0;

    for (auto [ i , bone ] : bones) {
      if (bone.parent_index == parent_index) ++ count;
    }

    return count;
  }


  Bone* Skeleton::get_parent (Bone const& child_bone) const {
    asset_assert(owns_bone(child_bone), "Cannot get parent of a Bone that does not belong to this Skeleton");
    if (child_bone.parent_index != -1) return &get_bone(child_bone.parent_index);
    else return NULL;
  }

  s64_t Skeleton::get_index (Bone const& bone) const {
    if (!owns_bone(bone)) return -1;
    else return pointer_to_index(bones.elements, &bone);
  }



  void Skeleton::calculate_bind_matrices () {
    using FN = void (*) (Skeleton&, Bone const&);
    
    static FN const calculate_descendant_bind_matrices = [] (Skeleton& skel, Bone const& parent_bone) {
      s64_t parent_index = skel.get_index(parent_bone);
      skel.asset_assert(parent_index != -1, "Cannot calculate child bind matrices for a Bone that does not belong to this Skeleton");

      for (auto [ i, child_bone ] : skel.bones) {
        if (child_bone.parent_index == parent_index) {
          child_bone.bind_matrix = parent_bone.bind_matrix * Matrix4::compose(child_bone.base_transform);
          child_bone.inverse_bind_matrix = child_bone.bind_matrix.inverse();

          calculate_descendant_bind_matrices(skel, child_bone);
        }
      }
    };

    Bone& root_bone = root();

    root_bone.bind_matrix = Matrix4::compose(root_bone.base_transform);
    root_bone.inverse_bind_matrix = root_bone.bind_matrix.inverse();

    calculate_descendant_bind_matrices(*this, root_bone);
  }
}