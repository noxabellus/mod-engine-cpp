#include "../../include/math/lib.hh"


namespace mod {
  Quaternion Quaternion::from_euler (Euler const& e) {
    f32_t hx = e.angles.x / 2.0f;
    f32_t hy = e.angles.y / 2.0f;
    f32_t hz = e.angles.z / 2.0f;

    f32_t cx = cosf(hx);
    f32_t cy = cosf(hy);
    f32_t cz = cosf(hz);

    f32_t sx = sinf(hx);
    f32_t sy = sinf(hy);
    f32_t sz = sinf(hz);

    switch (e.order) {
      case EulerOrder::XYZ: return {
        sx * cy * cz + cx * sy * sz,
        cx * sy * cz - sx * cy * sz,
        cx * cy * sz + sx * sy * cz,
        cx * cy * cz - sx * sy * sz
      };

      case EulerOrder::XZY: return {
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz - sx * cy * sz,
        cx * cy * sz + sx * sy * cz,
        cx * cy * cz + sx * sy * sz
      };

      case EulerOrder::YXZ: return {
        sx * cy * cz + cx * sy * sz,
        cx * sy * cz - sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
      };

      case EulerOrder::YZX: return {
        sx * cy * cz + cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz - sx * sy * sz
      };

      case EulerOrder::ZXY: return {
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz + sx * sy * cz,
        cx * cy * cz - sx * sy * sz
      };

      case EulerOrder::ZYX: return {
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
      };
      
      default: m_error("Cannot create Quaternion from invalid Euler order %" PRIu8 ", use EulerOrder::enum", e.order);
    }
  }

  Quaternion Quaternion::from_axis_angle (Vector3f const& axis, f32_t angle) {
    f32_t h = angle / 2.0f;
    f32_t s = sinf(h);

    return {
      axis.x * s,
      axis.y * s,
      axis.z * s,
      cosf(h)
    };
  }

  Quaternion Quaternion::from_unit_vectors (Vector3f const& from, Vector3f const& to) {
    f32_t w = from.dot(to) + 1.0f;

    Vector3f v;

    if (w < FLT_EPSILON) {
      w = 0.0f;

      if (num::abs(from.x) > num::abs(from.z)) {
        v = { -from.y, from.x, 0.0f };
      } else {
        v = { 0.0f, -from.z, from.y };
      }
    } else {
      v = from.cross(to);
    }

    return { v, w };
  }


  Quaternion Quaternion::normalize () const {
    f32_t l = length();

    if (l == 0.0f) {
      return { 0.0f, 0.0f, 0.0f, 1.0f };
    } else {
      return {
        x / l,
        y / l,
        z / l,
        w / l
      };
    }
  }

  Quaternion Quaternion::conjugate () const {
    return {
      x * -1.0f,
      y * -1.0f,
      z * -1.0f,
      w
    };
  }

  Quaternion Quaternion::inverse () const {
    return conjugate().normalize();
  }

  Quaternion Quaternion::multiply (Quaternion const& r) const {
    return {
      x * r.w + w * r.x + y * r.z - z * r.y,
      y * r.w + w * r.y + z * r.x - x * r.z,
      z * r.w + w * r.z + x * r.y - y * r.x,
      w * r.w - x * r.x - y * r.y - z * r.z
    };
  }


  Quaternion Quaternion::lerp (f32_t alpha, Quaternion const& finish) const {
    f32_t alpha_inv = 1.0f - alpha;

    Quaternion intermediate;

    if (dot(finish) < 0.0f) {
      intermediate = {
        alpha_inv * x + alpha * -finish.x,
        alpha_inv * y + alpha * -finish.y,
        alpha_inv * z + alpha * -finish.z,
        alpha_inv * w + alpha * -finish.w
      };
    } else {
      intermediate = {
        alpha_inv * x + alpha * finish.x,
        alpha_inv * y + alpha * finish.y,
        alpha_inv * z + alpha * finish.z,
        alpha_inv * w + alpha * finish.w
      };
    }

    return intermediate.normalize();
  }

  Quaternion Quaternion::slerp (f32_t alpha, Quaternion const& finish) const {
    if (alpha == 0.0f) return *this;
    else if (alpha == 1.0f) return finish;

    Quaternion intermediate;

    
    f32_t cos_theta = dot(finish);

    if (cos_theta < 0.0f) {
      intermediate = finish.negate();

      cos_theta = -cos_theta;
    } else {
      intermediate = finish;
    }

    if (cos_theta >= 1.0f) return *this;
    

    f32_t sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    if (num::abs(sin_theta) < 0.001) {
      intermediate.x = 0.5f * (x + intermediate.x);
      intermediate.y = 0.5f * (y + intermediate.y);
      intermediate.z = 0.5f * (z + intermediate.z);
      intermediate.w = 0.5f * (w + intermediate.w);

      return intermediate;
    }


    f32_t half_theta = atan2f(sin_theta, cos_theta);

    f32_t a = sinf((1.0f - alpha) * half_theta) / sin_theta;
    f32_t b = sinf(alpha * half_theta) / sin_theta;

    intermediate.x = x * a + intermediate.x * b;
    intermediate.y = y * a + intermediate.y * b;
    intermediate.z = z * a + intermediate.z * b;
    intermediate.w = w * a + intermediate.w * b;
    
    return intermediate;
  }
}