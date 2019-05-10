#ifndef QUATERNION_H
#define QUATERNION_H

#include "../cstd.hh"

#include "Vector2.hh"
#include "Vector3.hh"
#include "Vector4.hh"



namespace mod {
  struct Euler;

  struct Quaternion {
    union {
      struct {
        f32_t x;
        f32_t y;
        f32_t z;
        f32_t w;
      };
      f32_t elements [4];
    };



    /* Create a new identity-initialized Quaternion */
    constexpr Quaternion ()
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , w(1.0f)
    { }

    /* Create a new Quaternion and initialize its members */
    constexpr Quaternion (f32_t in_x, f32_t in_y, f32_t in_z, f32_t in_w)
    : x(in_x)
    , y(in_y)
    , z(in_z)
    , w(in_w)
    { }

    /* Create a new Quaternion by distributing a scalar value to all members */
    constexpr Quaternion (f32_t scalar)
    : x(scalar)
    , y(scalar)
    , z(scalar)
    , w(scalar)
    { }


    /* Convert a Vector4 to a Quaternion */
    template <typename T> constexpr Quaternion (Vector4<T> const& v)
    : x(static_cast<f32_t>(v.x))
    , y(static_cast<f32_t>(v.y))
    , z(static_cast<f32_t>(v.z))
    , w(static_cast<f32_t>(v.w))
    { }

    
    /* Convert a Vector3 and a w value to a Quaternion */
    template <typename T> constexpr Quaternion (Vector3<T> const& v, f32_t w)
    : x(static_cast<f32_t>(v.x))
    , y(static_cast<f32_t>(v.y))
    , z(static_cast<f32_t>(v.z))
    , w(w)
    { }



    /* Convert a Quaternion to a Vector4 */
    template <typename T = f32_t> operator Vector4<T> () const {
      return { (T) x, (T) y, (T) z, (T) w };
    }


    /* Get an element of a Quaternion by index.
     * For efficiency, the index is not bounds checked */
    f32_t& operator [] (size_t index) const {
      return const_cast<f32_t&>(elements[index]);
    }


    /* Create a Quaternion rotation from an Euler rotation */
    ENGINE_API static Quaternion from_euler (Euler const& e);

    /* Create a Quaternion rotation from an angular rotation around an axis */
    ENGINE_API static Quaternion from_axis_angle (Vector3f const& axis, f32_t angle);

    /* Create a Quaternion rotation from unit vectors */
    ENGINE_API static Quaternion from_unit_vectors (Vector3f const& from, Vector3f const& to);

    /* Normalize a Quaternion.
     * Note that this is slightly different from vector normalization */
    ENGINE_API Quaternion normalize () const;

    /* Conjugate a Quaternion */
    ENGINE_API Quaternion conjugate () const;

    /* Get the inverse of a Quaternion */
    ENGINE_API Quaternion inverse () const;


    /* Perform Quaternion multiplication */
    ENGINE_API Quaternion multiply (Quaternion const& r) const;

    /* Perform Quaternion multiplication */
    Quaternion operator * (Quaternion const& r) const {
      return multiply(r);
    }

    /* Perform Quaternion multiplication and put the result back into the left operand */
    Quaternion& operator *= (Quaternion const& r) {
      *this = multiply(r);
      return *this;
    }


    /* Perform Quaternion addition */
    Quaternion add (Quaternion const& r) const {
      return reinterpret_cast<Vector4f const*>(this)->add(*reinterpret_cast<Vector4f const*>(&r));
    }

    /* Perform Quaternion addition */
    Quaternion operator + (Quaternion const& r) const {
      return add(r);
    }
    
    /* Perform Quaternion addition and put the result back into the left operand */
    Quaternion& operator += (Quaternion const& r) {
      *this = add(r);
      return *this;
    }

    /* Perform Quaternion subtraction */
    Quaternion sub (Quaternion const& r) const {
      return reinterpret_cast<Vector4f const*>(this)->sub(*reinterpret_cast<Vector4f const*>(&r));
    }
    
    /* Perform Quaternion subtraction */
    Quaternion operator - (Quaternion const& r) const {
      return sub(r);
    }

    /* Perform Quaternion subtraction and put the result back into the left operand */
    Quaternion& operator -= (Quaternion const& r) {
      *this = sub(r);
      return *this;
    }

    /* Perform Quaternion division */
    Quaternion div (Quaternion const& r) const {
      return reinterpret_cast<Vector4f const*>(this)->div(*reinterpret_cast<Vector4f const*>(&r));
    }

    /* Perform Quaternion division */
    Quaternion operator / (Quaternion const& r) const {
      return div(r);
    }

    /* Perform Quaternion division and put the result back into the left operand */
    Quaternion& operator /= (Quaternion const& r) {
      *this = div(r);
      return *this;
    }


    /* Perform Quaternion multiplication with a scalar */
    Quaternion multiply (f32_t r) const {
      return reinterpret_cast<Vector4f const*>(this)->mul(r);
    }

    /* Perform Quaternion multiplication with a scalar */
    Quaternion operator * (f32_t r) const {
      return multiply(r);
    }

    /* Perform Quaternion multiplication with a scalar and put the result back into the left operand */
    Quaternion& operator *= (f32_t r) {
      *this = multiply(r);
      return *this;
    }


    /* Perform Quaternion addition with a scalar */
    Quaternion add (f32_t r) const {
      return reinterpret_cast<Vector4f const*>(this)->add(r);
    }

    /* Perform Quaternion addition with a scalar */
    Quaternion operator + (f32_t r) const {
      return add(r);
    }

    /* Perform Quaternion addition with a scalar and put the result back into the left operand */
    Quaternion& operator += (f32_t r) {
      *this = add(r);
      return *this;
    }

    /* Perform Quaternion subtraction with a scalar */
    Quaternion sub (f32_t r) const {
      return reinterpret_cast<Vector4f const*>(this)->sub(r);
    }
    
    /* Perform Quaternion subtraction with a scalar */
    Quaternion operator - (f32_t r) const {
      return sub(r);
    }

    /* Perform Quaternion subtraction with a scalar and put the result back into the left operand */
    Quaternion& operator -= (f32_t r) {
      *this = sub(r);
      return *this;
    }

    /* Perform Quaternion division with a scalar */
    Quaternion div (f32_t r) const {
      return reinterpret_cast<Vector4f const*>(this)->div(r);
    }

    /* Perform Quaternion division with a scalar */
    Quaternion operator / (f32_t r) const {
      return div(r);
    }

    /* Perform Quaternion division with a scalar and put the result back into the left operand */
    Quaternion& operator /= (f32_t r) {
      *this = div(r);
      return *this;
    }


    /* Linear interpolate between two Quaternions */
    ENGINE_API Quaternion lerp (f32_t alpha, Quaternion const& finish) const;

    /* Spherical linear interpolate between two Quaternions */
    ENGINE_API Quaternion slerp (f32_t alpha, Quaternion const& finish) const;


    /* Get the magnitude/length of a Quaternion vector */
    f32_t length () const {
      return reinterpret_cast<Vector4f const*>(this)->length();
    }

    /* Get the dot product of two Quaternion vectors */
    f32_t dot (Quaternion const& r) const {
      return reinterpret_cast<Vector4f const*>(this)->dot(*reinterpret_cast<Vector4f const*>(&r));
    }

    /* Negate each component of a Quaternion vector */
    Quaternion negate () const {
      return { -x, -y, -z, -w };
    }


    /* Determine if two Quaternions are essentialy equivalent.
     * Wrapper for num::almost_equal, see it for details */
    bool almost_equal (Quaternion const& r, s32_t ulp = 2) const {
      return num::almost_equal(x, r.x, ulp)
          && num::almost_equal(y, r.y, ulp)
          && num::almost_equal(z, r.z, ulp)
          && num::almost_equal(w, r.w, ulp);
    }


    /* Determine if two Quaternions are identical */
    bool equal (Quaternion const& r) const {
      return num::flt_equal(x, r.x)
          && num::flt_equal(y, r.y)
          && num::flt_equal(z, r.z)
          && num::flt_equal(w, r.w);
    }

    /* Determine if two Quaternions are identical */
    bool operator == (Quaternion const& r) const {
      return equal(r);
    }

    /* Determine if two Quaternions are not identical */
    bool not_equal (Quaternion const& r) const {
      return num::flt_not_equal(x, r.x)
          || num::flt_not_equal(y, r.y)
          || num::flt_not_equal(z, r.z)
          || num::flt_not_equal(w, r.w);
    }

    /* Determine if two Quaternions are not identical */
    bool operator != (Quaternion const& r) const {
      return not_equal(r);
    }
  };


  namespace Constants {
    namespace Quaternion {
      static constexpr ::mod::Quaternion zero = { 0.0f, 0.0f, 0.0f, 0.0f };
      static constexpr ::mod::Quaternion unit = { 1.0f, 1.0f, 1.0f, 1.0f };
      static constexpr ::mod::Quaternion inverse_unit = { -1.0f, -1.0f, -1.0f, -1.0f };
      static constexpr ::mod::Quaternion minimum = { std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min() };
      static constexpr ::mod::Quaternion maximum = { std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max() };
      static constexpr ::mod::Quaternion identity = { 0.0f, 0.0f, 0.0f, 1.0f };
    }
  }
}

#endif