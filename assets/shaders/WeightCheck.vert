#version 430 core


const vec3 joint_colors [17] = {
  vec3(1.00, 1.00, 0.00), // Torso
  vec3(1.00, 0.55, 0.00), // Chest
  vec3(0.00, 0.00, 0.55), // Neck
  vec3(0.68, 0.85, 0.90), // Head
  vec3(1.00, 0.75, 0.80), // Upper_Arm_L
  vec3(0.00, 0.50, 0.00), // Lower_Arm_L
  vec3(0.50, 0.00, 0.00), // Hand_L
  vec3(1.00, 0.00, 0.00), // Upper_Arm_R
  vec3(0.00, 1.00, 1.00), // Lower_Arm_R
  vec3(0.50, 0.00, 0.50), // Hand_R
  vec3(1.00, 0.00, 1.00), // Upper_Leg_L
  vec3(0.50, 0.50, 0.00), // Lower_Leg_L
  vec3(0.65, 0.16, 0.16), // Foot_L
  vec3(1.00, 0.75, 0.80), // Upper_Leg_R
  vec3(1.00, 1.00, 0.88), // Lower_Leg_R
  vec3(0.00, 0.55, 0.55), // Foot_R
  vec3(0,0,0)
};



layout (location = 0) in vec3 a_position;

layout (location = 1) in vec3 a_norm;

layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_color;

layout (location = 4) in uvec3 a_joints;
layout (location = 5) in vec3 a_weights;


uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_projection;

uniform mat3 m_normal; // transpose of the inverse of view * model


layout (location = 0) out vec3 v_pos;

layout (location = 1) out vec3 v_norm;

layout (location = 2) out vec2 v_uv;
layout (location = 3) out vec3 v_color;

layout (location = 4) out vec3 v_weights;



void main () {
  vec4 a_pos4 = vec4(a_position, 1.0);

  gl_Position = m_projection * m_view * m_model * a_pos4;

  v_pos = vec3(m_view * m_model * a_pos4);

  v_norm = m_normal * a_norm;

  v_uv = vec2(a_uv.x, 1.0 - a_uv.y);
  v_color = a_color;

  v_weights //= a_weights;
            = vec3(0, 0, 0)
            + joint_colors[a_joints.x] * a_weights.x
            + joint_colors[a_joints.y] * a_weights.y
            + joint_colors[a_joints.z] * a_weights.z
            ;
}