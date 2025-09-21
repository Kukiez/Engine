#version 450 core

struct ParticleData {
    vec4 beginColor;
    vec4 endColor;

    vec4 position;
    vec4 velocity;
    vec4 growthRate;
    vec4 scale;
    vec4 rotation;
    vec4 rotationSpeed;

    int lifetime;
    int age;
    int layer;
    int maxLayer;
};

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer Particle_in {
    ParticleData in_particles[];
};

layout(std430, binding = 2) buffer Particle_out {
    ParticleData out_particles[];
};

layout(binding = 3, offset = 0) uniform atomic_uint out_index;

uniform int max_particles;

uniform float step;

const float gravity = -9.81;

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= max_particles) return;

    in_particles[id].age ++;

    if (in_particles[id].age < in_particles[id].lifetime) {
        in_particles[id].position += in_particles[id].velocity;
        in_particles[id].scale += in_particles[id].growthRate;
        in_particles[id].rotation += in_particles[id].rotationSpeed;

        if (in_particles[id].scale.x < 0 || in_particles[id].scale.y < 0 || in_particles[id].scale.z < 0) {
            in_particles[id].age = in_particles[id].lifetime + 1;
        }

        uint index = atomicCounterIncrement(out_index);
        out_particles[index] = in_particles[id];
    }
}