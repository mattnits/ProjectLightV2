#version 460 core

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec3 v_WorldPosition;

layout(location = 0) out vec4 o_Color;

uniform vec2 u_GridCenter;
uniform float u_GridHalfSize;

void main() {
    vec2 distanceFromCenter = abs(v_WorldPosition.xz - u_GridCenter);
    float furthestDistance = max(distanceFromCenter.x, distanceFromCenter.y);
    float fadeStart = u_GridHalfSize * 0.75;
    float alpha = 1.0 - smoothstep(fadeStart, u_GridHalfSize, furthestDistance);

    if (alpha <= 0.001) {
        discard;
    }

    o_Color = vec4(v_Color, alpha);
}