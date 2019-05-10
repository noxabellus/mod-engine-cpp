#include "../../include/graphics/SkeletonState.hh"



namespace mod {
  SkeletonState::SkeletonState (SkeletonHandle in_skeleton, Array<SkeletalAnimationState> const& in_animation_states)
  : skeleton(in_skeleton)
  {
    animation_states.copy(in_animation_states);

    for (auto [ i, animation_state ] : animation_states) {
      m_assert(
        validate_animation(animation_state.animation),
        "Cannot create SkeletonState for Skeleton with origin '%s' using SkeletalAnimations that target Bone indices outside the range 0 - %zu",
        skeleton->origin, skeleton->bones.count
      );
    }
  }

  SkeletonState SkeletonState::from_ex (SkeletonHandle skeleton, Array<SkeletalAnimationState> const& animation_states) {
    SkeletonState state;
    
    state.skeleton = skeleton;

    state.animation_states = animation_states;

    for (auto [ i, animation_state ] : state.animation_states) {
      m_assert(
        state.validate_animation(animation_state.animation),
        "Cannot create SkeletonState for Skeleton with origin '%s' using SkeletalAnimations that target Bone indices outside the range 0 - %zu",
        state.skeleton->origin, state.skeleton->bones.count
      );
    }

    return state;
  }

  bool SkeletonState::validate_animation (SkeletalAnimationHandle const& animation) const {
    for (auto [ i, transform ] : animation->keyframes[0].transforms) {
      if (skeleton->bones.count <= transform.target_index) return false;
    }

    return true;
  }


  void SkeletonState::set_animation (SkeletalAnimationState const& state) {
    for (auto [ i, animation_state ] : animation_states) {
      if (animation_state.animation == state.animation) {
        animation_state = state;
        return;
      }
    }

    m_assert(
      validate_animation(state.animation),
      "Cannot create SkeletonState for Skeleton with origin '%s' using SkeletalAnimations that target Bone indices outside the range 0 - %zu",
      skeleton->origin, skeleton->bones.count
    );

    animation_states.append(state);
  }

  void SkeletonState::unset_animation (SkeletalAnimationHandle const& animation) {
    for (auto [ i, animation_state ] : animation_states) {
      if (animation_state.animation == animation) {
        animation_states.remove(i);
        break;
      }
    }
  }


  SkeletalAnimationState* SkeletonState::get_animation_pointer (SkeletalAnimationHandle const& animation) const {
    for (auto [ i, animation_state ] : animation_states) {
      if (animation_state.animation == animation) return &animation_state;
    }

    return NULL;
  }

  SkeletalAnimationState& SkeletonState::get_animation (SkeletalAnimationHandle const& animation) const {
    SkeletalAnimationState* ptr = get_animation_pointer(animation);

    m_assert(ptr != NULL, "Could not find SkeletalAnimation with name/origin '%s'", animation.get_name());

    return *ptr;
  }


  Array<Matrix4> const& SkeletonState::update_pose (f32_t delta_time) {
    Skeleton const& skeleton_ref = *skeleton;

    blend_states.clear();

    auto const get_blend_state = [&] (size_t target_index) -> SkeletalBlendState* {
      for (auto [ i, blend_state ] : blend_states) {
        if (blend_state.channel.target_index == target_index) return &blend_state;
      }
      
      return NULL;
    };


    f32_t total_blend_alpha = 0.0f;

    for (auto [ i, animation_state ] : animation_states) {
      if (!animation_state.active) continue;
      total_blend_alpha += animation_state.blend_alpha;
    }

    for (auto [ i, animation_state ] : animation_states) {
      if (!animation_state.active) continue;
      animation_state.normalized_blend_alpha = animation_state.blend_alpha / total_blend_alpha;
    }



    for (auto [ i, animation_state ] : animation_states) {
      if (!animation_state.active) continue;
      
      SkeletalAnimation& animation = *animation_state.animation;

      if (animation_state.playing) animation_state.local_time = num::remainder(animation_state.local_time + delta_time / animation_state.time_scale, animation.length * animation.time_scale);

      if (animation_state.normalized_blend_alpha > 0.0f) {
        animation.get_pose_slerp(
          (animation_state.local_time + animation_state.time_offset * animation.length * animation.time_scale),
          intermediate_channels
        );
        
        for (auto [ j, intermediate ] : intermediate_channels) {
          f32_t nba = animation_state.normalized_blend_alpha;
          Transform3D& i_tran = intermediate.transform;
          SkeletalBlendState* blend_state = get_blend_state(intermediate.target_index);
    
          if (blend_state != NULL) {
            Transform3D& blend_tran = blend_state->channel.transform;
            // Quaternion irot = i_tran.rotation * num::sign(i_tran.rotation.w);

            blend_tran = {
              blend_tran.position + i_tran.position * nba,

              blend_tran.rotation + i_tran.rotation * num::sign(i_tran.rotation.dot(i_tran.rotation)) * nba,
              // Quaternion {
              //   irot.x * nba,
              //   irot.y * nba,
              //   irot.z * nba,
              //   (irot.w - 1.0f) * nba + 1.0f
              // }.normalize() * blend_tran.rotation,

              blend_tran.scale    + i_tran.scale * nba
              // blend_tran.scale * (Vector3f { 1.0f - nba } + (i_tran.scale * nba))
            };

            blend_state->accumulated_weight += nba;
          } else blend_states.append({
            nba,
            {
              intermediate.target_index,
              Transform3D {
                i_tran.position * nba,
                i_tran.rotation * nba,
                i_tran.scale    * nba
              }
            }
          });
        }
      }
    }

    pose.clear();

    pose.reallocate(skeleton_ref.bones.count);
    pose.count = skeleton_ref.bones.count;

    // for (auto [ i, bone ] : skeleton_ref) {
    //   SkeletalBlendState* blend_state = get_blend_state(i);

    //   if (blend_state == NULL) pose.append(bone.base_transform.compose());
    //   else {
    //     Transform3D& blend_tran = blend_state->channel.transform;
        
    //     f32_t r = 1.0f / blend_state->accumulated_weight;

    //     pose.append(Transform3D {
    //       blend_tran.position * r,
    //       blend_tran.rotation.normalize(),
    //       blend_tran.scale * r
    //     }.compose() * bone.inverse_bind_matrix);
    //   }
    // }


    std::function<void(u32_t, Bone const&, Matrix4 const&)> const traverse = [&] (u32_t i, Bone const& bone, Matrix4 const& m_parent) {
      SkeletalBlendState* blend_state = get_blend_state(i);

      Matrix4 m_local;

      if (blend_state == NULL) m_local = bone.base_transform.compose();
      else {
        Transform3D& blend_tran = blend_state->channel.transform;

        f32_t r = 1.0f / blend_state->accumulated_weight;

        m_local = Transform3D {
          blend_tran.position * r,
          blend_tran.rotation.normalize(),
          blend_tran.scale * r
        }.compose();
      }

      Matrix4 m_model = m_parent.multiply(m_local);

      for (auto [ j, child_bone ] : skeleton_ref) {
        if (child_bone.parent_index == i) traverse(j, child_bone, m_model);
      }

      pose[i] = m_model * bone.inverse_bind_matrix;
    };

    traverse(skeleton_ref.root_index, skeleton_ref.root(), Constants::Matrix4::identity);
    

    return pose;
  }

  void SkeletonState::destroy () {
    animation_states.destroy();
    pose.destroy();
    blend_states.destroy();
    intermediate_channels.destroy();
  }
}