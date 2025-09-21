#version 460 core

#include "../common/global_transforms.glsl"
#include "../common/projection.glsl"

out float FragColor;

in vec2 v_TexCoords;

uniform sampler2D gLinearDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

layout(std140, binding = 1) uniform Samples {
    vec3 samples[64];
};

int kernelSize = 64;

uniform mat4 inverseProjection;
uniform vec2 noiseScale;

uniform float radius;
uniform float bias;

void main() {
    vec2 uv = v_TexCoords;
    vec3 fragPos = reconstructViewPosition(v_TexCoords, texture(gLinearDepth, v_TexCoords).r, inverseProjection);
    vec3 normal = texture(gNormal, uv).rgb;
    normal = normalize(normal);
    vec3 randomVec = normalize(texture(texNoise, v_TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection3D * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = texture(gLinearDepth, offset.xy).r;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; // changed >= to <=
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}