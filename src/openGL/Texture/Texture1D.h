#pragma once
#include "gl/glew.h"
#include "TextureGeneral.h"
#include "openGL/DataTypeEnum.h"
#include "TextureEnum.h"

class Texture1D {
    unsigned texture = 0;
public:
    Texture1D() = default;

    Texture1D(const Texture1D&) = delete;
    Texture1D& operator = (const Texture1D&) = delete;

    Texture1D(Texture1D&& other) noexcept : texture(other.texture) {
        other.texture = 0;
    }

    Texture1D& operator = (Texture1D&& other) noexcept {
        if (this != &other) {
            discard();
            texture = other.texture;
            other.texture = 0;
        }
        return *this;
    }

    ~Texture1D() {
        discard();
    }

    void discard() {
        if (texture) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
    }

    void allocate(ColorBufferInternalFormat internalFormat, ColorBufferFormat format, GLType type,
        const int width, const void* pixels,
        TextureMinFilter minFilter, TextureMagFilter magFilter,
        TextureWrap wrap, const bool mipmap, const bool anisotropic = false,
        const float anisotropy = 0.0f)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_1D, texture);
        glTexImage1D(GL_TEXTURE_1D, 0, static_cast<GLenum>(internalFormat), width, 0, static_cast<GLenum>(format), static_cast<GLenum>(type), pixels);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));

        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_1D);
            if (anisotropic) {
                glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
            }
        }
    }

    unsigned id() const {
        return texture;
    }

    void bind(const int unit = -1) {
        if (unit != -1) {
            glActiveTexture(GL_TEXTURE0 + unit);
        }
        glBindTexture(GL_TEXTURE_1D, texture);
    }
};