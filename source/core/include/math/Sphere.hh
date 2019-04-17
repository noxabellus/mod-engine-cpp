#ifndef SPHERE_H
#define SPHERE_H

#include "Circle.hh"

#include "Vector3.hh"
#include "Vector4.hh"
#include "Matrix3.hh"
#include "Matrix4.hh"



namespace mod {
  struct Line3;
  struct AABB3;
  struct Triangle;


  struct Sphere {
    Vector3f position = { 0.0f, 0.0f, 0.0f };
    f32_t radius = 0.0f;



    /* Create a new zero-initialized Sphere */
    constexpr Sphere () { }

    /* Create a new Sphere and explicitly initialize its position and radius */
    constexpr Sphere (Vector3f const& in_position, f32_t in_radius)
    : position(in_position)
    , radius(in_radius)
    { }

    /* Create a new Sphere and explicility initialize its position's components and its radius */
    constexpr Sphere (f32_t in_x, f32_t in_y, f32_t in_z, f32_t in_radius)
    : position(in_x, in_y, in_z)
    , radius(in_radius)
    { }

    /* Create a new Sphere from a Vector4f representing its position and radius */
    constexpr Sphere (Vector4f const& v)
    : position(v.x, v.y, v.z)
    , radius(v.w)
    { }


    /* Create a new Sphere from a Circle and an optional z-axis position (Defaults to 0.0f) */
    constexpr Sphere from_circle (Circle const& circle, f32_t z = 0.0f) {
      return {
        circle.position.x, circle.position.y, z,
        circle.radius
      };
    }


    /* Create a new Sphere that will fully contain a parameter pack list of points */
    template <typename ... A> Sphere from_vectors (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      Vector3f vectors [arg_count] = { ((Vector3f) args)... };
      return from_vector_list(vectors, arg_count);
    }

    /* Create a new Sphere that will fully contain a list of points */
    ENGINE_API static Sphere from_vector_list (Vector3f const* vectors, size_t vector_count);

    /* Create a new Sphere from an axis-aligned bounding box */
    ENGINE_API static Sphere from_aabb (AABB3 const& aabb);


    /* Convert a Sphere to a Vector4f representing its position and radius */
    constexpr operator Vector4f () const {
      return { position.x, position.y, position.z, radius };
    }


    /* Get the total volume covered by a Sphere */
    f32_t volume () const {
      return ((4.0f / 3.0f) * ((f32_t) M_PI)) * powf(radius, 3.0f);
    }


    /* Expand the radius of a Sphere to fully contain a point */
    ENGINE_API Sphere& expand_to_vector (Vector3f const& v);


    /* Clamp a vector to the inside of a Sphere */ 
    ENGINE_API Vector3f clamp (Vector3f const& v) const;


    /* Get the signed distance from a Sphere to a point (Negative if inside the sphere */
    f32_t distance (Vector3f const& v) const {
      return position.distance(v) - radius;
    }


    /* Determine if a Sphere designates a valid area (r > 0) */
    bool is_valid () const {
      return radius > 0.0f;
    }

    /* Determine if a Sphere does not designate a valid area (r <= 0) */
    bool is_empty () const {
      return radius <= 0.0f;
    }


    /* Determine if two Spheres are identical */
    bool equal (Sphere const& r) const {
      return radius == r.radius
          && position == r.position;
    }

    /* Determine if two Spheres are identical */
    bool operator == (Sphere const& r) const {
      return equal(r);
    }
    

    /* Determine if two Spheres are not identical */
    bool not_equal (Sphere const& r) const {
      return radius != r.radius
          || position != r.position;
    }

    /* Determine if two Spheres are not identical */
    bool operator != (Sphere const& r) const {
      return not_equal(r);
    }
    

    /* Transform a Sphere using a matrix3 */
    Sphere apply_matrix (Matrix3 const& m) const {
      return {
        position.apply_matrix(m),
        radius * m.get_scale().abs().max_element()
      };
    }

    /* Transform a Sphere using a matrix4 */
    Sphere apply_matrix (Matrix4 const& m) const {
      return {
        position.apply_matrix(m),
        radius * m.get_scale_on_axis()
      };
    }
  };
}

#endif