#ifndef PLANE_H
#define PLANE_H

#include "Vector3.hh"
#include "Matrix4.hh"



namespace mod {
  struct Triangle;
  struct Sphere;


  struct Plane {
    union {
      struct {
        Vector3f normal;
        f32_t constant;
      };
      f32_t elements [4];
    };



    /* Create a new zero-initialized Plane */
    constexpr Plane ()
    : normal(0.0f, 0.0f, 0.0f)
    , constant(0.0f)
    { }

    /* Create a new Plane an explicitly initialize its normal and constant */
    constexpr Plane (Vector3f const& in_normal, f32_t in_constant)
    : normal(in_normal)
    , constant(in_constant)
    { }

    /* Create a new Plane and initialize its normal vectors elements individually as well as its constant */
    constexpr Plane (f32_t n_x, f32_t n_y, f32_t n_z, f32_t in_constant)
    : normal(n_x, n_y, n_z)
    , constant(in_constant)
    { }

    /* Create a Plane from a normal and a coplanar vector */
    static Plane from_normal_with_coplanar_vector (Vector3f const& normal, Vector3f const& vector) {
      return {
        normal,
        -vector.dot(normal)
      };
    } 

    /* Create a Plane from 3 coplanar vectors.
     * Winding order is assumed to be counter-clockwise */
    ENGINE_API static Plane from_coplanar_vectors (Vector3f const& a, Vector3f const& b, Vector3f const& c);

    /* Extract the Plane of a Triangle */
    ENGINE_API static Plane from_triangle (Triangle const& triangle);


    /* Normalize a Plane vector and adjust its constant */
    Plane normalize () const {
      f32_t i_len = 1.0f / normal.length();

      return {
        normal * i_len,
        constant * i_len
      };
    }


    /* Negate the normal and constant of a Plane */
    Plane negate () const {
      return {
        normal.negate(),
        constant * -1.0f
      };
    }


    /* Get the signed distance from a Plane to a point */
    f32_t distance (Vector3f const& v) const {
      return normal.dot(v) + constant;
    }

    /* Get the signed distance from a Plane to a sphere */
    ENGINE_API f32_t distance (Sphere const& sphere) const;


    /* Project a point onto a Plane */
    Vector3f project (Vector3f const& v) const {
      return (normal * -distance(v)) + v;
    }

    /* Create a vector coplanar with a Plane */
    Vector3f coplanar_vector () const {
      return normal * -constant;
    }


    /* Translate a Plane along its normal vector */
    Plane& translate (Vector3f const& offset) {
      constant -= offset.dot(normal);
      return *this;
    }


    /* Determine if two Planes are essentially equivalent
     * Wrapper for num::almost_equal, see it for more details */
    bool almost_equal (Plane const& r, s32_t ulp = 2) const {
      return num::almost_equal(constant, r.constant, ulp)
          && normal.almost_equal(r.normal, ulp);
    }


    /* Determine if two Planes are identical */
    bool equal (Plane const& r) const {
      return num::flt_equal(constant, r.constant)
          && normal == r.normal;
    }
    
    /* Determine if two Planes are identical */
    bool operator == (Plane const& r) const {
      return equal(r);
    }


    /* Determine if two Planes are not identical */
    bool not_equal (Plane const& r) const {
      return num::flt_not_equal(constant, r.constant)
          || normal != r.normal;
    }
    
    /* Determine if two Planes are not identical */
    bool operator != (Plane const& r) const {
      return not_equal(r);
    }
    

    /* Transform a Plane using a Matrix4 */
    ENGINE_API Plane apply_matrix (Matrix4 const& m) const;
  };
}

#endif