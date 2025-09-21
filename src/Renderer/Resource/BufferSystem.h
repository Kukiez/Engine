#pragma once
#include "Resource.h"

class BufferSystem {
    struct GPUBuffer : mem::free_list_allocator_traits {
        unsigned SSBO;

        std::vector<GPUBufferTraits> slices;

        GPUBuffer(unsigned SSBO, char* mapped, const size_t bytes) :
        free_list_allocator_traits(mapped, bytes), SSBO(SSBO) {}
    };

    std::vector<GPUBuffer> buffers;

    GPUBuffer& createGPUBuffer(const size_t capacity) {
        unsigned SSBO;
        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferStorage(
            GL_SHADER_STORAGE_BUFFER,
            capacity,
            nullptr,
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
        );
        auto mapped = static_cast<char*>(glMapBufferRange(
            GL_SHADER_STORAGE_BUFFER,
            0,
            capacity,
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT // remove and flush
        ));
        buffers.emplace_back(SSBO, mapped, capacity);
        return buffers.back();
    }

    GPUBufferTraits allocate(const BufferKey key, const size_t bytes) {
        char* mem = buffers[key].allocate(mem::type_info_of<char>, bytes);
        cexpr::require(mem);
        return GPUBufferTraits{static_cast<uint16_t>(buffers.size() - 1), mem, bytes};
    }

    GPUBuffer& getGPUBuffer(const BufferKey key) {
        if (key.isValid()) {
            return buffers[key];
        }
        cexpr::require(false);
        std::unreachable();
    }
public:
    BufferKey create(const size_t capacity) {
        createGPUBuffer(capacity);

        return BufferKey{buffers.size() - 1};
    }

    GPUBufferTraits getBuffer(const SubBufferKey key) {
        return getGPUBuffer(key.buffer()).slices[key.slice()];
    }

    GPUBufferTraits getBuffer(const BufferKey key) {
        return GPUBufferTraits(key, getGPUBuffer(key).data(), getGPUBuffer(key).total_capacity());
    }

    unsigned getBufferID(const BufferKey key) {
        return getGPUBuffer(key).SSBO;
    }

    BufferSystem() = default;
    BufferSystem(const BufferSystem&) = delete;
    BufferSystem(BufferSystem&&) = delete;
    BufferSystem& operator=(const BufferSystem&) = delete;
    BufferSystem& operator=(BufferSystem&&) = delete;

    void unload() const {
        for (auto& buffer : buffers) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer.SSBO);
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glDeleteBuffers(1, &buffer.SSBO);

            if (buffer.remaining() != buffer.total_capacity()) {
                std::cout << "WARNING: Buffer was not fully deallocated" << std::endl;
            }
        }
    }
};
