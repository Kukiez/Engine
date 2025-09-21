#pragma once

#include <GL/glew.h>

#include "BufferGeneral.h"

class UniformBufferObject {
    unsigned UBO = 0;
    unsigned bytes = 0;
public:
    UniformBufferObject() = default;
    
    UniformBufferObject(const UniformBufferObject&) = delete;
    UniformBufferObject(UniformBufferObject&& other) noexcept
        : UBO(other.UBO), bytes(other.bytes) {
        other.UBO = 0;
        other.bytes = 0;
    }

    UniformBufferObject& operator=(const UniformBufferObject&) = delete;
    UniformBufferObject& operator=(UniformBufferObject&& other) noexcept {
        if (this != &other) {
            glDeleteBuffers(1, &UBO);
            UBO = other.UBO;
            bytes = other.bytes;
            other.UBO = 0;
            other.bytes = 0;
        }
        return *this;
    }

    ~UniformBufferObject() {
        glDeleteBuffers(1, &UBO);
    }

    /* Data is required to be equal to bytes */
    void allocate(const unsigned bytes, BufferUsage usage, const void* data = nullptr) {
        this->bytes = bytes;
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, bytes, data, static_cast<GLenum>(usage));
    }

    void uploadData(const void* data, const unsigned bytes) const {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, bytes, data);
    }

    void setToBindingPoint(const short bindingPoint) const {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
    }

    void setToBlockBinding(const unsigned shaderID, const char* uniform, const unsigned bindingPoint) const {
        const GLuint blockIndex = glGetUniformBlockIndex(shaderID, uniform);
        glUniformBlockBinding(shaderID, blockIndex, bindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
    }

    [[nodiscard]] Buffer::MappedData map(const BufferAccessMode mode) const {
        Buffer::bind(BufferTarget::UNIFORM_BUFFER_OBJECT, UBO);
        return {UBO, BufferTarget::UNIFORM_BUFFER_OBJECT, mode};
    }


    [[nodiscard]] Buffer::MappedData map(const BufferAccessBit bit, const unsigned index, const unsigned length, const unsigned bytesPerElement) const {
        Buffer::bind(BufferTarget::UNIFORM_BUFFER_OBJECT, UBO);
        return {UBO, BufferTarget::UNIFORM_BUFFER_OBJECT, index * bytesPerElement, length * bytesPerElement, bit};
    }

    void bind() const {
        Buffer::bind(BufferTarget::UNIFORM_BUFFER_OBJECT, UBO);
    }
};
