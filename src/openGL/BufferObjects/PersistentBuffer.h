#pragma once
#include <gl/glew.h>

template <GLenum Buffer>
class PersistentBuffer {
    unsigned buffer = 0;
    void* ptr = nullptr;
    size_t bytes = 0;
public:
    PersistentBuffer() = default;

    PersistentBuffer(const size_t bytes) : bytes(bytes) {
        glGenBuffers(1, &buffer);
        glBindBuffer(Buffer, buffer);

        GLbitfield flags = GL_MAP_WRITE_BIT |
                        GL_MAP_PERSISTENT_BIT |
                        GL_MAP_COHERENT_BIT;

        glBufferStorage(Buffer, bytes, nullptr, flags);

        ptr = glMapBufferRange(Buffer, 0, bytes, flags);
    }

    void setToBindingPoint(unsigned loc) {
        glBindBufferBase(Buffer, loc);
    }
    template <typename T>
    T* get() {
        return static_cast<T*>(ptr);
    }

    unsigned id() const {
        return buffer;
    }
};