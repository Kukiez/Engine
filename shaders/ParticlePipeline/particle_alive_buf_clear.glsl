#version 460 core

layout(local_size_x = 1) in;

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

layout(std430, binding = 5) buffer IndirectDispatchBuffer {
    uint numGroupsX;
    uint numGroupsY;
    uint numGroupsZ;
};

uniform int u_count;

void main() {
    uint workgroup = gl_GlobalInvocationID.x;

    if (workgroup == 0) {
        count = u_count;

        alive = instanceCount;
        numGroupsX = (instanceCount + 31u) / 32u;
        numGroupsY = 1u;
        numGroupsZ = 1u;

        instanceCount = 0u;
        firstIndex = 0u;
        baseVertex = 0u;
        baseInstance = 0u;
    }
}