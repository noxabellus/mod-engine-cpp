#ifndef SKELETON_STATE_H
#define SKELETON_STATE_H

#include "Skeleton.hh"
#include "SkeletalAnimation.hh"



namespace mod {
  struct SkeletalAnimationState {
    SkeletalAnimationHandle animation;
    bool active;
    bool playing;
    f32_t local_time;
    f32_t time_offset;
    f32_t time_scale;
    f32_t blend_alpha;
    f32_t normalized_blend_alpha = 1.0f;

    SkeletalAnimationState () = default;

    SkeletalAnimationState (
      SkeletalAnimationHandle in_animation,
      f32_t in_blend_alpha = 1.0f,
      f32_t in_time_scale = 1.0f,
      f32_t in_time_offset = 0.0f,
      bool in_active = true,
      bool in_playing = true,
      f32_t in_local_time = 0.0f
    )
    : animation(in_animation)
    , active(in_active)
    , playing(in_playing)
    , local_time(in_local_time)
    , time_offset(in_time_offset)
    , time_scale(in_time_scale)
    , blend_alpha(in_blend_alpha)
    { }
  };

  struct SkeletalBlendState {
    f32_t accumulated_weight;
    SkeletalKeyframeChannel channel;
  };

  struct SkeletonState {
    SkeletonHandle skeleton;
    Array<SkeletalAnimationState> animation_states;

    Array<Matrix4> pose;


    /* Create a new uninitialized SkeletonState */
    SkeletonState () { }

    /* Create a new SkeletonState and initialize its values by copying from an array of SkeletalAnimationStates */
    ENGINE_API SkeletonState (SkeletonHandle in_skeleton, Array<SkeletalAnimationState> const& in_animation_states);
    
    /* Create a new SkeletonState and initialize its values by taking ownership of an array of SkeletalAnimationStates */
    ENGINE_API static SkeletonState from_ex (SkeletonHandle skeleton, Array<SkeletalAnimationState> const& animation_states);


    /* Validate a SkeletalAnimation for use with the active Skeleton of a SkeletonState */
    ENGINE_API bool validate_animation (SkeletalAnimationHandle const& animation) const;

    
    /* Add a SkeletalAnimationState to a SkeletonState.
     * Panics if the SkeletalAnimation cannot be validated for the SkeletonState's active Skeleton.
     * If the SkeletonState already has this animation its state will be overwritten */
    ENGINE_API void set_animation (SkeletalAnimationState const& state);

    /* Add a SkeletalAnimation to a SkeletonState.
     * Panics if the SkeletalAnimation cannot be validated for the SkeletonState's active Skeleton.
     * If the SkeletonState already has this animation its state will be overwritten */
    void set_animation (
      SkeletalAnimationHandle animation,
      f32_t blend_alpha = 1.0f,
      f32_t time_scale = 1.0f,
      f32_t time_offset = 0.0f,
      bool active = true,
      bool playing = true,
      f32_t local_time = 0.0f
    ) {
      return set_animation({ animation, blend_alpha, time_scale, time_offset, active, playing, local_time });
    }


    /* Remove a SkeletalAnimation from a SkeletonState.
     * Does nothing if no SkeletalAnimationState is found matching the given SkeletalAnimation */
    ENGINE_API void unset_animation (SkeletalAnimationHandle const& animation);


    /* Get a pointer to the SkeletalAnimationState associated with a given SkeletalAnimation in a SkeletonState.
     * Returns NULL if no SkeletalAnimationState matching the given SkeletalAnimation is found */
    ENGINE_API SkeletalAnimationState* get_animation_pointer (SkeletalAnimationHandle const& animation) const;

    /* Get the SkeletalAnimationState associated with a given SkeletalAnimation in a SkeletonState.
     * Panics if no SkeletalAnimationState matching the given SkeletalAnimation is found */
    ENGINE_API SkeletalAnimationState& get_animation (SkeletalAnimationHandle const& animation) const;


    /* Update the bone transforms and pose of a SkeletonState by spherical linear interpolating between individual keyframes as well as animation states */
    ENGINE_API Array<Matrix4> const& update_pose (f32_t delta_time);


    /* Clean up the heap allocations of a SkeletonState */
    ENGINE_API void destroy ();


    private:
      Array<SkeletalBlendState> blend_states;
      Array<SkeletalKeyframeChannel> intermediate_channels;

  };
}

#endif