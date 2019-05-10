#include "../../include/math/lib.hh"



namespace mod {
  Matrix3 Transform2D::compose () const {
    return Matrix3::compose(*this);
  }

  Matrix3 Matrix3::from_4 (Matrix4 const& m) {
    return row_major(
      m.e0, m.e4, m.e8,
      m.e1, m.e5, m.e9,
      m.e2, m.e6, m.e10
    );
  }

  Matrix3 Matrix3::compose_components (Vector2f const& position, f32_t rotation, Vector2f const& scale) {
    Matrix3 out = Matrix3::from_translation_vector(position);

    Matrix3 intermediate = Matrix3::from_rotation_angle(rotation);
    out.multiply_in_place(intermediate);

    intermediate = Matrix3::from_scale_vector(scale);
    out.multiply_in_place(intermediate);

    return out;
  }
  

  Matrix3 Matrix3::inverse () const {
    f32_t t0 = e8 * e4 - e7 * e5;
    f32_t t1 = e7 * e2 - e8 * e1;
    f32_t t2 = e5 * e1 - e4 * e2;

    f32_t det = e0 * t0 + e3 * t1 + e6 * t2;

    m_assert(det != 0.0f, "Couldn't get inverse of Matrix3, determinant of source matrix is 0");

    f32_t inv = 1.0f / det;
    
    return {
      t0 * inv,
      t1 * inv,
      t2 * inv,

      (e6 * e5 - e8 * e3) * inv,
      (e8 * e0 - e6 * e2) * inv,
      (e3 * e2 - e5 * e0) * inv,

      (e7 * e3 - e6 * e4) * inv,
      (e6 * e1 - e7 * e0) * inv,
      (e4 * e0 - e3 * e1) * inv
    };
  }


  Matrix3& Matrix3::multiply_in_place (Matrix3 const& l, Matrix3 const& r) {
    f32_t l0 = l.e0, l1 = l.e1, l2 = l.e2;
    f32_t l3 = l.e3, l4 = l.e4, l5 = l.e5;
    f32_t l6 = l.e6, l7 = l.e7, l8 = l.e8;

    f32_t r0 = r.e0, r1 = r.e1, r2 = r.e2;
    f32_t r3 = r.e3, r4 = r.e4, r5 = r.e5;
    f32_t r6 = r.e6, r7 = r.e7, r8 = r.e8;

    e0 = l0 * r0 + l1 * r3 + l2 * r6;
    e1 = l0 * r1 + l1 * r4 + l2 * r7;
    e2 = l0 * r2 + l1 * r5 + l2 * r8;

    e3 = l3 * r0 + l4 * r3 + l5 * r6;
    e4 = l3 * r1 + l4 * r4 + l5 * r7;
    e5 = l3 * r2 + l4 * r5 + l5 * r8;

    e6 = l6 * r0 + l7 * r3 + l8 * r6;
    e7 = l6 * r1 + l7 * r4 + l8 * r7;
    e8 = l6 * r2 + l7 * r5 + l8 * r8;

    return *this;
  }

  Matrix3 Matrix3::multiply (Matrix3 const& r) const {
    return {
      e0 * r.e0 + e1 * r.e3 + e2 * r.e6,
      e0 * r.e1 + e1 * r.e4 + e2 * r.e7,
      e0 * r.e2 + e1 * r.e5 + e2 * r.e8,

      e3 * r.e0 + e4 * r.e3 + e5 * r.e6,
      e3 * r.e1 + e4 * r.e4 + e5 * r.e7,
      e3 * r.e2 + e4 * r.e5 + e5 * r.e8,

      e6 * r.e0 + e7 * r.e3 + e8 * r.e6,
      e6 * r.e1 + e7 * r.e4 + e8 * r.e7,
      e6 * r.e2 + e7 * r.e5 + e8 * r.e8,
    };
  }

  Matrix3 Matrix3::multiply_scl (f32_t s) const {
    return {
      e0 * s, e1 * s, e2 * s,
      e3 * s, e4 * s, e5 * s,
      e6 * s, e7 * s, e8 * s
    };
  }


  f32_t Matrix3::determinant () const {
    return (e0 * e4 * e8)
         - (e0 * e5 * e7)
         - (e1 * e3 * e8)
         + (e1 * e5 * e6)
         + (e2 * e3 * e7)
         - (e2 * e4 * e6);
  }
}