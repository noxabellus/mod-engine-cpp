#include "../../include/math/lib.hh"



namespace mod {
  Matrix4 Transform3D::compose () const {
    return Matrix4::compose(*this);
  }

  Matrix4 Matrix4::from_3 (Matrix3 const& m, Vector3f const& axis) {
    Transform2D t = m.decompose();

    Matrix4 out = Matrix4::from_translation_vector({ t.position.x, t.position.y, 0.0f });

    Matrix4 intermediate = Matrix4::from_axis_angle(axis, t.rotation);
    out.multiply_in_place(intermediate);

    intermediate = Matrix4::from_scale_vector({ t.scale.x, t.scale.y, 1.0f });
    out.multiply_in_place(intermediate);

    return out;
  }

  Matrix4 Matrix4::from_perspective (f32_t l, f32_t r, f32_t t, f32_t b, f32_t n, f32_t f) {
    f32_t x =  2.0f * n     / (r - l);
    f32_t y =  2.0f * n     / (t - b);
    f32_t z = -2.0f * f * n / (f - n);

    f32_t f0 =  (r + l) / (r - l);
    f32_t f1 =  (t + b) / (t - b);
    f32_t f2 = -(f + n) / (f - n);

    return {
      x,    0.0f, 0.0f,  0.0f,
      0.0f, y,    0.0f,  0.0f,
      f0,   f1,   f2,   -1.0f,
      0.0f, 0.0f, z,     0.0f 
    };
  }

  Matrix4 Matrix4::from_orthographic (f32_t l, f32_t r, f32_t t, f32_t b, f32_t n, f32_t f) {
    f32_t w = 1.0f / (r - l);
    f32_t h = 1.0f / (t - b);
    f32_t d = 1.0f / (f - n);

    f32_t x = (r + l) * w;
    f32_t y = (t + b) * h;
    f32_t z = (f + n) * d;

    return {
      2.0f * w, 0.0f,     0.0f,      0.0f,
      0.0f,     2.0f * h, 0.0f,      0.0f,
      0.0f,     0.0f,     -2.0f * d, 0.0f,
      -x,       -y,       -z,        1.0f
    };
  }

  Matrix4 Matrix4::from_look (Vector3f const& position, Vector3f const& target, Vector3f const& up, bool set_position) {
    Vector3f z = position - target;

    if (z.length_sq() == 0.0f) z.z = 1.0f;

    z = z.normalize();

    Vector3f x = up.cross(z);

    if (x.length_sq() == 0.0f) {
      if (num::abs(up.z) == 1.0f) {
        z.y += 0.0001f;
      } else {
        z.z += 0.0001f;
      }

      z = z.normalize();
      x = up.cross(z);
    }
    
    x = x.normalize();

    Vector3f y = z.cross(x);

    Vector3f p = set_position? position : Constants::Vector3f::zero;

    return {
      x.x, x.y, x.z, 0.0f,
      y.x, y.y, y.z, 0.0f,
      z.x, z.y, z.z, 0.0f,
      p.x, p.y, p.z, 1.0f
    };
  }

  Matrix4 Matrix4::from_euler (Euler const& euler) {
    f32_t a = cosf(euler.angles.x);
    f32_t b = sinf(euler.angles.x);

    f32_t c = cosf(euler.angles.y);
    f32_t d = sinf(euler.angles.y);

    f32_t e = cosf(euler.angles.z);
    f32_t f = sinf(euler.angles.z);

    Matrix4 out;

    switch (euler.order) {
      case EulerOrder::XYZ: {
        f32_t ae = a * e;
        f32_t af = a * f;
        f32_t be = b * e;
        f32_t bf = b * f;

        out.e0 = c * e;
        out.e4 = -c * f;
        out.e8 = d;

        out.e1 = af + be * d;
        out.e5 = ae - bf * d;
        out.e9 = -b * c;

        out.e2 = bf - ae * d;
        out.e6 = be + af * d;
        out.e10 = a * c;
      } break;

      case EulerOrder::XZY: {
        f32_t ac = a * c;
        f32_t ad = a * d;
        f32_t bc = b * c;
        f32_t bd = b * d;

        out.e0 = c * e;
        out.e4 = -f;
        out.e8 = d * e;

        out.e1 = ac * f + bd;
        out.e5 = a * e;
        out.e9 = ad * f - bc;

        out.e2 = bc * f - ad;
        out.e6 = b * e;
        out.e10 = bd * f + ac;
      } break;

      case EulerOrder::YXZ: {
        f32_t ce = c * e;
        f32_t cf = c * f;
        f32_t de = d * e;
        f32_t df = d * f;

        out.e0 = ce + df * b;
        out.e4 = de * b - cf;
        out.e8 = a * d;

        out.e1 = a * f;
        out.e5 = a * e;
        out.e9 = - b;

        out.e2 = cf * b - de;
        out.e6 = df + ce * b;
        out.e10 = a * c;
      } break;
      
      case EulerOrder::YZX: {
        f32_t ac = a * c;
        f32_t ad = a * d;
        f32_t bc = b * c;
        f32_t bd = b * d;

        out.e0 = c * e;
        out.e4 = bd - ac * f;
        out.e8 = bc * f + ad;

        out.e1 = f;
        out.e5 = a * e;
        out.e9 = -b * e;

        out.e2 = -d * e;
        out.e6 = ad * f + bc;
        out.e10 = ac - bd * f;
      } break;

      case EulerOrder::ZXY: {
        f32_t ce = c * e;
        f32_t cf = c * f;
        f32_t de = d * e;
        f32_t df = d * f;

        out.e0 = ce - df * b;
        out.e4 = -a * f;
        out.e8 = de + cf * b;

        out.e1 = cf + de * b;
        out.e5 = a * e;
        out.e9 = df - ce * b;

        out.e2 = -a * d;
        out.e6 = b;
        out.e10 = a * c;
      } break;

      case EulerOrder::ZYX: {
        f32_t ae = a * e;
        f32_t af = a * f;
        f32_t be = b * e;
        f32_t bf = b * f;

        out.e0 = c * e;
        out.e4 = be * d - af;
        out.e8 = ae * d + bf;

        out.e1 = c * f;
        out.e5 = bf * d + ae;
        out.e9 = af * d - be;

        out.e2 = -d;
        out.e6 = b * c;
        out.e10 = a * c;
      } break;

      default: m_error("Cannot create rotation Matrix4 from invalid Euler order %" PRIu8 ", use EulerOrder::enum", euler.order);
    }

    out.e3  = 0.0f;
    out.e7  = 0.0f;
    out.e11 = 0.0f;
    out.e12 = 0.0f;
    out.e13 = 0.0f;
    out.e14 = 0.0f;
    out.e15 = 1.0f;

    return out;
  }

  Matrix4 Matrix4::from_quaternion (Quaternion const& q) {
    f32_t x2 = q.x + q.x;
    f32_t y2 = q.y + q.y;
    f32_t z2 = q.z + q.z;

    f32_t xx = q.x * x2;
    f32_t xy = q.x * y2;
    f32_t xz = q.x * z2;

    f32_t yy = q.y * y2;
    f32_t yz = q.y * z2;
    f32_t zz = q.z * z2;

    f32_t wx = q.w * x2;
    f32_t wy = q.w * y2;
    f32_t wz = q.w * z2;

    return {
      1.0f - (yy + zz), xy + wz,          xz - wy,          0.0f,
      xy - wz,          1.0f - (xx + zz), yz + wx,          0.0f,
      xz + wy,          yz - wx,          1.0f - (xx + yy), 0.0f,
      0.0f,             0.0f,             0.0f,             1.0f
    };
  }

  Matrix4 Matrix4::from_axis_angle (Vector3f const& axis, f32_t angle) {
    f32_t c = cosf(angle);
    f32_t s = sinf(angle);

    f32_t t = 1.0f - c;

    f32_t tx = t * axis.x;
    f32_t ty = t * axis.y;

    return {
      tx * axis.x + c,          tx * axis.y + s * axis.z, tx * axis.z - s * axis.y, 0.0f,
      tx * axis.y - s * axis.z, ty * axis.y + c,          ty * axis.z + s * axis.x, 0.0f,
      tx * axis.z + s * axis.y, ty * axis.z - s * axis.x, t  * axis.z * axis.z + c, 0.0f,
      0.0f,                     0.0f,                     0.0f,                     1.0f      
    };
  }

  Matrix4 Matrix4::compose_components (Vector3f const& position, Quaternion const& rotation, Vector3f const& scale) {
    f32_t x2 = rotation.x + rotation.x;
    f32_t y2 = rotation.y + rotation.y;
    f32_t z2 = rotation.z + rotation.z;

    f32_t xx = rotation.x * x2;
    f32_t xy = rotation.x * y2;
    f32_t xz = rotation.x * z2;

    f32_t yy = rotation.y * y2;
    f32_t yz = rotation.y * z2;

    f32_t zz = rotation.z * z2;

    f32_t wx = rotation.w * x2;
    f32_t wy = rotation.w * y2;
    f32_t wz = rotation.w * z2;

    return {
      (1.0f - (yy + zz)) * scale.x,
      (xy + wz) * scale.x,
      (xz - wy) * scale.x,
      0.0f,

      (xy - wz) * scale.y,
      (1.0f - (xx + zz)) * scale.y,
      (yz + wx) * scale.y,
      0.0f,

      (xz + wy) * scale.z,
      (yz - wx) * scale.z,
      (1.0f - (xx + yy)) * scale.z,
      0.0f,

      position.x,
      position.y,
      position.z,
      1.0f
    };
  }

  Matrix4 Matrix4::compose_components_2d (Vector2f const& position, f32_t rotation_angle, Vector2f const& scale, Vector3f const& up) {
    Quaternion rotation = Quaternion::from_axis_angle(up, rotation_angle);

    f32_t x2 = rotation.x + rotation.x;
    f32_t y2 = rotation.y + rotation.y;
    f32_t z2 = rotation.z + rotation.z;

    f32_t xx = rotation.x * x2;
    f32_t xy = rotation.x * y2;
    f32_t xz = rotation.x * z2;

    f32_t yy = rotation.y * y2;
    f32_t yz = rotation.y * z2;

    f32_t zz = rotation.z * z2;

    f32_t wx = rotation.w * x2;
    f32_t wy = rotation.w * y2;
    f32_t wz = rotation.w * z2;

    return {
      (1.0f - (yy + zz)) * scale.x,
      (xy + wz) * scale.x,
      (xz - wy) * scale.x,
      0.0f,

      (xy - wz) * scale.y,
      (1.0f - (xx + zz)) * scale.y,
      (yz + wx) * scale.y,
      0.0f,

      (xz + wy),
      (yz - wx),
      (1.0f - (xx + yy)),
      0.0f,

      position.x,
      position.y,
      0.0f,
      1.0f
    };
  }

  Matrix4 Matrix4::compose_2d (Transform2D const& t, Vector3f const& up) {
    return compose_components_2d(t.position, t.rotation, t.scale, up);
  }

  Quaternion Matrix4::extract_raw_quaternion () const {
    f32_t trace = e0 + e5 + e10;
    f32_t s;

    if (trace > 0.0f) {
      s = 0.5f / sqrtf(trace + 1.0f);

      return {
        (e6 - e9) * s,
        (e8 - e2) * s,
        (e1 - e4) * s,
        0.25f / s
      };
    } else if (e0 > e5 && e0 > e10) {
      s = 2.0f * sqrtf(1.0f + e0 - e5 - e10);

      return {
        0.25f * s,
        (e4 + e1) / s,
        (e8 + e2) / s,
        (e6 - e9) / s
      };
    } else if (e5 > e10) {
      s = 2.0f * sqrtf(1.0f + e5 - e0 - e10);

      return {
        (e4 + e1) / s,
        0.25f * s,
        (e9 + e6) / s,
        (e8 - e2) / s
      };
    } else {
      s = 2.0f * sqrtf(1.0f + e10 - e0 - e5);

      return {
        (e8 + e2) / s,
        (e9 + e6) / s,
        0.25f * s,
        (e1 - e4) / s
      };
    }
  }

  Quaternion Matrix4::get_rotation_quaternion () const {
    f32_t lx = extract_column(0).length();

    f32_t ilx = 1.0f / (determinant() < 0.0f? -lx : lx);
    f32_t ily = 1.0f / extract_column(1).length();
    f32_t ilz = 1.0f / extract_column(2).length();

    Matrix4 intermediate = *this;

    intermediate.e0 = intermediate.e0 * ilx;
    intermediate.e1 = intermediate.e1 * ilx;
    intermediate.e2 = intermediate.e2 * ilx;

    intermediate.e4 = intermediate.e4 * ily;
    intermediate.e5 = intermediate.e5 * ily;
    intermediate.e6 = intermediate.e6 * ily;

    intermediate.e8  = intermediate.e8 * ilz;
    intermediate.e9  = intermediate.e9 * ilz;
    intermediate.e10 = intermediate.e10 * ilz;

    return intermediate.extract_raw_quaternion();
  }

  Euler Matrix4::get_rotation_euler (u8_t order) const {
    Euler e = { order };

    switch (order) {
      case EulerOrder::XYZ: {
        e.angles.y = asinf(num::clamp(e8, -1.0f, 1.0f));

        if (num::abs(e8) < 0.99999f) {
          e.angles.x = atan2f(-e9, e10);
          e.angles.z = atan2f(-e4, e0);
        } else {
          e.angles.x = atan2f(e6, e5);
          e.angles.z = 0;
        }
      } break;
      
      case EulerOrder::YXZ: {
        e.angles.x = asinf(-num::clamp(e9, -1.0f, 1.0f));

        if (num::abs(e9) < 0.99999f) {
          e.angles.y = atan2f(e8, e10);
          e.angles.z = atan2f(e1, e5);
        } else {
          e.angles.y = atan2f(-e2, e0);
          e.angles.z = 0.0f;
        }
      } break;
      
      case EulerOrder::ZXY: {
        e.angles.x = asinf(num::clamp(e6, -1.0f, 1.0f));

        if (num::abs(e6) < 0.99999f) {
          e.angles.y = atan2f(-e2, e10);
          e.angles.z = atan2f(-e4, e5);
        } else {
          e.angles.y = 0.0f;
          e.angles.z = atan2f(e1, e0);
        }
      } break;
      
      case EulerOrder::ZYX: {
        e.angles.y = asinf(-num::clamp(e2, -1.0f, 1.0f));

        if (num::abs(e2) < 0.99999f) {
          e.angles.x = atan2f(e6, e10);
          e.angles.z = atan2f(e1, e0);
        } else {
          e.angles.x = 0.0f;
          e.angles.z = atan2f(-e4, e5);
        }
      } break;
      
      case EulerOrder::YZX: {
        e.angles.z = asinf(num::clamp(e1, -1.0f, 1.0f));

        if (num::abs(e1) < 0.99999f) {
          e.angles.x = atan2f(-e9, e5);
          e.angles.y = atan2f(-e2, e0);
        } else {
          e.angles.x = 0.0f;
          e.angles.y = atan2f(e8, e10);
        }
      } break;
      
      case EulerOrder::XZY: {
        e.angles.z = asinf(-num::clamp(e4, -1.0f, 1.0f));

        if ( num::abs(e4) < 0.99999f) {
          e.angles.x = atan2f(e6, e5);
          e.angles.y = atan2f(e8, e0);
        } else {
          e.angles.x = atan2f(-e9, e10);
          e.angles.y = 0.0f;
        }
      } break;

      default: m_error("Cannot create Euler from Matrix4 with invalid order %" PRIu8, order);
    }

    return e;
  }

  Transform3D Matrix4::decompose () const {
    f32_t lx = extract_column(0).length();
    f32_t ly = extract_column(1).length();
    f32_t lz = extract_column(2).length();
    
    if (determinant() < 0.0f) lx = -lx;

    f32_t ilx = 1.0f / lx;
    f32_t ily = 1.0f / ly;
    f32_t ilz = 1.0f / lz;

    Matrix4 intermediate = *this;

    intermediate.e0 = intermediate.e0 * ilx;
    intermediate.e1 = intermediate.e1 * ilx;
    intermediate.e2 = intermediate.e2 * ilx;

    intermediate.e4 = intermediate.e4 * ily;
    intermediate.e5 = intermediate.e5 * ily;
    intermediate.e6 = intermediate.e6 * ily;

    intermediate.e8  = intermediate.e8 * ilz;
    intermediate.e9  = intermediate.e9 * ilz;
    intermediate.e10 = intermediate.e10 * ilz;

    return {
      get_position(),
      intermediate.extract_raw_quaternion(),
      { lx, ly, lz }
    };
  }

  Matrix4 Matrix4::inverse () const {
    f32_t t11 = e9  * e14 * e7 - e13 * e10 * e7 + e13 * e6 * e11 - e5 * e14 * e11 - e9 * e6 * e15 + e5 * e10 * e15;
    f32_t t12 = e12 * e10 * e7 - e8  * e14 * e7 - e12 * e6 * e11 + e4 * e14 * e11 + e8 * e6 * e15 - e4 * e10 * e15;
    f32_t t13 = e8  * e13 * e7 - e12 * e9  * e7 + e12 * e5 * e11 - e4 * e13 * e11 - e8 * e5 * e15 + e4 * e9  * e15;
    f32_t t14 = e12 * e9  * e6 - e8  * e13 * e6 - e12 * e5 * e10 + e4 * e13 * e10 + e8 * e5 * e14 - e4 * e9  * e14;

    f32_t det = e0 * t11 + e1 * t12 + e2 * t13 + e3 * t14;

    m_assert(det != 0.0f, "Couldn't get inverse of Matrix4, determinant of source matrix is 0");
    
    f32_t det_inv = 1.0f / det;

    return {
      t11 * det_inv,
      (e13 * e10 * e3 - e9  * e14 * e3 - e13 * e2 * e11 + e1 * e14 * e11 + e9 * e2 * e15 - e1 * e10 * e15) * det_inv,
      (e5  * e14 * e3 - e13 * e6  * e3 + e13 * e2 * e7  - e1 * e14 * e7  - e5 * e2 * e15 + e1 * e6  * e15) * det_inv,
      (e9  * e6  * e3 - e5  * e10 * e3 - e9  * e2 * e7  + e1 * e10 * e7  + e5 * e2 * e11 - e1 * e6  * e11) * det_inv,

      t12 * det_inv,
      (e8  * e14 * e3 - e12 * e10 * e3 + e12 * e2 * e11 - e0 * e14 * e11 - e8 * e2 * e15 + e0 * e10 * e15) * det_inv,
      (e12 * e6  * e3 - e4  * e14 * e3 - e12 * e2 * e7  + e0 * e14 * e7  + e4 * e2 * e15 - e0 * e6  * e15) * det_inv,
      (e4  * e10 * e3 - e8  * e6  * e3 + e8  * e2 * e7  - e0 * e10 * e7  - e4 * e2 * e11 + e0 * e6  * e11) * det_inv,

      t13 * det_inv,
      (e12 * e9  * e3 - e8  * e13 * e3 - e12 * e1 * e11 + e0 * e13 * e11 + e8 * e1 * e15 - e0 * e9 * e15) * det_inv,
      (e4  * e13 * e3 - e12 * e5  * e3 + e12 * e1 * e7  - e0 * e13 * e7  - e4 * e1 * e15 + e0 * e5 * e15) * det_inv,
      (e8  * e5  * e3 - e4  * e9  * e3 - e8  * e1 * e7  + e0 * e9  * e7  + e4 * e1 * e11 - e0 * e5 * e11) * det_inv,

      t14 * det_inv,
      (e8  * e13 * e2 - e12 * e9  * e2 + e12 * e1 * e10 - e0 * e13 * e10 - e8 * e1 * e14 + e0 * e9 * e14) * det_inv,
      (e12 * e5  * e2 - e4  * e13 * e2 - e12 * e1 * e6  + e0 * e13 * e6  + e4 * e1 * e14 - e0 * e5 * e14) * det_inv,
      (e4  * e9  * e2 - e8  * e5  * e2 + e8  * e1 * e6  - e0 * e9  * e6  - e4 * e1 * e10 + e0 * e5 * e10) * det_inv
    };
  }

  Matrix4& Matrix4::multiply_in_place (Matrix4 const& l, Matrix4 const& r) {
    f32_t l0  = l.e0,  l1  = l.e1,  l2  = l.e2,  l3  = l.e3;
    f32_t l4  = l.e4,  l5  = l.e5,  l6  = l.e6,  l7  = l.e7;
    f32_t l8  = l.e8,  l9  = l.e9,  l10 = l.e10, l11 = l.e11;
    f32_t l12 = l.e12, l13 = l.e13, l14 = l.e14, l15 = l.e15;
    f32_t r0  = r.e0,  r1  = r.e1,  r2  = r.e2,  r3  = r.e3;
    f32_t r4  = r.e4,  r5  = r.e5,  r6  = r.e6,  r7  = r.e7;
    f32_t r8  = r.e8,  r9  = r.e9,  r10 = r.e10, r11 = r.e11;
    f32_t r12 = r.e12, r13 = r.e13, r14 = r.e14, r15 = r.e15;

    e0  = l0 * r0 + l4 * r1 + l8  * r2 + l12 * r3;
    e1  = l1 * r0 + l5 * r1 + l9  * r2 + l13 * r3;
    e2  = l2 * r0 + l6 * r1 + l10 * r2 + l14 * r3;
    e3  = l3 * r0 + l7 * r1 + l11 * r2 + l15 * r3;

    e4  = l0 * r4 + l4 * r5 + l8  * r6 + l12 * r7;
    e5  = l1 * r4 + l5 * r5 + l9  * r6 + l13 * r7;
    e6  = l2 * r4 + l6 * r5 + l10 * r6 + l14 * r7;
    e7  = l3 * r4 + l7 * r5 + l11 * r6 + l15 * r7;

    e8  = l0 * r8 + l4 * r9 + l8  * r10 + l12 * r11;
    e9  = l1 * r8 + l5 * r9 + l9  * r10 + l13 * r11;
    e10 = l2 * r8 + l6 * r9 + l10 * r10 + l14 * r11;
    e11 = l3 * r8 + l7 * r9 + l11 * r10 + l15 * r11;

    e12 = l0 * r12 + l4 * r13 + l8  * r14 + l12 * r15;
    e13 = l1 * r12 + l5 * r13 + l9  * r14 + l13 * r15;
    e14 = l2 * r12 + l6 * r13 + l10 * r14 + l14 * r15;
    e15 = l3 * r12 + l7 * r13 + l11 * r14 + l15 * r15;

    return *this;
  }

  Matrix4 Matrix4::multiply (Matrix4 const& r) const {
    return {
      e0 * r.e0 + e4 * r.e1 + e8  * r.e2 + e12 * r.e3,
      e1 * r.e0 + e5 * r.e1 + e9  * r.e2 + e13 * r.e3,
      e2 * r.e0 + e6 * r.e1 + e10 * r.e2 + e14 * r.e3,
      e3 * r.e0 + e7 * r.e1 + e11 * r.e2 + e15 * r.e3,

      e0 * r.e4 + e4 * r.e5 + e8  * r.e6 + e12 * r.e7,
      e1 * r.e4 + e5 * r.e5 + e9  * r.e6 + e13 * r.e7,
      e2 * r.e4 + e6 * r.e5 + e10 * r.e6 + e14 * r.e7,
      e3 * r.e4 + e7 * r.e5 + e11 * r.e6 + e15 * r.e7,

      e0 * r.e8 + e4 * r.e9 + e8  * r.e10 + e12 * r.e11,
      e1 * r.e8 + e5 * r.e9 + e9  * r.e10 + e13 * r.e11,
      e2 * r.e8 + e6 * r.e9 + e10 * r.e10 + e14 * r.e11,
      e3 * r.e8 + e7 * r.e9 + e11 * r.e10 + e15 * r.e11,

      e0 * r.e12 + e4 * r.e13 + e8  * r.e14 + e12 * r.e15,
      e1 * r.e12 + e5 * r.e13 + e9  * r.e14 + e13 * r.e15,
      e2 * r.e12 + e6 * r.e13 + e10 * r.e14 + e14 * r.e15,
      e3 * r.e12 + e7 * r.e13 + e11 * r.e14 + e15 * r.e15
    };
  }

  Matrix4& Matrix4::multiply_scl_in_place (Matrix4 const& l, f32_t s) {
    e0  = l.e0  * s;
    e1  = l.e1  * s;
    e2  = l.e2  * s;
    e3  = l.e3  * s;

    e4  = l.e4  * s;
    e5  = l.e5  * s;
    e6  = l.e6  * s;
    e7  = l.e7  * s;

    e8  = l.e8  * s;
    e9  = l.e9  * s;
    e10 = l.e10 * s;
    e11 = l.e11 * s;
    
    e12 = l.e12 * s;
    e13 = l.e13 * s;
    e14 = l.e14 * s;
    e15 = l.e15 * s;

    return *this;
  }

  Matrix4 Matrix4::multiply_scl (f32_t s) const {
    return {
      e0  * s, e1  * s, e2  * s, e3  * s,
      e4  * s, e5  * s, e6  * s, e7  * s,
      e8  * s, e9  * s, e10 * s, e11 * s,
      e12 * s, e13 * s, e14 * s, e15 * s
    };
  }

  f32_t Matrix4::determinant () const {
    return e3  * ( (e12 * e9 * e6)  - (e8 * e13 * e6)  - (e12 * e5 * e10) + (e4 * e13 * e10) + (e8  * e5  * e14) - (e4  * e9  * e14))
         + e7  * ( (e0 * e9  * e14) - (e0 * e13 * e10) + (e12 * e1 * e10) - (e8 * e1  * e14) + (e8  * e13 * e2)  - (e12 * e9  * e2))
         + e11 * ( (e0 * e13 * e6)  - (e0 * e5  * e14) - (e12 * e1 * e6)  + (e4 * e1  * e14) + (e12 * e5  * e2)  - (e4  * e13 * e2))
         + e15 * (-(e8 * e5  * e2)  - (e0 * e9  * e6)  + (e0  * e5 * e10) + (e8 * e1  * e6)  - (e4  * e1  * e10) + (e4  * e9  * e2));
  }
}