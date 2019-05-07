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

    bone_transforms.clear();

    for (auto [ i, bone ] : skeleton_ref) {
      bone_transforms.append(bone.base_transform);
    }

    for (auto [ i, animation_state ] : animation_states) {
      if (!animation_state.active) continue;

      if (animation_state.playing) animation_state.local_time += delta_time;

      if (animation_state.blend_alpha > 0.0f) {
        animation_state.animation->get_pose_slerp(
          (animation_state.local_time + animation_state.time_offset) * animation_state.time_scale,
          intermediate_transforms
        );
        
        for (auto [ j, intermediate_transform ] : intermediate_transforms) {
          Transform3D& bone_transform = bone_transforms[intermediate_transform.target_index];

          bone_transform = bone_transform.slerp(animation_state.blend_alpha, intermediate_transform.transform);
        }
      }
    }

    pose.clear();

    for (auto [ i, bone_transform ] : bone_transforms) {
      pose.append(Matrix4::compose(bone_transform) * skeleton_ref.bones[i].inverse_bind_matrix);
    }

    return pose;
  }

  void SkeletonState::destroy () {
    animation_states.destroy();
    bone_transforms.destroy();
    pose.destroy();
    intermediate_transforms.destroy();
  }
}