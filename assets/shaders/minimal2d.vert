#version 430 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_color;


uniform mat4 transform;
uniform mat4 view;


layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec3 v_color;


void main () {
  gl_Position = view * transform * vec4(a_position, 0.0, 1.0);
  v_uv = vec2(a_uv.x, 1.0 - a_uv.y);
  v_color = a_color;
}