#version 460 core

uniform sampler2D gNormal;
in vec2 v_TexCoords;
out vec4 FragColor;

void main() {
    vec3 normal = texture(gNormal, v_TexCoords).rgb;

    vec3 visual = normal * 0.5 + 0.5;
    FragColor = vec4(clamp(visual, 0, 1), 1.0);
}