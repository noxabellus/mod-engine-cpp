#include "../../include/math/lib.hh"



namespace mod {
  Vector3f Triangle::normal () const {
    return (c - a).cross(b - a).normalize();
  }

  Vector3f Triangle::get_barycentric_coordinate (Vector3f const& v) const {
    Vector3f c_delta = c - a;
    Vector3f b_delta = b - a;
    Vector3f v_delta = v - a;

    f32_t dot00 = c_delta.dot(c_delta);
    f32_t dot01 = c_delta.dot(b_delta);
    f32_t dot02 = c_delta.dot(v_delta);

    f32_t dot11 = b_delta.dot(b_delta);
    f32_t dot12 = b_delta.dot(v_delta);

    f32_t d = dot00 * dot11 - dot01 * dot01;

    if (d != 0.0f) {
      f32_t i = 1.0f / d;

      f32_t u = (dot11 * dot02 - dot01 * dot12) * i;
      f32_t v = (dot00 * dot12 - dot01 * dot02) * i;

      return {
        1.0f - u - v,
        v,
        u
      };
    } else {
      return {
        -2.0f,
        -1.0f,
        -1.0f
      };
    }
  }

  Vector2f Triangle::get_uv (Vector2f const& uv0, Vector2f const& uv1, Vector2f const& uv2, Vector3f const& vector) const {
    Vector3f bary_coord = get_barycentric_coordinate(vector);

    return (uv0 * bary_coord.x) + (uv1 * bary_coord.y) + (uv2 * bary_coord.z);
  }

  

  Vector3f Triangle::clamp (Vector3f const& vector) const {
    Vector3f ab = b - a;
    Vector3f ac = c - a;
    Vector3f ap = vector - a;


    f32_t d1 = ab.dot(ap);
    f32_t d2 = ac.dot(ap);

    if (d1 <= 0.0f && d2 <= 0.0f) return a;


    Vector3f bp = vector - b;
    f32_t d3 = ab.dot(bp);
    f32_t d4 = ac.dot(bp);
    
    if (d3 >= 0.0f && d4 <= d3) return b;


    f32_t vc = d1 * d4 - d3 * d2;

    if (vc <= 0.0f && d1 >= 0 && d3 <= 0.0f) {
      return a + (ab * (d1 / (d1 - d3)));
    }


    Vector3f cp = vector - c;
    f32_t d5 = ab.dot(cp);
    f32_t d6 = ac.dot(cp);

    if (d6 >= 0.0f && d5 <= d6) return c;


    f32_t vb = d5 * d2 - d1 * d6;
    
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
      return a + (ac * (d2 / (d2 - d6)));
    }


    f32_t va = d3 * d6 - d5 * d4;
    
    if (va <= 0.0f && d4 - d2 >= 0.0f && d5 - d6 >= 0.0f) {
      return b + ((c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6))));
    }


    f32_t denom = 1.0f / (va + vb + vc);

    f32_t v = vb * denom;
    f32_t w = vc * denom;

    Vector3f abv = ab * v;
    Vector3f acw = ac * w;

    return a + abv + acw;
  }
}