#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveColor;

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 camera_position;
uniform samplerCube texture_unit;

void main()
{
    float ratio = 1 / 1.52;
    vec3 I = normalize(v_Position - camera_position);
    vec3 R = refract(I, normalize(v_Normal), ratio);
    FragColor = vec4(texture(texture_unit, R).rgb, 1.0);
    EmissiveColor = vec4(0);
}