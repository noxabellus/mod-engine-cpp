#pragma once

#include "../cstd.hh"
#include "Vector3.hh"
#include "Euler.hh"
#include "Quaternion.hh"

namespace mod {
  struct Transform2D;
  struct Matrix3;


  struct Transform3D {
    Vector3f position;
    Quaternion rotation;
    Vector3f scale;

    

    /* Create a new identity-initialized Transform3D */
    constexpr Transform3D ()
    : position(0.0f, 0.0f, 0.0f)
    , rotation(0.0f)
    , scale(1.0f, 1.0f, 1.0f)
    { }

    /* Create a new Transform3D and initialize its components */
    constexpr Transform3D (Vector3f const& in_position, Quaternion in_rotation, Vector3f const& in_scale)
    : position(in_position)
    , rotation(in_rotation)
    , scale(in_scale)
    { }

    /* Create a new Transform3D from a tri_t */
    constexpr Transform3D (tri_t<Vector3f, Quaternion, Vector3f> const& tri)
    : position(tri.a)
    , rotation(tri.b)
    , scale(tri.c)
    { }


    /* Create a tri_t from a Transform3D */
    constexpr operator tri_t<Vector3f, Quaternion, Vector3f> () const {
      return { position, rotation, scale };
    }
  };


  struct Matrix4 {
    union {
      struct {
        f32_t e0;  f32_t e1;  f32_t e2;  f32_t e3;
        f32_t e4;  f32_t e5;  f32_t e6;  f32_t e7;
        f32_t e8;  f32_t e9;  f32_t e10; f32_t e11;
        f32_t e12; f32_t e13; f32_t e14; f32_t e15;
      };
      f32_t elements [16];
    };


    /* Create a new identity-initialized Matrix4 */
    constexpr Matrix4 ()
    : e0(1.0f),  e1(0.0f),  e2(0.0f),  e3(0.0f)
    , e4(0.0f),  e5(1.0f),  e6(0.0f),  e7(0.0f)
    , e8(0.0f),  e9(0.0f),  e10(1.0f), e11(0.0f)
    , e12(0.0f), e13(0.0f), e14(0.0f), e15(1.0f)
    { }

    /* Create a new Matrix4 from column-major elements */
    constexpr Matrix4 (
      f32_t in_e0,  f32_t in_e1,  f32_t in_e2,  f32_t in_e3,
      f32_t in_e4,  f32_t in_e5,  f32_t in_e6,  f32_t in_e7,
      f32_t in_e8,  f32_t in_e9,  f32_t in_e10, f32_t in_e11,
      f32_t in_e12, f32_t in_e13, f32_t in_e14, f32_t in_e15
    )
    : e0(in_e0),   e1(in_e1),   e2(in_e2),   e3(in_e3)
    , e4(in_e4),   e5(in_e5),   e6(in_e6),   e7(in_e7)
    , e8(in_e8),   e9(in_e9),   e10(in_e10), e11(in_e11)
    , e12(in_e12), e13(in_e13), e14(in_e14), e15(in_e15)
    { }

    /* Create a new Matrix4 from column-major elements.
     * Note: This is the same order as the data strucutre/constructor, but is here for consistency */
    static constexpr Matrix4 column_major (
      f32_t e0,  f32_t e1,  f32_t e2,  f32_t e3,
      f32_t e4,  f32_t e5,  f32_t e6,  f32_t e7,
      f32_t e8,  f32_t e9,  f32_t e10, f32_t e11,
      f32_t e12, f32_t e13, f32_t e14, f32_t e15
    ) {
      return {
        e0,  e1,  e2,  e3,
        e4,  e5,  e6,  e7,
        e8,  e9,  e10, e11,
        e12, e13, e14, e15
      };
    }

    /* Create a new Matrix4 from row-major elements.
     * Note: This is not the same order as the data structure/constructor, it is here for convenience */
    static constexpr Matrix4 row_major (
      f32_t e0, f32_t e4, f32_t e8,  f32_t e12,
      f32_t e1, f32_t e5, f32_t e9,  f32_t e13,
      f32_t e2, f32_t e6, f32_t e10, f32_t e14,
      f32_t e3, f32_t e7, f32_t e11, f32_t e15
    ) {
      return {
        e0,  e1,  e2,  e3,
        e4,  e5,  e6,  e7,
        e8,  e9,  e10, e11,
        e12, e13, e14, e15
      };
    }


    /* Create a new Matrix4 from a Matrix3 and an aribtrary axis vector */
    ENGINE_API static Matrix4 from_3 (Matrix3 const& m, Vector3f const& axis);


    /* Create a new Matrix4 from column vectors */
    static Matrix4 from_column_vectors (Vector3f const& a, Vector3f const& b, Vector3f const& c) {
      return {
        a.x,  a.y,  a.z,  0.0f,
        b.x,  b.y,  b.z,  0.0f,
        c.x,  c.y,  c.z,  0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }


    /* Create a new Matrix4 from perspective components */
    ENGINE_API static Matrix4 from_perspective (f32_t l, f32_t r, f32_t t, f32_t b, f32_t n, f32_t f);
    
    /* Create a new Matrix4 from orthographic components */
    ENGINE_API static Matrix4 from_orthographic (f32_t l, f32_t r, f32_t t, f32_t b, f32_t n, f32_t f);

    /* Create a new Matrix4 from look components */
    ENGINE_API static Matrix4 from_look (Vector3f const& position, Vector3f const& target, Vector3f const& up, bool set_position);


    /* Create a new Matrix4 from an Euler rotation */
    ENGINE_API static Matrix4 from_euler (Euler const& e);

    /* Create a new Matrix4 from a Quaternion rotation */
    ENGINE_API static Matrix4 from_quaternion (Quaternion const& q);

    /* Create a new Matrix4 from an angular rotation around the x axis */
    static Matrix4 from_angle_x (f32_t a) {
      f32_t c = cosf(a);
      f32_t s = sinf(a);

      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c,    s,    0.0f,
        0.0f, -s,   c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    /* Create a new Matrix4 from an angular rotation around the y axis */
    static Matrix4 from_angle_y (f32_t a) {
      f32_t c = cosf(a);
      f32_t s = sinf(a);

      return {
        c,    0.0f, -s,   0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        s,    0.0f, c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    /* Create a new Matrix4 from an angular rotation around the z axis */
    static Matrix4 from_angle_z (f32_t a) {
      f32_t c = cosf(a);
      f32_t s = sinf(a);

      return {
        c,    s,    0.0f, 0.0f,
        -s,   c,    0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    /* Create a new Matrix4 from an angular rotation around an arbitrary axis vector */
    ENGINE_API static Matrix4 from_axis_angle (Vector3f const& axis, f32_t angle);


    /* Create a new Matrix4 from a translation vector */
    static Matrix4 from_translation_vector (Vector3f const& v) {
      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        v.x,  v.y,  v.z,  1.0f
      };
    }

    /* Create a new Matrix4 from a scale vector */
    static Matrix4 from_scale_vector (Vector3f const& v) {
      return {
        v.x,  0.0f, 0.0f, 0.0f,
        0.0f, v.y,  0.0f, 0.0f,
        0.0f, 0.0f, v.z,  0.0f,
        0.0f, 0.0f, 0.0f, 1.0f  
      };
    }

    /* Create a new Matrix4 from a shear vector */
    static Matrix4 from_shear_vector (Vector3f const& v) {
      return {
        1.0f, v.x,  v.x,  0.0f,
        v.y,  1.0f, v.y,  0.0f,
        v.z,  v.z,  1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }


    /* Combine a transform into a matrix using individual components */
    ENGINE_API static Matrix4 compose_components (Vector3f const& position, Quaternion const& rotation, Vector3f const& scale);
    
    /* Combine a transform into a matrix */
    static Matrix4 compose (Transform3D const& t) {
      return compose_components(t.position, t.rotation, t.scale);
    }

    /* Combine a 2d transform into a matrix using individual components and an optional up vector (Defaults to Constants::Vector3f::up */
    ENGINE_API static Matrix4 compose_components_2d (Vector2f const& position, f32_t rotation, Vector2f const& scale, Vector3f const& up = Constants::Vector3f::up);

    /* Combine a 2d transform and an optional up vector (Defaults to Constants::Vector3f::up) into a matrix */
    ENGINE_API static Matrix4 compose_2d (Transform2D const& t, Vector3f const& up = Constants::Vector3f::up);


    /* Get an element of a Matrix4 by index.
     * For efficiency, the index is not bounds checked */
    f32_t& operator [] (size_t index) const {
      return (f32_t&) elements[index];
    }


    /* Extract a column of a Matrix4 as a Vector3f */
    Vector3f extract_column (uint8_t column_number) const {
      switch (column_number) {
        case 0: return { e0,  e1,  e2  };
        case 1: return { e4,  e5,  e6  };
        case 2: return { e8,  e9,  e10 };
        case 3: return { e11, e12, e13 };
        default: m_error("Cannot extract invalid Matrix4 column number %" PRIu8 ", expected 0 -> 3", column_number);
      }
    } 


    /* Get the position elements of a matrix as a vector */
    Vector3f get_position () const {
      return { e12, e13, e14 };
    }

    /* Get the raw rotational component of a matrix as a Quaternion */
    ENGINE_API Quaternion extract_raw_quaternion () const;

    /* Get the rotational components of a matrix as a Quaternion */
    ENGINE_API Quaternion get_rotation_quaternion () const;

    /* Get the rotational components of a matrix as an Euler */
    ENGINE_API Euler get_rotation_euler (u8_t order) const;

    /* Get the scale components of a matrix as a vector */
    Vector3f get_scale () const {
      f32_t lx = extract_column(0).length();
      f32_t ly = extract_column(1).length();
      f32_t lz = extract_column(2).length();
      
      return { determinant() < 0.0f? -lx : lx, ly, lz };
    }


    /* Get the maximum scale of the axis of a matrix */
    f32_t get_scale_on_axis () const {
      Vector3f c0 = extract_column(0);
      Vector3f c1 = extract_column(1);
      Vector3f c2 = extract_column(2);

      return sqrtf(num::max(
        (c0 * c0).add_reduce(),
        (c1 * c1).add_reduce(),
        (c2 * c2).add_reduce()
      ));
    }


    /* Decompose a matrix into its component transform */
    ENGINE_API Transform3D decompose () const;
    
    /* Get the inverse of a matrix */
    ENGINE_API Matrix4 inverse () const;


    /* Transpose a matrix order */
    Matrix4 transpose () const {
      return {
        e0, e4, e8,  e12,
        e1, e5, e9,  e13,
        e2, e6, e10, e14,
        e3, e7, e11, e15
      };
    }


    /* Multiply a matrix with another matrix, and place the result into the active matrix */
    ENGINE_API Matrix4& multiply_in_place (Matrix4 const& l, Matrix4 const& r);

    /* Multiply a matrix with another matrix, and place the result into the active matrix */
    Matrix4& multiply_in_place (Matrix4 const& r) {
      return multiply_in_place(*this, r);
    }

    /* Multiply a matrix with another matrix, and return a new matrix as the result */
    ENGINE_API Matrix4 multiply (Matrix4 const& r) const;

    /* Multiply a matrix with another matrix, and return a new matrix as the result */
    Matrix4 operator * (Matrix4 const& r) const {
      return multiply(r);
    }

    /* Multiply each element of a matrix by a scalar value */
    ENGINE_API Matrix4 multiply_scl (f32_t s) const;

    /* Multiply each element of a matrix by a scalar value */
    Matrix4 operator * (f32_t s) const {
      return multiply_scl(s);
    }


    /* Get the determinant of a matrix */
    ENGINE_API f32_t determinant () const;


    /* Determine if two matrices are identical */
    bool equal (Matrix4 const& r) const {
      return memcmp(elements, r.elements, sizeof(elements)) == 0;
    }

    /* Determine if two matrices are identical */
    bool operator == (Matrix4 const& r) const {
      return equal(r);
    }


    /* Determine if two matrices are not identical */
    bool not_equal (Matrix4 const& r) const {
      return memcmp(elements, r.elements, sizeof(elements)) != 0;
    }

    /* Determine if two matrices are not identical */
    bool operator != (Matrix4 const& r) const {
      return not_equal(r);
    }



    /* Copy a vector into the translation components of a matrix */
    Matrix4& set_position (Vector3f const& v) {
      e12 = v.x;
      e13 = v.y;
      e14 = v.z;
      return *this;
    }

    /* Reset a matrix to the identity */
    Matrix4& reset () {
       e0 = 1.0f;  e1 = 0.0f;  e2 = 0.0f;  e3 = 0.0f;
       e4 = 0.0f;  e5 = 1.0f;  e6 = 0.0f;  e7 = 0.0f;
       e8 = 0.0f;  e9 = 0.0f; e10 = 1.0f; e11 = 0.0f;
      e12 = 0.0f; e13 = 0.0f; e14 = 0.0f; e15 = 1.0f;
      return *this;
    }
  };


  namespace Constants {
    namespace Matrix4 {
      static constexpr ::mod::Matrix4 identity = { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f };
    }

    namespace Transform3D {
      static constexpr ::mod::Transform3D identity = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } };
    }
  }
}