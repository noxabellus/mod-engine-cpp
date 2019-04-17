#ifndef CONTAINS_H
#define CONTAINS_H

#include "AABB2.hh"
#include "AABB3.hh"

#include "Circle.hh"
#include "Sphere.hh"

#include "Line2.hh"
#include "Line3.hh"

#include "Ray2.hh"
#include "Ray3.hh"

#include "Plane.hh"
#include "Triangle.hh"



namespace mod {
  namespace Contains {
    /* Determine if an axis-aligned bounding box fully contains another */
    ENGINE_API bool aabb2 (AABB2 const& l, AABB2 const& r);

    /* Determine if an axis-aligned bounding box fully contains a line segment */
    ENGINE_API bool aabb2_line2 (AABB2 const& l, Line2 const& r);

    /* Determine if an axis-aligned bounding box fully contains a circle */
    ENGINE_API bool aabb2_circle (AABB2 const& l, Circle const& r);
    
    /* Determine if an axis-aligned bounding box fully contains an arbitrary polygon */
    ENGINE_API bool aabb2_polygon (AABB2 const& aabb, Vector2f const& vertices, size_t vertex_count);


    /* Determine if an axis-aligned bounding box fully contains another */
    ENGINE_API bool aabb3 (AABB3 const& l, AABB3 const& r);

    /* Determine if an axis-aligned bounding box fully contains a line segment */
    ENGINE_API bool aabb3_line3 (AABB3 const& l, Line3 const& r);

    /* Determine if an axis-aligned bounding box fully contains a sphere */
    ENGINE_API bool aabb3_sphere (AABB3 const& l, Sphere const& r);
    
    /* Determine if an axis-aligned bounding box fully contains a triangle */
    ENGINE_API bool aabb3_triangle (AABB3 const& l, Triangle const& r);


    /* Determine if a Circle fully contains another */
    ENGINE_API bool circle (Circle const& l, Circle const& r);

    /* Determine if a Circle fully contains a line segment */
    ENGINE_API bool circle_line2 (Circle const& circle, Line2 const& line);

    /* Determine if a Circle fully contains an axis-aligned bounding box */
    ENGINE_API bool circle_aabb2 (Circle const& circle, AABB2 const& aabb);

    /* Determine if a Circle fully contains an axis-aligned bounding box */
    ENGINE_API bool circle_polygon (Circle const& circle, Vector2f const* vertices, size_t vertex_count);


    /* Determine if a Sphere fully contains another */
    ENGINE_API bool sphere (Sphere const& l, Sphere const& r);

    /* Determine if a Sphere fully contains an axis-aligned bounding box */
    ENGINE_API bool sphere_aabb3 (Sphere const& l, AABB3 const& r);
    
    /* Determine if a Sphere fully contains line segment */
    ENGINE_API bool sphere_line3 (Sphere const& l, Line3 const& r);

    /* Determine if a Sphere fully contains a triangle */
    ENGINE_API bool sphere_triangle (Sphere const& l, Triangle const& r);


    /* Determine if a polygon fully contains another */
    ENGINE_API bool polygon (Vector2f const* l_vertices, size_t l_vertex_count, Vector2f const* r_vertices, size_t r_vertex_count);

    /* Determine if a polygon fully contains a line */
    ENGINE_API bool polygon_line2 (Vector2f const* vertices, size_t vertex_count, Line2 const& line);

    /* Determine if a polygon fully contains a circle */
    ENGINE_API bool polygon_circle (Vector2f const* vertices, size_t vertex_count, Circle const& circle);

    /* Determine if a polygon fully contains an axis-aligned bounding box */
    ENGINE_API bool polygon_aabb2 (Vector2f const* vertices, size_t vertex_count, AABB2 const* aabb);
  }
}

#endif