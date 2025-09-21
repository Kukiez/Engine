#pragma once

#include <gl/glew.h>
#include <openGL/BufferObjects/BufferGeneral.h>

class VertexBufferObject {
    long long bytes = 0;
    unsigned VBO = 0;
    BufferUsage usage;
public:
    VertexBufferObject() = default;

    VertexBufferObject(const size_t bytes, const BufferUsage usage) {
        allocate(bytes, usage);
    }

    VertexBufferObject(const VertexBufferObject& other) = delete;
    VertexBufferObject& operator=(const VertexBufferObject& other) = delete;

    VertexBufferObject(VertexBufferObject&& other) noexcept
    : bytes(other.bytes), VBO(other.VBO) {
        other.VBO = 0;
    }


    VertexBufferObject& operator=(VertexBufferObject&& other) noexcept {
        if (this != &other) {
            VBO = other.VBO;
            bytes = other.bytes;
            other.VBO = 0;
        }
        return *this;
    }

    ~VertexBufferObject() {
        if (VBO)
        glDeleteBuffers(1, &VBO);
    }

    void discard() {
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }
    void allocate(const size_t bytes, BufferUsage usage, const void* data = 0) {
        this->bytes = static_cast<long long>(bytes);
        this->usage = usage;

        discard();
        glGenBuffers(1, &VBO);
        Buffer::bind(BufferTarget::VERTEX_BUFFER_OBJECT, VBO);
        glBufferData(GL_ARRAY_BUFFER, this->bytes, data, static_cast<GLenum>(usage));
    }

    /**
     * Requires the appropriate VertexBufferObject to be bound.
     * \n Subsequent calls will completely overwrite the buffer.
     */
    void uploadData(const void* data) const {
        glBufferSubData(GL_ARRAY_BUFFER, 0, this->bytes, data);
    }

    void bind() const {
        Buffer::bind(BufferTarget::VERTEX_BUFFER_OBJECT, VBO);
    }

    BufferMetadata metadata(unsigned storageID) const {
        return {
            .target = BufferTarget::VERTEX_BUFFER_OBJECT,
            .usage = usage,
            .ID = VBO,
            .storageID = storageID,
            .buffer = { .bytes = bytes}
        };
    }
};