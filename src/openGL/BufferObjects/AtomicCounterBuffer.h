#pragma once

#include <gl/glew.h>
#include <openGL/BufferObjects/BufferGeneral.h>

class AtomicCounterBuffer {
    unsigned ACO = 0;
    unsigned* value = nullptr;

public:
    AtomicCounterBuffer() = default;

    ~AtomicCounterBuffer() {
        glDeleteBuffers(1, &ACO);
    }

    void allocate(const long long elements) {
        glGenBuffers(1, &ACO);
        Buffer::bind(BufferTarget::ATOMIC_COUNTER_BUFFER, ACO);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, elements * sizeof(unsigned), nullptr, GL_DYNAMIC_DRAW);
    }

    [[nodiscard]] Buffer::MappedData map(const BufferAccessMode mode) const {
        return {ACO, BufferTarget::ATOMIC_COUNTER_BUFFER, mode};
    }

    void bindBuffer() const {
        Buffer::bind(BufferTarget::ATOMIC_COUNTER_BUFFER, ACO);
    }

    void setToBindingPoint(const int bindingPoint) const {
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, ACO);
    }

    /**
     * Requires the correct atomic buffer to be binded.
     * \n Does not unbind the buffer binded.
     *
     * @short Sets the Atomic Counter values
     */
    void setValue(const long long elements, const unsigned* data) const {
        auto* counterPtr = static_cast<unsigned*>(glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned),
                                                                    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
        memcpy(counterPtr, data, elements * sizeof(unsigned));
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }

    [[nodiscard]] unsigned id() const {
        return ACO;
    }
};