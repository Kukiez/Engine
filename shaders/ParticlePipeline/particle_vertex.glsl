#version 460 core

mat4 translate(mat4 m, vec3 v) {
    mat4 Result = m;
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return Result;
}

mat4 scale(mat4 m, vec3 v) {
    mat4 Result = mat4(1);
    Result[0] = m[0] * v[0];
    Result[1] = m[1] * v[1];
    Result[2] = m[2] * v[2];
    Result[3] = m[3];
    return Result;
}

mat4 rotationMatrix(vec3 euler) {
    float cx = cos(euler.x);
    float sx = sin(euler.x);
    float cy = cos(euler.y);
    float sy = sin(euler.y);
    float cz = cos(euler.z);
    float sz = sin(euler.z);

    mat4 rotX = mat4(
    1, 0, 0, 0,
    0, cx, -sx, 0,
    0, sx, cx, 0,
    0, 0, 0, 1
    );

    mat4 rotY = mat4(
    cy, 0, sy, 0,
    0, 1, 0, 0,
    -sy, 0, cy, 0,
    0, 0, 0, 1
    );

    mat4 rotZ = mat4(
    cz, -sz, 0, 0,
    sz, cz, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
    );

    return rotZ * rotY * rotX; // ZYX order
}

#include "particle_def.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec4 Color;

#include "../common/global_transforms.glsl"

layout(std430, binding = 1) buffer ParticleBuffer {
    Particle particles[];
};

void main() {
    Particle particle = particles[gl_InstanceID];

    mat4 model = mat4(1.0f);
    model = translate(model, particle.position);
    model *= rotationMatrix(particle.rotation);
    model = scale(model, vec3(particle.scale));

    gl_Position = projection3D * view * model * vec4(aPos, 1);

    Color = vec4(particle.color, particle.alpha);
    TexCoords = aTexCoords;
}