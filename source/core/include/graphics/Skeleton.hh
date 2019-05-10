#ifndef SKELETON_H
#define SKELETON_H

#include "../cstd.hh"
#include "../Exception.hh"
#include "../Array.hh"
#include "../String.hh"
#include "../JSON.hh"

#include "../math/lib.hh"

#include "../AssetHandle.hh"



namespace mod {
  struct Bone {
    String name;
    s32_t parent_index;
    
    Transform3D base_transform;
    
    Matrix4 bind_matrix;
    Matrix4 inverse_bind_matrix;


    Bone () { }

    Bone (String const& in_name, s32_t in_parent_index, Transform3D const& in_base_transform)
    : name(in_name)
    , parent_index(in_parent_index)
    , base_transform(in_base_transform)
    { }

    void destroy () {
      name.destroy();
    }
  };



  struct Skeleton {
    char* origin;
    u32_t asset_id = 0;

    Array<Bone> bones;
    u32_t root_index;



    /* Create a new uninitialized Skeleton */
    Skeleton () { }

    /* Create a new Skeleton and initialize its Bones by copying from an array, and optionally set its root index (Defaults to 0) */
    ENGINE_API Skeleton (char const* in_origin, Array<Bone> const& in_bones, u32_t in_root = 0);

    /* Create a new Skeleton and initialize its Bones by taking ownership of an existing array, and optionally set its root index (Defaults to 0) */
    ENGINE_API static Skeleton from_ex (char const* origin, Array<Bone> const& bones, u32_t root = 0);

    /* Create a new Skeleton from a JSONItem */
    ENGINE_API static Skeleton from_json_item (char const* origin, JSONItem const& json);
    
    /* Create a new Skeleton from JSON */
    static Skeleton from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new Skeleton from a source str */
    ENGINE_API static Skeleton from_str (char const* origin, char const* source);

    /* Create a new Skeleton from a source file */
    ENGINE_API static Skeleton from_file (char const* origin);
    

    /* Clean up a Skeleton's heap data */
    ENGINE_API void destroy ();


    /* Get an ArrayIterator representing the start of a Skeletons Bones */
    ArrayIterator<Bone> begin () const {
      return bones.begin();
    }

    /* Get an ArrayIterator representing the end of a Skeletons Bones */
    ArrayIterator<Bone> end () const {
      return bones.end();
    }



    /* Iterate a Skeleton's Bones in hierarchical order */
    ENGINE_API void traverse (std::function<void (u32_t, Bone const&)> callback, s64_t index = -1) const;

    /* Iterate a Skeleton's Bones in hierarchical order, and stop iteration early if the callback returns false */
    ENGINE_API bool traverse_cond (std::function<bool (u32_t, Bone const&)> callback, s64_t index = -1) const;



    /* Get a pointer to a Bone in a Skeleton by index.
     * Returns NULL if no Bone with the given index exists */
    ENGINE_API Bone* get_bone_pointer (size_t index) const;

    /* Get a pointer to a Bone in a Skeleton by name.
     * Returns NULL if no Bone with the given name exists */
    ENGINE_API Bone* get_bone_pointer (char const* name) const;


    /* Get a reference to a Bone in a Skeleton by index.
     * Throws if the index is out of range */
    ENGINE_API Bone& get_bone (size_t index) const;

    /* Get a reference to a Bone in a Skeleton by index.
     * Throws if the index is out of range */
    Bone& operator [] (size_t index) const {
      return get_bone(index);
    }

    /* Get a reference to a Bone in a Skeleton by name.
     * Throws if no Bone with the given name is found */
    ENGINE_API Bone& get_bone (char const* name) const;

    /* Get a reference to a Bone in a Skeleton by name.
     * Throws if no Bone with the given name is found */
    Bone& operator [] (char const* name) const {
      return get_bone(name);
    }


    /* Get the total number of child Bones belonging to a Bone in a Skeleton.
     * Throws if the Bone does not belong to this Skeleton */
    ENGINE_API size_t get_child_count (Bone const& parent_bone) const;


    /* Determine if a Bone belongs to a Skeleton */
    bool owns_bone (Bone const* bone) const {
      return bone >= bones.elements
          && bone <  bones.elements + bones.count;
    }

    /* Determine if a Bone belongs to a Skeleton */
    bool owns_bone (Bone const& bone) const {
      return owns_bone(&bone);
    }


    /* Get a pointer to the parent Bone of a Bone in a Skeleton.
     * Throws if the Bone does not belong to this Skeleton, or has an invalid parent index.
     * Returns NULL if the Bone is a root and has no parent */
    ENGINE_API Bone* get_parent (Bone const& child_bone) const;


    /* Get the index of a Bone in a Skeleton.
     * Returns -1 if the Bone does not belong to this Skeleton */
    ENGINE_API s64_t get_index (Bone const& bone) const;


    /* Get the root Bone of a Skeleton.
     * Throws if the root Bone is invalid */
    Bone& root () const {
      return get_bone(root_index);
    }


    /* Calculate the bind matrix and its inverse for all Bones in a Skeleton.
     * The bind matrix represents the Bone's positioning relative to the Skeleton's root.
     * (This is different from the base matrix which is the positioning relative to the Bone's parent) */
    ENGINE_API void calculate_bind_matrices ();
    

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


  using SkeletonHandle = AssetHandle<Skeleton>;
}

#endif