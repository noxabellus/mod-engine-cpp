#version 430 core

layout (location = 0) in vec3 a_position;

layout (location = 1) in vec3 a_norm;

layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_color;


uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_projection;

uniform mat3 m_normal; // transpose of the inverse of view * model


layout (location = 0) out vec3 v_pos;

layout (location = 1) out vec3 v_norm;

layout (location = 2) out vec2 v_uv;
layout (location = 3) out vec3 v_color;



void main () {
  vec4 a_pos4 = vec4(a_position, 1.0);

  gl_Position = m_projection * m_view * m_model * a_pos4;

  v_pos = vec3(m_view * m_model * a_pos4);

  v_norm = m_normal * a_norm;

  v_uv = vec2(a_uv.x, a_uv.y);
  v_color = a_color;
}