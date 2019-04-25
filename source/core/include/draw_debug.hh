#ifndef DRAW_H
#define DRAW_H

#include "math/lib.hh"
#include "graphics/lib.hh"
#include "Array.hh"



namespace mod {
  struct draw_debug_2d {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material basic_mat;

    RenderMesh2D line_mesh;
    RenderMesh2D rect_mesh;
    
    RenderMesh2D rect_mesh_basis;
    

    ENGINE_API void init (char const* vert_src, char const* frag_src, char const* rect_basis_src);
    ENGINE_API void destroy ();


    ENGINE_API void line (Line2 const& line, Line3 const& color);
    ENGINE_API void line (Line2 const& line, Vector3f const& color);

    ENGINE_API void rect (AABB2 const& rect, Vector3f const& color);


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector2f const& position);
  };

  struct draw_debug_3d {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material basic_mat;

    RenderMesh3D line_mesh;
    RenderMesh3D cube_mesh;
    
    RenderMesh3D cube_mesh_basis;
    

    ENGINE_API void init (char const* vert_src, char const* frag_src, char const* cube_basis_src);
    ENGINE_API void destroy ();


    ENGINE_API void line (Line3 const& line, Line3 const& color);
    ENGINE_API void line (Line3 const& line, Vector3f const& color);

    ENGINE_API void cube (AABB3 const& cube, Vector3f const& color);


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector3f const& camera_position, Vector3f const& camera_target, Vector2f const& nf_planes);
  };


  struct draw_debug_t {
    draw_debug_3d d3d;
    draw_debug_2d d2d;

    ENGINE_API draw_debug_t& init (
      char const* vert2d_src = "./assets/shaders/primitive2.vert", char const* frag2d_src = "./assets/shaders/primitive.frag", char const* rect_basis_src = "./assets/meshes/test_quad2d.json",
      char const* vert3d_src = "./assets/shaders/primitive3.vert", char const* frag3d_src = "./assets/shaders/primitive.frag", char const* cube_basis_src = "./assets/meshes/test_cube.json"
    );

    ENGINE_API void destroy ();


    ENGINE_API void begin_frame ();


    ENGINE_API void line2 (Line2 const& line, Line3 const& color);
    ENGINE_API void line2 (Line2 const& line, Vector3f const& color = { 1.0f });

    ENGINE_API void line3 (Line3 const& line, Line3 const& color);
    ENGINE_API void line3 (Line3 const& line, Vector3f const& color = { 1.0f });

    ENGINE_API void rect (AABB2 const& rect, Vector3f const& color = { 1.0f });
    ENGINE_API void cube (AABB3 const& cube, Vector3f const& color = { 1.0f });


    ENGINE_API void end_frame (Matrix4 const& matrix_2d, Matrix4 const& matrix_3d);
    ENGINE_API void end_frame (
      Vector2f const& screen_size,
      Vector3f const& camera_position,
      Vector3f const& camera_target = { 0.0f },
      Vector2f const& nf_planes = { 0.01f, 10000.0f },
      Vector2f const& position_2d = { 0.0f }
    );
    ENGINE_API void end_frame (Matrix4 const& camera_matrix_3d, Vector2f const& screen_size_2d, Vector2f const& screen_position_2d = { 0.0f });
  };

  ENGINE_API extern draw_debug_t draw_debug;
}

#endif