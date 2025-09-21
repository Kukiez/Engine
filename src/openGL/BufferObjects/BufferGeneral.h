#pragma once

#include <gl/glew.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <openGL/DataTypeEnum.h>

enum class BufferUsage {
    STATIC  = GL_STATIC_DRAW,
    DYNAMIC = GL_DYNAMIC_DRAW,
    STREAM  = GL_STREAM_DRAW
};

enum class BufferTarget {
    SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
    VERTEX_ARRAY_OBJECT   = 0x0,
    VERTEX_BUFFER_OBJECT  = GL_ARRAY_BUFFER,
    ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
    ELEMENT_BUFFER_OBJECT = GL_ELEMENT_ARRAY_BUFFER,
    INDIRECT_DRAW_BUFFER = GL_DRAW_INDIRECT_BUFFER,
    INDIRECT_DISPATCH_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
    UNIFORM_BUFFER_OBJECT = GL_UNIFORM_BUFFER
};

enum class BufferAccessMode {
    READ_ONLY  = GL_READ_ONLY,
    WRITE_ONLY = GL_WRITE_ONLY,
    READ_WRITE = GL_READ_WRITE
};

enum class BufferAccessBit {
    READ             = GL_MAP_READ_BIT,
    WRITE            = GL_MAP_WRITE_BIT,
    INVALIDATE_BUFFER= GL_MAP_INVALIDATE_BUFFER_BIT,
    INVALIDATE_RANGE = GL_MAP_INVALIDATE_RANGE_BIT,
    FLUSH            = GL_MAP_FLUSH_EXPLICIT_BIT,
    UNSYNCHRONIZED   = GL_MAP_UNSYNCHRONIZED_BIT,
    PERSISTENT       = GL_MAP_PERSISTENT_BIT,
    COHERENT         = GL_MAP_COHERENT_BIT,
};


enum class DepthBufferFormat {
    DEPTH_16 = GL_DEPTH_COMPONENT16,
    DEPTH_24 = GL_DEPTH_COMPONENT24,
    DEPTH_32 = GL_DEPTH_COMPONENT32,
    DEPTH_32F = GL_DEPTH_COMPONENT32F,

    DEPTH_24_STENCIL_8 = GL_DEPTH24_STENCIL8,
    DEPTH_32F_STENCIL_8 = GL_DEPTH32F_STENCIL8,
};

constexpr BufferAccessBit operator | (BufferAccessBit left, BufferAccessBit right) {
    return static_cast<BufferAccessBit>(static_cast<int>(left) | static_cast<int>(right));
}

constexpr BufferAccessBit operator & (BufferAccessBit left, BufferAccessBit right) {
    return static_cast<BufferAccessBit>(static_cast<int>(left) & static_cast<int>(right));
}

constexpr BufferAccessBit operator~(BufferAccessBit a) {
    return static_cast<BufferAccessBit>(~static_cast<unsigned int>(a));
}

constexpr BufferAccessBit clearBit(BufferAccessBit from, BufferAccessBit bit) {
    return static_cast<BufferAccessBit>(static_cast<int>(from) & ~static_cast<int>(bit));
}


class Buffer {
    static unsigned activeBuffer;
    unsigned buffer = 0;
public:
    class MappedData {
        unsigned buffer = 0;
        BufferTarget target;
        BufferAccessBit access{};
        BufferAccessMode mode{};
        void* data = nullptr;

    public:
        MappedData() = default;
        
        MappedData(const unsigned buffer, BufferTarget target, const unsigned index, const unsigned length, BufferAccessBit access)
        : buffer(buffer), target(target), access(access)
        {
            data = glMapBufferRange(static_cast<GLenum>(target), index, length, static_cast<GLenum>(access));
        }

        MappedData(const unsigned buffer, BufferTarget target, BufferAccessMode mode)
            : buffer(buffer), target(target), mode(mode)
        {
            data = glMapBuffer(static_cast<GLenum>(target), static_cast<GLenum>(mode));
        }

        MappedData(const MappedData& other) = delete;
        MappedData& operator=(const MappedData& other) = delete;

        MappedData(MappedData&& other) noexcept
        : buffer(other.buffer), target(other.target), access(other.access), mode(other.mode), data(other.data) {
            other.data = nullptr;
            other.buffer = 0;
        }

        MappedData& operator = (MappedData&& other) noexcept {
            if (this != &other) {
                data   = other.data;
                buffer = other.buffer;
                target = other.target;
                access = other.access;
                mode   = other.mode;

                other.data = nullptr;
                other.buffer = 0;
            }
            return *this;
        }

        ~MappedData() {
            if (buffer) {
                if (activeBuffer != buffer) {
                    bind(target, buffer);
                }
                glUnmapBuffer(static_cast<GLenum>(target));
            }
        }

        template <typename T = void>
        T* get(const size_t bytesOffset = 0) const {
            if (!data) {
                std::cerr << "[WARNING] MappedData is null" << std::endl;
            }
            if constexpr (!std::is_same_v<T, void>) assert(bytesOffset % alignof(T) == 0);
            return reinterpret_cast<T*>(static_cast<char*>(data) + bytesOffset);
        }

        void finish() {
            glBindBuffer(buffer, static_cast<GLenum>(access));
            glUnmapBuffer(static_cast<GLenum>(target));
        }
    };

    static void bind(BufferTarget target, const unsigned buffer) {
        glBindBuffer(static_cast<GLenum>(target), buffer);
        activeBuffer = buffer;
    }

    static unsigned getActiveBuffer() {
        return activeBuffer;
    }

    Buffer(const unsigned buff) : buffer(buff) {}

    operator GLuint() const {
        return buffer;
    }
};
inline void unbindBuffer(BufferTarget target) {
    if (target == BufferTarget::VERTEX_ARRAY_OBJECT)
        glBindVertexArray(0);
    else
        glBindBuffer(static_cast<GLenum>(target), 0);
}


struct BufferMetadata {
    BufferTarget target;
    BufferUsage usage;
    unsigned ID;
    unsigned storageID;

    union {
        long long bytes;       // Target: VBO::bytes
        unsigned attributes;   // Target: VAO::attributes
        unsigned indices;      // Target: EBO::indices 
    } buffer;
};


/**
 * Order of Initialization:
 * 1. vertexSize  - Total size of vertex data (in bytes)
 * 2. vertexCount - Number of vertices
 * 3. vertexData  - Pointer to vertex data (can be nullptr for streaming)
 * 4. attributes  - Vertex attributes
 * 5. useIndices  - Whether an EBO is used
 * 6. indexSize   - Total size of index data (in bytes)
 * 7. indexCount  - Number of indices
 * 8. indexData   - Pointer to index data (nullptr if not used)
 * 9. indexType   - Type of indices (Not Used Yet)
 * 10. usage      - Usage hint for VBO & EBO
 */
struct BufferCreateInfo {
    /**
     * Order of Initialization:
     * 1. index
     * 2. size
     * 3. type
     * 4. normalized
     * 5. stride
     * 6. offset
     * 7. bytesOfType
     */
    struct Attribute {
        uint32_t index;   // Shader attribute location
        int size;         // Components per vertex (e.g., 3 for vec3)
        GLType type;      // Data type (e.g., GL_FLOAT)
        bool normalized;  // Should OpenGL normalize?
        size_t stride;    // Stride between attributes 
        size_t offset;    // Byte offset in the buffer 
        int bytesOfType; // Size of the data type (e.g., sizeof(float))
    };

    // Vertex Buffer
    size_t vertexSize;    // Total size of vertex data (in bytes)
    size_t vertexCount;   // Number of vertices
    void* vertexData;     // Pointer to vertex data (can be nullptr for streaming)

    std::vector<Attribute> attributes; // Vertex attributes

    // Index Buffer (EBO)
    bool useIndices = false;  // Whether an EBO is used
    size_t indexCount;         // Number of indices
    size_t bytesPerIndex; // Size of each index (in bytes)
    void* indexData;          // Pointer to index data (nullptr if not used)

    BufferUsage usage; // Usage hint for VBO & EBO
};