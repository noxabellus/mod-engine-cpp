#ifndef AABB3_H
#define AABB3_H

#include "AABB2.hh"

#include "Vector3.hh"
#include "Matrix4.hh"


namespace mod {
  struct Line3;
  struct Sphere;
  struct Triangle;


  struct AABB3 {
    Vector3f min = { 0.0f, 0.0f, 0.0f };
    Vector3f max = { 0.0f, 0.0f, 0.0f };



    /* Create a new zero-initialized axis-aligned bounding box */
    constexpr AABB3 () { }

    /* Create a new axis-aligned bounding box and explicility initialize its minimum and maximum bounds */
    constexpr AABB3 (Vector3f const& in_min, Vector3f const& in_max)
    : min(in_min)
    , max(in_max)
    { }


    /* Create a new axis-aligned bounding box from a center position and a size */
    constexpr static AABB3 from_center_and_size (Vector3f const& center, Vector3f const& size) {
      Vector3f half_size = size / 2.0f;

      return {
        center - half_size,
        center + half_size
      };
    }

    /* Create a new axis-aligned bounding box and explicility initialize its bounds' components */
    constexpr AABB3 (f32_t min_x, f32_t min_y, f32_t min_z, f32_t max_x, f32_t max_y, f32_t max_z)
    : min(min_x, min_y, min_z)
    , max(max_x, max_y, max_z)
    { }

    /* Create a new 3D axis-aligned bounding box from a 2D one and optional z dimensions (Default to -0.5f, 0.5f) */
    constexpr static AABB3 from_2 (AABB2 const& aabb, f32_t min_z = -0.5f, f32_t max_z = 0.5f) {
      return {
        { aabb.min.x, aabb.min.y, min_z },
        { aabb.max.x, aabb.max.y, max_z }
      };
    }

    /* Create a new axis-aligned bounding box from a sphere */
    ENGINE_API static AABB3 from_sphere (Sphere const& sphere);
    

    /* Create a new axis-aligned bounding box that will fully contain a parameter-pack list of points */
    template <typename ... A> static AABB3 from_vectors (Vector3f arg0, A ... args) {
      static constexpr size_t arg_count = sizeof...(args) + 1;
      Vector3f vectors [arg_count] = { arg0, ((Vector3f) args)... };
      return from_vector_list(vectors, arg_count);
    }

    /* Create a new axis-aligned bounding box that will fully contain a list of points */
    ENGINE_API static AABB3 from_vector_list (Vector3f const* vectors, size_t vector_count);


    /* Create a new axis-aligned bounding box by combining with another */
    AABB3 union_with (AABB3 const& r) const {
      return {
        min.min(r.min),
        max.max(r.max)
      };
    }

    /* Create a new axis-aligned bounding box that is cropped to fit within another */
    AABB3 crop_with (AABB3 const& r) const {
      return {
        min.max(r.min),
        max.min(r.max)
      };
    }


    /* Expand an axis-aligned bounding box to contain a point */
    AABB3& expand_to_vector (Vector3f const& v) {
      min = min.min(v);
      max = max.max(v);
      return *this;
    }

    /* Expand an axis-aligned bounding box in both directions by a given size vector */
    AABB3& expand_by_vector (Vector3f const& v) {
      min -= v;
      max += v;
      return *this;
    }

    /* Expand an axis-aligned bounding box in both directions by a given size scalar */
    AABB3& expand_by_scalar (f32_t s) {
      min -= s;
      max += s;
      return *this;
    }


    /* Translate an axis-aligned bounding box so that it is centered on a new point */
    AABB3& move_to_vector (Vector3f const& v) {
      Vector3f hs = half_size();
      min = v - hs;
      max = v + hs;
      return *this;
    }

    /* Translate an axis-aligned bounding box using a vector offset from its current bounds */
    AABB3& move_by_vector (Vector3f const& v) {
      min += v;
      max += v;
      return *this;
    }


    /* Get the total size of an axis-aligned bounding box */
    Vector3f size () const {
      return max - min;
    }

    /* Get the total size of an axis-aligned bounding box and divide by two */
    Vector3f half_size () const {
      return size() / 2.0f;
    }

    /* Get the center point of an axis-aligned bounding box */
    Vector3f center () const {
      return min + half_size();  
    }

    /* Get the total volume covered by an axis-aligned bounding box */
    f32_t volume () const {
      return size().mul_reduce();
    }


    /* Clamp a vector to the inside of an axis-aligned bounding box */
    Vector3f clamp (Vector3f const& v) const {
      return v.clamp(min, max);
    }


    /* Get the square of the distance from an axis-aligned bounding box to some point */
    f32_t distance_sq (Vector3f const& v) const {
      return clamp(v).distance_sq(v);
    }

    /* Get the distance from an axis-aligned bounding box to some point */
    f32_t distance (Vector3f const& v) const {
      return sqrtf(distance_sq(v));
    }


    /* Determine if an axis-aligned bounding box designates a valid area (min < max) */
    bool is_valid () const {
      return min.x < max.x
          && min.y < max.y;
    }

    /* Determine if an axis-aligned bounding box does not desingate a valid area (min >= max) */
    bool is_empty () const {
      return min.x >= max.x
          || min.y >= max.y;
    }


    /* Determine if two axis-aligned bounding boxes are identical */
    bool equal (AABB3 const& r) const {
      return min == r.min
          && max == r.max;
    }

    /* Determine if two axis-aligned bounding boxes are identical */
    bool operator == (AABB3 const& r) const {
      return equal(r);
    }


    /* Determine if two axis-aligned bounding boxes are not identical */
    bool not_equal (AABB3 const& r) const {
      return min != r.min
          || max != r.max;
    }

    /* Determine if two axis-aligned bounding boxes are not identical */
    bool operator != (AABB3 const& r) const {
      return not_equal(r);
    }
    
    
    /* Transform an axis-aligned bounding box using a matrix3 */
    ENGINE_API AABB3 apply_matrix (Matrix3 const& m) const;

    /* Transform an axis-aligned bounding box using a matrix4 */
    ENGINE_API AABB3 apply_matrix (Matrix4 const& m) const;
  };


  namespace Constants {
    namespace AABB3 {
      static constexpr ::mod::AABB3 zero = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      static constexpr ::mod::AABB3 unit = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
      static constexpr ::mod::AABB3 unit_cen = { { -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f } };
      static constexpr ::mod::AABB3 infinite = { { -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity() }, { std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity() } };
      static constexpr ::mod::AABB3 empty = { { std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity() }, { -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity() } };
    }
  }
}

#endif