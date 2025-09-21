#pragma once

#include <gl/glew.h>
#include <openGL/BufferObjects/BufferGeneral.h>

template <GLenum Indirect>
class IndirectBuffer {
    unsigned IDB = 0;
    unsigned cmds = 0;
    unsigned bytes = 0;
public:
    IndirectBuffer() = default;

    IndirectBuffer(const IndirectBuffer&) = delete;
    IndirectBuffer& operator=(const IndirectBuffer&) = delete;

    IndirectBuffer(IndirectBuffer&& other) noexcept : IDB(other.IDB), cmds(other.cmds), bytes(other.bytes) {
        other.IDB = 0;
    }

    IndirectBuffer& operator=(IndirectBuffer&& other) noexcept {
        if (this != &other) {
            glDeleteBuffers(1, &IDB);
            IDB = other.IDB;
            cmds = other.cmds;
            bytes = other.bytes;
            other.IDB = 0;
            other.cmds = 0;
        }
        return *this;
    }

    ~IndirectBuffer() {
        if (IDB) glDeleteBuffers(1, &IDB);
    }

    void discard() {
        if (IDB) {
            glDeleteBuffers(1, &IDB);
            IDB = 0;
            cmds = 0;
            bytes = 0;
        }
    }

    IndirectBuffer(const unsigned commands, const unsigned perCommandSize, const void* data) : cmds(commands) {
        allocate(data, perCommandSize, data);
    }

    void allocate(const unsigned commands, const unsigned perCommandSize, const void* data = nullptr, BufferUsage usage = BufferUsage::DYNAMIC) {
        discard();
        cmds = commands;
        glGenBuffers(1, &IDB);
        glBindBuffer(Indirect, IDB);
        glBufferData(Indirect, commands * perCommandSize, data, (GLenum)usage);
        bytes = commands * perCommandSize;
    }

    void reallocate(const unsigned commands, const unsigned perCommandSize) {
        assert(IDB != 0);

        unsigned newIDB;
        glGenBuffers(1, &newIDB);
        glBindBuffer(Indirect, newIDB);
        glBufferData(Indirect, static_cast<GLsizeiptr>(bytes), nullptr, static_cast<unsigned>(BufferUsage::DYNAMIC));
        glBindBuffer(GL_COPY_WRITE_BUFFER, newIDB);

        glBindBuffer(GL_COPY_READ_BUFFER, IDB);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, std::min(bytes, commands * perCommandSize));

        glDeleteBuffers(1, &IDB);
        IDB = newIDB;
        bytes = commands * perCommandSize;
    }

    void setToBindingPoint(const int location) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, IDB);
    }

    void bind() const {
        glBindBuffer(Indirect, IDB);
    }
    
    unsigned size() const {
        return cmds;
    }

    unsigned id() const {
        return IDB;
    }

    unsigned getAllocatedBytes() const {
        return bytes;
    }
};

using IndirectDrawBuffer = IndirectBuffer<GL_DRAW_INDIRECT_BUFFER>;
using IndirectDispatchBuffer = IndirectBuffer<GL_DISPATCH_INDIRECT_BUFFER>;