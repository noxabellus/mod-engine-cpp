#include "../../include/math/lib.hh"



namespace mod {
  Circle Circle::from_vector_list (Vector2f const* vectors, size_t vector_count) {
    Circle out;

    for (size_t i = 0; i < vector_count; i ++) {
      out.expand_to_vector(vectors[i]);
    }

    return out;
  }

  Circle Circle::from_aabb (AABB2 const& aabb) {
    return {
      aabb.center(),
      aabb.size().length() * 0.5f
    };
  }

  Circle Circle::from_sphere (Sphere const& sphere) {
    return {
      sphere.position.x, sphere.position.y,
      sphere.radius
    };
  }

  Circle& Circle::expand_to_vector (Vector2f const& v) {
    f32_t dist_sq = position.distance_sq(v);

    if (dist_sq > radius * radius) {
      radius = sqrtf(dist_sq);
    }

    return *this;
  }

  Vector2f Circle::clamp (Vector2f const& v) const {
    Vector2f delta = position.sub(v);

    f32_t dist_sq = delta.length_sq();

    if (dist_sq > radius * radius) {
      Vector2f direction = delta / sqrtf(dist_sq);

      return position + (direction * radius);
    } else {
      return v;
    }
  }
}