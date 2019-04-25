#include "../../include/math/lib.hh"


namespace mod::Intersects {
  static constexpr pair_t<bool, Vector2f> v2_intersect_none = { false, Constants::Vector2f::zero };


  pair_t<bool, Vector2f> line2 (Line2 const& l, Line2 const& r) {
    Vector2f s0 = l.delta();
    Vector2f s1 = r.delta();

    f32_t d = -s1.x * s0.y + s0.x * s1.y;

    if (num::abs(d) > FLT_EPSILON) {
      Vector2f s2 = l.a - r.a;

      f32_t t0 = (-s0.y * s2.x + s0.x * s2.y) / d;

      if (t0 >= 0.0f && t0 <= 1.0f) {
        f32_t t1 = s1.cross(s2) / d;

        if (t1 >= 0.0f && t1 <= 1.0f) {
          return { true, l.a + (s0 * t1) };
        }
      }
    }

    return v2_intersect_none;
  }

  pair_t<bool, Vector2f> line2_ray2 (Line2 const& line, Ray2 const& ray) {
    Vector2f s0 = line.delta();
    Vector2f s1 = { -ray.direction.y, ray.direction.x };

    f32_t d = s0.dot(s1);

    if (num::abs(d) > FLT_EPSILON) {
      Vector2f s2 = ray.origin - line.a;

      f32_t t0 = s0.cross(s2) / d;

      if (t0 >= 0.0f) {
        f32_t t1 = s2.dot(s1) / d;

        if (t1 >= 0.0f && t1 <= 1.0f) {
          return { true, ray.origin + (ray.direction * t0) };
        }
      }
    }

    return v2_intersect_none;
  }

  pair_t<bool, Vector2f> line2_aabb2 (Line2 const& line, AABB2 const& aabb) {
    Vector2f l_delta = line.delta();

    Vector2f sign = l_delta.sign();

    Vector2f half_size = aabb.half_size();

    Vector2f signed_hs = sign * half_size;

    Vector2f center = aabb.min + half_size;
    
    Vector2f s0 = (center - signed_hs - line.a) / l_delta;
    Vector2f s1 = (center + signed_hs + line.a) / l_delta;

    if (s0.x <= s1.y && s0.y <= s1.x) {
      f32_t t0 = num::max(s0.x, s0.y);
      f32_t t1 = num::min(s1.x, s1.y);

      if (t0 <= 1.0f && t1 >= 0.0f) {
        return { true, line.a + (l_delta * num::clamp(t0, 0.0f, 1.0f)) };
      }
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> line2_aabb2_exit (Line2 const& line, AABB2 const& aabb) {
    if (!aabb2_vector(aabb, line.b)) {
      Vector2f l_delta = line.delta();

      Vector2f sign = l_delta.sign();

      Vector2f half_size = aabb.half_size();

      Vector2f signed_hs = sign * half_size;

      Vector2f center = aabb.min + half_size;
      
      Vector2f s0 = (center - signed_hs - line.a) / l_delta;
      Vector2f s1 = (center + signed_hs + line.a) / l_delta;

      if (s0.x <= s1.y && s0.y <= s1.x) {
        f32_t t0 = num::max(s0.x, s0.y);
        f32_t t1 = num::min(s1.x, s1.y);

        if (t0 <= 1.0f && t1 >= 0.0f) {
          return { true, line.a + (l_delta * num::clamp(t1, 0.0f, 1.0f)) };
        }
      }
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> line2_circle (Line2 const& line, Circle const& circle) {
    Vector2f c_delta = circle.position - line.a;

    f32_t r_sq = circle.radius * circle.radius;

    if (c_delta.length_sq() <= r_sq) {
      return { true, line.a };
    }

    Vector2f l_delta = line.delta();

    f32_t l_len = l_delta.length();
    Vector2f l_dir = l_delta / l_len;
    
    f32_t p_offset = c_delta.dot(l_dir);

    if (p_offset >= 0.0f) {
      f32_t i_len_t_sq = c_delta.dot(c_delta) - (p_offset * p_offset);

      f32_t i_len = sqrtf(r_sq - i_len_t_sq);

      if (i_len >= 0) {
        f32_t t = p_offset - i_len;

        if (t <= l_len) {
          return { true, line.a + (l_dir * t) };
        }
      }
    }

    return v2_intersect_none;
  }

  pair_t<bool, Vector2f> line2_circle_exit (Line2 const& line, Circle const& circle) {
    Line2 inverse_line = { line.b, line.a };

    Vector2f c_delta = circle.position - inverse_line.a;

    f32_t r_sq = circle.radius * circle.radius;

    if (c_delta.length_sq() > r_sq) {
      Vector2f l_delta = inverse_line.delta();

      f32_t l_len = l_delta.length();
      Vector2f l_dir = l_delta / l_len;
      
      f32_t p_offset = c_delta.dot(l_dir);

      if (p_offset >= 0.0f) {
        f32_t i_len_t_sq = c_delta.dot(c_delta) - (p_offset * p_offset);

        f32_t i_len = sqrtf(r_sq - i_len_t_sq);

        if (i_len >= 0) {
          f32_t t = p_offset - i_len;

          if (t <= l_len) {
            return { true, inverse_line.a + (l_dir * t) };
          }
        }
      }
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> line2_polygon (Line2 const& line, Vector2f const* vertices, size_t vertex_count) {
    if (polygon_vector(vertices, vertex_count, line.a)) {
      return { true, line.a };
    }

    f32_t min_dist_sq;
    pair_t<bool, Vector2f> intersect = { false, Constants::Vector2f::zero };

    for (size_t i = 0; i < vertex_count ; i ++) {
      if (auto res = line2(line, { vertices[i], vertices[(i + 1) % vertex_count] }); res.a) {
        f32_t dist_sq = line.a.distance_sq(res.b);

        if (!intersect.a || dist_sq < min_dist_sq) {
          min_dist_sq = dist_sq;
          intersect = res;
        }
      }
    }

    return intersect;
  }


  pair_t<bool, Vector2f> line2_polygon_exit (Line2 const& line, Vector2f const* vertices, size_t vertex_count) {
    f32_t max_dist_sq;
    pair_t<bool, Vector2f> intersect = { false, Constants::Vector2f::zero };

    if (!polygon_vector(vertices, vertex_count, line.b)) {
      for (size_t i = 0; i < vertex_count ; i ++) {
        if (auto res = line2(line, { vertices[i], vertices[(i + 1) % vertex_count] }); res.a) {
          f32_t dist_sq = line.a.distance_sq(res.b);

          if (!intersect.a || dist_sq > max_dist_sq) {
            max_dist_sq = dist_sq;
            intersect = res;
          }
        }
      }
    }

    return intersect;
  }



  pair_t<bool, Vector2f> ray2 (Ray2 const& l, Ray2 const& r) {
    f32_t d = -r.direction.x * l.direction.y + l.direction.x * r.direction.y;

    if (num::abs(d) > FLT_EPSILON) {
      Vector2f s = l.origin - r.origin;

      f32_t t0 = (-l.direction.y * s.x + l.direction.x * s.y) / d;

      if (t0 >= 0.0f) {
        f32_t t1 = r.direction.cross(s) / d;

        if (t1 >= 0.0f) {
          return { true, l.origin + (l.direction * t1) };
        }
      }
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> ray2_aabb2 (Ray2 const& ray, AABB2 const& aabb) {
    if (aabb2_vector(aabb, ray.origin)) {
      return { true, ray.origin };
    }

    Vector2f t_min = (aabb.min - ray.origin) / ray.direction;
    Vector2f t_max = (aabb.max - ray.origin) / ray.direction;

    Vector2f r_min = t_min.min(t_max);
    Vector2f r_max = t_min.max(t_max);

    f32_t max_of_min = num::max(r_min.x, r_min.y);
    f32_t min_of_max = num::min(r_max.x, r_max.y);

    if (max_of_min >= 0.0f && min_of_max >= max_of_min) {
      return { true, ray.origin + (ray.direction * max_of_min) };
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> ray2_aabb2_exit (Ray2 const& ray, AABB2 const& aabb) {
    Vector2f t_min = (aabb.min - ray.origin) / ray.direction;
    Vector2f t_max = (aabb.max - ray.origin) / ray.direction;

    Vector2f r_min = t_min.min(t_max);
    Vector2f r_max = t_min.max(t_max);

    f32_t max_of_min = num::max(r_min.x, r_min.y);
    f32_t min_of_max = num::min(r_max.x, r_max.y);

    if (min_of_max >= max_of_min) {
      if (aabb2_vector(aabb, ray.origin)) {
        return { true, ray.origin + (ray.direction * (max_of_min >= 0.0f? max_of_min : min_of_max)) };
      } else if (max_of_min >= 0.0f) {
        return { true, ray.origin + (ray.direction * min_of_max) };
      }
    }

    return v2_intersect_none;
  }


  pair_t<bool, Vector2f> ray2_circle (Ray2 const& ray, Circle const& circle) {
    Vector2f c_delta = circle.position - ray.origin;

    f32_t r_sq = circle.radius * circle.radius;

    if (c_delta.length_sq() <= r_sq) {
      return { true, ray.origin };
    }
    
    f32_t p_offset = c_delta.dot(ray.direction);

    if (p_offset >= 0.0f) {
      f32_t i_len_t_sq = c_delta.dot(c_delta) - (p_offset * p_offset);

      f32_t i_len = sqrtf(r_sq - i_len_t_sq);

      if (i_len >= 0) {
        return { true, ray.origin + (ray.direction * (p_offset - i_len)) };
      }
    }

    return v2_intersect_none;
  }

  pair_t<bool, Vector2f> ray2_circle_exit (Ray2 const& ray, Circle const& circle) {
    Vector2f c_delta = circle.position - ray.origin;

    f32_t r_sq = circle.radius * circle.radius;
    
    f32_t p_offset = c_delta.dot(ray.direction);

    f32_t i_len_t_sq = c_delta.dot(c_delta) - (p_offset * p_offset);

    f32_t i_len = sqrtf(r_sq - i_len_t_sq);

    if (i_len >= 0) {
      return { true, ray.origin + (ray.direction * (p_offset + i_len)) };
    }

    return v2_intersect_none;
  }

  pair_t<bool, Vector2f> ray2_polygon (Ray2 const& ray, Vector2f const* vertices, size_t vertex_count) {
    if (polygon_vector(vertices, vertex_count, ray.origin)) {
      return { true, ray.origin };
    }

    f32_t min_dist_sq;
    pair_t<bool, Vector2f> intersect = { false, Constants::Vector2f::zero };

    for (size_t i = 0; i < vertex_count ; i ++) {
      if (auto res = line2_ray2({ vertices[i], vertices[(i + 1) % vertex_count] }, ray); res.a) {
        f32_t dist_sq = ray.origin.distance_sq(res.b);

        if (!intersect.a || dist_sq < min_dist_sq) {
          min_dist_sq = dist_sq;
          intersect = res;
        }
      }
    }

    return intersect;
  }


  pair_t<bool, Vector2f> ray2_polygon_exit (Ray2 const& ray, Vector2f const* vertices, size_t vertex_count) {
    f32_t max_dist_sq;
    pair_t<bool, Vector2f> intersect = { false, Constants::Vector2f::zero };

    for (size_t i = 0; i < vertex_count ; i ++) {
      if (auto res = line2_ray2({ vertices[i], vertices[(i + 1) % vertex_count] }, ray); res.a) {
        f32_t dist_sq = ray.origin.distance_sq(res.b);

        if (!intersect.a || dist_sq > max_dist_sq) {
          max_dist_sq = dist_sq;
          intersect = res;
        }
      }
    }

    return intersect;
  }




  bool aabb2 (AABB2 const& l, AABB2 const& r) {
    return !(
         r.max.x < l.min.x || l.max.x < r.min.x
      || r.max.y < l.min.y || l.max.y < r.min.y
    );
  }


  bool aabb2_vector (AABB2 const& aabb, Vector2f const& v) {
    return !(
         aabb.min.x > v.x || v.x > aabb.max.x
      || aabb.min.y > v.y || v.y > aabb.max.y
    );
  }

  bool aabb2_circle (AABB2 const& aabb, Circle const& circle) {
    return (circle.position - aabb.clamp(circle.position)).length_sq() <= circle.radius * circle.radius;
  }

  bool aabb2_polygon (AABB2 const& aabb, Vector2f const* vertices, size_t vertex_count) {
    quad_t<Vector2f> aabb_poly = aabb.to_polygon();
    
    if (polygon_vector(vertices, vertex_count, aabb_poly.a)
    ||  polygon_vector(vertices, vertex_count, aabb_poly.b)
    ||  polygon_vector(vertices, vertex_count, aabb_poly.c)
    ||  polygon_vector(vertices, vertex_count, aabb_poly.d)) {
      return true;
    }

    for (size_t j = 0; j < vertex_count; j ++) {
      if (auto res = line2_aabb2(
        { vertices[j], vertices[(j + 1) % vertex_count] },
        aabb
      ); res.a) return true;
    }

    return false;
  }



  bool circle (Circle const& l, Circle const& r) {
    return l.position.distance_sq(r.position) <= powf(l.radius + r.radius, 2.0f);
  }

  bool circle_vector (Circle const& circle, Vector2f const& vector) {
    return circle.position.distance_sq(vector) <= circle.radius * circle.radius;
  }

  bool circle_polygon (Circle const& circle, Vector2f const* vertices, size_t vertex_count) {
    if (polygon_vector(vertices, vertex_count, circle.position)) return true;

    for (size_t i = 0; i < vertex_count; i ++) {
      if (auto res = line2_circle(
        { vertices[i], vertices[(i + 1) % vertex_count] },
        circle
      ); res.a) return true;
    }

    return false;
  }



  bool polygon (Vector2f const* l_vertices, size_t l_vertex_count, Vector2f const* r_vertices, size_t r_vertex_count) {
    for (size_t i = 0; i < l_vertex_count; i ++) { // check if any lines intersect
      if (polygon_vector(r_vertices, r_vertex_count, l_vertices[i])) return true;

      for (size_t j = 0; j < r_vertex_count; j ++) {
        if (i == 0) {
          if (polygon_vector(l_vertices, l_vertex_count, r_vertices[j])) return true;
        }

        if (auto res = line2(
          { l_vertices[i], l_vertices[(i + 1) % l_vertex_count] },
          { r_vertices[j], r_vertices[(j + 1) % r_vertex_count] }
        ); res.a) return true;
      }
    }

    return false;
  }


  bool polygon_vector (Vector2f const* vertices, size_t vertex_count, Vector2f const& vector) {
    bool inside = false;

    for (size_t i = 0; i < vertex_count; i ++) {
      Vector2f const& vi = vertices[i];
      Vector2f const& vj = vertices[(i + 1) % vertex_count];

      if (((vi.y > vector.y) != (vj.y > vector.y))
      && (vector.x < (vj.x - vi.x) * (vector.y - vi.y) / (vj.y - vi.y) + vi.x)) {
        inside = !inside;
      }
    }

    return inside;
  }










  
  bool line3_plane (Line3 const& line, Plane const& plane) {
    f32_t s = plane.distance(line.a);
    f32_t e = plane.distance(line.b);

    return (s < 0.0f && e > 0.0f)
        || (e < 0.0f && s > 0.0f);
  }


  static constexpr pair_t<bool, f32_t> v3_intersect_none = { false, -1.0f };



  pair_t<bool, f32_t> ray3_aabb3 (Ray3 const& ray, AABB3 const& aabb) {
    f32_t tmin, tmax, tymin, tymax, tzmin, tzmax;

    f32_t idx = 1.0f / ray.direction.x;
    f32_t idy = 1.0f / ray.direction.y;
    f32_t idz = 1.0f / ray.direction.z;

    if (idx >= 0.0f) {
      tmin = (aabb.min.x - ray.origin.x) * idx;
      tmax = (aabb.max.x - ray.origin.x) * idx;
    } else {
      tmin = (aabb.max.x - ray.origin.x) * idx;
      tmax = (aabb.min.x - ray.origin.x) * idx;
    }

    if (idy >= 0.0f) {
      tymin = (aabb.min.y - ray.origin.y) * idy;
      tymax = (aabb.max.y - ray.origin.y) * idy;
    } else {
      tymin = (aabb.max.y - ray.origin.y) * idy;
      tymax = (aabb.min.y - ray.origin.y) * idy;
    }

    if ((tmin > tymax) || (tymin > tmax)) return v3_intersect_none;


    if (tymin > tmin || tmin != tmin) tmin = tymin;

    if (tymax < tmax || tmax != tmax) tmax = tymax;

    if (idz >= 0.0f) {
      tzmin = (aabb.min.z - ray.origin.z) * idz;
      tzmax = (aabb.max.z - ray.origin.z) * idz;
    } else {
      tzmin = (aabb.max.z - ray.origin.z) * idz;
      tzmax = (aabb.min.z - ray.origin.z) * idz;
    }

    if ((tmin > tzmax) || (tzmin > tmax)) return v3_intersect_none;


    if (tzmin > tmin || tmin != tmin) tmin = tzmin;

    if (tzmax < tmax || tmax != tmax) tmax = tzmax;

    if (tmax < 0.0f) return v3_intersect_none;


    return { true, tmin >= 0.0f? tmin : tmax };
  }

  pair_t<bool, f32_t> ray3_sphere (Ray3 const& ray, Sphere const& sphere) {
    Vector3f delta = sphere.position - ray.origin;

    f32_t tca = delta.dot(ray.direction);
    f32_t d_sq = delta.dot(delta) - (tca * tca);
    
    f32_t r_sq = sphere.radius * sphere.radius;

    if (d_sq > r_sq) return v3_intersect_none;


    f32_t thc = sqrtf(r_sq - d_sq);

    f32_t t0 = tca - thc;
    f32_t t1 = tca + thc;

    bool t0_behind = t0 < 0.0f;
    
    if (t0_behind && t1 < 0.0f) return v3_intersect_none;


    return { true, t0_behind? t1 : t0 };

    
    return v3_intersect_none;
  }

  pair_t<bool, f32_t> ray3_plane (Ray3 const& ray, Plane const& plane) {
    f32_t t = ray.distance(plane);

    if (t == INFINITY) return v3_intersect_none;
    else return { true, t };
  }

  pair_t<bool, f32_t> ray3_triangle (Ray3 const& ray, Triangle const& triangle, bool allow_backface) {
    Vector3f edge1 = triangle.c - triangle.a;
    Vector3f edge2 = triangle.b - triangle.a;

    Vector3f normal = edge1.cross(edge2);

    f32_t DdN = ray.direction.dot(normal);
    f32_t sign;

    if (DdN > 0.0f) {
      if (!allow_backface) return v3_intersect_none;

      sign = 1.0f;
    } else if (DdN < 0.0f) {
      sign = -1.0f;
      DdN = -DdN;
    } else {
      return v3_intersect_none;
    }


    Vector3f diff = ray.origin - triangle.a;

    f32_t DdQxE2 = sign * ray.direction.dot(diff.cross(edge2));

    if (DdQxE2 < 0.0f) return v3_intersect_none;


    f32_t DdE1xQ = sign * ray.direction.dot(edge1.cross(diff));

    if (DdE1xQ < 0.0f
    ||  DdQxE2 + DdE1xQ > DdN) return v3_intersect_none;


    f32_t QdN = -sign * diff.dot(normal);

    if (QdN < 0.0f) return v3_intersect_none;


    return { true, QdN / DdN };


    return v3_intersect_none;
  }



  bool aabb3 (AABB3 const& l, AABB3 const& r) {
    return !(
         r.max.x < l.min.x || r.min.x > l.max.x
      || r.max.y < l.min.y || r.min.y > l.max.y
      || r.max.z < l.min.z || r.min.z > l.max.z
    );
  }

  bool aabb3_vector (AABB3 const& aabb, Vector3f const& v) {
    return !(
         v.x < aabb.min.x || v.x > aabb.max.x
      || v.y < aabb.min.y || v.y > aabb.max.y
      || v.z < aabb.min.z || v.z > aabb.max.z
    );
  }

  bool aabb3_sphere (AABB3 const& aabb, Sphere const& sphere) {
    return aabb.distance_sq(sphere.position) <= sphere.radius * sphere.radius;
  }

  bool aabb3_plane (AABB3 const& aabb, Plane const& plane) {
    f32_t min;
    f32_t max;

    if (plane.normal.x > 0.0f) {
      min = plane.normal.x * aabb.min.x;
      max = plane.normal.x * aabb.max.x;
    } else {
      min = plane.normal.x * aabb.max.x;
      max = plane.normal.x * aabb.min.x;
    }

    if (plane.normal.y > 0.0f) {
      min += plane.normal.y * aabb.min.y;
      max += plane.normal.y * aabb.max.y;
    } else {
      min += plane.normal.y * aabb.max.y;
      max += plane.normal.y * aabb.min.y;
    }

    if (plane.normal.z > 0.0f) {
      min += plane.normal.z * aabb.min.z;
      max += plane.normal.z * aabb.max.z;
    } else {
      min += plane.normal.z * aabb.max.z;
      max += plane.normal.z * aabb.min.z;
    }

    return min <= -plane.constant
        && max >= -plane.constant;
  }

  

  bool aabb3_triangle (AABB3 const& aabb, Triangle const& triangle) {
    static const auto SAT = [] (
      Triangle const& offset,
      Vector3f const& extents,
      Vector3f const* axes, size_t axes_count
    ) -> bool {
      for (size_t i = 0; i <= axes_count; i ++) {
        Vector3f const& test_axis = axes[i];

        f32_t r = extents.x * num::abs(test_axis.x)
                + extents.y * num::abs(test_axis.y)
                + extents.z * num::abs(test_axis.z);

        f32_t p0 = offset.a.dot(test_axis);
        f32_t p1 = offset.b.dot(test_axis);
        f32_t p2 = offset.c.dot(test_axis);

        if (num::max(-num::max(p0, p1, p2), num::min(p0, p1, p2)) > r) {
          return false;
        }
      }

      return true;
    };

    Vector3f center = aabb.center();
    Vector3f extents = aabb.max - center;

    Triangle offset = {
      triangle.a - center,
      triangle.b - center,
      triangle.c - center
    };

    Vector3f f0 = offset.b - offset.a;
    Vector3f f1 = offset.c - offset.b;
    Vector3f f2 = offset.a - offset.c;

    Vector3f axes [9] = {
      {  0.0f, -f0.z,  f0.y },
      {  0.0f, -f1.z,  f1.y },
      {  0.0f, -f2.z,  f2.y },

      {  f0.z,  0.0f, -f0.x },
      {  f1.z,  0.0f, -f1.x },
      {  f2.z,  0.0f, -f2.x },

      { -f0.y,  f0.x,  0.0f },
      { -f1.y,  f1.x,  0.0f },
      { -f2.y,  f2.x,  0.0f }
    };

    if (!SAT(
      offset,
      extents,
      axes, 9
    )) {
      return false;
    }


    axes[0] = { 1.0f, 0.0f, 0.0f };
    axes[1] = { 0.0f, 1.0f, 0.0f };
    axes[2] = { 0.0f, 0.0f, 1.0f };

    if (!SAT(
      offset,
      extents,
      axes, 3
    )) {
      return false;
    }
    
    axes[0] = f0.cross(f1);

    return SAT(
      offset,
      extents,
      axes, 1
    );
  }

  bool aabb3_frustum (AABB3 const& aabb, Frustum const& frustum) {
    for (size_t i = 0; i < 6; i ++) {
      Plane const& plane = frustum.planes[i];

      if (plane.distance({
        plane.normal.x > 0.0f? aabb.max.x : aabb.min.x,
        plane.normal.y > 0.0f? aabb.max.y : aabb.min.y,
        plane.normal.z > 0.0f? aabb.max.z : aabb.min.z
      }) < 0.0f) {
        return false;
      }
    }

    return true;
  }



  bool sphere (Sphere const& l, Sphere const& r) {
    f32_t t_radius = l.radius + r.radius;

    return l.position.distance_sq(r.position) <= t_radius * t_radius;
  }

  bool sphere_vector (Sphere const& sphere, Vector3f const& vector) {
    return sphere.position.distance_sq(vector) <= sphere.radius * sphere.radius;
  }

  bool sphere_plane (Sphere const& sphere, Plane const& plane) {
    return num::abs(plane.distance(sphere.position)) <= sphere.radius;
  }

  bool sphere_frustum (Sphere const& sphere, Frustum const& frustum) {
    f32_t nr = -sphere.radius;

    for (size_t i = 0; i < 6; i ++) {
      f32_t d = frustum.planes[i].distance(sphere.position);

      if (d < nr) return false;
    }

    return true;
  }



  bool triangle_projected_vector (Triangle const& triangle, Vector3f const& vector) {
    Vector3f bary_coord = triangle.get_barycentric_coordinate(vector);

    return bary_coord.x >= 0.0f
        && bary_coord.y >= 0.0f
        && bary_coord.x + bary_coord.y <= 1.0f;
  }



  bool Interesct_frustum_vector (Frustum const& frustum, Vector3f const& vector) {
    for (size_t i = 0; i < 6; i ++) {
      f32_t d = frustum.planes[i].distance(vector);

      if (d < 0.0f) return false;
    }

    return true;
  }
}