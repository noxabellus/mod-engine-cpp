#include "../../include/math/lib.hh"



namespace mod {
  Ray2 Ray2::from_line (Line2 const& line) {
    return {
      line.a,
      line.direction()
    };
  }

  f32_t Ray2::offset_at_vector (Vector2f const& v, bool clamp_offset) const {
    Vector2f p_delta = v - origin;

    f32_t o = direction.dot(p_delta) / p_delta.dot(p_delta);

    return clamp_offset? num::max(0.0f, o) : o;
  }

  Line2 Ray2::shortest_line_to_ray (Ray2 const& other) const {
    pair_t<bool, Vector2f> result = Intersects::ray2(*this, other);

    if (result.a) return { result.b, result.b };

    Line2 lines [2] = {
      { origin, other.clamp(origin) },
      { clamp(other.origin), other.origin }
    };

    return lines[Line2::index_of_shortest(lines, 2)];
  }
}