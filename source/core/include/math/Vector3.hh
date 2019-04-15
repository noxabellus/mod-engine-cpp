#pragma once

#include "../cstd.hh"



namespace mod {
  struct Euler;
  struct Quaternion;
  struct Matrix3;
  struct Matrix4;


  template <typename T> struct Vector3 {
    union {
      struct {
        T x;
        T y;
        T z;
      };
      T elements [3];
    };



    /* Create a new zero-initialized vector */
    constexpr Vector3 ()
    : x(0)
    , y(0)
    , z(0)
    { }

    /* Create a new vector and initialize its members */
    constexpr Vector3 (T in_x, T in_y, T in_z)
    : x(in_x)
    , y(in_y)
    , z(in_z)
    { }

    /* Create a new vector by distributing a scalar value to all members */
    constexpr Vector3 (T scalar)
    : x(scalar)
    , y(scalar)
    , z(scalar)
    { }


    /* Convert a vector to another type of vector */
    template <typename U> constexpr operator Vector3<U> () const {
      return { (U) x, (U) y, (U) z };
    }


    /* Get an element of a vector by index.
     * For efficiency, the index is not bounds checked */
    T& operator [] (size_t index) const {
      return (T&) elements[index];
    }



    /* x + r.x, y + r.y, z + r.z */
    Vector3 add (Vector3 const& r) const {
      return {
        x + r.x,
        y + r.y,
        z + r.z
      };
    }

    /* x + r.x, y + r.y, z + r.z */
    Vector3 operator + (Vector3 const& r) const {
      return add(r);
    }

    /* x += r.x, y += r.y, z += r.z */
    Vector3& operator += (Vector3 const& r) {
      x += r.x;
      y += r.y;
      z += r.z;
      return *this;
    }


    /* x - r.x, y - r.y, z - r.z */
    Vector3 sub (Vector3 const& r) const {
      return {
        x - r.x,
        y - r.y,
        z - r.z
      };
    }

    /* x - r.x, y - r.y, z - r.z */
    Vector3 operator - (Vector3 const& r) const {
      return sub(r);
    }

    /* x -= r.x, y -= r.y, z -= r.z */
    Vector3& operator -= (Vector3 const& r) {
      x -= r.x;
      y -= r.y;
      z -= r.z;
      return *this;
    }


    /* x * r.x, y * r.y, z * r.z */
    Vector3 mul (Vector3 const& r) const {
      return {
        x * r.x,
        y * r.y,
        z * r.z
      };
    }

    /* x * r.x, y * r.y, z * r.z */
    Vector3 operator * (Vector3 const& r) const {
      return mul(r);
    }

    /* x *= r.x, y *= r.y, z *= r.z */
    Vector3& operator *= (Vector3 const& r) {
      x *= r.x;
      y *= r.y;
      z *= r.z;
      return *this;
    }


    /* x / r.x, y / r.y, z / r.z */
    Vector3 div (Vector3 const& r) const {
      return {
        x / r.x,
        y / r.y,
        z / r.z
      };
    }

    /* x / r.x, y / r.y, z / r.z */
    Vector3 operator / (Vector3 const& r) const {
      return div(r);
    }

    /* x /= r.x, y /= r.y, z /= r.z */
    Vector3& operator /= (Vector3 const& r) {
      x /= r.x;
      y /= r.y;
      z /= r.z;
      return *this;
    }


    /* x % r.x, y % r.y, z % r.z */
    Vector3 rem (Vector3 const& r) const {
      return {
        num::remainder(x, r.x),
        num::remainder(y, r.y),
        num::remainder(z, r.z)
      };
    }

    /* x % r.x, y % r.y, z % r.z */
    Vector3 operator % (Vector3 const& r) const {
      return rem(r);
    }

    /* x %= r.x, y %= r.y, z %= r.z */
    Vector3& operator %= (Vector3 const& r) {
      x = num::remainder(x, r.x);
      y = num::remainder(y, r.y);
      z = num::remainder(z, r.z);
      return *this;
    }


    /* x + r, y + r, z + r */
    Vector3 add_scl (T r) const {
      return {
        x + r,
        y + r,
        z + r
      };
    }

    /* x + r, y + r, z + r */
    Vector3 operator + (T r) const {
      return add_scl(r);
    }

    /* x += r, y += r, z += r */
    Vector3& operator += (T r) {
      x += r;
      y += r;
      z += r;
      return *this;
    }


    /* x - r, y - r, z - r */
    Vector3 sub_scl (T r) const {
      return {
        x - r,
        y - r,
        z - r
      };
    }

    /* x - r, y - r, z - r */
    Vector3 operator - (T r) const {
      return sub_scl(r);
    }

    /* x -= r, y -= r, z -= r */
    Vector3& operator -= (T r) {
      x -= r;
      y -= r;
      z -= r;
      return *this;
    }


    /* x * r, y * r, z * r */
    Vector3 mul_scl (T r) const {
      return {
        x * r,
        y * r,
        z * r
      };
    }

    /* x * r, y * r, z * r */
    Vector3 operator * (T r) const {
      return mul_scl(r);
    }

    /* x *= r, y *= r, z *= r */
    Vector3& operator *= (T r) {
      x *= r;
      y *= r;
      z *= r;
      return *this;
    }


    /* x / r, y / r, z / r */
    Vector3 div_scl (T r) const {
      return {
        x / r,
        y / r,
        z / r
      };
    }

    /* x / r, y / r, z / r */
    Vector3 operator / (T r) const {
      return div_scl(r);
    }

    /* x /= r, y /= r, z /= r */
    Vector3& operator /= (T r) {
      x /= r;
      y /= r;
      z /= r;
      return *this;
    }


    /* x % r, y % r, z % r */
    Vector3 rem_scl (T r) const {
      return {
        num::remainder(x, r),
        num::remainder(y, r),
        num::remainder(z, r)
      };
    }

    /* x % r, y % r, z % r */
    Vector3 operator % (T r) const {
      return rem_scl(r);
    }

    /* x %= r, y %= r, z %= r */
    Vector3& operator %= (T r) {
      x = num::remainder(x, r);
      y = num::remainder(y, r);
      z = num::remainder(z, r);
      return *this;
    }


    /* x + y + z */
    template <typename U = T> U add_reduce () const {
      return ((U) x) + ((U) y) + ((U) z);
    }
    
    /* x - y - z */
    template <typename U = T> U sub_reduce () const {
      return ((U) x) - ((U) y) - ((U) z);
    }
    
    /* x * y * z */
    template <typename U = T> U mul_reduce () const {
      return ((U) x) * ((U) y) * ((U) z);
    }
    
    /* x / y / z */
    template <typename U = T> U div_reduce () const {
      return ((U) x) / ((U) y) / ((U) z);
    }
    
    /* x % y % z */
    template <typename U = T> U rem_reduce () const {
      return num::remainder(num::remainder(((U) x), ((U) y)), (U) z);
    }


    /* x == r.x && y == r.y && z == r.z */
    bool equal (Vector3 const& r) const {
      return x == r.x
          && y == r.y
          && z == r.z;
    }

    /* x == r.x && y == r.y && z == r.z */
    bool operator == (Vector3 const& r) const {
      return equal(r);
    }


    /* x != r.x || y != r.y || z != r.z */
    bool not_equal (Vector3 const& r) const {
      return x != r.x
          || y != r.y
          || z != r.z;
    }

    /* x != r.x || y != r.y || z != r.z */
    bool operator != (Vector3 const& r) const {
      return not_equal(r);
    }


    /* Take the absolute value of each component of a vector */
    Vector3 abs () {
      return {
        num::abs<T>(x),
        num::abs<T>(y),
        num::abs<T>(z)
      };
    }


    /* Clamp a vector between two other vectors */
    Vector3 clamp (Vector3 const& min, Vector3 const& max) const {
      return {
        num::clamp(x, min.x, max.x),
        num::clamp(y, min.y, max.y),
        num::clamp(z, min.z, max.z)
      };
    }
    
    /* Clamp each of a vector's components between two scalars */
    Vector3 clamp_scl (T min, T max) const {
      return {
        num::clamp(x, min, max),
        num::clamp(y, min, max),
        num::clamp(z, min, max)
      };
    }
    
    /* Linear interpolate to an offset between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3 lerp (U alpha, Vector3 const& finish) const {
      return {
        num::lerp<T, U>(alpha, x, finish.x),
        num::lerp<T, U>(alpha, y, finish.y),
        num::lerp<T, U>(alpha, z, finish.z)
      };
    }
    
    /* Take the floor of each component of a vector */
    Vector3 floor () const {
      return {
        ::floor(x),
        ::floor(y),
        ::floor(z)
      };
    }
    
    /* Round each component of a vector */
    Vector3 round () const {
      return {
        ::round(x),
        ::round(y),
        ::round(z)
      };
    }
    
    /* Take the ceil of each component of a vector */
    Vector3 ceil () const {
      return {
        ::ceil(x),
        ::ceil(y),
        ::ceil(z)
      };
    }

    
    /* Take the minimum of each component of two vectors */
    Vector3 min (Vector3 const& r) const {
      return {
        num::min(x, r.x),
        num::min(y, r.y),
        num::min(z, r.z)
      };
    }
    
    /* Take the maximum of each component of two vectors */
    Vector3 max (Vector3 const& r) const {
      return {
        num::max(x, r.x),
        num::max(y, r.y),
        num::max(z, r.z)
      };
    }

    
    /* Take the minimum of each component of a vector and a scalar value */
    Vector3 min_scl (T r) const {
      return {
        num::min(x, r),
        num::min(y, r),
        num::min(z, r)
      };
    }

    /* Take the maximum of each component of a vector and a scalar value */
    Vector3 max_scl (T r) const {
      return {
        num::max(x, r),
        num::max(y, r),
        num::max(z, r)
      };
    }

    
    /* Negate each component of a vector */
    Vector3 negate () const {
      return { -x, -y, -z };
    }
    
    /* Get the sign of each component of a vector */
    Vector3 sign () const {
      return {
        num::sign(x),
        num::sign(y),
        num::sign(z)
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
      return sqrt((U) length_sq());
    }
    
    /* Get the dot product of two vectors */
    T dot (Vector3 const& r) const {
      return mul(r).add_reduce();
    }
    
    /* Get the angle between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U angle (Vector3 const& r) const {
      U t = ((U) dot(r)) / sqrt(((U) length_sq()) * ((U) r.length_sq()));
      return acos(num::clamp<U>(t, U(-1), U(1)));
    }
    
    /* Get the square length/magnitude of the difference between two vectors */
    T distance_sq (Vector3 const& r) const {
      return sub(r).length_sq();
    }
    
    /* Get the length/magnitude of the difference between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U distance (Vector3 const& r) const {
      return sqrt((U) distance_sq(r));
    }


    /* Get the cross product of two vectors */
    Vector3 cross (Vector3 const& r) const {
      return {
        y * r.z - r.y * z,
        z * r.x - r.z * x,
        x * r.y - r.x * y
      };
    }

    /* Get the unit-scale version of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> normalize () const {
      U l = length<U>();
      if (l == U(0)) return { U(0), U(0), U(0) };
      else return ((Vector3<U>) *this) / l;
    }

    /* Get the unit-scale version of a vector and multiply it by the given length/magnitude */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> scale_to_length (U new_length) const {
      return normalize<U>() * new_length;
    }

    /* Limit the position of a vector to between a minimum and maximum distance from the origin */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> clamp_to_length (U min_length, U max_length) const {
      U l = length<U>();
      return (((Vector3<U>) *this) / l) * num::clamp<U>(l, min_length, max_length));
    }

    /* Get the proportion of the components of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> proportion () const {
      return (Vector3<U> *this) / num::max<U>(x, y, z);
    }

    /* Get the unit-scale direction vector from one vector to another */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> direction (Vector3 const& r) const {
      return sub(r).normalize<U>();
    }

    /* Project a vector onto another vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> project_on_vec (Vector3 const& r) const {
      return ((Vector3<U>) r) * (((Vector3<U>) r.dot(l)) / (U) r.length_sq());
    }

    /* Project a vector onto the normal of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> project_on_normal (Vector3 const& r) const {
      return ((Vector3<U>) *this) - project_on_vec<U>(r);
    }

    /* Reflect a vector across a normal */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector3<U> reflect (Vector3 const* r) const {
      return ((Vector3<U>) *this) - (((Vector3<U>) r) * U(2) * ((U) dot(r)));
    }


    /* Apply an euler rotation to a vector */
    Vector3<f32_t> apply_euler (Euler const& e) const {
      return apply_quaternion(Quaternion::from_euler(e));
    }

    /* Apply an angular rotation around an axis to a vector */
    Vector3<f32_t> apply_axis_angle (Vector3<f32_t> const& axis, f32_t angle) const {
      return apply_quaternion(Quaternion::from_axis_angle(axis, angle));
    }

    /* Apply a quaternion rotation to a vector */
    Vector3<f32_t> apply_quaternion (Quaternion const& q) const {
      f32_t ix =  q.w * ((f32_t) x) + q.y * ((f32_t) z) - q.z * ((f32_t) y);
      f32_t iy =  q.w * ((f32_t) y) + q.z * ((f32_t) x) - q.x * ((f32_t) z);
      f32_t iz =  q.w * ((f32_t) z) + q.x * ((f32_t) y) - q.y * ((f32_t) x);
      f32_t iw = -q.x * ((f32_t) x) - q.y * ((f32_t) y) - q.z * ((f32_t) z);

      return {
        ix * q.w + iw * -q.x + iy * -q.z - iz * -q.y,
        iy * q.w + iw * -q.y + iz * -q.x - ix * -q.z,
        iz * q.w + iw * -q.z + ix * -q.y - iy * -q.x
      };
    }


    /* Transform a vector using a matrix3 */
    Vector3<f32_t> apply_matrix3 (Matrix3 const& m) const {
      return {
        m[0] * ((f32_t) x) + m[3] * ((f32_t) y) + m[6] * ((f32_t) z),
        m[1] * ((f32_t) x) + m[4] * ((f32_t) y) + m[7] * ((f32_t) z),
        m[2] * ((f32_t) x) + m[5] * ((f32_t) y) + m[8] * ((f32_t) z)
      };
    }

    /* Transform a vector using a matrix4 */
    Vector3<f32_t> apply_matrix4 (Matrix4 const& m) const {
      return {
        m[0] * ((f32_t) x) + m[4] * ((f32_t) y) + m[8]  * ((f32_t) z) + m[12],
        m[1] * ((f32_t) x) + m[5] * ((f32_t) y) + m[9]  * ((f32_t) z) + m[13],
        m[2] * ((f32_t) x) + m[6] * ((f32_t) y) + m[10] * ((f32_t) z) + m[14]
      };
    }


    /* Rotate a direction vector using a matrix4 */
    Vector3<f32_t> transform_direction (Matrix4 const& m) const {
      return (Vector3<f32_t> {
        m[0] * ((f32_t) x) + m[4] * ((f32_t) y) + m[8]  * ((f32_t) z),
        m[1] * ((f32_t) x) + m[5] * ((f32_t) y) + m[9]  * ((f32_t) z),
        m[2] * ((f32_t) x) + m[6] * ((f32_t) y) + m[10] * ((f32_t) z)
      }).normalize();
    }
  };
  

  using Vector3f  = Vector3<f32_t>;
  using Vector3d  = Vector3<f64_t>;
  using Vector3ld = Vector3<f128_t>;
  using Vector3s  = Vector3<s32_t>;
  using Vector3ls = Vector3<s64_t>;
  using Vector3u  = Vector3<u32_t>;
  using Vector3lu = Vector3<u64_t>;


  namespace Constants {
    namespace Vector3f {
      static constexpr ::mod::Vector3f zero = { 0.0f, 0.0f, 0.0f };
      static constexpr ::mod::Vector3f unit = { 1.0f, 1.0f, 1.0f };
      static constexpr ::mod::Vector3f inverse_unit = { -1.0f, -1.0f, -1.0f };
      static constexpr ::mod::Vector3f up = { 0.0f, 0.0f, 1.0f };
      static constexpr ::mod::Vector3f down = { 0.0f, 0.0f, -1.0f };
      static constexpr ::mod::Vector3f minimum = { std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min() };
      static constexpr ::mod::Vector3f maximum = { std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max() };
    }

    namespace Vector3d {
      static constexpr ::mod::Vector3d zero = { 0.0, 0.0, 0.0 };
      static constexpr ::mod::Vector3d unit = { 1.0, 1.0, 1.0 };
      static constexpr ::mod::Vector3d inverse_unit = { -1.0, -1.0, -1.0 };
      static constexpr ::mod::Vector3d up = { 0.0, 0.0, 1.0 };
      static constexpr ::mod::Vector3d down = { 0.0, 0.0, -1.0 };
      static constexpr ::mod::Vector3d minimum = { std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min() };
      static constexpr ::mod::Vector3d maximum = { std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max() };
    }

    namespace Vector3s {
      static constexpr ::mod::Vector3s zero = { 0, 0, 0 };
      static constexpr ::mod::Vector3s unit = { 1, 1, 1 };
      static constexpr ::mod::Vector3s inverse_unit = { -1, -1, -1 };
      static constexpr ::mod::Vector3s up = { 0, 0, 1 };
      static constexpr ::mod::Vector3s down = { 0, 0, -1 };
      static constexpr ::mod::Vector3s minimum = { std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min() };
      static constexpr ::mod::Vector3s maximum = { std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max() };
    }

    namespace Vector3ls {
      static constexpr ::mod::Vector3ls zero = { 0, 0, 0 };
      static constexpr ::mod::Vector3ls unit = { 1, 1, 1 };
      static constexpr ::mod::Vector3ls inverse_unit = { -1, -1, -1 };
      static constexpr ::mod::Vector3ls up = { 0, 0, 1 };
      static constexpr ::mod::Vector3ls down = { 0, 0, -1 };
      static constexpr ::mod::Vector3ls minimum = { std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min() };
      static constexpr ::mod::Vector3ls maximum = { std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max() };
    }

    namespace Vector3u {
      static constexpr ::mod::Vector3u zero = { 0, 0, 0 };
      static constexpr ::mod::Vector3u unit = { 1, 1, 1 };
      static constexpr ::mod::Vector3u up = { 0, 0, 1 };
      static constexpr ::mod::Vector3u minimum = { std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min() };
      static constexpr ::mod::Vector3u maximum = { std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max() };
    }

    namespace Vector3lu {
      static constexpr ::mod::Vector3lu zero = { 0, 0, 0 };
      static constexpr ::mod::Vector3lu unit = { 1, 1, 1 };
      static constexpr ::mod::Vector3lu up = { 0, 0, 1 };
      static constexpr ::mod::Vector3lu minimum = { std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min() };
      static constexpr ::mod::Vector3lu maximum = { std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max() };
    }
  }
}