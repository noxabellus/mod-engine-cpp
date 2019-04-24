#ifndef VECTOR4_H
#define VECTOR4_H

#include "../cstd.hh"



namespace mod {
  struct Matrix4;

  template <typename T> struct Vector4 {
    union {
      struct {
        T x;
        T y;
        T z;
        T w;
      };
      T elements [4];
    };



    /* Create a new zero-initialized vector */
    constexpr Vector4 ()
    : x(0)
    , y(0)
    , z(0)
    , w(0)
    { }

    /* Create a new vector and initialize its members */
    constexpr Vector4 (T in_x, T in_y, T in_z, T in_w)
    : x(in_x)
    , y(in_y)
    , z(in_z)
    , w(in_w)
    { }

    /* Create a new vector by distributing a scalar value to all members */
    constexpr Vector4 (T scalar)
    : x(scalar)
    , y(scalar)
    , z(scalar)
    , w(scalar)
    { }

    

    /* Create a representation of an angular rotation around an axis from a quaternion */
    static Vector4<f32_t> axis_angle_from_quaternion (Quaternion const& q) {
      Vector4<f32_t> out = {
        .w = 2.0f * cosf(q.w)
      };

      f32_t s = sqrtf(1.0f - q.w * q.w);

      if (s < 0.0001f) {
        out.x = 1.0f;
        out.y = 0.0f;
        out.z = 0.0f;
      } else {
        out.x = q.x / s;
        out.y = q.y / s;
        out.z = q.z / s;
      }

      return out;
    }


    /* Create a representation of an angular rotation around an axis from a rotational matrix4 */
    static Vector4<f32_t> axis_angle_from_rotation_matrix (Matrix4 const& m) {
      static constexpr f32_t AA_EPS00 = 0.001f;
      static constexpr f32_t AA_EPS01 = 0.01f;
      static constexpr f32_t AA_EPS02 = 0.1f;

      Vector4f out = { 1.0f, 0.0f, 0.0f, 0.0f };

      if ((num::abs(m[4] - m[1]) < AA_EPS01)
      &&  (num::abs(m[8] - m[2]) < AA_EPS01)
      &&  (num::abs(m[9] - m[6]) < AA_EPS01)) {
        if ((num::abs(m[4] + m[1]) < AA_EPS02) 
        &&  (num::abs(m[8] + m[2]) < AA_EPS02) 
        &&  (num::abs(m[9] + m[6]) < AA_EPS02) 
        &&  (num::abs(m[0] + m[5] + m[10] - 3.0f) < AA_EPS02)) {
          return out;
        }

        out.w = M_PI;

        f32_t xx = (m[0] + 1.0f) / 2.0f;
        f32_t yy = (m[5] + 1.0f) / 2.0f;
        f32_t zz = (m[10] + 1.0f) / 2.0f;
        f32_t xy = (m[4] + m[1]) / 4.0f;
        f32_t xz = (m[8] + m[2]) / 4.0f;
        f32_t yz = (m[9] + m[6]) / 4.0f;

        if ((xx > yy) && (xx > zz)) {
          if (xx < AA_EPS01) {
            out.x = 0.0f;
            out.y = 0.707106781f;
            out.z = 0.707106781f;
          } else {
            out.x = sqrtf(xx);
            out.y = xy / out.x;
            out.z = xz / out.x;
          }
        } else if (yy > zz) {
          if (yy < AA_EPS01) {
            out.x = 0.707106781f;
            out.y = 0.0f;
            out.z = 0.707106781f;
          } else {
            out.y = sqrtf(yy);
            out.x = xy / out.y;
            out.z = yz / out.y;
          }
        } else {
          if (zz < AA_EPS01) {
            out.x = 0.707106781f;
            out.y = 0.707106781f;
            out.z = 0.0f;
          } else {
            out.z = sqrtf(zz);
            out.x = xz / out.z;
            out.y = yz / out.z;
          }
        }

        return out;
      }

      f32_t s = sqrtf((m[6] - m[9]) * (m[6] - m[9])
                    + (m[8] - m[2]) * (m[8] - m[2])
                    + (m[1] - m[4]) * (m[1] - m[4]));

      if (num::abs(s) < AA_EPS00) s = 1.0f;

      out.x = (m[6] - m[9]) / s;
      out.y = (m[8] - m[2]) / s;
      out.z = (m[1] - m[4]) / s;
      out.w = acosf((m[0] + m[5] + m[10] - 1.0f) / 2.0f);

      return out;
    }


    /* Convert a vector to another type of vector */
    template <typename U> constexpr operator Vector4<U> () const {
      return { (U) x, (U) y, (U) z, (U) w };
    }


    /* Get an element of a vector by index.
     * For efficiency, the index is not bounds checked */
    T& operator [] (size_t index) const {
      return (T&) elements[index];
    }


    /* x + r.x, y + r.y, z + r.z, w + r.w */
    Vector4 add (Vector4 const& r) const {
      return {
        x + r.x,
        y + r.y,
        z + r.z,
        w + r.w
      };
    }

    /* x + r.x, y + r.y, z + r.z, w + r.w */
    Vector4 operator + (Vector4 const& r) const {
      return add(r);
    }

    /* x += r.x, y += r.y, z += r.z, w += r.w */
    Vector4& operator += (Vector4 const& r) {
      x += r.x;
      y += r.y;
      z += r.z;
      w += r.w;
      return *this;
    }


    /* x - r.x, y - r.y, z - r.z, w - r.w */
    Vector4 sub (Vector4 const& r) const {
      return {
        x - r.x,
        y - r.y,
        z - r.z,
        w - r.w
      };
    }

    /* x - r.x, y - r.y, z - r.z, w - r.w */
    Vector4 operator - (Vector4 const& r) const {
      return sub(r);
    }

    /* x -= r.x, y -= r.y, z -= r.z, w -= r.w */
    Vector4& operator -= (Vector4 const& r) {
      x -= r.x;
      y -= r.y;
      z -= r.z;
      w -= r.w;
      return *this;
    }


    /* x * r.x, y * r.y, z * r.z, w * r.w */
    Vector4 mul (Vector4 const& r) const {
      return {
        x * r.x,
        y * r.y,
        z * r.z,
        w * r.w
      };
    }

    /* x * r.x, y * r.y, z * r.z, w * r.w */
    Vector4 operator * (Vector4 const& r) const {
      return mul(r);
    }

    /* x *= r.x, y *= r.y, z *= r.z, w *= r.w */
    Vector4& operator *= (Vector4 const& r) {
      x *= r.x;
      y *= r.y;
      z *= r.z;
      w *= r.w;
      return *this;
    }

    /* x / r.x, y / r.y, z / r.z, w / r.w */
    Vector4 div (Vector4 const& r) const {
      return {
        x / r.x,
        y / r.y,
        z / r.z,
        w / r.w
      };
    }

    /* x / r.x, y / r.y, z / r.z, w / r.w */
    Vector4 operator / (Vector4 const& r) const {
      return div(r);
    }

    /* x /= r.x, y /= r.y, z /= r.z, w /= r.w */
    Vector4& operator /= (Vector4 const& r) {
      x /= r.x;
      y /= r.y;
      z /= r.z;
      w /= r.w;
      return *this;
    }

    /* x % r.x, y % r.y, z % r.z, w % r.w */
    Vector4 rem (Vector4 const& r) const {
      return {
        num::remainder(x, r.x),
        num::remainder(y, r.y),
        num::remainder(z, r.z),
        num::remainder(w, r.w)
      };
    }

    /* x % r.x, y % r.y, z % r.z, w % r.w */
    Vector4 operator % (Vector4 const& r) const {
      return rem(r);
    }
    
    /* x %= r.x, y %= r.y, z %= r.z, w %= r.w */
    Vector4& operator %= (Vector4 const& r) {
      x = num::remainder(x, r.x);
      y = num::remainder(y, r.y);
      z = num::remainder(z, r.z);
      w = num::remainder(w, r.w);
      return *this;
    }


    /* x + r, y + r, z + r, w + r */
    Vector4 add_scl (T r) const {
      return {
        x + r,
        y + r,
        z + r,
        w + r
      };
    }

    /* x + r, y + r, z + r, w + r */
    Vector4 operator + (T r) const {
      return add_scl(r);
    }
    
    /* x += r, y += r, z += r, w += r */
    Vector4& operator += (T r) {
      x += r;
      y += r;
      z += r;
      w += r;
      return *this;
    }


    /* x - r, y - r, z - r, w - r */
    Vector4 sub_scl (T r) const {
      return {
        x - r,
        y - r,
        z - r,
        w - r
      };
    }

    /* x - r, y - r, z - r, w - r */
    Vector4 operator - (T r) const {
      return sub_scl(r);
    }
    
    /* x -= r, y -= r, z -= r, w -= r */
    Vector4& operator -= (T r) {
      x -= r;
      y -= r;
      z -= r;
      w -= r;
      return *this;
    }


    /* x * r, y * r, z * r, w * r */
    Vector4 mul_scl (T r) const {
      return {
        x * r,
        y * r,
        z * r,
        w * r
      };
    }

    /* x * r, y * r, z * r, w * r */
    Vector4 operator * (T r) const {
      return mul_scl(r);
    }
    
    /* x *= r, y *= r, z *= r, w *= r */
    Vector4& operator *= (T r) {
      x *= r;
      y *= r;
      z *= r;
      w *= r;
      return *this;
    }


    /* x / r, y / r, z / r, w / r */
    Vector4 div_scl (T r) const {
      return {
        x / r,
        y / r,
        z / r,
        w / r
      };
    }

    /* x / r, y / r, z / r, w / r */
    Vector4 operator / (T r) const {
      return div_scl(r);
    }
    
    /* x /= r, y /= r, z /= r, w /= r */
    Vector4& operator /= (T r) {
      x /= r;
      y /= r;
      z /= r;
      w /= r;
      return *this;
    }


    /* x % r, y % r, z % r, w % r */
    Vector4 rem_scl (T r) const {
      return {
        num::remainder(x, r),
        num::remainder(y, r),
        num::remainder(z, r),
        num::remainder(w, r)
      };
    }

    /* x % r, y % r, z % r, w % r */
    Vector4 operator % (T r) const {
      return rem_scl(r);
    }
    
    /* x %= r, y %= r, z %= r, w %= r */
    Vector4& operator %= (T r) {
      x = num::remainder(x, r);
      y = num::remainder(y, r);
      z = num::remainder(z, r);
      w = num::remainder(w, r);
      return *this;
    }


    /* x + y + z + w */
    template <typename U = T> U add_reduce () const {
      return ((U) x) + ((U) y) + ((U) z) + ((U) w);
    }
    
    /* x - y - z - w */
    template <typename U = T> U sub_reduce () const {
      return ((U) x) - ((U) y) - ((U) z) - ((U) w);
    }
    
    /* x * y * z * w */
    template <typename U = T> U mul_reduce () const {
      return ((U) x) * ((U) y) * ((U) z) * ((U) w);
    }
    
    /* x / y / z / w */
    template <typename U = T> U div_reduce () const {
      return ((U) x) / ((U) y) / ((U) z) / ((U) w);
    }
    
    /* x % y % z % w */
    template <typename U = T> U rem_reduce () const {
      return num::remainder(num::remainder(num::remainder(((U) x), ((U) y)), (U) z), (U) w);
    }


    /* x == r.x && y == r.y && z == r.z  && w == r.w */
    bool equal (Vector4 const& r) const {
      return x == r.x
          && y == r.y
          && z == r.z
          && w == r.w;
    }

    /* x == r.x && y == r.y && z == r.z  && w == r.w */
    bool operator == (Vector4 const& r) const {
      return equal(r);
    }


    /* x != r.x || y != r.y || z != r.z  || w != r.w */
    bool not_equal (Vector4 const& r) const {
      return x != r.x
          || y != r.y
          || z != r.z
          || w != r.w;
    }

    /* x != r.x || y != r.y || z != r.z  || w != r.w */
    bool operator != (Vector4 const& r) const {
      return not_equal(r);
    }


    /* Take the absolute value of each component of a vector */
    Vector4 abs () {
      return {
        num::abs<T>(x),
        num::abs<T>(y),
        num::abs<T>(z),
        num::abs<T>(w)
      };
    }


    /* Clamp a vector between two other vectors */
    Vector4 clamp (Vector4 const& min, Vector4 const& max) const {
      return {
        num::clamp(x, min.x, max.x),
        num::clamp(y, min.y, max.y),
        num::clamp(z, min.z, max.z),
        num::clamp(w, min.w, max.w)
      };
    }
    
    /* Clamp each of a vector's components between two scalars */
    Vector4 clamp_scl (T min, T max) const {
      return {
        num::clamp(x, min, max),
        num::clamp(y, min, max),
        num::clamp(z, min, max),
        num::clamp(w, min, max)
      };
    }
    
    /* Linear interpolate to an offset between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4 lerp (U alpha, Vector4 const& finish) const {
      return {
        num::lerp<T, U>(alpha, x, finish.x),
        num::lerp<T, U>(alpha, y, finish.y),
        num::lerp<T, U>(alpha, z, finish.z),
        num::lerp<T, U>(alpha, w, finish.w)
      };
    }
    
    /* Take the floor of each component of a vector */
    Vector4 floor () const {
      return {
        ::floor(x),
        ::floor(y),
        ::floor(z),
        ::floor(w)
      };
    }
    
    /* Round each component of a vector */
    Vector4 round () const {
      return {
        ::round(x),
        ::round(y),
        ::round(z),
        ::round(w)
      };
    }
    
    /* Take the ceil of each component of a vector */
    Vector4 ceil () const {
      return {
        ::ceil(x),
        ::ceil(y),
        ::ceil(z),
        ::ceil(w)
      };
    }

    
    /* Take the minimum of each component of two vectors */
    Vector4 min (Vector4 const& r) const {
      return {
        num::min(x, r.x),
        num::min(y, r.y),
        num::min(z, r.z),
        num::min(w, r.w)
      };
    }
    
    /* Take the maximum of each component of two vectors */
    Vector4 max (Vector4 const& r) const {
      return {
        num::max(x, r.x),
        num::max(y, r.y),
        num::max(z, r.z),
        num::max(w, r.w)
      };
    }

    
    /* Take the minimum of each component of a vector and a scalar value */
    Vector4 min_scl (T r) const {
      return {
        num::min(x, r),
        num::min(y, r),
        num::min(z, r),
        num::min(w, r)
      };
    }

    /* Take the maximum of each component of a vector and a scalar value */
    Vector4 max_scl (T r) const {
      return {
        num::max(x, r),
        num::max(y, r),
        num::max(z, r),
        num::max(w, r)
      };
    }


    /* Take the minimum of all elements of a vector */
    T min_element () const {
      return num::min(x, y, z, w);
    }

    /* Take the maximum of all elements of a vector */
    T max_element () const {
      return num::max(x, y, z, w);
    }

    
    /* Negate each component of a vector */
    Vector4 negate () const {
      return { -x, -y, -z, -w };
    }

    /* Negate each component of a vector */
    Vector4 operator - () const {
      return negate();
    }
    
    /* Get the sign of each component of a vector */
    Vector4 sign () const {
      return {
        num::sign(x),
        num::sign(y),
        num::sign(z),
        num::sign(w)
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
    T dot (Vector4 const& r) const {
      return mul(r).add_reduce();
    }
    
    /* Get the angle between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U angle (Vector4 const& r) const {
      U t = ((U) dot(r)) / sqrt(((U) length_sq()) * ((U) r.length_sq()));
      return acos(num::clamp<U>(t, U(-1), U(1)));
    }
    
    /* Get the square length/magnitude of the difference between two vectors */
    T distance_sq (Vector4 const& r) const {
      return sub(r).length_sq();
    }
    
    /* Get the length/magnitude of the difference between two vectors */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> U distance (Vector4 const& r) const {
      return sqrt((U) distance_sq(r));
    }


    /* Get the unit-scale version of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> normalize () const {
      U l = length<U>();
      if (l == U(0)) return { U(0), U(0), U(0), U(0) };
      else return ((Vector4<U>) *this) / l;
    }

    /* Get the unit-scale version of a vector and multiply it by the given length/magnitude */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> scale_to_length (U new_length) const {
      return normalize<U>() * new_length;
    }

    /* Limit the position of a vector to between a minimum and maximum distance from the origin */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> clamp_to_length (U min_length, U max_length) const {
      U l = length<U>();
      return (((Vector4<U>) *this) / l) * num::clamp<U>(l, min_length, max_length));
    }

    /* Get the proportion of the components of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> proportion () const {
      return (Vector4<U> *this) / num::max<U>(x, y, z);
    }

    /* Get the unit-scale direction vector from one vector to another */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> direction (Vector4 const& r) const {
      return r.sub(*this).normalize<U>();
    }

    /* Project a vector onto another vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> project_on_vec (Vector4 const& r) const {
      return ((Vector4<U>) r) * (((Vector4<U>) r.dot(l)) / (U) r.length_sq());
    }

    /* Project a vector onto the normal of a vector */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> project_on_normal (Vector4 const& r) const {
      return ((Vector4<U>) *this) - project_on_vec<U>(r);
    }

    /* Reflect a vector across a normal */
    template <typename U = typename std::conditional<std::is_floating_point<T>::value, T, f64_t>::type> Vector4<U> reflect (Vector4 const* r) const {
      return ((Vector4<U>) *this) - (((Vector4<U>) r) * U(2) * ((U) dot(r)));
    }


    /* Transform a vector using a matrix4 */
    Vector4<f32_t> apply_matrix (Matrix4 const& m) const {
      return {
        m[0] * ((f32_t) x) + m[4] * ((f32_t) y) + m[8]  * ((f32_t) z) + m[12] * ((f32_t) w),
        m[1] * ((f32_t) x) + m[5] * ((f32_t) y) + m[9]  * ((f32_t) z) + m[13] * ((f32_t) w),
        m[2] * ((f32_t) x) + m[6] * ((f32_t) y) + m[10] * ((f32_t) z) + m[14] * ((f32_t) w),
        m[3] * ((f32_t) x) + m[7] * ((f32_t) y) + m[11] * ((f32_t) z) + m[15] * ((f32_t) w)
      };
    }

    /* Transform a vector using a matrix3 */
    Vector3<f32_t> apply_matrix (Matrix3 const& m, Vector3f const& axis = Constants::Vector3f::up) const {
      return apply_matrix(Matrix4::from_3(m, axis));
    }
  };


  using Vector4f  = Vector4<f32_t>;
  using Vector4d  = Vector4<f64_t>;
  using Vector4ld = Vector4<f128_t>;
  using Vector4s  = Vector4<s32_t>;
  using Vector4ls = Vector4<s64_t>;
  using Vector4u  = Vector4<u32_t>;
  using Vector4lu = Vector4<u64_t>;


  namespace Constants {
    namespace Vector4f {
      static constexpr ::mod::Vector4f zero = { 0.0f, 0.0f, 0.0f, 0.0f };
      static constexpr ::mod::Vector4f unit = { 1.0f, 1.0f, 1.0f, 1.0f };
      static constexpr ::mod::Vector4f inverse_unit = { -1.0f, -1.0f, -1.0f, -1.0f };
      static constexpr ::mod::Vector4f minimum = { std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min(), std::numeric_limits<f32_t>::min() };
      static constexpr ::mod::Vector4f maximum = { std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max(), std::numeric_limits<f32_t>::max() };
    }

    namespace Vector4d {
      static constexpr ::mod::Vector4d zero = { 0.0, 0.0, 0.0, 0.0 };
      static constexpr ::mod::Vector4d unit = { 1.0, 1.0, 1.0, 1.0 };
      static constexpr ::mod::Vector4d inverse_unit = { -1.0, -1.0, -1.0, -1.0 };
      static constexpr ::mod::Vector4d minimum = { std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min(), std::numeric_limits<f64_t>::min() };
      static constexpr ::mod::Vector4d maximum = { std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max(), std::numeric_limits<f64_t>::max() };
    }

    namespace Vector4ld {
      static constexpr ::mod::Vector4d zero = { 0.0, 0.0, 0.0, 0.0 };
      static constexpr ::mod::Vector4d unit = { 1.0, 1.0, 1.0, 1.0 };
      static constexpr ::mod::Vector4d inverse_unit = { -1.0, -1.0, -1.0, -1.0 };
      static constexpr ::mod::Vector4d minimum = { std::numeric_limits<f128_t>::min(), std::numeric_limits<f128_t>::min(), std::numeric_limits<f128_t>::min(), std::numeric_limits<f128_t>::min() };
      static constexpr ::mod::Vector4d maximum = { std::numeric_limits<f128_t>::max(), std::numeric_limits<f128_t>::max(), std::numeric_limits<f128_t>::max(), std::numeric_limits<f128_t>::max() };
    }

    namespace Vector4s {
      static constexpr ::mod::Vector4s zero = { 0, 0, 0, 0 };
      static constexpr ::mod::Vector4s unit = { 1, 1, 1, 1 };
      static constexpr ::mod::Vector4s inverse_unit = { -1, -1, -1, -1 };
      static constexpr ::mod::Vector4s minimum = { std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min(), std::numeric_limits<s32_t>::min() };
      static constexpr ::mod::Vector4s maximum = { std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max(), std::numeric_limits<s32_t>::max() };
    }

    namespace Vector4ls {
      static constexpr ::mod::Vector4ls zero = { 0, 0, 0, 0 };
      static constexpr ::mod::Vector4ls unit = { 1, 1, 1, 1 };
      static constexpr ::mod::Vector4ls inverse_unit = { -1, -1, -1, -1 };
      static constexpr ::mod::Vector4ls minimum = { std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min(), std::numeric_limits<s64_t>::min() };
      static constexpr ::mod::Vector4ls maximum = { std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max(), std::numeric_limits<s64_t>::max() };
    }

    namespace Vector4u {
      static constexpr ::mod::Vector4u zero = { 0, 0, 0, 0 };
      static constexpr ::mod::Vector4u unit = { 1, 1, 1, 1 };
      static constexpr ::mod::Vector4u minimum = { std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min(), std::numeric_limits<u32_t>::min() };
      static constexpr ::mod::Vector4u maximum = { std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max(), std::numeric_limits<u32_t>::max() };
    }

    namespace Vector4lu {
      static constexpr ::mod::Vector4lu zero = { 0, 0, 0, 0 };
      static constexpr ::mod::Vector4lu unit = { 1, 1, 1, 1 };
      static constexpr ::mod::Vector4lu minimum = { std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min(), std::numeric_limits<u64_t>::min() };
      static constexpr ::mod::Vector4lu maximum = { std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max(), std::numeric_limits<u64_t>::max() };
    }
  }
}

#endif