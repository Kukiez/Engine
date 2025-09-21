#pragma once
#include <gl/glew.h>
#include <openGL/Texture/TextureLoader.h>
#include "openGL/DataTypeEnum.h"

#include <openGL/Texture/TextureMetadata.h>
#include "TextureGeneral.h"

enum class TextureType {
    DIFFUSE,
    EMISSIVE,
    NORMAL,
    ROUGHNESS,
    SPECULAR,
    MAX_TEXTURE_TYPES
};

inline std::string textureTypeToString(const TextureType type) {
    switch (type) {
        case TextureType::DIFFUSE:
            return "diffuse";
        case TextureType::EMISSIVE:
            return "emissive";
        case TextureType::NORMAL:
            return "normal";
        case TextureType::ROUGHNESS:
            return "roughness";
        case TextureType::SPECULAR:
            return "specular";
        default:
            return "";
    }
}

class Texture2D {
    std::string _path{};
    std::string _type{};

    TextureFormat format{};
    unsigned texture = 0;
    int width = 0, height = 0;
    int unit = 0;
public:
    Texture2D() = default;

    Texture2D(const TextureData& tex, const TextureFormat format, const GLType type = GLType::FLOAT, const int unit = 0)
    {
        allocate(tex, format, type, unit);
    }

    explicit Texture2D(const char* path, const bool flip = false) {
        allocate(path, flip);
        setDefaultFilterwrap();
    }

    Texture2D(TextureType type, const char* path, const bool flip = false, const bool blend = false) {
        allocate(type, path, flip, blend);
    }

    Texture2D(const Texture2D& other) = delete;
    Texture2D& operator=(const Texture2D& other) = delete;

    Texture2D(Texture2D&& other) noexcept
    : _path(other._path), _type(other._type), format(other.format),
    texture(other.texture), width(other.width), height(other.height), unit(other.unit)
    {
        other.texture = 0;
    }

    unsigned orphan() {
        const unsigned result = texture;
        texture = 0;
        return result;
    }

    Texture2D& operator=(Texture2D&& other) noexcept {
        if (this != &other) {
            texture = other.texture;
            width   = other.width;
            height  = other.height;
            unit    = other.unit;
            format  = other.format;
            _path   = other._path;
            _type   = other._type;
            other.texture = 0;
        }
        return *this;
    }

    ~Texture2D() {
        if (texture != 0) glDeleteTextures(1, &texture);
    }

    void allocate(const char* path, const bool flip) {
        const TextureData texData = TextureLoader::load(path, flip);

        allocate(texData, texData.format);
    }

    void allocate(TextureFormat internalFormat, TextureFormat format, GLType type, int width, int height, void* pixels) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(internalFormat), width, height, 0, (GLenum)format, (GLenum)type ,pixels);

        this->width = width;
        this->height = height;
    }

    void allocate(ColorBufferInternalFormat internalFormat, ColorBufferFormat format, GLType type,
        const int width, const int height, const void* pixels,
        TextureMinFilter minFilter, TextureMagFilter magFilter,
        TextureWrap wrap, const bool mipmap, const bool anisotropic = false,
        const float anisotropy = 0.0f)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(internalFormat), width, height, 0, static_cast<GLenum>(format), static_cast<GLenum>(type), pixels);

        this->width = width;
        this->height = height;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));

        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
            if (anisotropic) {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
            }
        }
    }

    void allocate(TextureType type, const char* path, const bool flip = false, const bool blend = false) {
        const TextureData tex = TextureLoader::load(path, flip, blend ? 4 : 0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        this->width = tex.width;
        this->height = tex.height;
        switch (type) {
            case TextureType::DIFFUSE:
                if (blend) {
                    glTexImage2D(GL_TEXTURE_2D, 0,
                        GL_RGBA8, width, height, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, tex.pixels);
                } else {
                    glTexImage2D(GL_TEXTURE_2D, 0,
                        GL_RGB8, width, height, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, tex.pixels);
                }
                break;
            case TextureType::NORMAL:
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GL_RGB8, width, height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, tex.pixels);
                break;
            case TextureType::EMISSIVE:
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GL_RGB8, width, height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, tex.pixels);
                break;
            case TextureType::ROUGHNESS:
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GL_RGB8, width, height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, tex.pixels);
                break;
            default: break;
        }
        setDefaultFilterwrap();
    }

    void allocate(const TextureData& tex, TextureFormat format, GLType type = GLType::UNSIGNED_BYTE, const int unit = 0) {
        width = tex.width;
        height = tex.height;
        this->unit = unit;
        this->format = format;
        _path = tex.path;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            static_cast<int>(format),
            width, height,
            0, static_cast<int>(tex.format), static_cast<GLenum>(type), tex.pixels);
    }

    void discard() {
        if (texture != 0) glDeleteTextures(1, &texture);
        texture = 0;
    }

    void allocate(const unsigned char* pixels, TextureFormat format, const int width, const int height, GLType type = GLType::UNSIGNED_BYTE, const int unit = 0) {
        this->width = width;
        this->height = height;
        this->format = format;
        this->unit = unit;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (format == TextureFormat::RGBA) {
            glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGBA8,
                width, height,
                0, static_cast<int>(format), static_cast<GLenum>(type), pixels);
        } else if (format == TextureFormat::RGB) {
            glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGB8,
                width, height,
                0, static_cast<int>(format), static_cast<GLenum>(type), pixels);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0,
            static_cast<int>(format),
            width, height,
            0, static_cast<int>(format), static_cast<GLenum>(type), pixels);
        }
    }

    void allocate(const unsigned char* pixels, TextureFormat iformat, TextureFormat format, const int width, const int height, GLType type = GLType::UNSIGNED_BYTE, const int unit = 0) {
        this->width = width;
        this->height = height;
        this->format = format;
        this->unit = unit;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0,
            static_cast<int>(iformat),
            width, height,
            0, static_cast<int>(format), static_cast<GLenum>(type), pixels);
    }

    void setWrap(TextureWrap horizontal, TextureWrap vertical) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<int>(horizontal));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<int>(vertical));
    }

    void setFilter(TextureMinFilter minFilter, TextureMagFilter magFilter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<int>(minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<int>(magFilter));
    }

    Texture2D& setDefaultFilterwrap() {
        glGenerateMipmap(GL_TEXTURE_2D);
        setWrap(TextureWrap::REPEAT, TextureWrap::REPEAT);
        setFilter(TextureMinFilter::NEAREST_MIPMAP_NEAREST, TextureMagFilter::NEAREST);
        return *this;
    }

    void setMipmap() {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    /**
    * Requires the appropriate Texture2D to be bound
    */
    void uploadData(const void* data) const {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, static_cast<int>(format), GL_FLOAT, data);
    }


    void bind(const unsigned unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    [[nodiscard]] unsigned id() const {
        return texture;
    }

    void setType(const std::string& type) {
        _type = type;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    const std::string& path() const {
        return _path;
    }

    const std::string& type() const {
        return _type;
    }


    TextureMetadata metadata(const unsigned uuid) const {
        return TextureMetadata(TextureTarget::TEXTURE_2D, format, texture, glm::vec3(width, height, 0), 0, uuid);
    }
};
