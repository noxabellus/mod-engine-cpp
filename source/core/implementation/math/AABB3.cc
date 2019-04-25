#include "../../include/math/lib.hh"



namespace mod {
  AABB3 AABB3::from_sphere (Sphere const& sphere) {
    return from_center_and_size(sphere.position, sphere.radius);
  }

  AABB3 AABB3::from_vector_list (Vector3f const* vectors, size_t vector_count) {
    AABB3 out = from_center_and_size(vectors[0], { 0 });
    
    for (size_t i = 1; i < vector_count; i ++) {
      out.expand_to_vector(vectors[i]);
    }

    return out;
  }


  AABB3 AABB3::apply_matrix (Matrix3 const& m) const {
    Vector3f points [8] = {
      min.apply_matrix(m),
      (Vector3f { min.x, min.y, max.z }).apply_matrix(m),
      (Vector3f { min.x, max.y, min.z }).apply_matrix(m),
      (Vector3f { min.x, max.y, max.z }).apply_matrix(m),
      (Vector3f { max.x, min.y, min.z }).apply_matrix(m),
      (Vector3f { max.x, min.y, max.z }).apply_matrix(m),
      (Vector3f { max.x, max.y, min.z }).apply_matrix(m),
      max.apply_matrix(m)
    };

    return from_vector_list(points, 8);
  }

  AABB3 AABB3::apply_matrix (Matrix4 const& m) const {
    Vector3f points [8] = {
      min.apply_matrix(m),
      (Vector3f { min.x, min.y, max.z }).apply_matrix(m),
      (Vector3f { min.x, max.y, min.z }).apply_matrix(m),
      (Vector3f { min.x, max.y, max.z }).apply_matrix(m),
      (Vector3f { max.x, min.y, min.z }).apply_matrix(m),
      (Vector3f { max.x, min.y, max.z }).apply_matrix(m),
      (Vector3f { max.x, max.y, min.z }).apply_matrix(m),
      max.apply_matrix(m)
    };

    return from_vector_list(points, 8);
  }
}