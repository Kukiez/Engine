#pragma once
#include <gl/glew.h>

enum class ColorAttachment {
    COLOR_0 = 0,
    COLOR_1 = 1,
    COLOR_2 = 2,
    COLOR_3 = 3,
    COLOR_4 = 4,
    COLOR_5 = 5,
    COLOR_6 = 6,
    COLOR_7 = 7,

    GBUFFER_ALBEDO = 0,
    GBUFFER_EMISSIVE = 1,
    GBUFFER_NORMAL = 2,
    GBUFFER_DEPTH = 3,
    GBUFFER_METALLIC_ROUGHNESS = 4
};

class FrameBufferObject {
    unsigned FBO = 0;
    unsigned textures[8];
    unsigned texturesCount = 0;
    unsigned depthTexture = 0;
    unsigned width, height;
public:
    FrameBufferObject() = default;

    void allocateDepthAttachment() {
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    }

    void discard() {
        if (FBO != 0) glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }

    void allocate(const int width, const int height, const int attachments, const bool depthAttachment = true) {
        this->width = width;
        this->height = height;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);;
        glGenTextures(attachments, textures);

        texturesCount = std::clamp(attachments, 0, 8);

        for (unsigned i = 0; i < texturesCount; ++i) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
        }

        if (depthAttachment) {
            allocateDepthAttachment();
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO is not complete!" << std::endl;
    }

    void allocate(const int width, const int height, ColorBufferInternalFormat* internal, ColorBufferFormat* format, GLType* type, const int attachments, const bool depth = true) {
        this->width = width;
        this->height = height;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        texturesCount = std::clamp(attachments, 0, 8);
        glGenTextures(texturesCount, textures);

        for (int i = 0; i < texturesCount; ++i) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(internal[i]), width, height, 0, static_cast<GLenum>(format[i]), static_cast<GLenum>(type[i]), nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
        }
        if (depth) allocateDepthAttachment();

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO is not complete!" << std::endl;
    }

    unsigned getTexture(ColorAttachment attachment) const {
        return textures[static_cast<int>(attachment)];
    }

    unsigned getDepthTexture() const {
        return depthTexture;
    }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, width, height);

        GLenum targets[8];

        for (int i = 0; i < texturesCount; ++i) {
            targets[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(texturesCount, targets);

    }

    unsigned id() const {
        return FBO;
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};


class HDRIObject {
    Cubemap hdri;
    unsigned FBO = 0;
    int width, height;
    bool mipmap = false;

    void allocate(const ColorBufferInternalFormat internal) {
        hdri.storage(internal, width, height,
            mipmap ? TextureMinFilter::LINEAR_MIPMAP_LINEAR : TextureMinFilter::LINEAR,
            TextureMagFilter::LINEAR, TextureWrap::EDGE_CLAMP,
            true, true
        );
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, hdri.getCubemap(), 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "HDRI Framebuffer Failed to create" << std::endl;
        }
    }
public:
    HDRIObject() : width(0), height(0) {}

    HDRIObject(const HDRIObject &other) = delete;
    HDRIObject &operator=(const HDRIObject &other) = delete;

    HDRIObject(HDRIObject &&other) noexcept
        : hdri(std::move(other.hdri)), FBO(other.FBO), width(other.width), height(other.height) {
        other.FBO = 0;
    }

    ~HDRIObject() {
        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
        }
    }

    HDRIObject &operator=(HDRIObject &&other) noexcept {
        if (this != &other) {
            FBO = other.FBO;
            width = other.width;
            height = other.height;
            hdri = std::move(other.hdri);
            other.FBO = 0;
        }
        return *this;
    }

    void allocate(const ColorBufferInternalFormat format, const int width, const int height, const bool mipmap) {
        this->width = width;
        this->height = height;
        this->mipmap = mipmap;
        allocate(format);
    }
    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void setActiveFace(CubemapFace face, const int mip = 0) const /* Requires Framebuffer to be bound */ {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast<GLenum>(face), hdri.getCubemap(), mip);
        const int mipWidth = width * std::pow(0.5f, mip);
        const int mipHeight = mipWidth;
        glViewport(0, 0, mipWidth, mipHeight);
    }

    void discard() /* Erases the current FBO if exists */ {
        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
            FBO = 0;
            hdri.discard();
        }
    }

    const Cubemap& getCubemap() const {
        return hdri;
    }
};