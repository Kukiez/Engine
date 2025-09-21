#pragma once

#include <gl/glew.h>
#include <openGL/Texture/TextureLoader.h>
#include "openGL/DataTypeEnum.h"

class Texture3D {
    TextureFormat format{};
    unsigned texture = 0;
    int width = 0, height = 0, depth =0;
    int _unit = 0;

    void genTexture3D(TextureFormat format, GLType type, const void* pixels) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);

        glTexImage3D(GL_TEXTURE_3D, 0, static_cast<int>(format),
            width, height, depth, 0, (int) format,
            (GLenum) type, pixels);
    }
public:
    Texture3D() = default;

    Texture3D(glm::vec3 dimensions, TextureFormat format, GLType type, const void* pixels, const int unit = 0) {
        allocate(dimensions, format, type, pixels, unit);
    }

    Texture3D(const Texture3D&) = delete;
    Texture3D& operator = (const Texture3D&) = delete;

    Texture3D(Texture3D&& other) noexcept 
        : format(other.format), texture(other.texture), 
          width(other.width), height(other.height), depth(other.depth), 
          _unit(other._unit) {
        other.texture = 0;
    }

    Texture3D& operator=(Texture3D&& other) noexcept {
        if (this != &other) {
            format  = other.format;
            texture = other.texture;
            width   = other.width;
            height  = other.height;
            depth   = other.depth;
            _unit   = other._unit;

            other.texture = 0;
        }
        return *this;
    }

    ~Texture3D() {
        glDeleteTextures(1, &texture);
    }

    Texture3D& allocate(glm::vec3 dimensions, TextureFormat format, GLType type, const void* pixels, const int unit = 0) {
        width  = (int) dimensions.x;
        height = (int) dimensions.y;
        depth  = (int) dimensions.z;
        _unit  = unit;

        this->format = format;
        genTexture3D(format, type, pixels);

        return *this;
    }

    Texture3D& allocate(TextureData texData, TextureFormat format, GLType type, const int unit = 0) {
        width  = texData.width;
        height = texData.height;
        depth  = texData.depth;
        _unit  = unit;

        this->format = format;
        genTexture3D(format, type, texData.pixels);

        return *this;
    }


    void setWrap(TextureWrap horizontal, TextureWrap vertical, TextureWrap depth) {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(horizontal));
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(vertical));
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, static_cast<GLenum>(depth));
    }

    void setWrap(TextureWrap wrap) {
        setWrap(wrap, wrap, wrap); 
    }

    void setFilter(TextureMinFilter min, TextureMagFilter mag) {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(min));
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mag));
    }

    void bind() {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(GL_TEXTURE_3D, texture);
    }

    unsigned id() const {
        return texture;
    }

    unsigned unit() const {
        return _unit;
    }
};
