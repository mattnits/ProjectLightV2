#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_Color;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main() {
    v_Color = a_Color;
    gl_Position =
        u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}