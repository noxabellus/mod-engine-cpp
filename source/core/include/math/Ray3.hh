#pragma once

#include "Vector3.hh"



namespace mod {
  struct Line3;
  struct Plane;

  
  struct Ray3 {
    Vector3f origin = { 0.0f, 0.0f, 0.0f };
    Vector3f direction = { 0.0f, 0.0f, 0.0f };

    

    /* Create a new zero-initialized ray */
    constexpr Ray3 () { }


    /* Create a new ray and explicitly initialize its origin and direction */
    constexpr Ray3 (Vector3f const& in_origin, Vector3f const& in_direction)
    : origin(in_origin)
    , direction(in_direction)
    { }

    
    /* Create a new ray and initialize its origin and direction elements directly */
    constexpr Ray3 (f32_t o_x, f32_t o_y, f32_t o_z, f32_t d_x, f32_t d_y, f32_t d_z)
    : origin(o_x, o_y, o_z)
    , direction(d_x, d_y, d_z)
    { }


    /* Create a new ray from a line segment */
    ENGINE_API static Ray3 from_line (Line3 const& line);


    /* Get a vector at an offset distance along a ray */
    Vector3f vector_at_offset (f32_t d) const {
      return origin + (direction * d);
    }

    /* Get the nearest offset distance along a ray relative to a point, optionally clamped to the forward side of the ray origin (Defaults to true) */
    ENGINE_API f32_t offset_at_vector (Vector3f const& v, bool clamp_offset = true) const;

    /* Clamp a point to the nearest point along a ray, optionally clamped to the forward side of the ray origin (Defaults to true) */
    Vector3f clamp (Vector3f const& v, bool clamp_offset = true) const {
      return vector_at_offset(offset_at_vector(v, clamp_offset));
    }


    /* Get the square distance from a ray to a vector */
    f32_t distance_sq (Vector3f const& v) const {
      return clamp(v).distance_sq(v);
    }

    /* Get the distance from a ray to a vector */
    f32_t distance (Vector3f const& v) const {
      return clamp(v).distance(v);
    }


    /* Get the shortest line segment between a ray and a line segment */
    ENGINE_API Line3 shortest_line_to_line (Line3 const& line) const;


    /* Get the minimum square distance between a ray and a line segment */
    f32_t distance_sq (Line3 const& line) const {
      return shortest_line_to_line(line).length_sq();
    }

    /* Get the minimum distance between a ray and a line segment */
    f32_t distance (Line3 const& line) const {
      return sqrtf(distance_sq(line));
    }

    /* Get the minimum distance between a ray and a plane, if the ray intersects the plane. If no intersection occurs, returns INFINITY */
    ENGINE_API f32_t distance (Plane const& plane) const;


    /* Determine if two rays are identical */
    bool equal (Ray3 const& r) {
      return origin == r.origin
          && direction == r.direction;
    }

    /* Determine if two rays are identical */
    bool operator == (Ray3 const& r) {
      return equal(r);
    }


    /* Determine if two rays are not identical */
    bool not_equal (Ray3 const& r) {
      return origin != r.origin
          || direction != r.direction;
    }

    /* Determine if two rays are not identical */
    bool operator != (Ray3 const& r) {
      return not_equal(r);
    }


    Ray3 apply_matrix (Matrix3 const& m) const {
      return {
        origin.apply_matrix(m),
        direction.transform_direction(m)
      };
    }

    Ray3 apply_matrix (Matrix4 const& m) const {
      return {
        origin.apply_matrix(m),
        direction.transform_direction(m)
      };
    }
  };
}