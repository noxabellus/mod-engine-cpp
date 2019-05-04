#ifndef MATRIX3_H
#define MATRIX3_H

#include "../cstd.hh"
#include "Vector2.hh"



namespace mod {
  struct Matrix4;


  struct Transform2D {
    Vector2f position;
    f32_t rotation;
    Vector2f scale;



    /* Create a new identity-initialized Transform2D */
    constexpr Transform2D ()
    : position(0.0f, 0.0f)
    , rotation(0.0f)
    , scale(1.0f, 1.0f)
    { }

    /* Create a new Transform2D and initialize its components */
    constexpr Transform2D (Vector2f const& in_position, f32_t in_rotation, Vector2f const& in_scale)
    : position(in_position)
    , rotation(in_rotation)
    , scale(in_scale)
    { }

    /* Create a new Transform2D from a tri_t */
    constexpr Transform2D (tri_t<Vector2f, f32_t, Vector2f> const& tri)
    : position(tri.a)
    , rotation(tri.b)
    , scale(tri.c)
    { }


    /* Create a tri_t from a Transform2D */
    constexpr operator tri_t<Vector2f, f32_t, Vector2f> () const {
      return { position, rotation, scale };
    }

    /* Linear interpolate between two Transforms */
    Transform2D lerp (f32_t alpha, Transform2D const& other) const {
      return {
        position.lerp(alpha, other.position),
        num::lerp(alpha, rotation, other.rotation),
        scale.lerp(alpha, other.scale)
      };
    }
  };


  struct Matrix3 {
    union {
      struct {
        f32_t e0; f32_t e1; f32_t e2;
        f32_t e3; f32_t e4; f32_t e5;
        f32_t e6; f32_t e7; f32_t e8;
      };
      f32_t elements [9];
    };



    /* Create a new identity-initialized Matrix3 */
    constexpr Matrix3 ()
    : e0(1.0f), e1(0.0f), e2(0.0f)
    , e3(0.0f), e4(1.0f), e5(0.0f)
    , e6(0.0f), e7(0.0f), e8(1.0f)
    { }
    
    /* Create a new Matrix3 from column-major elements */
    constexpr Matrix3 (
      f32_t in_e0, f32_t in_e1, f32_t in_e2,
      f32_t in_e3, f32_t in_e4, f32_t in_e5,
      f32_t in_e6, f32_t in_e7, f32_t in_e8
    )
    : e0(in_e0), e1(in_e1), e2(in_e2)
    , e3(in_e3), e4(in_e4), e5(in_e5)
    , e6(in_e6), e7(in_e7), e8(in_e8)
    { }

    /* Create a new Matrix3 from column-major elements.
     * Note: This is the same order as the data structure/constructor, but is here for consistency */
    static constexpr Matrix3 column_major (
      f32_t e0, f32_t e1, f32_t e2,
      f32_t e3, f32_t e4, f32_t e5,
      f32_t e6, f32_t e7, f32_t e8
    ) {
      return {
        e0, e1, e2,
        e3, e4, e5,
        e6, e7, e8
      };
    }

    /* Create a new Matrix3 from row-major elements.
     * Note: This is not the same order as the data structure/constructor, it is here for convenience */
    static constexpr Matrix3 row_major (
      f32_t e0, f32_t e3, f32_t e6,
      f32_t e1, f32_t e4, f32_t e7,
      f32_t e2, f32_t e5, f32_t e8
    ) {
      return {
        e0, e1, e2,
        e3, e4, e5,
        e6, e7, e8
      };
    }

    ArrayIterator<f32_t> begin () const {
      return { const_cast<f32_t*>(&elements[0]), 0 };
    }

    ArrayIterator<f32_t> end () const {
      return { const_cast<f32_t*>(&elements[0]), 9 };
    }

    /* Create a Matrix3 from the upper left portion of a Matrix4 */
    ENGINE_API static Matrix3 from_4 (Matrix4 const& m);

    /* Create a Matrix3 normal matrix from a Matrix4 */
    static Matrix3 normal (Matrix4 const& m) {
      return from_4(m).inverse().transpose();
    }


    /* Create a Matrix3 from a translation vector */
    static Matrix3 from_translation_vector (Vector2f const& v) {
      return {
        1.0f, 0.0f, v.x,
        0.0f, 1.0f, v.y,
        0.0f, 0.0f, 1.0f
      };
    }

    /* Create a Matrix3 from a rotation angle */
    static Matrix3 from_rotation_angle (f32_t r) {
      f32_t c = cosf(r);
      f32_t s = sinf(r);

      return {
        c,    -s,   0.0f,
        s,     c,   0.0f,
        0.0f, 0.0f, 1.0f 
      };
    }

    /* Create a Matrix3 from a scale vector */
    static Matrix3 from_scale_vector (Vector2f const& v) {
      return {
        v.x,  0.0f, 0.0f,
        0.0f, v.y,  0.0f,
        0.0f, 0.0f, 1.0f
      };
    }

    /* Create a Matrix3 from a shear vector */
    static Matrix3 from_shear_vector (Vector2f const& v) {
      return {
        1.0f,       tanf(v.x), 0.0f,
        tanf(v.y), 1.0f,       0.0f,
        0.0f,       0.0f,      1.0f
      };
    }


    /* Combine a transform into a matrix using individual components */
    ENGINE_API static Matrix3 compose_components (Vector2f const& position, f32_t rotation, Vector2f const& scale);


    /* Combine a transform into a matrix */
    static Matrix3 compose (Transform2D const& transform) {
      return compose_components(transform.position, transform.rotation, transform.scale);
    }


    /* Get an element of a Matrix3 by index.
     * For efficiency, the index is not bounds checked */
    f32_t& operator [] (size_t index) const {
      return const_cast<f32_t&>(elements[index]);
    }


    /* Get the position elements of a matrix as a vector */
    Vector2f get_position () const {
      return { e2, e5 };
    }

    /* Get the rotational components of a matrix as a floating-point angle */
    f32_t get_rotation () const {
      return atan2f(e3, e0);
    }

    /* Get the scale components of a matrix as a vector */
    Vector2f get_scale () const {
      return {
        sqrtf(e0 * e0 + e3 * e3),
        sqrtf(e1 * e1 + e4 * e4)
      };
    }

    /* Decompose a matrix into its component transform */
    Transform2D decompose () const {
      return {
        get_position(),
        get_rotation(),
        get_scale()
      };
    }


    /* Get the inverse of a matrix */
    ENGINE_API Matrix3 inverse () const;


    /* Transpose a matrix order */
    Matrix3 transpose () const {
      return {
        e0, e3, e6,
        e1, e4, e7,
        e2, e5, e8
      };
    }


    /* Multiply a matrix with another matrix, and place the result into the active matrix */
    ENGINE_API Matrix3& multiply_in_place (Matrix3 const& l, Matrix3 const& r);

    /* Multiply a matrix with another matrix, and place the result into the active matrix */
    Matrix3& multiply_in_place (Matrix3 const& r) {
      return multiply_in_place(*this, r);
    }

    /* Multiply a matrix with another matrix, and return a new matrix as the result */
    ENGINE_API Matrix3 multiply (Matrix3 const& r) const;

    /* Multiply a matrix with another matrix, and return a new matrix as the result */
    Matrix3 operator * (Matrix3 const& r) const {
      return multiply(r);
    }


    /* Multiply all components of a matrix with a scalar value */
    ENGINE_API Matrix3 multiply_scl (f32_t s) const;

    /* Multiply all components of a matrix with a scalar value */
    Matrix3 operator * (f32_t s) const {
      return multiply_scl(s);
    }


    /* Get the determinant of a matrix */
    ENGINE_API f32_t determinant () const;


    /* Determine if two matrices are essentially equivalent.
     * Wrapper for num::almost_equal, see it for details.
     * Warning: This is an expensive operation! */
    bool almost_equal (Matrix3 const& r, s32_t ulp = 2) const {
      for (auto [ i, e ] : *this) {
        if (!num::almost_equal(e, r[i], ulp)) return false;
      }
      
      return true;
    }


    /* Determine if two matrices are identical */
    bool equal (Matrix3 const& r) const {
      return memcmp(elements, r.elements, sizeof(elements)) == 0;
    }

    /* Determine if two matrices are identical */
    bool operator == (Matrix3 const& r) const {
      return equal(r);
    }
    

    /* Determine if two matrices are not identical */
    bool not_equal (Matrix3 const& r) const {
      return memcmp(elements, r.elements, sizeof(elements)) != 0;
    }

    /* Determine if two matrices are not identical */
    bool operator != (Matrix3 const& r) const {
      return not_equal(r);
    }


    /* Copy a vector into the translation components of a matrix */
    Matrix3& set_position (Vector2f const& v) {
      e2 = v.x;
      e5 = v.y;
      return *this;
    }

    /* Reset a matrix to the identity */
    Matrix3& reset () {
      e0 = 1.0f; e1 = 0.0f; e2 = 0.0f;
      e3 = 0.0f; e4 = 1.0f; e5 = 0.0f;
      e6 = 0.0f; e7 = 0.0f; e8 = 1.0f;
      return *this;
    }
  };


  namespace Constants {
    namespace Matrix3 {
      static constexpr ::mod::Matrix3 identity = { 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f };
    }

    namespace Transform2D {
      static constexpr ::mod::Transform2D identity = { { 0.0f, 0.0f }, 0.0f, { 1.0f, 1.0f } };
    }
  }
}

#endif