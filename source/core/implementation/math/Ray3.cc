#include "../../include/math/lib.hh"



namespace mod {
  Ray3 Ray3::from_line (Line3 const& line) {
    return {
      line.a,
      line.direction()
    };
  }

  f32_t Ray3::offset_at_vector (Vector3f const& v, bool clamp_offset) const {
    Vector3f p_delta = v - origin;

    f32_t o = direction.dot(p_delta) / p_delta.dot(p_delta);

    return clamp_offset? num::max(0.0f, o) : o;
  }

  Line3 Ray3::shortest_line_to_line (Line3 const& line) const {
    Vector3f seg_center = (line.a + line.b) * 0.5f;
    Vector3f seg_dir = (line.b - line.a).normalize();
    Vector3f diff = origin - seg_center;

    f32_t seg_ext = line.length() * 0.5f;
    f32_t a01 = -direction.dot(seg_dir);
    f32_t b0 = diff.dot(direction);
    f32_t b1 = -diff.dot(seg_dir);
    // f32_t c = diff.length_sq();
    f32_t det = num::abs(1.0f - a01 * a01);
    f32_t s0, s1, det_ext;

    if (det > 0.0f) {
      s0 = a01 * b1 - b0;
      s1 = a01 * b0 - b1;
      det_ext = seg_ext * det;

      if (s0 >= 0.0f) {
        if (s1 >= -det_ext) {
          if (s1 <= det_ext) {
            f32_t invDet = 1.0f / det;
            s0 *= invDet;
            s1 *= invDet;
          } else {
            s1 = seg_ext;
            s0 = num::max(0.0f, -(a01 * s1 + b0));
          }
        } else {
          s1 = -seg_ext;
          s0 = num::max(0.0f, -(a01 * s1 + b0));
        }
      } else {
        if (s1 <= -det_ext) {
          s0 = num::max(0.0f, -(-a01 * seg_ext + b0));
          s1 = (s0 > 0.0f) ? -seg_ext : num::min(num::max(-seg_ext, -b1), seg_ext);
        } else if (s1 <= det_ext) {
          s0 = 0.0f;
          s1 = num::min(num::max(-seg_ext, -b1), seg_ext);
        } else {
          s0 = num::max(0.0f, -(a01 * seg_ext + b0));
          s1 = (s0 > 0.0f) ? seg_ext : num::min(num::max(-seg_ext, -b1), seg_ext);
        }
      }
    } else {
      s1 = (a01 > 0.0f) ? -seg_ext : seg_ext;
      s0 = num::max(0.0f, -(a01 * s1 + b0));
    }
    
    return {
      (direction * s0) + origin,
      (seg_dir * s1) + seg_center
    };
  }

  f32_t Ray3::distance (Plane const& plane) const {
    f32_t d = plane.normal.dot(direction);

    if (d == 0.0f) {
      if (plane.distance(origin) == 0.0f) {
        return 0.0f;
      }
    } else {
      f32_t t = -(origin.dot(plane.normal) + plane.constant) / d;

      if (t >= 0.0f) {
        return t;
      }
    }

    return INFINITY;
  }
}