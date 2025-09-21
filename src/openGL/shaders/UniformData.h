#pragma once

#include <unordered_map>
#include <string>
#include "Uniform.h"

struct UniformPlaceholder {};

class UniformData {
    std::unordered_map<std::string, Uniform> uniforms;
    std::unordered_map<std::string, Uniform> placeholders;
    unsigned programID = 0;

    void upload(const char* key, const Uniform& uniform);
public:
    UniformData() = default;

    explicit UniformData(const unsigned id) : programID(id) {}

    template <typename... Pairs>
    requires std::conjunction_v<std::is_convertible<std::decay_t<Pairs>, std::pair<std::string, Uniform>>...>
    explicit UniformData(Pairs&&... pairs) {
        (uniforms.insert(
            std::forward<Pairs>(pairs)
        ), ...);
    }

    template <typename String, typename... T>
    requires std::is_convertible_v<std::decay_t<String>, std::string>
    void write(String&& name, T&&... uniform) {
        uniforms.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<String>(name)),
            std::forward_as_tuple(std::forward<T>(uniform)...)
        );
    }

    template <typename String, typename... T>
    requires std::is_convertible_v<std::decay_t<String>, std::string>
    void overwrite(String&& name, T&&... uniform) {
        uniforms[name] = Uniform(std::forward<T>(uniform)...);
    }

    template <typename String, typename... T>
    requires std::is_convertible_v<std::decay_t<String>, std::string>
    void upload_write(String&& name, T&&... uniform) {
        auto u = uniforms.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<String>(name)),
            std::forward_as_tuple(std::forward<T>(uniform)...)
        ).first;

        upload(u->first.c_str(), u->second);
    }

    void upload_nowrite(const char* key, const Uniform& uniform) {
        upload(key, uniform);
    }

    const Uniform& operator [] (const std::string& key) const {
        return uniforms.at(key);
    }

    template <typename String>
    Uniform& operator [] (String&& key) {
        return uniforms[std::forward<String>(key)];
    }

    void upload_all(int programID = -1) {
        if (programID != -1) {
            this->programID = programID;
        }
        
        for (const auto& [key, uniform] : uniforms) {
            upload(key.c_str(), uniform);
        }
    }

    void setProgram(const unsigned id) {
        programID = id;
    }

    auto begin() const {
        return uniforms.begin();
    }

    auto end() const {
        return uniforms.end();
    }

    void print() {
        for (const auto& [key, uniform] : uniforms) {
            std::cout << "Uniform " << key << ": "; uniform.print();
        }
    }

    void clear() {
        uniforms.clear();
    }
};