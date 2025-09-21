#pragma once

#include <expected>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gl/glew.h>
#include <iostream>

#include "ShaderCompiler.h"

class Shader {
protected:
    unsigned shader = 0;
public:
    explicit Shader(const unsigned shader) : shader(shader) {}

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : shader(other.shader) {
        other.shader = 0;
    }

    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            shader = other.shader;
            other.shader = 0;
        }
        return *this;
    }

    ~Shader() {
        if (shader) glDeleteProgram(shader);
    }

    static std::expected<Shader, std::string> load(const char* vertexShader, const char* fragShader) {
        if (const unsigned shader = ShaderCompiler::createShaderProgram(vertexShader, fragShader); shader != 0) {
            return Shader(shader);
        }
        return std::unexpected("Shader Program creation failed: " + std::string(vertexShader));
    }

    void setActiveShader() const {
        glUseProgram(shader);
    }

    bool operator == (const Shader& other) const {
        return shader == other.shader;
    }

    int id() const {
        return shader;
    }

    static void destroy(Shader& shader) {
        if (!shader.shader) return;
        glDeleteProgram(shader.shader);
        shader.shader = 0;
    }
};

class ComputeShader : public Shader {
    glm::vec3 threads;
public:
    ComputeShader(const unsigned shader, const glm::vec3 threads) : Shader(shader), threads(threads) {}

    static std::expected<ComputeShader, std::string> load(const char* computeShader, const glm::vec3 threads) {
        if (unsigned shader = ShaderCompiler::createComputeShader(computeShader)) {
            return ComputeShader(shader, threads);
        }
        return std::unexpected("ComputeShader creation failed: " + std::string(computeShader));

    }
    const glm::vec3& getThreads() const {
        return threads;
    }

    void dispatch(const size_t amount) const {
        const int num_groups_x = static_cast<int>((amount + threads.x - 1) / threads.x);

        glDispatchCompute(num_groups_x, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
};