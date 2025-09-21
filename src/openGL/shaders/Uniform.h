#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

class Uniform {
public:
    enum class Type {
        INTEGER = 0,
        FLOAT   = 1,
        VEC2    = 2,
        VEC3    = 3,
        VEC4    = 4,
        MAT4    = 16
    };

    union Value {
        const float* ref;
        union {
            long long integer;
            float floating;
            glm::vec2 vec2;
            glm::vec3 vec3;
            glm::vec4 vec4;
            glm::mat4 mat4;            
        };
    };
private:
    Type type;
    uint8_t _amount = 1;
    Value value;
    bool isPointer = false;
public:
    Uniform() = default;

    Uniform(const long long i) : type(Type::INTEGER) {
        value.integer = i;
    }

    Uniform(const int i) : type(Type::INTEGER) {
        value.integer = i;
    }

    Uniform(const float f) : type(Type::FLOAT) {
        value.floating = f;
    }

    Uniform(Type type, uint8_t amount, const float* ref) : type(type), _amount(amount) {
        value.ref = ref;
        isPointer = true;
    }

    template <typename T>
    requires 
        std::is_same_v<std::decay_t<T>, glm::vec2> ||
        std::is_same_v<std::decay_t<T>, glm::vec3> ||
        std::is_same_v<std::decay_t<T>, glm::vec4> ||
        std::is_same_v<std::decay_t<T>, glm::mat4>
    Uniform(T&& t) : type(static_cast<Type>(sizeof(std::decay_t<T>) / 4)) {
        *this = std::forward<T>(t);
    }

    Type activeType() const {
        return type;
    }

    template <typename T>
    const T&    get() const {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned>) {
            return value.integer;
        } else if constexpr (std::is_same_v<T, float>) {
            return value.floating;
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return value.vec2;
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return value.vec3;
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return value.vec4;
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            return value.mat4;
        } else if constexpr (std::is_same_v<T, const float*>) {
            return value.ref;
        }
    }

    template <typename T>
    requires (!std::is_same_v<std::decay_t<T>, Uniform>)
    Uniform& operator = (T&& val) {
        using dT = std::decay_t<T>;

        if constexpr (std::is_same_v<dT, int>) {
            value.integer = std::forward<T>(val);
            type = Type::INTEGER;
        } else if constexpr (std::is_same_v<dT, float>) {
            value.floating = std::forward<T>(val);
            type = Type::FLOAT;
        } else if constexpr (std::is_same_v<dT, glm::vec2>) {
            value.vec2 = std::forward<T>(val);
            type = Type::VEC2;
        } else if constexpr (std::is_same_v<dT, glm::vec3>) {
            value.vec3 = std::forward<T>(val);
            type = Type::VEC3;
        } else if constexpr (std::is_same_v<dT, glm::vec4>) {
            value.vec4 = std::forward<T>(val);
            type = Type::VEC4;
        } else if constexpr (std::is_same_v<dT, glm::mat4>) {
            value.mat4 = std::forward<T>(val);
            type = Type::MAT4;
        } else {
            static_assert(false, "Invalid Uniform Type");
        }
        return *this;
    }

    Uniform(const Uniform& other)
    : type(other.type), _amount(other._amount),
    value(other.value), isPointer(other.isPointer)
    {}

    Uniform& operator = (const Uniform& rhs) {
        this->type = rhs.type;
        _amount = rhs._amount;
        value = rhs.value;
        isPointer = rhs.isPointer;
        return *this;
    }

    const void* raw() const {
        return isPointer ? value.ref : reinterpret_cast<const float*>(&value);
    }

    uint8_t amount() const {
        return _amount;
    }

    void adapt(Type type) {

    }
    void print() const {
        switch (type) {
            case Type::INTEGER:
                std::cout << "int(" << value.integer << ")" << std::endl;
                break;
            case Type::FLOAT:
                std::cout << "float(" << value.floating << ")" << std::endl;
                break;
            case Type::VEC2:
                std::cout << "vec2(" << value.vec2.x << ", " << value.vec2.y << ")" << std::endl;
                break;
            case Type::VEC3:
                std::cout << "vec3(" << value.vec3.x << ", " << value.vec3.y << ", " << value.vec3.z << ")" << std::endl;
                break;
            case Type::VEC4:
                std::cout << "vec4(" << value.vec4.x << ", " << value.vec4.y << ", " << value.vec4.z << ", " << value.vec4.w << ")" << std::endl;
                break;
            case Type::MAT4:
                std::cout << "mat4(" << std::endl;
                for (int i = 0; i < 4; ++i) {
                    std::cout << "  [" << value.mat4[i][0] << ", " << value.mat4[i][1] << ", " 
                                << value.mat4[i][2] << ", " << value.mat4[i][3] << "]" << std::endl;
                }
                std::cout << ")\n";
                break;
            default:
                std::cout << "Unknown Uniform Type" << std::endl;
                break;
        }
    }
};