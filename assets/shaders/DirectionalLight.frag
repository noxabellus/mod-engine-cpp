#version 430 core

layout (location = 0) in vec3 v_pos;

layout (location = 1) in vec3 v_norm;

layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_color;

layout (location = 4) in vec3 v_light_pos;


uniform sampler2D t_diffuse;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);

uniform vec3 ambient_color = vec3(0.1, 0.1, 0.1);

uniform float specular_strength = 0.5;
uniform float specular_power = 2.0;


out vec4 f_color;



void main () {
  vec4 object_color = texture(t_diffuse, v_uv) * vec4(v_color, 1.0);


  vec3 f_norm = normalize(v_norm);
  vec3 light_dir = normalize(v_light_pos - v_pos);

  float light_contact = max(dot(f_norm, light_dir), 0.0);


  vec3 view_dir = normalize(-v_pos); // view space
  vec3 reflect_dir = reflect(-light_dir, f_norm);
  
  float specular_contact = pow(max(dot(view_dir, reflect_dir), 0.0), specular_power);


  f_color = vec4(ambient_color + (light_contact * light_color) + (specular_strength * specular_contact * light_color), 1.0) * object_color;
}