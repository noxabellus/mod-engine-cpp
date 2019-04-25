#include "../include/draw_debug.hh"



namespace mod {
  draw_debug_2d_t draw_debug_2d = { };

  draw_debug_2d_t& draw_debug_2d_t::create (char const* vert_src, char const* frag_src, char const* rect_basis_src) {
    draw_debug_2d.primitive_vert = Shader::from_file(vert_src);
    draw_debug_2d.primitive_frag = Shader::from_file(frag_src);

    draw_debug_2d.primitive_prog = { "draw::primitive", &draw_debug_2d.primitive_vert, &draw_debug_2d.primitive_frag };

    draw_debug_2d.basic_mat = { "draw::line", &draw_debug_2d.primitive_prog, { false }, { false }, { false }, false, false };
  
    draw_debug_2d.line_mesh = { "draw::line", true };
    draw_debug_2d.line_mesh.enable_colors(NULL);
    draw_debug_2d.rect_mesh = { "draw::rect", true };
    draw_debug_2d.rect_mesh.enable_colors(NULL);

    draw_debug_2d.rect_mesh_basis = RenderMesh2D::from_file(rect_basis_src);


    return draw_debug_2d;
  }

  void draw_debug_2d_t::destroy () {
    basic_mat.destroy();
    line_mesh.destroy();
    rect_mesh.destroy();
    rect_mesh_basis.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_2d_t::line (Line2 const& positions, Line3 const& colors) {
    line_mesh.append_vertex(positions.a, colors.a);
    line_mesh.append_vertex(positions.b, colors.b);
  }

  void draw_debug_2d_t::line (Line2 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }

  void draw_debug_2d_t::rect (AABB2 const& rect, Vector3f const& color) {
    Matrix3 mat = Matrix3::compose_components(rect.center(), 0, rect.size());

    size_t p_offset = rect_mesh.positions.count;

    for (auto [ i, position ] : rect_mesh_basis.positions) {
      rect_mesh.append_vertex(position.apply_matrix(mat), color);
    }

    for (auto [ i, face ] : rect_mesh_basis.faces) {
      rect_mesh.append_face(face + p_offset);
    }
  }


  void draw_debug_2d_t::begin_frame () {
    line_mesh.clear();
    rect_mesh.clear();
  }

  void draw_debug_2d_t::end_frame (Matrix4 const& matrix) {
    basic_mat.set_uniform("view", matrix);

    basic_mat.use();

    line_mesh.use();
    glDrawArrays(GL_LINES, 0, line_mesh.positions.count);

    rect_mesh.draw_with_active_shader();
  }

  void draw_debug_2d_t::end_frame (Vector2f const& screen_size, Vector2f const& position) {
    Vector2f max = screen_size + position;

    end_frame(Matrix4::from_orthographic(
      position.x, max.x,
      position.y, max.y,
      -1.0f, 1.0f
    ));
  }



  draw_debug_3d_t draw_debug_3d = { };

  draw_debug_3d_t& draw_debug_3d_t::create (char const* vert_src, char const* frag_src, char const* cube_basis_src) {
    draw_debug_3d.primitive_vert = Shader::from_file(vert_src);
    draw_debug_3d.primitive_frag = Shader::from_file(frag_src);

    draw_debug_3d.primitive_prog = { "draw::primitive", &draw_debug_3d.primitive_vert, &draw_debug_3d.primitive_frag };

    draw_debug_3d.basic_mat = { "draw::line", &draw_debug_3d.primitive_prog, { false }, { false }, { true, DepthFactor::Lesser }, false, false };
  
    draw_debug_3d.line_mesh = { "draw::line", true };
    draw_debug_3d.line_mesh.enable_colors(NULL);

    draw_debug_3d.cube_mesh = { "draw::cube", true };
    draw_debug_3d.cube_mesh.enable_colors(NULL);

    draw_debug_3d.cube_mesh_basis = RenderMesh3D::from_file(cube_basis_src);

    return draw_debug_3d;
  }

  void draw_debug_3d_t::destroy () {
    basic_mat.destroy();
    line_mesh.destroy();
    cube_mesh.destroy();
    cube_mesh_basis.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_3d_t::line (Line3 const& positions, Line3 const& colors) {
    line_mesh.append_vertex(positions.a, { 0.0f }, colors.a);
    line_mesh.append_vertex(positions.b, { 0.0f }, colors.b);
  }

  void draw_debug_3d_t::line (Line3 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }


  void draw_debug_3d_t::cube (AABB3 const& cube, Vector3f const& color) {
    Matrix4 mat = Matrix4::compose_components(cube.center(), Constants::Quaternion::identity, cube.size());

    size_t p_offset = cube_mesh.positions.count;

    for (auto [ i, position ] : cube_mesh_basis.positions) {
      cube_mesh.append_vertex(position.apply_matrix(mat), cube_mesh_basis.normals[i], color);
    }

    for (auto [ i, face ] : cube_mesh_basis.faces) {
      cube_mesh.append_face(face + p_offset);
    }
  }


  void draw_debug_3d_t::begin_frame () {
    line_mesh.clear();
    cube_mesh.clear();
  }

  void draw_debug_3d_t::end_frame (Matrix4 const& matrix) {
    basic_mat.set_uniform("view", matrix);

    basic_mat.use();

    line_mesh.use();
    glDrawArrays(GL_LINES, 0, line_mesh.positions.count);

    cube_mesh.draw_with_active_shader();
  }

  void draw_debug_3d_t::end_frame (Vector2f const& screen_size, Vector3f const& camera_position, Vector3f const& camera_target, Vector2f const& nf_planes) {
    Vector2f half_screen_size = screen_size / 2.0f;

    end_frame(Matrix4::from_orthographic(
      -half_screen_size.x, half_screen_size.x,
      -half_screen_size.y, half_screen_size.y,
      nf_planes.x, nf_planes.y
    ) * Matrix4::from_look(
      camera_position,
      camera_target,
      Constants::Vector3f::down,
    true).inverse());
  }
}