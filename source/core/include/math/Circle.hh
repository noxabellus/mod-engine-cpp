#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector2.hh"
#include "Vector3.hh"
#include "Matrix3.hh"
#include "Matrix4.hh"


namespace mod {
  struct Sphere;

  struct Line2;
  struct AABB2;


  struct Circle {
    Vector2f position = { 0.0f, 0.0f };
    f32_t radius = 0.0f;



    /* Create a new zero-initialized Circle */
    constexpr Circle () { }

    /* Create a new Circle and explicitly initialize its position and radius */
    constexpr Circle (Vector2f const& in_position, f32_t in_radius)
    : position(in_position)
    , radius(in_radius)
    { }

    /* Create a new Circle and explicility initialize its position's components and its radius */
    constexpr Circle (f32_t in_x, f32_t in_y, f32_t in_radius)
    : position(in_x, in_y)
    , radius(in_radius)
    { }

    /* Create a new Circle from a Vector3f representing its position and radius */
    constexpr Circle (Vector3f const& v)
    : position(v.x, v.y)
    , radius(v.z)
    { }


    /* Create a new Circle that will fully contain a parameter pack list of points */
    template <typename ... A> static Circle from_vectors (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      Vector2f vectors [arg_count] = { ((Vector2f) args)... };
      return from_vector_list(vectors, arg_count);
    }

    /* Create a new Circle that willy fully contain a list of points */
    ENGINE_API static Circle from_vector_list (Vector2f const* vectors, size_t vector_count);

    /* Create a new Circle from an axis-aligned bounding box */
    ENGINE_API static Circle from_aabb (AABB2 const& aabb);

    /* Create a new Circle from a Sphere */
    ENGINE_API static Circle from_sphere (Sphere const& sphere);


    /* Convert a Circle to a Vector3f representing its position and radius */
    constexpr operator Vector3f () const {
      return { position.x, position.y, radius };
    }


    /* Get the total area covered by a Circle */
    f32_t area () const {
      return ((f32_t) M_PI) * powf(radius, 2.0f);
    }

    
    /* Expand the radius of a Circle to fully contain a point */
    ENGINE_API Circle& expand_to_vector (Vector2f const& v);

    
    /* Clamp a vector to the inside of a Circle */
    ENGINE_API Vector2f clamp (Vector2f const& v) const;


    /* Determine if a Circle designates a valid area (r > 0) */
    bool is_valid () const {
      return radius > 0.0f;
    }
    
    /* Determine if a Circle does not designate a valid area (r <= 0) */
    bool is_empty () const {
      return radius <= 0.0f;
    }
    

    /* Determine if two Circles are identical */
    bool equal (Circle const& r) const {
      return radius == r.radius
          && position == r.position;
    }

    /* Determine if two Circles are identical */
    bool operator == (Circle const& r) const {
      return equal(r);
    }
    

    /* Determine if two Circles are not identical */
    bool not_equal (Circle const& r) const {
      return radius != r.radius
          || position != r.position;
    }

    /* Determine if two Circles are not identical */
    bool operator != (Circle const& r) const {
      return not_equal(r);
    }


    /* Transform a Circle using a matrix3 */
    Circle apply_matrix (Matrix3 const& m) const {
      return {
        position.apply_matrix(m),
        radius * m.get_scale().abs().max_element()
      };
    }

    /* Transform a Circle using a matrix4 */
    Circle apply_matrix (Matrix4 const& m) const {
      return {
        position.apply_matrix(m),
        radius * m.get_scale_on_axis()
      };
    }
  };
}

#endif