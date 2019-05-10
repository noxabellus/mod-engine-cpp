#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H

#include "../cstd.hh"
#include "../Exception.hh"
#include "../Array.hh"
#include "../JSON.hh"

#include "../math/lib.hh"

#include "../AssetHandle.hh"



namespace mod {
  struct SkeletalKeyframeChannel {
    u32_t target_index;
    Transform3D transform;
  };

  struct SkeletalKeyframe {
    f32_t time;
    Array<SkeletalKeyframeChannel> transforms;

    void destroy () {
      transforms.destroy();
    }

    Transform3D* get_bone_transform (u32_t target_index) const {
      for (auto [ i, kft ] : transforms) {
        if (kft.target_index == target_index) return &kft.transform;
      }

      return NULL;
    }
  };

  struct SkeletalAnimation {
    char* origin;
    u32_t asset_id = 0;

    Array<SkeletalKeyframe> keyframes;
    f32_t time_scale;
    f32_t length;

    

    /* Create a new uninitialized SkeletalAnimation */
    SkeletalAnimation () { }

    /* Create a new SkeletalAnimation, initialize its Keyframes by copying from an array,
     * and optionally set its time_scale (Defaults to 1.0f)
     * and its length (Defaults to -1.0f which indicates that the length should be determined by the time value of the last Keyframe) */
    ENGINE_API SkeletalAnimation (char const* in_origin, Array<SkeletalKeyframe> const& in_keyframes, f32_t in_time_scale = 1.0f, f32_t in_length = -1.0);


    /* Create a new SkeletalAnimation, initialize its Keyframes by taking ownership of an array,
     * and optionally set its time_scale (Defaults to 1.0f)
     * and its length (Defaults to -1.0f which indicates that the length should be determined by the time value of the last Keyframe) */
    ENGINE_API static SkeletalAnimation from_ex (char const* origin, Array<SkeletalKeyframe> const& keyframes, f32_t time_scale = 1.0f, f32_t length = -1.0f);


    /* Create a new SkeletalAnimation from a JSONItem */
    ENGINE_API static SkeletalAnimation from_json_item (char const* origin, JSONItem const& json);

    /* Create a new SkeletalAnimation from JSON */
    static SkeletalAnimation from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new SkeletalAnimation from a source str */
    ENGINE_API static SkeletalAnimation from_str (char const* origin, char const* source);

    /* Create a new SkeletalAnimation from a source file */
    ENGINE_API static SkeletalAnimation from_file (char const* origin);


    /* Throws an optionally terminal (defaults to true) asset error if a SkeletalAnimation's keyframes do not all have matching target indices for their transforms */
    ENGINE_API void validate_keyframes (bool terminal = true);


    /* Clean up a SkeletalAnimation's heap allocations */
    ENGINE_API void destroy ();


    /* Calculate and update the length value of a SkeletalAnimation from its last Keyframe.
     * Panics if the Keyframe array is empty */
    ENGINE_API void calculate_length ();


    /* Get the last SkeletalKeyframe before a given time in a SkeletalAnimation.
     * If the time is exactly equal to a Keyframe's time, returns that Keyframe.
     * Only works for times between 0 and `length` */
    ENGINE_API SkeletalKeyframe const& keyframe_before (f32_t offset_time) const;

    /* Get the first SkeletalKeyframe after a given time in a SkeletalAnimation.
     * If the time is exactly equal to a Keyframe's time, returns that Keyframe.
     * Only works for times between 0 and `length` */
    ENGINE_API SkeletalKeyframe const& keyframe_after (f32_t offset_time) const;



    /* Get the spherical interpolated version of every Transform at a given time in a SkeletalAnimation,
     * and copy them into the given output array, overwriting any existing data.
     * Time is divided by the Animations `time_scale` and modulused with its `length` */
    ENGINE_API void get_pose_slerp (f32_t offset_time, Array<SkeletalKeyframeChannel>& out_transforms) const;

    /* Get the interpolated version of every Transform at a given time in a SkeletalAnimation,
     * and copy them into the given output array, overwriting any existing data.
     * Time is divided by the Animations `time_scale` and modulused with its `length` */
    ENGINE_API void get_pose_lerp (f32_t offset_time, Array<SkeletalKeyframeChannel>& out_transforms) const;

    

    /* Throw an exception using the origin of this Asset, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (s32_t line, char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, char const* fmt, A ... args) {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, s32_t line, char const* fmt, A ... args) {
      if (!cond) asset_error(line, fmt, args...);
    }


    /* Throw an exception using the origin of this Asset, and potentially destroy the asset */
    template <typename ... A> NORETURN void asset_error_cond_terminal (bool terminal, char const* fmt, A ... args) {
      if (terminal) asset_error_terminal(fmt, args...);
      else asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and potentially destroy the asset */
    template <typename ... A> NORETURN void asset_error_cond_terminal (bool terminal, s32_t line, char const* fmt, A ... args) {
      if (terminal) asset_error_terminal(fmt, args...);
      else asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, and potentially destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_cond_terminal (bool terminal, bool cond, char const* fmt, A ... args) {
      if (!cond) asset_error_cond_terminal(terminal, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and potentially destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_cond_terminal (bool terminal, bool cond, s32_t line, char const* fmt, A ... args) {
      if (!cond) asset_error_cond_terminal(terminal, line, fmt, args...);
    }


    /* Throw an exception using the origin of this Asset */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number */
    template <typename ... A> NORETURN void asset_error (s32_t line, char const* fmt, A ... args) const {
      m_asset_error(origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, s32_t line, char const* fmt, A ... args) const {
      if (!cond) asset_error(line, fmt, args...);
    }
  };

  using SkeletalAnimationHandle = AssetHandle<SkeletalAnimation>;
}

#endif