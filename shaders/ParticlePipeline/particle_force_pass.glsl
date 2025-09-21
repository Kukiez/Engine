#version 460 core

#include "particle_def.glsl"
#line 5

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

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

uniform sampler1D forceSampler1D;
uniform vec3 minVelocity;
uniform vec3 maxVelocity;

const int PARTICLE_FORCE_SWIRL_VORTEX = 1;
uniform vec3 center;
uniform vec3 axis;
uniform float strength;
uniform float falloff;

const int PARTICLE_FORCE_SPIRAL_VORTEX = 2;
uniform float u_pullStrength;

uniform int FORCE_TYPE;

vec3 swirl(vec3 position, vec3 velocity, vec3 center, vec3 axis, float strength, float falloff) {
    vec3 offset = position - center;

    vec3 radial = offset - axis * dot(offset, axis);

    float distance = max(length(radial), 0.0001);
    radial = normalize(radial);

    vec3 tangential = normalize(cross(axis, radial));

    float swirlStrength = strength * exp(-falloff * distance); 
    vec3 swirlVelocity = swirlStrength * tangential;
    return swirlVelocity;
}

vec3 computeSpiralVelocity(
    vec3 position,
    vec3 center,
    vec3 axis,           
    float swirlStrength,
    float pullStrength,
    float falloff
) {
    vec3 offset = position - center;
    vec3 dir = normalize(offset);

    return dir;
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= instanceCount) return;

    switch (FORCE_TYPE) {
        case PARTICLE_FORCE_SWIRL_VORTEX: {
            particles[id].velocity = swirl(particles[id].position, particles[id].velocity, center, axis, strength, falloff);
            break;
        }
        case PARTICLE_FORCE_SPIRAL_VORTEX: {
            particles[id].velocity += computeSpiralVelocity(
                particles[id].position,
                center,
                axis,
                strength,
                u_pullStrength,
                falloff
            );
            break;
        }
    }
}