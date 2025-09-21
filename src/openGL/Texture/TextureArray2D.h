#pragma once

#include <gl/glew.h>
#include <openGL/Texture/TextureEnum.h>
#include <openGL/DataTypeEnum.h>
#include "TextureMetadata.h"
#include <openGL/Texture/TextureLoader.h>
#include <memory/Span.h>
#include "TextureGeneral.h"

class TextureArray2D {
    TextureFormat _format{};
    unsigned textureArray = 0;
    unsigned _unit = 0;
    unsigned _size = 0;
    int _width = 0, _height = 0;
public:
    TextureArray2D() = default;

    TextureArray2D(const TextureArray2D&) = delete;
    TextureArray2D& operator=(const TextureArray2D&) = delete;

    TextureArray2D(TextureArray2D&& other) noexcept
        : textureArray(other.textureArray), _unit(other._unit), _size(other._size),
          _width(other._width), _height(other._height)
    {
        other.textureArray = 0;
    }

    TextureArray2D& operator=(TextureArray2D&& other) noexcept {
        if (this != &other) {
            textureArray = other.textureArray;
            _unit = other._unit;
            _size = other._size;
            _width = other._width;
            _height = other._height;
            other.textureArray = 0;
        }
        return *this;
    }
    
    ~TextureArray2D() {
        std::cout << "[TEXTURE ARRAY DELETE]: " << textureArray << std::endl;
        if (textureArray) glDeleteTextures(1, &textureArray);
    }

    void allocate(TextureFormat format, const int width, const int height, const unsigned layers, const unsigned unit = 0) {
        _unit   = unit;
        _size   = layers;
        _width  = width;
        _height = height;
        _format = format;

        glGenTextures(1, &textureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, static_cast<GLenum>(format), width, height, layers);
    }

    /**
     * Requires the appropriate TextureArray2D to be bound
     */
    void uploadData(const void* pixels, const int width, const int height, const int layer, TextureFormat pixelFormat, GLType type) {
        std::cout << "TextureID: " << textureArray << std::endl;
        std::cout << "Glyph: " << layer << std::endl;
        std::cout << "Width: " << width << std::endl;
        std::cout << "Height: " << height << std::endl;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
            0, 0, 0, layer,
            width, height, 1,
            static_cast<GLenum>(pixelFormat),
            static_cast<GLenum>(type), pixels
        );
    }

    /** Requirements :
     * @param dataPtr equal or longer than layers,
     * with width and height being consistent across all.
     **/

    void allocate(ColorBufferInternalFormat internalFormat, ColorBufferFormat format, GLType type,
        const int width, const int height, const int layers, const MemorySpan<TextureData> dataPtr,
        TextureMinFilter minFilter, TextureMagFilter magFilter,
        TextureWrap wrap, const bool mipmap, const bool anisotropic = false,
        const float anisotropy = 0.0f)
    {
        glGenTextures(1, &textureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

        glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            static_cast<GLenum>(internalFormat),
            width,
            height,
            layers,
            0,
            static_cast<GLenum>(format),
            static_cast<GLenum>(type),
            nullptr
        );

        for (int layer = 0; layer < layers; ++layer) {
            const void* layerData = dataPtr[layer].pixels;

            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                0, 0, layer,
                width,
                height,
                1,
                static_cast<GLenum>(format),
                static_cast<GLenum>(type),
                layerData
            );
        }

        this->_width = width;
        this->_height = height;

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrap));

        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            if (anisotropic) {
                glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
            }
        }
    }

    /**
     * Requires the appropriate TextureArray2D to be bound
     */
    void uploadData(const std::vector<TextureData>& textures, TextureFormat pixelFormat, GLType type, const size_t offset = 0) const {
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
        const int width = textures.front().width;
        const int height = textures.front().height;

        for (size_t i = 0; i < textures.size(); ++i) {
            if (!textures[i].pixels) {
                std::cout << "NULL TEXTURE, SKIPPING" << std::endl;
            }

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                0, 0, 0, offset + i,
                width, height, 1,
                static_cast<GLenum>(pixelFormat),
                static_cast<GLenum>(type), textures[i].pixels);            
        }
        std::cout << "Uploaded " << textures.size() << " textures" << std::endl;
    }

    void setWrap(TextureWrap horizontal, TextureWrap vertical) {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, static_cast<int>(horizontal));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, static_cast<int>(vertical));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void setFilter(TextureMinFilter minFilter, TextureMagFilter magFilter) {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, static_cast<int>(minFilter));
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, static_cast<int>(magFilter));
    }

    void bind() const {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    }

    void unbind() const {
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    unsigned id() const {
        return textureArray;
    }

    unsigned unit() const {
        return _unit;
    }

    unsigned size() const {
        return _size;
    }

    int width() const {
        return _width;
    }

    int height() const {
        return _height;
    }

    TextureMetadata metadata(unsigned uuid) const {
        return TextureMetadata(TextureTarget::TEXTURE_ARRAY_2D, _format, textureArray, glm::vec3(_width, _height, 0), _size, uuid);
    }
};
