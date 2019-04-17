#ifndef RAY2_H
#define RAY2_H

#include "Vector2.hh"



namespace mod {
  struct Line2;

  struct Ray2 {
    Vector2f origin = { 0.0f, 0.0f };
    Vector2f direction = { 0.0f, 0.0f };

    

    /* Create a new zero-initialized ray */
    constexpr Ray2 () { }


    /* Create a new ray and explicitly initialize its origin and direction */
    constexpr Ray2 (Vector2f const& in_origin, Vector2f const& in_direction)
    : origin(in_origin)
    , direction(in_direction)
    { }

    
    /* Create a new ray and initialize its origin and direction elements directly */
    constexpr Ray2 (f32_t o_x, f32_t o_y, f32_t d_x, f32_t d_y)
    : origin(o_x, o_y)
    , direction(d_x, d_y)
    { }


    /* Create a new ray from a line segment */
    ENGINE_API static Ray2 from_line (Line2 const& line);


    /* Get a vector at an offset distance along a ray */
    Vector2f vector_at_offset (f32_t d) const {
      return origin + (direction * d);
    }

    /* Get the nearest offset distance along a ray relative to a point, optionally clamped to the forward side of the ray origin (Defaults to true) */
    ENGINE_API f32_t offset_at_vector (Vector2f const& v, bool clamp_offset = true) const;

    /* Clamp a point to the nearest point along a ray, optionally clamped to the forward side of the ray origin (Defaults to true) */
    Vector2f clamp (Vector2f const& v, bool clamp_offset = true) const {
      return vector_at_offset(offset_at_vector(v, clamp_offset));
    }


    /* Get the square distance from a ray to a vector */
    f32_t distance_sq (Vector2f const& v) const {
      return clamp(v).distance_sq(v);
    }

    /* Get the distance from a ray to a vector */
    f32_t distance (Vector2f const& v) const {
      return clamp(v).distance(v);
    }


    /* Find the shortest line segment between two rays */
    ENGINE_API Line2 shortest_line_to_ray (Ray2 const& other) const;


    /* Determine if two rays are identical */
    bool equal (Ray2 const& r) {
      return origin == r.origin
          && direction == r.direction;
    }

    /* Determine if two rays are identical */
    bool operator == (Ray2 const& r) {
      return equal(r);
    }


    /* Determine if two rays are not identical */
    bool not_equal (Ray2 const& r) {
      return origin != r.origin
          || direction != r.direction;
    }

    /* Determine if two rays are not identical */
    bool operator != (Ray2 const& r) {
      return not_equal(r);
    }


    Ray2 apply_matrix (Matrix3 const& m) const {
      return {
        origin.apply_matrix(m),
        direction.transform_direction(m)
      };
    }

    Ray2 apply_matrix (Matrix4 const& m) const {
      return {
        origin.apply_matrix(m),
        direction.transform_direction(m)
      };
    }
  };
}

#endif