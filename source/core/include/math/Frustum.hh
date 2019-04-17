#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Plane.hh"



namespace mod {
  struct Frustum {
    union {
      struct {
        Plane a;
        Plane b;
        Plane c;
        Plane d;
        Plane e;
        Plane f;
      };
      Plane planes [6];
      f32_t elements [6 * 4];
    };


    /* Create a new zero-initialized Frustum */
    constexpr Frustum ()
    : a(0.0f, 0.0f, 0.0f, 0.0f)
    , b(0.0f, 0.0f, 0.0f, 0.0f)
    , c(0.0f, 0.0f, 0.0f, 0.0f)
    , d(0.0f, 0.0f, 0.0f, 0.0f)
    , e(0.0f, 0.0f, 0.0f, 0.0f)
    , f(0.0f, 0.0f, 0.0f, 0.0f)
    { }

    /* Create a new Frustum and explicitly initialize each of its planes */
    constexpr Frustum (
      Plane const& in_a,
      Plane const& in_b,
      Plane const& in_c,
      Plane const& in_d,
      Plane const& in_e,
      Plane const& in_f
    )
    : a(in_a)
    , b(in_b)
    , c(in_c)
    , d(in_d)
    , e(in_e)
    , f(in_f)
    { }

    /* Create a new Frustum and initialize each of its planes' elements individually */
    constexpr Frustum (
      f32_t a_x, f32_t a_y, f32_t a_z, f32_t a_c,
      f32_t b_x, f32_t b_y, f32_t b_z, f32_t b_c,
      f32_t c_x, f32_t c_y, f32_t c_z, f32_t c_c,
      f32_t d_x, f32_t d_y, f32_t d_z, f32_t d_c,
      f32_t e_x, f32_t e_y, f32_t e_z, f32_t e_c,
      f32_t f_x, f32_t f_y, f32_t f_z, f32_t f_c
    )
    : a(a_x, a_y, a_z, a_c)
    , b(b_x, b_y, b_z, b_c)
    , c(c_x, c_y, c_z, c_c)
    , d(d_x, d_y, d_z, d_c)
    , e(e_x, e_y, e_z, e_c)
    , f(f_x, f_y, f_z, f_c)
    { }

    /* Create a new Frustum from a projection matrix */
    static Frustum from_matrix (Matrix4 const& m) {
      return {
        (Plane { m[3] - m[0], m[7] - m[4], m[11] - m[8],  m[15] - m[12] }).normalize(),
        (Plane { m[3] + m[0], m[7] + m[4], m[11] + m[8],  m[15] + m[12] }).normalize(),
        (Plane { m[3] + m[1], m[7] + m[5], m[11] + m[9],  m[15] + m[13] }).normalize(),
        (Plane { m[3] - m[1], m[7] - m[5], m[11] - m[9],  m[15] - m[13] }).normalize(),
        (Plane { m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14] }).normalize(),
        (Plane { m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14] }).normalize()
      };
    }
  };
}

#endif