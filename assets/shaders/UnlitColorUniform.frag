#version 430 core

layout (location = 0) in vec3 v_pos;

layout (location = 1) in vec3 v_norm;

layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_color;


uniform vec3 color;


out vec4 f_color;



void main () {
  f_color = vec4(color, 1.0);
}