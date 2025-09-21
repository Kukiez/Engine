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

uniform sampler1D colorSampler;


const int PARTICLE_COLOR_SAMPLER_LIFETIME = 1;

uniform int COLOR_TYPE;

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= instanceCount) return;

    switch (COLOR_TYPE) {
        case PARTICLE_COLOR_SAMPLER_LIFETIME: {
            float t = clamp(float(particles[id].age) / float(particles[id].lifetime), 0.0, 1.0);
            vec4 color = texture(colorSampler, t);
            particles[id].color = color.rgb;
            particles[id].alpha = color.a;
            break;
        }
    }
}