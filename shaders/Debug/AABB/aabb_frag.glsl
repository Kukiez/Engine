#version 450 core

layout(location = 0) out vec4 f_FragColor;
layout(location = 1) out vec4 f_Emissive;
in vec4 color;

void main() {
    f_FragColor = color;
    f_Emissive = vec4(0);
}