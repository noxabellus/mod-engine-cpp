#include "../../include/math/lib.hh"


namespace mod {
  Line3 Line3::from_ray (Ray3 const& ray, f32_t length) {
    return {
      ray.origin,
      ray.origin + ray.direction * length
    };
  }

  f32_t Line3::offset_at_vector (Vector3f const& v, bool clamp_offset) const {
    Vector3f l_delta = delta();
    Vector3f p_delta = v - a;

    f32_t o = l_delta.dot(p_delta) / l_delta.dot(l_delta);

    return clamp_offset? num::clamp(o, 0.0f, 1.0f) : o;
  }

  size_t Line3::index_of_shortest (Line3 const* lines, size_t line_count) {
    f32_t min_len_sq = lines[0].length_sq();
    size_t index = 0;
    
    for (size_t i = 0; i < line_count; i ++) {
      f32_t len_sq = lines[i].length_sq();

      if (len_sq < min_len_sq) {
        min_len_sq = len_sq;
        index = i;
      }
    }

    return index;
  }
  
  size_t Line3::index_of_longest (Line3 const* lines, size_t line_count) {
    f32_t max_len_sq = lines[0].length_sq();
    size_t index = 0;
    
    for (size_t i = 0; i < line_count; i ++) {
      f32_t len_sq = lines[i].length_sq();

      if (len_sq > max_len_sq) {
        max_len_sq = len_sq;
        index = i;
      }
    }

    return index;
  }
}