#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec3 v_WorldPosition;

uniform mat4 u_ViewProjection;

void main() {
    v_Color = a_Color;
    v_WorldPosition = a_Position;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}