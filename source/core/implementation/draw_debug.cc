#include "../include/draw_debug.hh"



namespace mod {
  draw_debug_2d_t draw_debug_2d = { };

  draw_debug_2d_t& draw_debug_2d_t::create (char const* vert_src, char const* frag_src) {
    draw_debug_2d.primitive_vert = Shader::from_file(vert_src);
    draw_debug_2d.primitive_frag = Shader::from_file(frag_src);

    draw_debug_2d.primitive_prog = { "draw::primitive", &draw_debug_2d.primitive_vert, &draw_debug_2d.primitive_frag };

    draw_debug_2d.line_mat = { "draw::line", &draw_debug_2d.primitive_prog, { false }, { false }, { false }, false, true };
  
    draw_debug_2d.line_mesh.init();

    return draw_debug_2d;
  }

  void draw_debug_2d_t::destroy () {
    line_mat.destroy();
    line_mesh.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_2d_t::line (Line2 const& positions, Line3 const& colors) {
    line_mesh.append(positions, colors);
  }

  void draw_debug_2d_t::line (Line2 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }


  void draw_debug_2d_t::begin_frame () {
    line_mesh.clear();
  }

  void draw_debug_2d_t::end_frame (Matrix4 const& matrix) {
    line_mat.set_uniform("view", matrix);

    line_mat.use();
    line_mesh.draw();
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

  draw_debug_3d_t& draw_debug_3d_t::create (char const* vert_src, char const* frag_src) {
    draw_debug_3d.primitive_vert = Shader::from_file(vert_src);
    draw_debug_3d.primitive_frag = Shader::from_file(frag_src);

    draw_debug_3d.primitive_prog = { "draw::primitive", &draw_debug_3d.primitive_vert, &draw_debug_3d.primitive_frag };

    draw_debug_3d.line_mat = { "draw::line", &draw_debug_3d.primitive_prog, { false }, { false }, { false }, false, true };
  
    draw_debug_3d.line_mesh.init();

    return draw_debug_3d;
  }

  void draw_debug_3d_t::destroy () {
    line_mat.destroy();
    line_mesh.destroy();
    primitive_prog.destroy();
    primitive_frag.destroy();
    primitive_vert.destroy();
  }


  void draw_debug_3d_t::line (Line3 const& positions, Line3 const& colors) {
    line_mesh.append(positions, colors);
  }

  void draw_debug_3d_t::line (Line3 const& positions, Vector3f const& color) {
    return line(positions, { color, color });
  }


  void draw_debug_3d_t::begin_frame () {
    line_mesh.clear();
  }

  void draw_debug_3d_t::end_frame (Matrix4 const& matrix) {
    line_mat.set_uniform("view", matrix);

    line_mat.use();
    line_mesh.draw();
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