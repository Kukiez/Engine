#version 460 core

#include "particle_def.glsl"
#line 5

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer Particle_in {
    Particle inParticles[];
};

layout(std430, binding = 2) buffer Particle_out {
    Particle outParticles[];
};

layout(std430, binding = 3) buffer IndirectDrawBuffer {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

layout(std430, binding = 4) buffer AliveCount {
    uint alive;
};

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= alive) return;

    Particle particle = inParticles[id];

    particle.position += particle.velocity;
    particle.age++;

    if (particle.age < particle.lifetime) {
        uint idx = atomicAdd(instanceCount, 1u);
        outParticles[idx] = particle;
    }
}