#pragma once
#include <openGL/BufferObjects/BufferGeneral.h>

class ElementBufferObject {
    int _indices = 0;
    unsigned EBO = 0;
    long long bytes = 0;
    BufferUsage usage;
public:
    ElementBufferObject() = default;

    ElementBufferObject(const ElementBufferObject&) = delete;
    ElementBufferObject& operator=(const ElementBufferObject&) = delete;


    ElementBufferObject(ElementBufferObject&& other) noexcept
    : EBO(other.EBO), bytes(other.bytes), _indices(other._indices)
    {
        other.EBO = 0;
        other.bytes = 0;
    }

    ElementBufferObject& operator=(ElementBufferObject&& other) noexcept {
        if (this != &other) {
            glDeleteBuffers(1, &EBO);
            EBO = other.EBO;
            bytes = other.bytes;
            _indices = other._indices;

            other.EBO = 0;
            other.bytes = 0;
        }
        return *this;
    }

    ~ElementBufferObject() {
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    }

    void discard() {
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    }

    /**
     * Requires the appropriate VertexArrayObject (VAO) to be bound.
     */
    void allocate(const size_t indices, const size_t bytesPerIndex, BufferUsage usage, const void* data = 0) {
        this->bytes = static_cast<long long>(indices * bytesPerIndex);
        _indices = static_cast<int>(indices);
        this->usage = usage;

        discard();
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, static_cast<GLenum>(usage));
    }

    /**
     * Requires the appropriate VertexArrayObject to be bound.
     *\n Subsequent calls will completely overwrite the buffer's data.
     */
    void uploadData(const void* data) const {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytes, data);
    }

    void bindBuffer() const {
        Buffer::bind(BufferTarget::ELEMENT_BUFFER_OBJECT, EBO);
    }


    int indices() const {
        return _indices;
    }

    bool isValid() const {
        return EBO != 0;
    }

    BufferMetadata metadata(unsigned storageID) const {
        return {
            .target = BufferTarget::ELEMENT_BUFFER_OBJECT,
            .usage = usage,
            .ID = EBO,
            .storageID = storageID,
            .buffer = { .indices = static_cast<unsigned>(_indices)}
        };
    }
};
