#include "../../include/math/lib.hh"


namespace mod {
  Euler Euler::from_quaternion (Quaternion const& q, u8_t order) {
    Matrix4 m = Matrix4::from_quaternion(q);

    return m.get_rotation_euler(order);
  }

  Euler Euler::reorder (Euler const& e, u8_t order) {
    if (order == e.order) return e;

    Quaternion q = Quaternion::from_euler(e);

    return Euler::from_quaternion(q, order);
  }
}