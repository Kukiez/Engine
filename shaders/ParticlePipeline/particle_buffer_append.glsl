#version 460 core

#include "particle_def.glsl"
#line 5

layout(local_size_x = 32) in;

layout(std430, binding = 2) buffer Particles {
    Particle particles[];
};

layout(std430, binding = 3) buffer IndirectDrawBuffer {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

layout(std430, binding = 4) buffer IndirectDispatchBuffer {
    uint numGroupsX;
    uint numGroupsY;
    uint numGroupsZ;
};

layout(std430, binding = 6) buffer NewParticles {
    Particle newParticles[];
};

uniform int particleCount;
uniform int maxParticles;

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= particleCount) return;

    uint idx = (instanceCount + id) % maxParticles;
    particles[idx] = newParticles[id];
}