#pragma once

#include "TextureEnum.h"
#include <glm/vec3.hpp>

class TextureMetadata {
    unsigned dataUUID;

    TextureTarget _target{};
    TextureFormat _format{};
    unsigned _textureID = 0;
    int _size = 0;
    glm::vec3 _dimensions{};

public:
    TextureMetadata() = default;

    TextureMetadata(TextureTarget target, TextureFormat format, unsigned textureID, glm::vec3 dimensions, unsigned size, unsigned uuid)
    : _format(format), _textureID(textureID), _target(target),
    _dimensions(dimensions), dataUUID(uuid), _size(size)
    {}

    unsigned textureID() const {
        return _textureID;
    }
    
    TextureTarget target() const {
        return _target;
    }

    const glm::vec3& dimensions() const {
        return _dimensions;
    }

    unsigned uuid() const {
        return dataUUID;
    }

    unsigned size() const {
        return _size;
    }
};