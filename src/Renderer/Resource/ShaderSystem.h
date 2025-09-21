#pragma once
#include "Resource.h"

enum class ShaderType {
    VERTEX_FRAGMENT, COMPUTE
};

class ShaderSystem {
    union ShaderUnion {
        Shader shader;
        ComputeShader computeShader;

        explicit ShaderUnion(Shader&& shader) : shader(std::move(shader)) {}
        explicit ShaderUnion(ComputeShader&& computeShader) : computeShader(std::move(computeShader)) {}

        ShaderUnion(const ShaderUnion&) = delete;
        ShaderUnion& operator=(const ShaderUnion& other) = delete;

        ShaderUnion& operator=(ShaderUnion&& other) noexcept {
            if (this != &other) {
                memcpy(this, &other, sizeof(*this));
                memset(&other, 0, sizeof(*this));
            }
            return *this;
        }

        ShaderUnion(ShaderUnion&& other) noexcept {
            std::memcpy(this, &other, sizeof(*this));
            std::memset(&other, 0, sizeof(*this));
        }

        ~ShaderUnion() {
            Shader::destroy(shader);
        }
    };
    std::vector<ShaderUnion> shaders;

    std::unordered_map<std::string, std::pair<ShaderType, size_t>> shaderByString;
public:
    ShaderSystem() = default;

    ShaderSystem(const ShaderSystem&) = delete;
    ShaderSystem(ShaderSystem&&) = delete;

    ShaderKey loadShader(const char* vertex, const char* fragment, const char* as) {
        if (auto shader = Shader::load(vertex, fragment); shader.has_value()) {
            shaders.emplace_back(std::move(shader.value()));
            shaderByString.emplace(as, std::make_pair(ShaderType::VERTEX_FRAGMENT, shaders.size() - 1));
            return ShaderKey{shaders.size() - 1};
        } else {
            std::cout << shader.error() << std::endl;
        }
        return ShaderKey{0};
    }

    ShaderKey loadComputeShader(const char* compute, const glm::vec3 threads, const char* as) {
        if (auto shader = ComputeShader::load(compute, threads)) {
            shaders.emplace_back(std::move(shader.value()));
            shaderByString.emplace(as, std::make_pair(ShaderType::COMPUTE, shaders.size() - 1));
            return ShaderKey{shaders.size() - 1};
        } else {
            std::cout << shader.error() << std::endl;
        }
        return ShaderKey{0};
    }

    ShaderKey getShaderKey(const char* as) {
        const auto it = shaderByString.find(as);
        return it != shaderByString.end() ? ShaderKey{it->second.second} : ShaderKey{0};
    }

    Shader& getShader(ShaderKey key) {
        cexpr::require(static_cast<size_t>(key) != 0);
        return shaders[static_cast<size_t>(key)].shader;
    }

    ComputeShader& getComputeShader(ShaderKey key) {
        cexpr::require(static_cast<size_t>(key) != 0);
        return shaders[static_cast<size_t>(key)].computeShader;
    }
};