#pragma once
#include <GL/glew.h>
#include <openGL/BufferObjects/VertexBufferObject.h>
#include <openGL/Texture/TextureLoader.h>
#include "openGL/Texture/TextureGeneral.h"

class Cubemap {
    unsigned cubemap = 0;
    int width, height;

    static void setAnisotropy() {
        float maxAnisotropy = 0.f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
    }
public:
    Cubemap() = default;

    Cubemap(const Cubemap& skybox) = delete;
    Cubemap& operator=(const Cubemap& skybox) = delete;

    Cubemap(Cubemap&& skybox) noexcept : cubemap(skybox.cubemap), width(skybox.width), height(skybox.height) {
        skybox.cubemap = 0;
    }

    Cubemap& operator=(Cubemap&& skybox) noexcept {
        if (this != &skybox) {
            cubemap = skybox.cubemap;
            width = skybox.width;
            height = skybox.height;
            skybox.cubemap = 0;
        }
        return *this;
    }

    ~Cubemap() {
        if (cubemap != 0) glDeleteTextures(1, &cubemap);
    }

    int mipmaps() const {
        return 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));
    }

    void generateMipmaps() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void storage(ColorBufferInternalFormat internalFormat, const int width, const int height,
        TextureMinFilter minFilter, TextureMagFilter magFilter,
        TextureWrap wrap, const bool mipmap, const bool anisotropic)
    {
        this->width = width;
        this->height = height;
        discard();
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        const int levels = mipmap ? 1 + static_cast<int>(std::floor(std::log2(std::max(width, height)))) : 1;
        glTexStorage2D(GL_TEXTURE_CUBE_MAP, levels, static_cast<GLenum>(internalFormat), width, height);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrap));

        if (anisotropic) {
            setAnisotropy();
        }
    }

    void allocate(ColorBufferInternalFormat iformat, ColorBufferFormat format,
        GLType type, const int width, const int height,
        TextureMinFilter minFilter, TextureMagFilter magFilter,
        TextureWrap wrap, const bool mipmap, const bool anisotropic,
        const float maxAniso, const std::array<TextureData, 6>& faces)
    {
        this->width = width;
        this->height = height;
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        for (unsigned i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, static_cast<GLenum>(iformat),
                width, height, 0,
                static_cast<GLenum>(format), static_cast<GLenum>(type), faces[i].pixels);

            if (mipmap) {
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                if (anisotropic) {
                    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrap));
        }
    }

    void bind(const int unit = -1) const {
        if (unit != -1) glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    }

    unsigned getCubemap() const {
        return cubemap;
    }

    void discard() {
        if (cubemap != 0) glDeleteTextures(1, &cubemap);
        cubemap = 0;
    }
};

enum class CubemapFace {
    POS_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    NEG_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    POS_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    NEG_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    POS_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    NEG_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};