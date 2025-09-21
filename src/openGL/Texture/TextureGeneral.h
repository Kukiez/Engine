#pragma once

enum class ColorBufferFormat {
    RGBA = GL_RGBA,
    RGB = GL_RGB,
    RED = GL_RED,
    RG = GL_RG
};

constexpr uint8_t channel_count(const ColorBufferFormat format) {
    switch (format) {
        case ColorBufferFormat::RGB:
            return 3;
        case ColorBufferFormat::RGBA:
            return 4;
        case ColorBufferFormat::RED:
            return 1;
        case ColorBufferFormat::RG:
            return 2;
        default: std::unreachable();
    }
}

enum class ColorBufferInternalFormat {
    RGB_8 = GL_RGB8,
    RGBA_8 = GL_RGBA8,
    RGBA_16F = GL_RGBA16F,
    RGB_16F = GL_RGB16F,
    RGBA_32F = GL_RGBA32F,
    RGB_32F = GL_RGB32F,
    RED_32F = GL_R32F,
    RED = GL_RED,
    RGB = GL_RGB,
    RG_16F = GL_RG16F,
    RG_8 = GL_RG8,
    RG_32F = GL_RG32F
};

constexpr uint8_t channel_count(const ColorBufferInternalFormat format) {
    switch (format) {
        case ColorBufferInternalFormat::RGB:
        case ColorBufferInternalFormat::RGB_8:
        case ColorBufferInternalFormat::RGB_16F:
        case ColorBufferInternalFormat::RGB_32F:
            return 3;

        case ColorBufferInternalFormat::RGBA_8:
        case ColorBufferInternalFormat::RGBA_16F:
        case ColorBufferInternalFormat::RGBA_32F:
            return 4;

        case ColorBufferInternalFormat::RED_32F:
        case ColorBufferInternalFormat::RED:
            return 1;

        case ColorBufferInternalFormat::RG_16F:
        case ColorBufferInternalFormat::RG_32F:
        case ColorBufferInternalFormat::RG_8:
            return 2;

        default:
            std::unreachable();
    }
}

constexpr ColorBufferFormat internal_format_to_format(const ColorBufferInternalFormat format) {
    switch (format) {
        case ColorBufferInternalFormat::RGB:
        case ColorBufferInternalFormat::RGB_8:
        case ColorBufferInternalFormat::RGB_16F:
        case ColorBufferInternalFormat::RGB_32F:
            return ColorBufferFormat::RGB;

        case ColorBufferInternalFormat::RGBA_8:
        case ColorBufferInternalFormat::RGBA_16F:
        case ColorBufferInternalFormat::RGBA_32F:
            return ColorBufferFormat::RGBA;

        case ColorBufferInternalFormat::RG_32F:
        case ColorBufferInternalFormat::RG_16F:
        case ColorBufferInternalFormat::RG_8:
            return ColorBufferFormat::RG;

        case ColorBufferInternalFormat::RED_32F:
        case ColorBufferInternalFormat::RED:
            return ColorBufferFormat::RED;
        default: std::unreachable();
    }
}

constexpr GLType internal_format_to_type(const ColorBufferInternalFormat format) {
    switch (format) {
        case ColorBufferInternalFormat::RGB:
        case ColorBufferInternalFormat::RGB_8:
        case ColorBufferInternalFormat::RGBA_8:
        case ColorBufferInternalFormat::RG_8:
        case ColorBufferInternalFormat::RED:
            return GLType::UNSIGNED_BYTE;

        case ColorBufferInternalFormat::RGB_16F:
        case ColorBufferInternalFormat::RGBA_16F:
        case ColorBufferInternalFormat::RG_16F:
            return GLType::HALF_FLOAT;

        case ColorBufferInternalFormat::RG_32F:
        case ColorBufferInternalFormat::RGB_32F:
        case ColorBufferInternalFormat::RGBA_32F:
        case ColorBufferInternalFormat::RED_32F:
            return GLType::FLOAT;

        default:
            std::unreachable();
    }
}