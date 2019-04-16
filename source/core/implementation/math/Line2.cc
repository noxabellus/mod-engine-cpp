#include "../../include/math/lib.hh"


namespace mod {
  Line2 Line2::from_ray (Ray2 const& ray, f32_t length) {
    return {
      ray.origin,
      ray.origin + ray.direction * length
    };
  }

  f32_t Line2::offset_at_vector (Vector2f const& v, bool clamp_offset) const {
    Vector2f l_delta = delta();
    Vector2f p_delta = v - a;

    f32_t o = l_delta.dot(p_delta) / l_delta.dot(l_delta);

    return clamp_offset? num::clamp(o, 0.0f, 1.0f) : o;
  }

  size_t Line2::index_of_shortest (Line2 const* lines, size_t line_count) {
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

  size_t Line2::index_of_longest (Line2 const* lines, size_t line_count) {
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

  Line2 Line2::shortest_line_to_line (Line2 const& other) const {
    pair_t<bool, Vector2f> result = Intersects::line2(*this, other);

    if (result.a) return { result.b, result.b };

    Line2 lines [4] = {
      { a, other.clamp(a) },
      { b, other.clamp(b) },
      { clamp(other.a), other.a },
      { clamp(other.b), other.b }
    };

    return lines[index_of_shortest(lines, 4)];
  }

  Line2 Line2::shortest_line_to_ray (Ray2 const& ray) const {
    pair_t<bool, Vector2f> result = Intersects::line2_ray2(*this, ray);

    if (result.a) return { result.b, result.b };

    Line2 lines [4] = {
      { ray.origin, clamp(ray.origin) },
      { ray.clamp(a), a },
      { ray.clamp(b), b }
    };

    return lines[index_of_shortest(lines, 4)];
  }
}