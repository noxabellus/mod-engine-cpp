#ifndef AABB2_H
#define AABB2_H

#include "Vector2.hh"



namespace mod {
  struct AABB3;

  struct Line2;
  struct Circle;
  struct Polygon;


  struct AABB2 {
    Vector2f min = { 0.0f, 0.0f };
    Vector2f max = { 0.0f, 0.0f };



    /* Create a new zero-initialized axis-aligned bounding box */
    constexpr AABB2 () { }

    /* Create a new axis-aligned bounding box and explicility initialize its minimum and maximum bounds */
    constexpr AABB2 (Vector2f const& in_min, Vector2f const& in_max)
    : min(in_min)
    , max(in_max)
    { }

    /* Create a new axis-aligned bounding box and explicility initialize its bounds' components */
    constexpr AABB2 (f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y)
    : min(min_x, min_y)
    , max(max_x, max_y)
    { }

  
    /* Create a new axis-aligned bounding box from a center position and a size */
    constexpr static AABB2 from_center_and_size (Vector2f const& center, Vector2f const& size) {
      Vector2f half_size = size / 2.0f;

      return {
        center - half_size,
        center + half_size
      };
    }

    /* Create a new axis-aligned bounding box from a circle */
    ENGINE_API static AABB2 from_circle (Circle const& cirlce);

    /* Create a 2D axis-aligned bounding box from a 3d one */
    ENGINE_API static AABB2 from_3 (AABB3 const& aabb);
    

    /* Create a new axis-aligned bounding box that will fully contain a parameter-pack list of points */
    template <typename ... A> static AABB2 from_vectors (Vector2f arg0, A ... args) {
      static constexpr size_t arg_count = sizeof...(args) + 1;
      Vector2f vargs [arg_count] = { arg0, ((Vector2f) args)... };
      return from_vector_list(vargs, arg_count);
    }

    /* Create a new axis-aligned bounding box that will fully contain a list of points */
    ENGINE_API static AABB2 from_vector_list (Vector2f const* vectors, size_t vector_count);

    /* Create a quad_t with the corner vertices of an AABB */
    quad_t<Vector2f> to_polygon () const {
      return {
        min,
        { max.x, min.y },
        max,
        { min.x, max.y }
      };
    }


    /* Create a new axis-aligned bounding box by combining with another */
    AABB2 union_with (AABB2 const& r) const {
      return {
        min.min(r.min),
        max.max(r.max)
      };
    }

    /* Create a new axis-aligned bounding box that is cropped to fit within another */
    AABB2 crop_with (AABB2 const& r) const {
      return {
        min.max(r.min),
        max.min(r.max)
      };
    }


    /* Expand an axis-aligned bounding box to contain a point */
    AABB2& expand_to_vector (Vector2f const& v) {
      min = min.min(v);
      max = max.max(v);
      return *this;
    }

    /* Expand an axis-aligned bounding box in both directions by a given size vector */
    AABB2& expand_by_vector (Vector2f const& v) {
      min -= v;
      max += v;
      return *this;
    }

    /* Expand an axis-aligned bounding box in both directions by a given size scalar */
    AABB2& expand_by_scalar (f32_t s) {
      min -= s;
      max += s;
      return *this;
    }


    /* Translate an axis-aligned bounding box so that it is centered on a new point */
    AABB2& move_to_vector (Vector2f const& v) {
      Vector2f hs = half_size();
      min = v - hs;
      max = v + hs;
      return *this;
    }

    /* Translate an axis-aligned bounding box using a vector offset from its current bounds */
    AABB2& move_by_vector (Vector2f const& v) {
      min += v;
      max += v;
      return *this;
    }


    /* Get the total size of an axis-aligned bounding box */
    Vector2f size () const {
      return max - min;
    }

    /* Get the total size of an axis-aligned bounding box and divide by two */
    Vector2f half_size () const {
      return size() / 2.0f;
    }

    /* Get the center point of an axis-aligned bounding box */
    Vector2f center () const {
      return min + half_size();  
    }

    /* Get the total area covered by an axis-aligned bounding box */
    f32_t area () const {
      return size().mul_reduce();
    }


    /* Clamp a vector to the inside of an axis-aligned bounding box */
    Vector2f clamp (Vector2f const& v) const {
      return v.clamp(min, max);
    }


    /* Get the square of the distance from an axis-aligned bounding box to some point */
    f32_t distance_sq (Vector2f const& v) const {
      return clamp(v).distance_sq(v);
    }

    /* Get the distance from an axis-aligned bounding box to some point */
    f32_t distance (Vector2f const& v) const {
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


    /* Determine if two axis-aligned bounding boxes are essentially equivalent.
     * Wrapper for num::almost_equal, see it for details */
    bool almost_equal (AABB2 const& r, s32_t ulp = 2) const {
      return min.almost_equal(r.min, ulp)
          && max.almost_equal(r.max, ulp);
    }


    /* Determine if two axis-aligned bounding boxes are identical */
    bool equal (AABB2 const& r) const {
      return min == r.min
          && max == r.max;
    }

    /* Determine if two axis-aligned bounding boxes are identical */
    bool operator == (AABB2 const& r) const {
      return equal(r);
    }


    /* Determine if two axis-aligned bounding boxes are not identical */
    bool not_equal (AABB2 const& r) const {
      return min != r.min
          || max != r.max;
    }

    /* Determine if two axis-aligned bounding boxes are not identical */
    bool operator != (AABB2 const& r) const {
      return not_equal(r);
    }


    /* Transform an axis-aligned bounding box using a matrix3 */
    ENGINE_API AABB2 apply_matrix (Matrix3 const& m) const;

    /* Transform an axis-aligned bounding box using a matrix4 */
    ENGINE_API AABB2 apply_matrix (Matrix4 const& m) const;
  };


  namespace Constants {
    namespace AABB2 {
      static constexpr ::mod::AABB2 zero = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
      static constexpr ::mod::AABB2 unit = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
      static constexpr ::mod::AABB2 unit_cen = { { -0.5f, -0.5f }, { 0.5f, 0.5f } };
      static constexpr ::mod::AABB2 infinite = { { -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity() }, { std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity() } };
      static constexpr ::mod::AABB2 empty = { { std::numeric_limits<f32_t>::infinity(), std::numeric_limits<f32_t>::infinity() }, { -std::numeric_limits<f32_t>::infinity(), -std::numeric_limits<f32_t>::infinity() } };
    }
  }
}

#endif