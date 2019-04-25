#ifndef DRAW_H
#define DRAW_H

#include "math/lib.hh"
#include "graphics/lib.hh"
#include "Array.hh"



namespace mod {
  struct draw_debug_2d_t {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material basic_mat;

    RenderMesh2D line_mesh;
    RenderMesh2D rect_mesh;
    
    RenderMesh2D rect_mesh_basis;
    

    ENGINE_API static draw_debug_2d_t& create (char const* vert_src = "./assets/shaders/primitive2.vert", char const* frag_src = "./assets/shaders/primitive.frag", char const* rect_basis_src = "./assets/meshes/test_quad2d.json");
    ENGINE_API void destroy ();


    ENGINE_API void line (Line2 const& line, Line3 const& color);
    ENGINE_API void line (Line2 const& line, Vector3f const& color = { 1.0f });

    ENGINE_API void rect (AABB2 const& rect, Vector3f const& color = { 1.0f });


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& in_matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector2f const& position = { 0.0f });
  };

  ENGINE_API extern draw_debug_2d_t draw_debug_2d;

  struct draw_debug_3d_t {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material basic_mat;

    RenderMesh3D line_mesh;
    RenderMesh3D cube_mesh;
    
    RenderMesh3D cube_mesh_basis;
    

    ENGINE_API static draw_debug_3d_t& create (char const* vert_src = "./assets/shaders/primitive3.vert", char const* frag_src = "./assets/shaders/primitive.frag", char const* cube_basis_src = "./assets/meshes/test_cube.json");
    ENGINE_API void destroy ();


    ENGINE_API void line (Line3 const& line, Line3 const& color);
    ENGINE_API void line (Line3 const& line, Vector3f const& color = { 1.0f });

    ENGINE_API void cube (AABB3 const& cube, Vector3f const& color = { 1.0f });


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& in_matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector3f const& camera_position, Vector3f const& camera_target = { 0.0f }, Vector2f const& nf_planes = { 0.01f, 10000.0f });
  };

  ENGINE_API extern draw_debug_3d_t draw_debug_3d;
}

#endif