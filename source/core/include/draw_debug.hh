#ifndef DRAW_H
#define DRAW_H

#include "math/lib.hh"
#include "graphics/lib.hh"
#include "Array.hh"



namespace mod {
  template <typename T> struct LineMesh {
    static_assert(std::is_same<T, Vector2f>::value || std::is_same<T, Vector3f>::value, "LineMesh requires a Vector2f or Vector3f for its T parameter");

    static constexpr GLint component_count = std::is_same<T, Vector2f>::value? 2 : 3;

    using LineT = typename std::conditional<std::is_same<T, Vector2f>::value, Line2, Line3>::type;


    u32_t vao;
    u32_t vbos [2];
    Array<T> positions;
    Array<Vector3f> colors;

    void init () {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(2, vbos);

      glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
      glVertexAttribPointer(0, component_count, GL_FLOAT, GL_FALSE, sizeof(T), (void*) 0);
      glEnableVertexAttribArray(0);

      glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*) 0);
      glEnableVertexAttribArray(1);
    }

    void destroy () {
      positions.destroy();
      colors.destroy();
      glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(2, vbos);
    }

    void clear () {
      positions.clear();
      colors.clear();
    }

    void append (T const& position, Vector3f const& color) {
      positions.append(position);
      colors.append(color);
    }

    void append (LineT const& line, Line3 const& color) {
      positions.append_multiple(&line.a, 2);
      colors.append_multiple(&color.a, 2);
    }

    void update () const {
      glBindVertexArray(vao);
    
      glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
      glBufferData(GL_ARRAY_BUFFER, positions.count * sizeof(T), positions.elements, GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
      glBufferData(GL_ARRAY_BUFFER, colors.count * sizeof(Vector3f), colors.elements, GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw () const {
      update();
      glDrawArrays(GL_LINES, 0, positions.count);
    }
  };

  struct draw_debug_2d_t {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material line_mat;

    LineMesh<Vector2f> line_mesh;
    

    ENGINE_API static draw_debug_2d_t& create (char const* vert_src = "./assets/shaders/primitive2.vert", char const* frag_src = "./assets/shaders/primitive.frag");
    ENGINE_API void destroy ();


    ENGINE_API void line (Line2 const& line, Line3 const& color);
    ENGINE_API void line (Line2 const& line, Vector3f const& color = { 1.0f });


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& in_matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector2f const& position = { 0.0f });
  };

  ENGINE_API extern draw_debug_2d_t draw_debug_2d;

  struct draw_debug_3d_t {
    Shader primitive_vert;
    Shader primitive_frag;
    ShaderProgram primitive_prog;
    Material line_mat;

    LineMesh<Vector3f> line_mesh;
    

    ENGINE_API static draw_debug_3d_t& create (char const* vert_src = "./assets/shaders/primitive3.vert", char const* frag_src = "./assets/shaders/primitive.frag");
    ENGINE_API void destroy ();


    ENGINE_API void line (Line3 const& line, Line3 const& color);
    ENGINE_API void line (Line3 const& line, Vector3f const& color = { 1.0f });


    ENGINE_API void begin_frame ();

    ENGINE_API void end_frame (Matrix4 const& in_matrix);
    ENGINE_API void end_frame (Vector2f const& screen_size, Vector3f const& camera_position, Vector3f const& camera_target = { 0.0f }, Vector2f const& nf_planes = { 0.01f, 10000.0f });
  };

  ENGINE_API extern draw_debug_3d_t draw_debug_3d;
}

#endif