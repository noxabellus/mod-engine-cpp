#include "../../include/math/lib.hh"


namespace mod::Contains {
  bool aabb2 (AABB2 const& l, AABB2 const& r) {
    return l.min.x <= r.min.x && r.max.x <= l.max.x
        && l.min.y <= r.min.y && r.max.y <= l.max.y; 
  }

  bool aabb2_line2 (AABB2 const& aabb, Line2 const& line) {
    return Intersects::aabb2_vector(aabb, line.a)
        && Intersects::aabb2_vector(aabb, line.b);
  }

  bool aabb2_circle (AABB2 const& aabb, Circle const& circle) {
    for (size_t i = 0; i < 2; i ++) {
      if (circle.position.elements[i] - circle.radius < aabb.min.elements[i]
      ||  circle.position.elements[i] + circle.radius > aabb.max.elements[i]) {
        return false;
      }
    }

    return true;
  }

  bool aabb2_polygon (AABB2 const& aabb, Vector2f const& vertices, size_t vertex_count) {
    for (size_t i = 0; i < vertex_count; i ++) {
      if (!Intersects::aabb2_vector(aabb, vertices[i])) return false;
    }

    return true;
  }


  bool aabb3 (AABB3 const& l, AABB3 const& r) {
    return l.min.x <= r.min.x && r.max.x <= l.max.x
        && l.min.y <= r.min.y && r.max.y <= l.max.y
        && l.min.z <= r.min.z && r.max.z <= l.max.z; 
  }

  bool aabb3_line3 (AABB3 const& aabb, Line3 const& line) {
    return Intersects::aabb3_vector(aabb, line.a)
        && Intersects::aabb3_vector(aabb, line.b);
  }

  bool aabb3_sphere (AABB3 const& aabb, Sphere const& sphere) {
    for (size_t i = 0; i < 2; i ++) {
      if (sphere.position.elements[i] - sphere.radius < aabb.min.elements[i]
      ||  sphere.position.elements[i] + sphere.radius > aabb.max.elements[i]) {
        return false;
      }
    }

    return true;
  }

  bool aabb3_triangle (AABB3 const& aabb, Triangle const& triangle) {
    return Intersects::aabb3_vector(aabb, triangle.a)
        && Intersects::aabb3_vector(aabb, triangle.b)
        && Intersects::aabb3_vector(aabb, triangle.c);
  }


  bool circle (Circle const& l, Circle const& r) {
    return l.radius >= r.radius
        && powf(l.radius - r.radius, 2.0f) >= l.position.distance_sq(r.position);
  }

  bool circle_line2 (Circle const& circle, Line2 const& line) {
    return Intersects::circle_vector(circle, line.a)
        && Intersects::circle_vector(circle, line.b);
  }

  bool circle_aabb2 (Circle const& circle, AABB2 const& aabb) {
    f32_t t_d_sq = 0.0f;

    for (size_t i = 0; i < 2; i ++) {
      f32_t a = num::abs(circle.position.elements[i] - aabb.min.elements[i]);
      f32_t b = num::abs(circle.position.elements[i] - aabb.max.elements[i]);
      f32_t d = num::max(a, b);

      t_d_sq += d * d;
    }

    return t_d_sq <= circle.radius * circle.radius;
  }

  bool circle_polygon (Circle const& circle, Vector2f const* vertices, size_t vertex_count) {
    f32_t r_sq = circle.radius * circle.radius;

    for (size_t i = 0; i < vertex_count; i ++) {
      if (circle.position.distance_sq(vertices[i]) > r_sq) return false;
    }

    return true;
  }


  bool sphere (Sphere const& l, Sphere const& r) {
    return l.radius >= r.radius
        && powf(l.radius - r.radius, 2.0f) >= l.position.distance_sq(r.position);
  }

  bool sphere_aabb3 (Sphere const& sphere, AABB3 const& aabb) {
    f32_t t_d_sq = 0.0f;

    for (size_t i = 0; i < 3; i ++) {
      f32_t a = num::abs(sphere.position.elements[i] - aabb.min.elements[i]);
      f32_t b = num::abs(sphere.position.elements[i] - aabb.max.elements[i]);
      f32_t d = num::max(a, b);

      t_d_sq += d * d;
    }

    return t_d_sq <= sphere.radius * sphere.radius;
  }

  bool sphere_line3 (Sphere const& sphere, Line3 const& line) {
    return Intersects::sphere_vector(sphere, line.a)
        && Intersects::sphere_vector(sphere, line.b);
  }

  bool sphere_triangle (Sphere const& sphere, Triangle const& triangle) {
    return Intersects::sphere_vector(sphere, triangle.a)
        && Intersects::sphere_vector(sphere, triangle.b)
        && Intersects::sphere_vector(sphere, triangle.c);
  }


  bool polygon (Vector2f const* l_vertices, size_t l_vertex_count, Vector2f const* r_vertices, size_t r_vertex_count) {
    if (!Intersects::polygon_vector(l_vertices, l_vertex_count, r_vertices[0])) return false;

    for (size_t i = 0; i < l_vertex_count; i ++) {
      for (size_t j = 0; j < r_vertex_count; j ++) {
        if (Intersects::line2(
          { l_vertices[i], l_vertices[(i + 1) % l_vertex_count] },
          { r_vertices[j], r_vertices[(j + 1) % r_vertex_count] }
        ).a) return false;
      }
    }

    return true;
  }

  bool polygon_line2 (Vector2f const* vertices, size_t vertex_count, Line2 const& line) {
    if (!Intersects::polygon_vector(vertices, vertex_count, line.a)) return false;

    for (size_t i = 0; i < vertex_count; i ++) {
      if (Intersects::line2(
        line,
        { vertices[i], vertices[(i + 1) % vertex_count] }
      ).a) return false;
    }

    return true;
  }

  bool polygon_circle (Vector2f const* vertices, size_t vertex_count, Circle const& circle) {
    if (!Intersects::polygon_vector(vertices, vertex_count, circle.position)) return false;

    f32_t r_sq = circle.radius * circle.radius;

    for (size_t i = 0; i < vertex_count; i ++) {
      if (r_sq > circle.position.distance_sq(vertices[i])) return false;
    }

    return true;
  }

  bool polygon_aabb2 (Vector2f const* vertices, size_t vertex_count, AABB2 const& aabb) {
    quad_t<Vector2f> aabb_poly = aabb.to_polygon();

    return polygon(vertices, vertex_count, &aabb_poly.a, 4);
  }
}