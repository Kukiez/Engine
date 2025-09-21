#version 460 core

out vec4 FragColor;
in vec2 v_TexCoords;

uniform sampler2D depthTexture;
uniform bool fromDepthBuffer;

#include "../common/global_transforms.glsl"

float linearizeDepth(float depth, float near, float far) {
    float z = depth * 2.0 - 1.0;
    return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}

void main() {
    float depthValue = texture(depthTexture, v_TexCoords).r;
    float linearDepth = fromDepthBuffer ? linearizeDepth(depthValue, 0.1f, 100.0f) : depthValue / 100.0f;

    FragColor = vec4(vec3(linearDepth), 1.0);
}