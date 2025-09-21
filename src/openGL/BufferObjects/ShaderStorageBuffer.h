#pragma once
#include <gl/glew.h>

#include "openGL/shaders/Shader.h"
#include <limits>
#include <cassert>
#include <openGL/BufferObjects/BufferGeneral.h>

class ShaderStorageBuffer {
    int buffer;
    unsigned SSBO = 0;

public:
    ShaderStorageBuffer(): buffer(0) {}

    explicit ShaderStorageBuffer(const int bufferSize, BufferUsage usage, const void* data)
        : buffer(bufferSize)
    {
        allocate(bufferSize, usage, data);
    }

    ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
    ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;

    ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept
        : buffer(other.buffer), SSBO(other.SSBO)
    {
        other.buffer = 0;
        other.SSBO = 0;
    }

    ShaderStorageBuffer& operator=(ShaderStorageBuffer&& other) noexcept {
        if (this != &other) {
            glDeleteBuffers(1, &SSBO);
            buffer = other.buffer;
            SSBO = other.SSBO;

            other.buffer = 0;
            other.SSBO = 0;
        }
        return *this;
    }
    
    ~ShaderStorageBuffer() {
        glDeleteBuffers(1, &SSBO);
    }

    void allocate(const int bytes, BufferUsage usage, const void* data = 0) {
        buffer = bytes;
        if (!SSBO) glGenBuffers(1, &SSBO);
        Buffer::bind(BufferTarget::SHADER_STORAGE_BUFFER, SSBO);

        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(bytes), data, static_cast<unsigned>(usage));
    }

    void overwrite(const int bytes, BufferUsage usage, const void* data) {
        assert(SSBO != 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(bytes), data, static_cast<unsigned>(usage));
    }

    void reallocate(const int bytes, BufferUsage usage) {
        assert(SSBO != 0);

        unsigned newSSBO;
        glGenBuffers(1, &newSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(bytes), nullptr, static_cast<unsigned>(usage));
        glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO);

        glBindBuffer(GL_COPY_READ_BUFFER, SSBO);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, std::min(buffer, bytes));

        glDeleteBuffers(1, &SSBO);

        SSBO = newSSBO;
        buffer = bytes;
    }

    /**
     * Requires the appropriate SSBO to be bound.
     */
    void uploadData(const void* data, const int elements, const int bytesPerElement) {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bytesPerElement * elements, data);
    }

    [[nodiscard]] Buffer::MappedData map(const BufferAccessMode mode) const {
        bindBuffer();
        return {SSBO, BufferTarget::SHADER_STORAGE_BUFFER, mode};
    }

    [[nodiscard]] Buffer::MappedData map(const BufferAccessBit bit, const unsigned elementsOffset, const unsigned bytesPerElement) const {
        bindBuffer();
        return {SSBO, BufferTarget::SHADER_STORAGE_BUFFER, elementsOffset * bytesPerElement, static_cast<unsigned>(buffer) - elementsOffset * bytesPerElement, bit};
    }

    [[nodiscard]] Buffer::MappedData map(const BufferAccessBit bit, const unsigned index, const unsigned length, const unsigned bytesPerElement) const {
        bindBuffer();
        return {SSBO, BufferTarget::SHADER_STORAGE_BUFFER, index * bytesPerElement, length * bytesPerElement, bit};
    }

    void bindBuffer() const {
        Buffer::bind(BufferTarget::SHADER_STORAGE_BUFFER, SSBO);
    }

    void setToBindingPoint(const unsigned bindingPoint) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, SSBO);
    }

    unsigned id() const {
        return SSBO;
    }

    int getAllocatedBytes() const {
        return buffer;
    }
};