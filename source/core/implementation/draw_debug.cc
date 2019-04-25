#include "../include/draw_debug.hh"



namespace mod {
  draw_debug_t draw_debug = { };

  void draw_debug_2d::init (char const* vert_src, char const* frag_src, char const* rect_basis_src) {
    primitive_vert = Shader::from_file(vert_src);
    primitive_frag = Shader::from_file(frag_src);

    primitive_prog = { "draw::primitive", &primitive_vert, &primitive_frag };

    basic_mat = { "draw::line", &primitive_prog, { false }, { false }, { false }, false, false };
  
    line_mesh = { "draw::line", true };
    line_mesh.enable_colors(NULL);
    rect_mesh = { "draw::rect", true };
    rect_mesh.enable_colors(NULL);

    rect_mesh_basis = RenderMesh2D::from_file(rect_basis_src);
  }

  void draw_debug_2d::destroy () {
    basic_mat.destroy();
    line_mesh.destroy();
    rect_mesh.destroy();
    rect_mesh_basis.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_2d::line (Line2 const& positions, Line3 const& colors) {
    line_mesh.append_vertex(positions.a, colors.a);
    line_mesh.append_vertex(positions.b, colors.b);
  }

  void draw_debug_2d::line (Line2 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }

  void draw_debug_2d::rect (AABB2 const& rect, Vector3f const& color) {
    Matrix3 mat = Matrix3::compose_components(rect.center(), 0, rect.size());

    size_t p_offset = rect_mesh.positions.count;

    for (auto [ i, position ] : rect_mesh_basis.positions) {
      rect_mesh.append_vertex(position.apply_matrix(mat), color);
    }

    for (auto [ i, face ] : rect_mesh_basis.faces) {
      rect_mesh.append_face(face + p_offset);
    }
  }


  void draw_debug_2d::begin_frame () {
    line_mesh.clear();
    rect_mesh.clear();
  }

  void draw_debug_2d::end_frame (Matrix4 const& matrix) {
    basic_mat.set_uniform("view", matrix);

    basic_mat.use();

    line_mesh.use();
    glDrawArrays(GL_LINES, 0, line_mesh.positions.count);

    rect_mesh.draw_with_active_shader();
  }

  void draw_debug_2d::end_frame (Vector2f const& screen_size, Vector2f const& position) {
    Vector2f max = screen_size + position;

    end_frame(Matrix4::from_orthographic(
      position.x, max.x,
      position.y, max.y,
      -1.0f, 1.0f
    ));
  }



  void draw_debug_3d::init (char const* vert_src, char const* frag_src, char const* cube_basis_src) {
    primitive_vert = Shader::from_file(vert_src);
    primitive_frag = Shader::from_file(frag_src);

    primitive_prog = { "draw::primitive", &primitive_vert, &primitive_frag };

    basic_mat = { "draw::line", &primitive_prog, { false }, { false }, { true, DepthFactor::Lesser }, false, false };
  
    line_mesh = { "draw::line", true };
    line_mesh.enable_colors(NULL);

    cube_mesh = { "draw::cube", true };
    cube_mesh.enable_colors(NULL);

    cube_mesh_basis = RenderMesh3D::from_file(cube_basis_src);
  }

  void draw_debug_3d::destroy () {
    basic_mat.destroy();
    line_mesh.destroy();
    cube_mesh.destroy();
    cube_mesh_basis.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_3d::line (Line3 const& positions, Line3 const& colors) {
    line_mesh.append_vertex(positions.a, { 0.0f }, colors.a);
    line_mesh.append_vertex(positions.b, { 0.0f }, colors.b);
  }

  void draw_debug_3d::line (Line3 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }


  void draw_debug_3d::cube (AABB3 const& cube, Vector3f const& color) {
    Matrix4 mat = Matrix4::compose_components(cube.center(), Constants::Quaternion::identity, cube.size());

    size_t p_offset = cube_mesh.positions.count;

    for (auto [ i, position ] : cube_mesh_basis.positions) {
      cube_mesh.append_vertex(position.apply_matrix(mat), cube_mesh_basis.normals[i], color);
    }

    for (auto [ i, face ] : cube_mesh_basis.faces) {
      cube_mesh.append_face(face + p_offset);
    }
  }


  void draw_debug_3d::begin_frame () {
    line_mesh.clear();
    cube_mesh.clear();
  }

  void draw_debug_3d::end_frame (Matrix4 const& matrix) {
    basic_mat.set_uniform("view", matrix);

    basic_mat.use();

    line_mesh.use();
    glDrawArrays(GL_LINES, 0, line_mesh.positions.count);

    cube_mesh.draw_with_active_shader();
  }

  void draw_debug_3d::end_frame (Vector2f const& screen_size, Vector3f const& camera_position, Vector3f const& camera_target, Vector2f const& nf_planes) {
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



  draw_debug_t& draw_debug_t::init (
    char const* vert2d_src, char const* frag2d_src, char const* rect_basis_src,
    char const* vert3d_src, char const* frag3d_src, char const* cube_basis_src
  ) {
    d3d.init(vert3d_src, frag3d_src, cube_basis_src);
    d2d.init(vert2d_src, frag2d_src, rect_basis_src);
    return *this;
  }

  void draw_debug_t::destroy () {
    d3d.destroy();
    d2d.destroy();
  }


  void draw_debug_t::begin_frame () {
    d3d.begin_frame();
    d2d.begin_frame();
  }


  void draw_debug_t::line2 (Line2 const& line, Line3 const& color) {
    d2d.line(line, color);
  }

  void draw_debug_t::line2 (Line2 const& line, Vector3f const& color) {
    d2d.line(line, color);
  }


  void draw_debug_t::line3 (Line3 const& line, Line3 const& color) {
    d3d.line(line, color);
  }

  void draw_debug_t::line3 (Line3 const& line, Vector3f const& color) {
    d3d.line(line, color);
  }


  void draw_debug_t::rect (AABB2 const& rect, Vector3f const& color) {
    d2d.rect(rect, color);
  }

  void draw_debug_t::cube (AABB3 const& cube, Vector3f const& color) {
    d3d.cube(cube, color);
  }


  void draw_debug_t::end_frame (Matrix4 const& matrix_2d, Matrix4 const& matrix_3d) {
    d3d.end_frame(matrix_3d);
    d2d.end_frame(matrix_2d);
  }

  void draw_debug_t::end_frame (
    Vector2f const& screen_size,
    Vector3f const& camera_position,
    Vector3f const& camera_target,
    Vector2f const& nf_planes,
    Vector2f const& position_2d
  ) {
    d3d.end_frame(screen_size, camera_position, camera_target, nf_planes);
    d2d.end_frame(screen_size, position_2d);
  }

  void draw_debug_t::end_frame (Matrix4 const& camera_matrix_3d, Vector2f const& screen_size_2d, Vector2f const& screen_position_2d) {
    d3d.end_frame(camera_matrix_3d);
    d2d.end_frame(screen_size_2d, screen_position_2d);
  }
}