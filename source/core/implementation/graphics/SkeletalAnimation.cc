#include "../../include/graphics/SkeletalAnimation.hh"


namespace mod {
  SkeletalAnimation::SkeletalAnimation (char const* in_origin, Array<SkeletalKeyframe> const& in_keyframes, f32_t in_time_scale, f32_t in_length)
  : origin(str_clone(in_origin))
  , time_scale(in_time_scale)
  , length(in_length)
  {
    keyframes.copy(in_keyframes);

    if (num::flt_equal(length, -1.0f)) {
      asset_assert_terminal(keyframes.count > 0, "Cannot calculate length of SkeletalAnimation with empty keyframes array");
      calculate_length();
    }

    validate_keyframes();
  }


  SkeletalAnimation SkeletalAnimation::from_ex (char const* origin, Array<SkeletalKeyframe> const& keyframes, f32_t time_scale, f32_t length) {
    SkeletalAnimation animation;

    animation.origin = str_clone(origin);
    animation.keyframes = keyframes;
    animation.time_scale = time_scale;
    if (num::flt_equal(length, -1.0f)) {
      animation.asset_assert_terminal(animation.keyframes.count > 0, "Cannot calculate length of SkeletalAnimation with empty keyframes array");
      animation.calculate_length();
    } else {
      animation.length = length;
    }

    animation.validate_keyframes();

    return animation;
  }


  SkeletalAnimation SkeletalAnimation::from_json_item (char const* origin, JSONItem const& json) {
    Array<SkeletalKeyframe> keyframes;

    Array<SkeletalKeyframeChannel> intermediate_transforms;

    JSONItem* keyframes_item =  json.get_object_item("keyframes");
    json.asset_assert(keyframes_item != NULL, "Expected a key/value pair 'keyframes'");

    JSONArray keyframes_arr = json.get_object_array("keyframes");
    keyframes_item->asset_assert(keyframes_arr.count > 0, "Expected at least one Keyframe");

    f32_t time_scale = 1.0f;
    f32_t length = -1.0f;

    size_t transform_count = 0;

    try {
      for (auto [ i, keyframe_item ] : keyframes_arr) {
        f32_t time = keyframe_item.get_object_number("time");

        JSONItem* transforms_item =  json.get_object_item("transforms");
        json.asset_assert(transforms_item != NULL, "Expected a key/value pair 'transforms'");

        JSONArray transforms_arr = json.get_object_array("transforms");

        if (transform_count == 0) {
          transforms_item->asset_assert(transforms_arr.count > 0, "Expected at least one Transform");
          transform_count = transforms_arr.count;
        } else {
          transforms_item->asset_assert(transforms_arr.count == transform_count, "Expected the same number of Transforms as the previous entry (%zu) not %zu", transform_count, transforms_arr.count);
        }

        intermediate_transforms.clear();

        for (auto [ j, keyframe_transform_item ] : transforms_arr) {
          u32_t target_index = keyframe_transform_item.get_object_number("target_index");

          JSONItem* transform_item = keyframe_transform_item.get_object_item("transform");

          keyframe_transform_item.asset_assert(transform_item != NULL, "Expected a key/value pair 'transform'");

          Transform3D tran;

          if (transform_item->type == JSONType::Array) {
            Matrix4 mat4;

            for (auto [ k, e ] : mat4) e = transform_item->get_array_number(k);

            tran = mat4.decompose();
          } else {
            JSONItem* position_item = transform_item->get_object_item("position");
            JSONItem* rotation_item = transform_item->get_object_item("rotation");
            JSONItem* scale_item = transform_item->get_object_item("scale");

            transform_item->asset_assert(position_item != NULL, "Expected a key/value pair 'position'");
            transform_item->asset_assert(rotation_item != NULL, "Expected a key/value pair 'rotation'");
            transform_item->asset_assert(scale_item != NULL, "Expected a key/value pair 'scale'");

            for (size_t i = 0; i < 4; i ++) {
              if (i < 3) {
                tran.position[i] = position_item->get_array_number(i);
                tran.scale[i] = scale_item->get_array_number(i);
              }

              tran.rotation[i] = rotation_item->get_array_number(i);
            }
          }

          intermediate_transforms.append({ target_index, tran });
        }

        keyframes.append({ time, intermediate_transforms.clone() });
      }
    } catch (Exception& exception) {
      for (auto [ i, keyframe ] : keyframes) keyframe.destroy();
      keyframes.destroy();
      intermediate_transforms.destroy();
      throw exception;
    }

    intermediate_transforms.destroy();

    return from_ex(origin, keyframes, time_scale, length);
  }


  SkeletalAnimation SkeletalAnimation::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    SkeletalAnimation animation;

    try {
      animation = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return animation;
  }

  SkeletalAnimation SkeletalAnimation::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(source != NULL, origin, "Failed to load SkeletalAnimation: unable to read file");

    SkeletalAnimation animation;

    try {
      animation = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      memory::deallocate(source);
      throw exception;
    }

    memory::deallocate(source);

    return animation;
  }

  
  void SkeletalAnimation::validate_keyframes (bool terminal) { 
    auto& keyframe_b = keyframes[0];

    for (auto [ i, keyframe_a ] : keyframes) {
      asset_assert_cond_terminal(
        terminal,
        keyframe_a.transforms.count == keyframe_b.transforms.count,
        "Keyframe %zu has a different transform count than the base (%zu & %zu)",
        i, keyframe_a.transforms.count, keyframe_b.transforms.count
      );

      for (auto [ j, transform_a ] : keyframe_a.transforms) {
        auto& transform_b = keyframe_b.transforms[j];

        asset_assert_cond_terminal(
          terminal,
          transform_a.target_index == transform_b.target_index,
          "Keyframe %zu Transform %zu has a different target index than the base (%" PRIu32 " & %" PRIu32 ")",
          i, j, transform_a.target_index, transform_b.target_index
        );
      }
    }
  }


  void SkeletalAnimation::destroy () {
    if (origin != NULL) memory::deallocate(origin);
    for (auto [ i, keyframe ] : keyframes) keyframe.destroy();
    keyframes.destroy();
  }


  void SkeletalAnimation::calculate_length () {
    length = keyframes.last().time;
  }


  SkeletalKeyframe const& SkeletalAnimation::keyframe_before (f32_t offset_time) const {
    SkeletalKeyframe const* last_frame = NULL;

    for (auto [ i, keyframe ] : keyframes) {
      if (num::flt_equal(keyframe.time, offset_time)) return keyframe;
      else if (keyframe.time > offset_time) {
        if (last_frame != NULL) return *last_frame;
        else break;
      } else last_frame = &keyframe;
    }

    return keyframes.last();
  }

  SkeletalKeyframe const& SkeletalAnimation::keyframe_after (f32_t offset_time) const {
    for (auto [ i, keyframe ] : keyframes) {
      if (keyframe.time >= offset_time) return keyframe;
    }
    
    return keyframes.last();
  }


  void SkeletalAnimation::get_pose_slerp (f32_t offset_time, Array<SkeletalKeyframeChannel>& out_transforms) const {
    offset_time = fmod(offset_time / time_scale, length);

    out_transforms.clear();

    SkeletalKeyframe const& prev = keyframe_before(offset_time);
    SkeletalKeyframe const& next = keyframe_after(offset_time);

    f32_t k_delta = next.time - prev.time;
    f32_t o_delta = offset_time - prev.time;
    f32_t alpha = o_delta / k_delta;

    for (size_t i = 0; i < prev.transforms.count; i ++) {
      out_transforms.append({ prev.transforms[i].target_index, prev.transforms[i].transform.slerp(alpha, next.transforms[i].transform) });
    }
  }
  
  void SkeletalAnimation::get_pose_lerp (f32_t offset_time, Array<SkeletalKeyframeChannel>& out_transforms) const {
    offset_time = fmod(offset_time / time_scale, length);

    out_transforms.clear();

    SkeletalKeyframe const& prev = keyframe_before(offset_time);
    SkeletalKeyframe const& next = keyframe_after(offset_time);

    f32_t k_delta = next.time - prev.time;
    f32_t o_delta = offset_time - prev.time;
    f32_t alpha = o_delta / k_delta;

    for (size_t i = 0; i < prev.transforms.count; i ++) {
      out_transforms.append({ prev.transforms[i].target_index, prev.transforms[i].transform.lerp(alpha, next.transforms[i].transform) });
    }
  }
}