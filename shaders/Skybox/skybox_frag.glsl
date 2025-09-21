#version 460 core

in vec3 v_WorldPos;
out vec4 FragColor;

uniform samplerCube skybox;

void main() {
    vec3 dir = normalize(v_WorldPos);
    vec3 color = textureLod(skybox, dir, 0).rgb;
    color = color /  (color + vec3(1));
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}