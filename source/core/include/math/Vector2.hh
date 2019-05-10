#ifndef VECTOR2_H
#define VECTOR2_H

#include "../cstd.hh"



namespace mod {
  struct Matrix3;
  struct Matrix4;

  template <typename T> struct Vector2 {
    union {
      struct {
        T x;
        T y;
      };
      T elements [2];
    };



    /* Create a new zero-initialized vector */
    constexpr Vector2 ()
    : x(0)
    , y(0)
    { }

    /* Create a new vector and initialize its members */
    constexpr Vector2 (T in_x, T in_y)
    : x(in_x)
    , y(in_y)
    { }

    /* Create a new vector by distributing a scalar value to all members */
    constexpr Vector2 (T scalar)
    : x(scalar)
    , y(scalar)
    { }

    /* Convert an imgui vector to a modengine vector */
    constexpr Vector2 (ImVec2 const& imvec)
    : x(imvec.x)
    , y(imvec.y)
    { }


    /* Convert a vector to another type of vector */
    template <typename U> constexpr operator Vector2<U> () const {
      return { static_cast<U>(x), static_cast<U>(y) };
    }

    /* Convert a vector to an imgui vector */
    operator ImVec2 () const {
      return { x, y };
    }


    /* Get an element of a vector by index.
     * For efficiency, the index is not bounds checked */
    T& operator [] (size_t index) const {
      return const_cast<T&>(elements[index]);
    }


    /* Get an ArrayIterator representing the start of a vectors elements */
    ArrayIterator<T> begin () const {
      return { const_cast<T*>(&elements[0]), 0 };
    }

    /* Get an ArrayIterator representing the end of a vectors elements */
    ArrayIterator<T> end () const {
      return { const_cast<T*>(&elements[0]), 2 };
    }

    
    /* x + r.x, y + r.y */
    Vector2 add (Vector2 const& r) const {
      return {
        x + r.x,
        y + r.y
      };
    }

    /* x + r.x, y + r.y */
    Vector2 operator + (Vector2 const& r) const {
      return add(r);
    }

    /* x += r.x, y += r.y */
    Vector2& operator += (Vector2 const& r) {
      x += r.x;
      y += r.y;
      return *this;
    }
    

    /* x - r.x, y - r.y */
    Vector2 sub (Vector2 const& r) const {
      return {
        x - r.x,
        y - r.y
      };
    }

    /* x - r.x, y - r.y */
    Vector2 operator - (Vector2 const& r) const {
      return sub(r);
    }

    /* x -= r.x, y -= r.y */
    Vector2& operator -= (Vector2 const& r) {
      x -= r.x;
      y -= r.y;
      return *this;
    }

    
    /* x * r.x, y * r.y */
    Vector2 mul (Vector2 const& r) const {
      return {
        x * r.x,
        y * r.y
      };
    }

    /* x * r.x, y * r.y */
    Vector2 operator * (Vector2 const& r) const {
      return mul(r);
    }

    /* x *= r.x, y *= r.y */
    Vector2& operator *= (Vector2 const& r) {
      x *= r.x;
      y *= r.y;
      return *this;
    }
    
    /* x / r.x, y / r.y */
    Vector2 div (Vector2 const& r) const {
      return {
        x / r.x,
        y / r.y
      };
    }

    /* x / r.x, y / r.y */
    Vector2 operator / (Vector2 const& r) const {
      return div(r);
    }

    /* x /= r.x, y /= r.y */
    Vector2& operator /= (Vector2 const& r) {
      x /= r.x;
      y /= r.y;
      return *this;
    }
    
    
    /* x % r.x, y % r.y */
    Vector2 rem (Vector2 const& r) const {
      return {
        num::remainder(x, r.x),
        num::remainder(y, r.y)
      };
    }

    /* x % r.x, y % r.y */
    Vector2 operator % (Vector2 const& r) const {
      return rem(r);
    }

    /* x %= r.x, y %= r.y */
    Vector2& operator %= (Vector2 const& r) {
      x = num::remainder(x, r.x);
      y = num::remainder(x, r.y);
      return *this;
    }

    
    /* x + r, y + r */
    Vector2 add_scl (T r) const {
      return {
        x + r,
        y + r
      };
    }

    /* x + r, y + r */
    Vector2 operator + (T r) const {
      return add_scl(r);
    }

    /* x += r, y += r */
    Vector2& operator += (T r) {
      x += r;
      y += r;
      return *this;
    }
    

    /* x * r, y * r */
    Vector2 sub_scl (T r) const {
      return {
        x - r,
        y - r
      };
    }
    /* x - r, y - r */
    Vector2 operator - (T r) const {
      return sub_scl(r);
    }

    /* x -= r, y -= r */
    Vector2& operator -= (T r) {
      x -= r;
      y -= r;
      return *this;
    }

    
    /* x - r, y - r */
    Vector2 mul_scl (T r) const {
      return {
        x * r,
        y * r
      };
    }
    
    /* x * r, y * r */
    Vector2 operator * (T r) const {
      return mul_scl(r);
    }

    /* x *= r, y *= r */
    Vector2& operator *= (T r) {
      x *= r;
      y *= r;
      return *this;
    }

    
    /* x / r, y / r */
    Vector2 div_scl (T r) const {
      return {
        x / r,
        y / r
      };
    }

    /* x / r, y / r */
    Vector2 operator / (T r) const {
      return div_scl(r);
    }

    /* x /= r, y /= r */
    Vector2& operator /= (T r) {
      x /= r;
      y /= r;
      return *this;
    }

    
    /* x % r, y % r */
    Vector2 rem_scl (T r) const {
      return {
        num::remainder(x, r),
        num::remainder(y, r)
      };
    }

    /* x % r, y % r */
    Vector2 operator % (T r) const {
      return rem_scl(r);
    }

    /* x %= r, y %= r */
    Vector2& operator %= (T r) {
      x = num::remainder(x, r);
      y = num::remainder(y, r);
      return *this;
    }

    
    /* x + y */
    template <typename U = T> U add_reduce () const {
      return static_cast<U>(x) + static_cast<U>(y);
    }
    
    /* x - y */
    template <typename U = T> U sub_reduce () const {
      return static_cast<U>(x) - static_cast<U>(y);
    }
    
    /* x * y */
    template <typename U = T> U mul_reduce () const {
      return static_cast<U>(x) * static_cast<U>(y);
    }
    
    /* x / y */
    template <typename U = T> U div_reduce () const {
      return static_cast<U>(x) / static_cast<U>(y);
    }
    
    /* x % y */
    template <typename U = T> U rem_reduce () const {
      return num::remainder(static_cast<U>(x), static_cast<U>(y));
    }


    /* Determine whether two floating point vectors are essentially equivalent.
     * Wrapper for num::almost_equal, see it for more detail */
    bool almost_equal (Vector2 const& r, s32_t ulp = 2) const {
      return num::almost_equal(x, r.x, ulp)
          && num::almost_equal(y, r.y, ulp);
    }
    
    /* x == r.x && y == r.y */
    bool equal (Vector2 const& r) const {
      return num::flt_equal(x, r.x)
          && num::flt_equal(y, r.y);
    }

    /* x == r.x && y == r.y */
    bool operator == (Vector2 const& r) const {
      return equal(r);
    }

    /* x != r.x || y != r.y */
    bool not_equal (Vector2 const& r) const {
      return num::flt_not_equal(x, r.x)
          || num::flt_not_equal(y, r.y);
    }

    /* x != r.x || y != r.y */
    bool operator != (Vector2 const& r) const {
      return not_equal(r);
    }

    
    /* Get the absolute value of each component of a vector */
    Vector2 abs () const {
      return {
        num::abs<T>(x),
        num::abs<T>(y)
      };
    }

    
    /* Clamp a vector between two other vectors */
    Vector2 clamp (Vector2 const& min, Vector2 const& max) const {
      return {
        num::clamp(x, min.x, max.x),
        num::clamp(y, min.y, max.y)
      };
    }
    
    /* Clamp each of a vector's components between two scalars */
    Vector2 clamp_scl (T min, T max) const {
      return {
        num::clamp(x, min, max),
        num::clamp(y, min, max)
      };
    }
    
    /* Linear interpolate to an offset between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2 lerp (U alpha, Vector2 const& finish) const {
      return {
        num::lerp<T, U>(alpha, x, finish.x),
        num::lerp<T, U>(alpha, y, finish.y)
      };
    }
    
    /* Take the floor of each component of a vector */
    Vector2 floor () const {
      return {
        ::floor(x),
        ::floor(y)
      };
    }
    
    /* Round each component of a vector */
    Vector2 round () const {
      return {
        ::round(x),
        ::round(y)
      };
    }
    
    /* Take the ceil of each component of a vector */
    Vector2 ceil () const {
      return {
        ::ceil(x),
        ::ceil(y)
      };
    }

    
    /* Take the minimum of each component of two vectors */
    Vector2 min (Vector2 const& r) const {
      return {
        num::min(x, r.x),
        num::min(y, r.y)
      };
    }
    
    /* Take the maximum of each component of two vectors */
    Vector2 max (Vector2 const& r) const {
      return {
        num::max(x, r.x),
        num::max(y, r.y)
      };
    }

    
    /* Take the minimum of each component of a vector and a scalar value */
    Vector2 min_scl (T r) const {
      return {
        num::min(x, r),
        num::min(y, r)
      };
    }

    /* Take the maximum of each component of a vector and a scalar value */
    Vector2 max_scl (T r) const {
      return {
        num::max(x, r),
        num::max(y, r)
      };
    }


    /* Take the minimum of all elements of a vector */
    T min_element () const {
      return num::min(x, y);
    }

    /* Take the maximum of all elements of a vector */
    T max_element () const {
      return num::max(x, y);
    }

    
    /* Negate each component of a vector */
    Vector2 negate () const {
      return { -x, -y };
    }

    /* Negate each component of a vector */
    Vector2 operator - () const {
      return negate();
    }

    
    /* Get the sign of each component of a vector */
    Vector2 sign () const {
      return {
        num::sign(x),
        num::sign(y)
      };
    }


    
    /* Get the square length/magnitude of a vector */
    T length_sq () const {
      return mul(*this).add_reduce();
    }
    
    /* Get the manhattan length/magnitude of a vector */
    T length_manh () const {
      return abs().add_reduce();
    }
    
    /* Get the length/magnitude of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U length () const {
      return sqrt(static_cast<U>(length_sq()));
    }
    
    /* Get the dot product of two vectors */
    T dot (Vector2 const& r) const {
      return mul(r).add_reduce();
    }
    
    /* Get the cross product of two vectors */
    T cross (Vector2 const& r) const {
      return x * r.y - y * r.x;
    }
    
    /* Get the angle between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U angle (Vector2 const& r) const {
      U t = static_cast<U>(dot(r)) / sqrt(static_cast<U>(length_sq()) * static_cast<U>(r.length_sq()));
      return acos(num::clamp<U>(t, U(-1), U(1)));
    }
    
    /* Get the square length/magnitude of the difference between two vectors */
    T distance_sq (Vector2 const& r) const {
      return sub(r).length_sq();
    }
    
    /* Get the length/magnitude of the difference between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U distance (Vector2 const& r) const {
      return sqrt(static_cast<U>(distance_sq(r)));
    }
    
    /* Get the unit-scale version of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> normalize () const {
      U l = length<U>();
      if (num::flt_equal(l, U(0))) return { U(0), U(0) };
      else return Vector2<U>(*this) / l;
    }
    
    /* Get the unit-scale version of a vector and multiply it by the given length/magnitude */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> scale_to_length (U new_length) const {
      return normalize<U>() * new_length;
    }
    
    /* Limit the position of a vector to between a minimum and maximum distance from the origin */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> clamp_to_length (U min_length, U max_length) const {
      U l = length<U>();
      return (Vector2<U>(*this) / l) * num::clamp<U>(l, min_length, max_length));
    }
    
    /* Get the proportion of the components of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> proportion () const {
      if (x < y) return {
        .x = static_cast<U>(x) / static_cast<U>(y),
        .y = U(1)
      }; else if (y < x) return {
        .x = U(1),
        .y = static_cast<U>(y) / static_cast<U>(x)
      }; else return { U(1), U(1) };
    }
    
    /* Get the unit-scale direction vector from one vector to another */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> direction (Vector2 const& r) const {
      return r.sub(*this).template normalize<U>();
    }
    
    /* Project a vector onto another vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> project_on_vec (Vector2 const& r) const {
      return Vector2<U>(r) * (Vector2<U>(r.dot(l)) / static_cast<U>(r.length_sq()));
    }
    
    /* Project a vector onto the normal of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> project_on_normal (Vector2 const& r) const {
      return Vector2<U>(*this) - project_on_vec<U>(r);
    }
    
    /* Reflect a vector across a normal */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector2<U> reflect (Vector2 const* r) const {
      return Vector2<U>(*this) - (Vector2<U>(r) * U(2) * static_cast<U>(dot(r)));
    }

    
    /* Transform a vector using a matrix3 */
    Vector2<f32_t> apply_matrix (Matrix3 const& m) const {
      return {
        m[0] * static_cast<f32_t>(x) + m[1] * static_cast<f32_t>(y) + m[2],
        m[3] * static_cast<f32_t>(x) + m[4] * static_cast<f32_t>(y) + m[5]
      };
    }
    
    /* Transform a vector using a matrix4 */
    Vector2<f32_t> apply_matrix (Matrix4 const& m) const {
      return {
        m[0] * static_cast<f32_t>(x) + m[4] * static_cast<f32_t>(y) + m[12],
        m[1] * static_cast<f32_t>(x) + m[5] * static_cast<f32_t>(y) + m[13]
      };
    }
  

    /* Rotate a direction vector using a matrix3 */
    Vector2<f32_t> transform_direction (Matrix3 const& m) const {
      Matrix3 rot_only = Matrix3::from_rotation_angle(m.get_rotation());
      return apply_matrix(rot_only).normalize();
    }

    /* Rotate a direction vector using a matrix4 */
    Vector2<f32_t> transform_direction (Matrix4 const& m) const {
      return (Vector2<f32_t> {
        m[0] * static_cast<f32_t>(x) + m[4] * static_cast<f32_t>(y),
        m[1] * static_cast<f32_t>(x) + m[5] * static_cast<f32_t>(y)
      }).normalize();
    }
  };


  using Vector2f  = Vector2<f32_t>;
  using Vector2d  = Vector2<f64_t>;
  using Vector2ld = Vector2<f128_t>;
  using Vector2s  = Vector2<s32_t>;
  using Vector2ls = Vector2<s64_t>;
  using Vector2u  = Vector2<u32_t>;
  using Vector2lu = Vector2<u64_t>;


  namespace Constants {
    namespace Vector2f {
      static constexpr ::mod::Vector2f zero = { 0.0f, 0.0f };
      static constexpr ::mod::Vector2f unit = { 1.0f, 1.0f };
      static constexpr ::mod::Vector2f inverse_unit = { -1.0f, -1.0f };
      static constexpr ::mod::Vector2f minimum = { std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min() };
      static constexpr ::mod::Vector2f maximum = { std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max() };
    }

    namespace Vector2d {
      static constexpr ::mod::Vector2d zero = { 0.0, 0.0 };
      static constexpr ::mod::Vector2d unit = { 1.0, 1.0 };
      static constexpr ::mod::Vector2d inverse_unit = { -1.0, -1.0 };
      static constexpr ::mod::Vector2d minimum = { std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min() };
      static constexpr ::mod::Vector2d maximum = { std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max() };
    }

    namespace Vector2s {
      static constexpr ::mod::Vector2s zero = { 0, 0 };
      static constexpr ::mod::Vector2s unit = { 1, 1 };
      static constexpr ::mod::Vector2s inverse_unit = { -1, -1 };
      static constexpr ::mod::Vector2s minimum = { std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min() };
      static constexpr ::mod::Vector2s maximum = { std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max() };
    }

    namespace Vector2ls {
      static constexpr ::mod::Vector2ls zero = { 0, 0 };
      static constexpr ::mod::Vector2ls unit = { 1, 1 };
      static constexpr ::mod::Vector2ls inverse_unit = { -1, -1 };
      static constexpr ::mod::Vector2ls minimum = { std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min() };
      static constexpr ::mod::Vector2ls maximum = { std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max() };
    }

    namespace Vector2u {
      static constexpr ::mod::Vector2u zero = { 0, 0 };
      static constexpr ::mod::Vector2u unit = { 1, 1 };
      static constexpr ::mod::Vector2u minimum = { std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min() };
      static constexpr ::mod::Vector2u maximum = { std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max() };
    }

    namespace Vector2lu {
      static constexpr ::mod::Vector2lu zero = { 0, 0 };
      static constexpr ::mod::Vector2lu unit = { 1, 1 };
      static constexpr ::mod::Vector2lu minimum = { std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min() };
      static constexpr ::mod::Vector2lu maximum = { std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max() };
    }
  }
}

#endif