#include "../../include/math/lib.hh"



namespace mod {
  Plane Plane::from_coplanar_vectors (Vector3f const& a, Vector3f const& b, Vector3f const& c) {
    return from_normal_with_coplanar_vector((c - b).cross(a - b).normalize(), a);
  }

  Plane Plane::from_triangle (Triangle const& triangle) {
    return from_coplanar_vectors(triangle.a, triangle.b, triangle.c);
  }

  f32_t Plane::distance (Sphere const& sphere) const {
    return distance(sphere.position) - sphere.radius;
  }

  Plane Plane::apply_matrix (Matrix4 const& m) const {
    return from_normal_with_coplanar_vector(
      normal.apply_matrix(Matrix3::normal(m)).normalize(),
      coplanar_vector().apply_matrix(m)
    );
  }
}