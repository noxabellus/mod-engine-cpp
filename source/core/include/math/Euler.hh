#ifndef EULER_H
#define EULER_H

#include "Vector3.hh"

#include "../util.hh"


namespace mod {
  struct Quaternion;


  namespace EulerOrder {
    enum: u8_t {
      XYZ,
      XZY,

      YXZ,
      YZX,

      ZXY,
      ZYX,

      total_order_count,

      Invalid = (u8_t) -1,
    };

    static constexpr char const* names [total_order_count] = {
      "XYZ",
      "XZY",

      "YXZ",
      "YZX",

      "ZXY",
      "ZYX"
    };

    /* Get the name of a EulerOrder as a str */
    static constexpr char const* name (u8_t order) {
      if (order < total_order_count) return names[order];
      else return "Invalid";
    }

    /* Get a EulerOrder from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t order = 0; order < total_order_count; order ++) {
        if ((max_length == SIZE_MAX || strlen(names[order]) == max_length) && str_cmp_caseless(name, names[order], max_length) == 0) return order;
      }

      return Invalid;
    }

    /* Determine if a value is a valid EulerOrder */
    static constexpr bool validate (u8_t order) {
      return order < total_order_count;
    }
  };



  struct Euler {
    Vector3f angles;
    u8_t order;


    /* Create a zero-initialized Euler rotation with optional order (Defaults to XYZ) */
    constexpr Euler (u8_t in_order = EulerOrder::XYZ)
    : angles(0, 0, 0)
    , order(in_order)
    { }

    /* Create a new Euler rotation with an optional order (Defaults to XYZ) */
    constexpr Euler (f32_t x, f32_t y, f32_t z, u8_t in_order = EulerOrder::XYZ)
    : angles(x, y, z)
    , order(in_order)
    { }

    /* Create a new Euler rotation from a Vector3 with optional order (Defaults to XYZ) */
    template <typename T> constexpr Euler (Vector3<T> const& in_angles, u8_t in_order = EulerOrder::XYZ)
    : angles(in_angles)
    , order(in_order)
    { }

    /* Create a new Euler rotation from a Quaternion and an optional order (Defaults to XYZ) */
    ENGINE_API static Euler from_quaternion (Quaternion const& q, u8_t order = EulerOrder::XYZ);


    /* Create a new Euler from another Euler with a different order.
     * Note: Discards revolutions */
    ENGINE_API Euler reorder (Euler const& e, u8_t order) const;


    /* Get an element of an Euler's angles by index.
     * For efficiency, the index is not bounds checked */
    f32_t& operator [] (size_t index) const {
      return angles[index];
    }


    /* Determine if two Euler rotations are identical */
    bool equal (Euler const& r) const {
      return order  == r.order
          && angles == r.angles;
    }

    /* Determine if two Euler rotations are identical */
    bool operator == (Euler const& r) const {
      return equal(r);
    }

    /* Determine if two Euler rotations are not identical */
    bool not_equal (Euler const& r) const {
      return order  != r.order
          || angles != r.angles;
    }

    /* Determine if two Euler rotations are not identical */
    bool operator != (Euler const& r) const {
      return not_equal(r);
    }
  };


  namespace Constants {
    namespace Euler {
      static constexpr ::mod::Euler zero = { 0, 0, 0, EulerOrder::XYZ };
    }
  }
}

#endif