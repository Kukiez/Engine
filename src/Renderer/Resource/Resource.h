#pragma once
#include <limits>

enum class Texture2DKey : size_t {};
enum class Texture3DKey : size_t {};
enum class TextureArray2DKey : size_t {};
enum class TextureCubeMapKey : size_t {};

enum class ModelKey : size_t {};
enum class ShaderKey : size_t {};

struct GPUBufferTraits {
    uint16_t bufferIndex;
    char* first;
    size_t byteLength;
};

class BufferKey {
    static constexpr auto INVALID = std::numeric_limits<uint16_t>::max();
    uint16_t key = INVALID;
public:
    BufferKey() = default;
    explicit BufferKey(const uint16_t key) : key(key) {}
    explicit BufferKey(const size_t key) : key(key) {}

    operator uint16_t() const {
        return key;
    }

    bool isValid() const {
        return key != INVALID;
    }
};

class SubBufferKey {
    static constexpr auto INVALID = std::numeric_limits<uint16_t>::max();
    uint16_t bufferKey = INVALID;
    uint16_t sliceKey = INVALID;
public:
    SubBufferKey() = default;
    SubBufferKey(const uint16_t buffer, const uint16_t slice) : bufferKey(buffer), sliceKey(slice) {}

    operator uint32_t() const {
        return bufferKey << 16 | sliceKey;
    }

    bool isValid() const {
        return bufferKey != INVALID;
    }

    BufferKey buffer() const {
        return BufferKey(bufferKey);
    }

    uint16_t slice() const {
        return sliceKey;
    }
};

template <typename T>
class BufferView {
    T* dataPtr;
    size_t capacity;
public:
    BufferView() = default;
    BufferView(GPUBufferTraits buffer) {
        dataPtr = reinterpret_cast<T*>(buffer.first);
        capacity = buffer.byteLength / sizeof(T);
    }

    template <typename Index>
    T& operator [] (const Index index) {
        return dataPtr[index];
    }

    T* data() {
        return dataPtr;
    }
};