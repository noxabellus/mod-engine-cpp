#pragma once

#include "Vector3.hh"



namespace mod {
  struct Ray3;

  struct Line3 {
    Vector3f a = { 0.0f, 0.0f, 0.0f };
    Vector3f b = { 0.0f, 0.0f, 0.0f };



    /* Create a new zero-initialized line segment */
    constexpr Line3 () { }

    /* Create a new line segment and explicitly initialize its end points */
    constexpr Line3 (Vector3f const& in_a, Vector3f const& in_b)
    : a(in_a)
    , b(in_b)
    { }

    /* Create a new line segment and directly initialize the components of its end points */
    constexpr Line3 (f32_t a_x, f32_t a_y, f32_t a_z, f32_t b_x, f32_t b_y, f32_t b_z)
    : a(a_x, a_y, a_z)
    , b(b_x, b_y, b_z)
    { }


    /* Create a new line segment from a ray, with an optional length (Defaults to 1.0f) */
    ENGINE_API static Line3 from_ray (Ray3 const& ray, f32_t length = 1.0f);


    /* Get the difference between the end points of a line segment */
    Vector3f delta () const {
      return b - a;
    }

    /* Get the direction vector pointing from a line segment's a end point to its b end point */
    Vector3f direction () const {
      return a.direction(b);
    }

    /* Get the square length/magnitude of the difference between the end points of a line segment */
    f32_t length_sq () const {
      return a.distance_sq(b);
    }

    /* Get the length/magnitude of the difference between the end points of a line segment */
    f32_t length () const {
      return a.distance(b);
    }

    /* Get a vector at some offset 0 to 1 along a line segment */
    Vector3f vector_at_offset (f32_t offset) const {
      return a + (delta() * offset);
    }

    /* Get an offset 0 to 1 along a line segment where it is closest to a vector, optionally clamped to within the end points (Defaults to true) */
    ENGINE_API f32_t offset_at_vector (Vector3f const& v, bool clamp_offset = true) const;

    /* Clamp a vector onto a line segment, optionally clamped to within the end points (Defaults to true) */
    Vector3f clamp (Vector3f const& v, bool clamp_offset = true) const {
      return vector_at_offset(offset_at_vector(v, clamp_offset));
    }

    /* Get the center point of a line segment */
    Vector3f center () const {
      return vector_at_offset(0.5f);
    }


    /* Get the square distance from a line segment to a vector */
    f32_t distance_sq (Vector3f const& v) const {
      return clamp(v).distance_sq(v);
    }

    /* Get the distance from a line segment to a vector */
    f32_t distance (Vector3f const& v) const {
      return clamp(v).distance(v);
    }


    /* Get the index of the shortest line in a list of lines */
    ENGINE_API static size_t index_of_shortest (Line3 const* lines, size_t line_count);

    /* Get the index of the longest line in a list of lines */
    ENGINE_API static size_t index_of_longest (Line3 const* lines, size_t line_count);


    // TODO shortest line to line 3d
    // TODO shortest line to ray 3d


    /* Determine if two line segments are identical */
    bool equal (Line3 const& r) const {
      return a == r.a
          && b == r.b;
    }

    /* Determine if two line segments are identical */
    bool operator == (Line3 const& r) const {
      return equal(r);
    }


    /* Determine if two line segments are not identical */
    bool not_equal (Line3 const& r) const {
      return a != r.a
          || b != r.b;
    }

    /* Determine if two line segments are not identical */
    bool operator != (Line3 const& r) const {
      return not_equal(r);
    }


    /* Transform a line segment using a matrix3 */
    Line3 apply_matrix (Matrix3 const& m) {
      return { a.apply_matrix(m), b.apply_matrix(m) };
    }

    /* Transform a line segment using a matrix4 */
    Line3 apply_matrix (Matrix4 const& m) {
      return { a.apply_matrix(m), b.apply_matrix(m) };
    }
  };
}