#include "../../include/math/lib.hh"



namespace mod {
  AABB2 AABB2::from_circle (Circle const& circle) {
    return from_center_and_size(circle.position, circle.radius);
  }

  AABB2 AABB2::from_3 (AABB3 const& aabb) {
    return {
      aabb.min.x, aabb.min.y,
      aabb.max.x, aabb.max.y
    };
  }

  AABB2 AABB2::from_vector_list (Vector2f const* vectors, size_t vector_count) {
    AABB2 out;
    
    for (size_t i = 0; i < vector_count; i ++) {
      out.expand_to_vector(vectors[i]);
    }

    return out;
  }


  AABB2 AABB2::apply_matrix (Matrix3 const& m) const {
    Vector2f points [8] = {
      min.apply_matrix(m),
      (Vector2f { min.x, max.y }).apply_matrix(m),
      (Vector2f { max.x, min.y }).apply_matrix(m),
      max.apply_matrix(m)
    };

    return from_vector_list(points, 8);
  }

  AABB2 AABB2::apply_matrix (Matrix4 const& m) const {
    Vector2f points [8] = {
      min.apply_matrix(m),
      (Vector2f { min.x, max.y }).apply_matrix(m),
      (Vector2f { max.x, min.y }).apply_matrix(m),
      max.apply_matrix(m)
    };

    return from_vector_list(points, 8);
  }
}