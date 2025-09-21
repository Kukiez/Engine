#pragma once

#include <gl/glew.h>
#include <iostream>
#include <glm/vec4.hpp>
#include <openGL/shaders/ShaderProgramsInitializer.h>

enum class PolygonMode {
    POINTS    = GL_POINT,
    WIREFRAME = GL_LINE,
    SOLID     = GL_FILL
};

enum class CullMode {
    NONE = 0,
    BACK = GL_BACK,
    FRONT = GL_FRONT,
    FRONT_BACK = GL_FRONT_AND_BACK
};

enum class MultiSampling {
    DISABLED = 0,
    ENABLED = GL_MULTISAMPLE
};

enum class StencilCondition {
    LESS = GL_LESS,
    LESS_OR_EQUAL = GL_LEQUAL,
    EQUAL = GL_EQUAL,
    GREATER = GL_GREATER,
    GREATER_OR_EQUAL = GL_GEQUAL,
    NOT_EQUAL = GL_NOTEQUAL,
    ALWAYS = GL_ALWAYS,
    NEVER = GL_NEVER
};

enum class StencilOperation {
    KEEP = GL_KEEP,
    REPLACE = GL_REPLACE,
    ZERO = GL_ZERO,
    INCREMENT = GL_INCR,
    INCREMENT_WRAP = GL_INCR_WRAP,
    DECREMENT = GL_DECR,
    DECREMENT_WRAP = GL_DECR_WRAP,
    INVERT = GL_INVERT
};


enum class Scope {
    NONE, PERSISTENT, SCOPED
};

class Rasterizer {
public:
    class Scissors {
        friend class Rasterizer;
        Rasterizer* rasterizer;
        Scope scope;

        Scissors(Rasterizer* rasterizer, const Scope scope) : rasterizer(rasterizer), scope(scope) {
            glEnable(GL_SCISSOR_TEST);
        }
    public:
        ~Scissors() {
            if (scope == Scope::SCOPED)
                glDisable(GL_SCISSOR_TEST);
        }
        Scissors(const Scissors&) = delete;
        Scissors& operator=(const Scissors&) = delete;
        Scissors(Scissors&&) = delete;
        Scissors& operator=(Scissors&&) = delete;

        void set(const float x, const float y, const float z, const float w) {
            glScissor(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), static_cast<int>(w));
        }

        void set(const glm::vec4& border) {
            const int yScissor = static_cast<int>(WINDOW_HEIGHT - border.y - border.w);
            glScissor(static_cast<int>(border.x), yScissor, static_cast<int>(border.z), static_cast<int>(border.w));
        }

        void disable() {
            glDisable(GL_SCISSOR_TEST);
        }
    };

    class Blending {
        friend class Rasterizer;
        Rasterizer* rasterizer;
        Scope scope;

        Blending(Rasterizer* rasterizer, const Scope scope) : rasterizer(rasterizer), scope(scope) {
            glEnable(GL_BLEND);
        }

    public:
        Blending(const Blending&) = delete;
        Blending& operator=(const Blending&) = delete;
        Blending(Blending&&) = delete;
        Blending& operator=(Blending&&) = delete;

        ~Blending() {
            if (scope == Scope::SCOPED)
                glDisable(GL_BLEND);
        }

        void setBlendFunc(const GLenum src, const GLenum dst) {
            glBlendFunc(src, dst);
        }
    };


    Scissors enableScissors(const Scope scope) {
        return Scissors(this, scope);
    }

    Blending enableBlending(const Scope scope) {
        return Blending(this, scope);
    }
};