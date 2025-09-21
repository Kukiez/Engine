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

const int PARTICLE_SCALE_LIFE_TIME = 1;
const int PARTICLE_SCALE_DECAY = 2;
const int PARTICLE_SCALE_OSCILLATE = 3;
const int PARTICLE_SCALE_VELOCITY = 4;
const int PARTICLE_SCALE_DISTANCE = 5;

uniform int SCALE_TYPE;

/* Scale Type: DECAY */
uniform float scaleDecay;

/* Scale Type: LIFE_TIME */
uniform float minScale;
uniform float maxScale;

/* Scale Type: OSCILLATE */
uniform float amplitude;
uniform float frequency;
uniform float phase;
uniform float t;
uniform float baseScale; 

/* Scale Type: VELOCITY */
uniform float velocityScaleFactor;
// uniform float minScale;
// uniform float maxScale;

/* Scale Type: DISTANCE */
uniform float center_x;
uniform float center_y;
uniform float center_z;
uniform float scalePerUnit;

const float MIN_SCALE = 0.01f;

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= instanceCount) return;

    if (SCALE_TYPE == PARTICLE_SCALE_LIFE_TIME) {
        float mixPercentage = float(particles[id].age) / float(particles[id].lifetime);
        particles[id].scale = mix(minScale, maxScale, mixPercentage);
    } else if (SCALE_TYPE == PARTICLE_SCALE_DECAY) {
        particles[id].scale += scaleDecay;
    } else if (SCALE_TYPE == PARTICLE_SCALE_OSCILLATE) {
        float t = float(particles[id].age);
        particles[id].scale = baseScale + amplitude * sin(t * frequency + phase);
    } else if (SCALE_TYPE == PARTICLE_SCALE_VELOCITY) {
        float speed = length(particles[id].velocity);
        particles[id].scale = clamp(speed * velocityScaleFactor, minScale, maxScale);
    } else if (SCALE_TYPE == PARTICLE_SCALE_DISTANCE) {
        float dist = length(particles[id].position - vec3(center_x, center_y, center_z));
        particles[id].scale = clamp(dist * scalePerUnit, minScale, maxScale);
    }

    if (particles[id].scale <= 0.0f) {
        particles[id].age = particles[id].lifetime;
        particles[id].scale = MIN_SCALE;
    }
}