#version 450 core
#define MAX_SPAWNERS 8

struct ParticleData {
    vec4 color;
    vec4 beginColor;
    vec4 endColor;

    vec4 position;
    vec4 velocity;
    vec4 growthRate;
    vec4 scale;

    int lifetime;
    int age;
    int layer;
    int maxLayer;
};


struct ParticleSpawner {
    vec4 minTint;
    vec4 maxTint;

    vec4 position;
    vec4 range;

    vec4 minVelocity;
    vec4 maxVelocity;

    vec4 minGrowthRate;
    vec4 maxGrowthRate;

    vec4 minScale;
    vec4 maxScale;

    vec4 minRotation;
    vec4 maxRotation;

    vec4 minRotationSpeed;
    vec4 maxRotationSpeed;
    
    int minLifetime;
    int maxLifetime;

    int spawnCount;
    int particleWriteOffset;

    int layer;
    int layerMax;

    int pad0;
    int pad1;
};

uint pcg(uint state) {
    state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rng(uint seed) {
    return fract(float(seed) / 4294967296.0);
}

vec3 randomInUnitSphere(inout uint seed) {
    float u = pcg(seed);
    float v = pcg(seed);
    float w = pcg(seed);

    float theta = u * 6.2831853;
    float phi = acos(2.0 * v - 1.0);
    float r = pow(w, 1.0 / 3.0);

    return vec3(
        r * sin(phi) * cos(theta),
        r * sin(phi) * sin(theta),
        r * cos(phi)
    );
}

vec3 randomPointInSphere(vec3 center, float radius, inout uint seed) {
    return center + radius * randomInUnitSphere(seed);
}

ParticleData spawnParticle(ParticleSpawner spawner, uint time) {
    ParticleData particle;

    uint seed = time;
    seed = pcg(seed);

    particle.lifetime = int(mix(spawner.minLifetime, spawner.maxLifetime, rng(seed)));

    seed = pcg(seed);

    particle.position.x = mix(-spawner.range.x, spawner.range.x, rng(seed));
    seed = pcg(seed);
    particle.position.y = mix(-spawner.range.y, spawner.range.y, rng(seed));
    seed = pcg(seed);
    particle.position.z = mix(-spawner.range.z, spawner.range.z, rng(seed));
    seed = pcg(seed);
    particle.position.w = 1.0f;

    particle.velocity.x = mix(spawner.minVelocity.x, spawner.maxVelocity.x, rng(seed));
    seed = pcg(seed);
    particle.velocity.y = mix(spawner.minVelocity.y, spawner.maxVelocity.y, rng(seed));
    seed = pcg(seed);
    particle.velocity.z = mix(spawner.minVelocity.z, spawner.maxVelocity.z, rng(seed));
    seed = pcg(seed);
    particle.velocity.w = 1.0f;

    particle.growthRate = mix(spawner.minGrowthRate, spawner.maxGrowthRate, rng(seed));

    particle.scale = mix(spawner.minScale, spawner.maxScale, rng(seed));

    particle.beginColor = spawner.minTint;
    particle.endColor = spawner.maxTint;

    particle.color = particle.beginColor;

    particle.age = 0;
    particle.layer = spawner.layer;
    particle.maxLayer = spawner.layerMax;

    return particle;
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std140, binding = 1) uniform Spawners {
    ParticleSpawner spawners[MAX_SPAWNERS];
};

layout(std430, binding = 2) buffer Particles {
    ParticleData particles[];
};

uniform float time;

void main() {
    uint spawnerIndex = gl_GlobalInvocationID.x;

    ParticleSpawner spawner = spawners[spawnerIndex];
    uint particlesToSpawn = spawner.spawnCount;

    for (uint i = 0; i < particlesToSpawn; ++i) {
        uint particleIndex = spawner.particleWriteOffset + i;
        particles[particleIndex] = spawnParticle(spawner, uint(time) ^ gl_GlobalInvocationID.x);
    }
}