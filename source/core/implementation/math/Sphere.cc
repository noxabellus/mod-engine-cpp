#include "../../include/math/lib.hh"



namespace mod {
  Sphere Sphere::from_vector_list (Vector3f const* vectors, size_t vector_count) {
    Sphere out;

    for (size_t i = 0; i < vector_count; i ++) {
      out.expand_to_vector(vectors[i]);
    }

    return out;
  }

  Sphere Sphere::from_aabb (AABB3 const& aabb) {
    return {
      aabb.center(),
      aabb.size().length() * 0.5f
    };
  }

  Sphere& Sphere::expand_to_vector (Vector3f const& v) {
    f32_t dist_sq = position.distance_sq(v);

    if (dist_sq > radius * radius) {
      radius = sqrtf(dist_sq);
    }

    return *this;
  }

  Vector3f Sphere::clamp (Vector3f const& v) const {
    Vector3f delta = position.sub(v);

    f32_t dist_sq = delta.length_sq();

    if (dist_sq > radius * radius) {
      Vector3f direction = delta / sqrtf(dist_sq);

      return position + (direction * radius);
    } else {
      return v;
    }
  }
}