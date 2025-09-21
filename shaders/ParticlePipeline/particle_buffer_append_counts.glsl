#version 460 core

layout(local_size_x = 1) in;

layout(std430, binding = 3) buffer IndirectDrawBuffer {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

layout(std430, binding = 5) buffer IndirectDispatchBuffer {
    uint numGroupsX;
    uint numGroupsY;
    uint numGroupsZ;
};

uniform int particleCount;

void main() {
    uint workgroup = gl_GlobalInvocationID.x;

    if (workgroup == 0) {
        uint newInstances = instanceCount + particleCount;
        instanceCount = newInstances;
        numGroupsX = (newInstances + 31u) / 32u;
    }
}