
#include "Renderer.h"
#include <openGL/shaders/ShaderProgramsInitializer.h>
#include "Rasterizer/Rasterizer.h"
#include <openGL/shaders/Shader.h>
#include <openGL/shaders/UniformData.h>
#include <util/stacktrace.h>

#include "Math/Shapes/AABB.h"
#include "Math/Shapes/AABB.h"

Renderer::~Renderer() {
    for (auto it = ShaderPrograms.begin(); it != ShaderPrograms.end(); ++it) {
        delete it->second;
    }
}

Shader* Renderer::setActiveShader(const std::string &shader) {
    if (const auto key = ShaderPrograms.find(shader); key != ShaderPrograms.end()) {
        activeShader = key->second;
        activeShader->setActiveShader();
        return activeShader;
    }
    std::cout << "[ERROR]: Shader \"" << shader << "\" not found!" << std::endl;
    printStackTrace();
    return nullptr;

}

Shader* Renderer::setActiveShader(Shader *shader) {
    activeShader = shader;
    activeShader->setActiveShader();
    return activeShader;
}

void Renderer::clear(ClearTarget target) {
    glClear(static_cast<GLenum>(target));
}

void Renderer::setViewport(const int x, const int y, const int width, const int height) {
    glViewport(x, y, width, height);
}

void Renderer::bindUBO(const unsigned UBO, const unsigned location) {
    glBindBufferBase(GL_UNIFORM_BUFFER, location, UBO);
}

void Renderer::bindSSBO(unsigned SBO, unsigned location) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, SBO);
}

void Renderer::bindVAO(const unsigned VAO) {
    glBindVertexArray(VAO);
}

void Renderer::bindTexture(unsigned texture, unsigned location) {
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, texture);
}
