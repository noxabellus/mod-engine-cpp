#ifndef INTERSECTS_H
#define INTERSECTS_H

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

#include "Frustum.hh"



namespace mod {
  namespace Intersects {
    /* Determine whether or not two line segments intersect.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2 (Line2 const& l, Line2 const& r);

    /* Determine whether or not a line segment intersects a ray.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2_ray2 (Line2 const& line, Ray2 const& ray);

    /* Determine whether or not a line segment intersects an axis-aligned bounding box.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2_aabb2 (Line2 const& line, AABB2 const& aabb);

    /* Determine whether or not a line segment intersects an axis-aligned bounding box *and* exits.
     * Optionally stores the location of the exit intersection at out_loc */
    ENGINE_API pair_t<bool, Vector2f> line2_aabb2_exit (Line2 const& line, AABB2 const& aabb);

    /* Determine whether or not a line segment intersects a circle.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2_circle (Line2 const& line, Circle const& circle);

    /* Determine whether or not a line segment intersects a circle *and* exits.
     * Optionally stores the location of the exit intersection at out_loc */
    ENGINE_API pair_t<bool, Vector2f> line2_circle_exit (Line2 const& line, Circle const& circle);

    /* Determine whether or not a line segment intersects a polygon.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2_polygon (Line2 const& line, Vector2f const* vertices, size_t vertex_count);

    /* Determine whether or not a line segment intersects a polygon *and* exits.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> line2_polygon_exit (Line2 const& line, Vector2f const* vertices, size_t vertex_count);


    /* Determine whether or not two rays intersect.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> ray2 (Ray2 const& l, Ray2 const& r);

    /* Determine whether or not a ray intersects an axis-aligned bounding box.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> ray2_aabb2 (Ray2 const& ray, AABB2 const& aabb);

    /* Determine whether or not a ray intersects an axis-aligned bounding box.
     * Optionally stores the location of the exit intersection at out_loc */
    ENGINE_API pair_t<bool, Vector2f> ray2_aabb2_exit (Ray2 const& ray, AABB2 const& aabb);

    /* Determine whether or not a ray intersects a circle.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> ray2_circle (Ray2 const& ray, Circle const& circle);

    /* Determine whether or not a ray intersects a circle.
     * Optionally stores the location of the exit intersection at out_loc */
    ENGINE_API pair_t<bool, Vector2f> ray2_circle_exit (Ray2 const& ray, Circle const& circle);

    /* Determine whether or not a ray intersects a polygon.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector2f> ray2_polygon (Ray2 const& ray, Vector2f const* vertices, size_t vertex_count);

    /* Determine whether or not a ray intersects a polygon.
     * Optionally stores the location of the exit intersection at out_loc */
    ENGINE_API pair_t<bool, Vector2f> ray2_polygon_exit (Ray2 const& ray, Vector2f const* vertices, size_t vertex_count);


    /* Determine whether or not two axis-aligned bounding boxes intersect */
    ENGINE_API bool aabb2 (AABB2 const& l, AABB2 const& r);

    /* Determine whether or not an axis-aligned bounding box contains a point */
    ENGINE_API bool aabb2_vector (AABB2 const& aabb, Vector2f const& v);

    /* Determine whether or not an axis-aligned bounding box intersects a circle */
    ENGINE_API bool aabb2_circle (AABB2 const& aabb, Circle const& circle);

    /* Determine whether or not an axis-aligned bounding box intersects a polygon */
    ENGINE_API bool aabb2_polygon (AABB2 const& aabb, Vector2f const* vertices, size_t vertex_count);

    /* Determine whether or not two circles intersect */
    ENGINE_API bool circle (Circle const& l, Circle const& r);

    /* Determine whether or not a circle contains a point */
    ENGINE_API bool circle_vector (Circle const& circle, Vector2f const& vector);

    /* Determine whether or not a circle intersects a polygon */
    ENGINE_API bool circle_polygon (Circle const& circle, Vector2f const* vertices, size_t vertex_count);


    /* Determine whether or not two polygons intersect */
    ENGINE_API bool polygon (Vector2f const* l_vertices, size_t l_vertex_count, Vector2f const* r_vertices, size_t r_vertex_count);

    /* Determine whether or not a polygon contains a point */
    ENGINE_API bool polygon_vector (Vector2f const* vertices, size_t vertex_count, Vector2f const& vector);


    // TODO find sources for missing intersection tests



    // ENGINE_API pair_t<bool, Vector3f> line3 (Line3 const& l, Line3 const& r); // not likely to be useful, odds of line/line intersection in 3d are extremely small

    // ENGINE_API pair_t<bool, Vector3f> line3_ray3 (Line3 const& line, Ray3 const& ray);  // not likely to be useful, odds of line/ray intersection in 3d are extremely small

    // ENGINE_API pair_t<bool, Vector3f> line3_aabb3 (Line3 const& line, AABB3 const& aabb);

    // ENGINE_API pair_t<bool, Vector3f> line3_sphere (Line3 const& line, Sphere const& sphere);

    /* Determine whether or not a line segment intersects a plane
     * NOTE: this does not detect coplanar end points, just a pass-through */
    ENGINE_API bool line3_plane (Line3 const& line, Plane const& plane);

    // ENGINE_API pair_t<bool, Vector3f> line3_triangle (Line3 const& line, Triangle const& triangle);

    // ENGINE_API pair_t<bool, Vector3f> line3_frustum (Line3 const& line, Frustum const& frustum);


    // ENGINE_API pair_t<bool, Vector3f> ray3 (Ray3 const& l, Ray3 const& r);  // not likely to be useful, odds of ray/ray intersection in 3d are extremely small

    /* Determine whether or not a ray intersects an axis-aligned bounding box.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector3f> ray3_aabb3 (Ray3 const& ray, AABB3 const& aabb);

    /* Determine whether or not a ray intersects a sphere.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector3f> ray3_sphere (Ray3 const& ray, Sphere const& sphere);

    /* Determine whether or not a ray intersects a plane.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector3f> ray3_plane (Ray3 const& ray, Plane const& plane);

    /* Determine whether or not a ray intersects a triangle.
     * Returns a pair with a boolean which is true if there was an intersection, and a location which is defined if there was an intersection */
    ENGINE_API pair_t<bool, Vector3f> ray3_triangle (Ray3 const& ray, Triangle const& triangle, bool allow_backface);

    // ENGINE_API pair_t<bool, Vector3f> ray3_frustum (Ray3 const& ray, Frustum const& frustum);

    /* Determine whether or not two axis-aligned bounding boxes intersect */
    ENGINE_API bool aabb3 (AABB3 const& l, AABB3 const& r);

    /* Determine whether or not an axis-aligned bounding box contains a point */
    ENGINE_API bool aabb3_vector (AABB3 const& aabb, Vector3f const& v);

    /* Determine whether or not an axis-aligned bounding box intersects a sphere */
    ENGINE_API bool aabb3_sphere (AABB3 const& aabb, Sphere const& sphere);

    /* Determine whether or not an axis-aligned bounding box intersects a plane */
    ENGINE_API bool aabb3_plane (AABB3 const& aabb, Plane const& plane);

    /* Determine whether or not an axis-aligned bounding box intersects a triangle */
    ENGINE_API bool aabb3_triangle (AABB3 const& aabb, Triangle const& triangle);

    /* Determine whether or not an axis-aligned bounding box intersects a frustum */
    ENGINE_API bool aabb3_frustum (AABB3 const& aabb, Frustum const& frustum);

    /* Determine whether or not two spheres intersect */
    ENGINE_API bool sphere (Sphere const& l, Sphere const& r);

    /* Determine whether or not a sphere contains a point */
    ENGINE_API bool sphere_vector (Sphere const& sphere, Vector3f const& vector);

    /* Determine whether or not a sphere intersects a plane */
    ENGINE_API bool sphere_plane (Sphere const& sphere, Plane const& plane);

    // ENGINE_API bool sphere_triangle (Sphere const& sphere, Triangle const& triangle);

    /* Determine whether or not a sphere intersects a frustum */
    ENGINE_API bool sphere_frustum (Sphere const& sphere, Frustum const& frustum);


    // ENGINE_API bool plane (Plane const& l, Plane const& r);

    // ENGINE_API bool plane_triangle (Plane const& plane, Triangle const& triangle);

    // ENGINE_API bool plane_frustum (Plane const& plane, Frustum const& frustum);


    // ENGINE_API bool triangle (Triangle const& l, Triangle const& r);

    /* Determine whether or not a vector lines inside a triangle when projected onto the plane of the triangle */
    ENGINE_API bool triangle_projected_vector (Triangle const& triangle, Vector3f const& vector);

    // ENGINE_API bool triangle_frustum (Triangle const& triangle, Frustum const& frustum);


    // ENGINE_API bool frustum (Frustum const& l, Frustum const& r);

    /* Determine whether or not a frustum contains a vector */
    ENGINE_API bool frustum_vector (Frustum const& frustum, Vector3f const& vector);
  }
}

#endif