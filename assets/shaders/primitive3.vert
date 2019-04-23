#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;


uniform mat4 view;


layout (location = 0) out vec3 v_color;


void main () {
  gl_Position = view * vec4(a_position, 1.0);
  v_color = a_color;
}