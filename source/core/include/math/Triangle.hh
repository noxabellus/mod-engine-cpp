#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector3.hh"
#include "Matrix3.hh"
#include "Matrix4.hh"



namespace mod {
  struct Triangle {
    union {
      struct {
        Vector3f a;
        Vector3f b;
        Vector3f c;
      };
      Vector3f corners [3];
      f32_t elements [9];
    };


    /* Create a new zero-initialized Triangle */
    constexpr Triangle ()
    : a(0.0f, 0.0f, 0.0f)
    , b(0.0f, 0.0f, 0.0f)
    , c(0.0f, 0.0f, 0.0f)
    { }

    /* Create a new Triangle and explicitly initialize its corner points */
    constexpr Triangle (Vector3f const& in_a, Vector3f const& in_b, Vector3f const& in_c)
    : a(in_a)
    , b(in_b)
    , c(in_c)
    { }

    /* Create a new Triangle and initialize its corner points' elements individually */
    constexpr Triangle (
      f32_t a_x, f32_t a_y, f32_t a_z,
      f32_t b_x, f32_t b_y, f32_t b_z,
      f32_t c_x, f32_t c_y, f32_t c_z
    )
    : a(a_x, a_y, a_z)
    , b(b_x, b_y, b_z)
    , c(c_x, c_y, c_z)
    { }



    /* Get the normal of the face of a Triangle.
     * Assumes counter-clockwise winding order */
    ENGINE_API Vector3f normal () const;

    /* Convert a vector to barycentric coordinates relative to  a Triangle */
    ENGINE_API Vector3f get_barycentric_coordinate (Vector3f const& v) const;

    /* Get the UV coordinate offset of a location relative to a Triangle's uv coordinates */
    ENGINE_API Vector2f get_uv (Vector2f const& uv0, Vector2f const& uv1, Vector2f const& uv2, Vector3f const& vector) const;

    /* Get the total surface area of a Triangle */
    f32_t area () const {
      return (c - b).cross(a - b).length() * 0.5f;
    }

    /* Get the center point of a Triangle */
    Vector3f center () const {
      return (a + b + c) * (1.0f / 3.0f);
    }

    /* Clamp a vector to the coplanar interior of a Triangle */
    ENGINE_API Vector3f clamp (Vector3f const& v) const;


    /* Determine if two Triangles are identical */
    bool equal (Triangle const& r) const {
      return a == r.a
          && b == r.b
          && c == r.c;
    }

    /* Determine if two Triangles are identical */
    bool operator == (Triangle const& r) const {
      return equal(r);
    }


    /* Determine if two Triangles are not identical */
    bool not_equal (Triangle const& r) const {
      return a != r.a
          || b != r.b
          || c != r.c;
    }

    /* Determine if two Triangles are not identical */
    bool operator != (Triangle const& r) const {
      return not_equal(r);
    }


    /* Tranform a Triangle using a matrix3 */
    Triangle apply_matrix (Matrix3 const& m) {
      return {
        a.apply_matrix(m),
        b.apply_matrix(m),
        c.apply_matrix(m)
      };
    }

    /* Tranform a Triangle using a matrix4 */
    Triangle apply_matrix (Matrix4 const& m) {
      return {
        a.apply_matrix(m),
        b.apply_matrix(m),
        c.apply_matrix(m)
      };
    }
  };
}

#endif