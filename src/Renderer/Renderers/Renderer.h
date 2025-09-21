#pragma once

#include <unordered_map>
#include <string>
#include <GL/glew.h>

#include "glm/fwd.hpp"

class Rasterizer;
class Shader;

enum class ClearTarget {
    STENCIL = GL_STENCIL_BUFFER_BIT,
    COLOR = GL_COLOR_BUFFER_BIT,
    DEPTH = GL_DEPTH_BUFFER_BIT,
};

class Renderer {
    Shader* activeShader = nullptr;
    std::unordered_map<std::string, Shader*> ShaderPrograms;

    float maxAniso = 0;
public:
    Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    ~Renderer();

    Shader* getActiveShader() const {
        return activeShader;
    }

    Shader* setActiveShader(const std::string &shader);
    Shader* setActiveShader(Shader* shader);

    Shader* getShader(const char* shader) {
        return ShaderPrograms[shader];
    }

    void clear(ClearTarget target);
    void setViewport(int x, int y, int width, int height);

    float getMaxAnisotropy() const {
        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        return maxAniso;
    }

    void bindUBO(unsigned UBO, unsigned location);
    void bindSSBO(unsigned SBO, unsigned location);
    void bindVAO(const unsigned VAO);

    void bindTexture(unsigned texture, unsigned location);
};
