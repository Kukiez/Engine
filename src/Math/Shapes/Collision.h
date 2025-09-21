#pragma once
#include <EntityManager.h>
#include <gl/glew.h>
#include <openGL/BufferObjects/ShaderStorageBuffer.h>
#include <openGL/shaders/Shader.h>
#include <openGL/shaders/UniformData.h>
#include <openGL/shaders/Shapes/GeometryCache.h>
#include <Renderer/Renderer.h>
#include <Systems/Scheduler.h>
#include <Systems/Components/CollisionComponents.h>
#include <util/Color.h>
#include <util/TransformationMatrix.h>

#include "AABB.h"
#include <Core/Renderer/RenderingDetails.h>
#include <Systems/Renderers/RenderInfo.h>

struct Instance {
    glm::mat4 model;
    Color color;

    Instance(const glm::mat4 &model, const Color color) : model(model), color(color) {}
};

class AABBRenderer final : public RenderPass::MainPass, public RenderPass::PrePass {
    static glm::mat4 makeModel3D(const glm::vec3& translation, const glm::vec3& scale) {
        auto model = glm::mat4(1.0f);

        model = glm::translate(model, translation);
        model = glm::scale(model, scale);
        return model;
    }

    static glm::mat4 makeModel3D(const OOBB& oobb) {
        auto model = glm::mat4(1.0f);

        model[0][0] = oobb.orientation[0][0];
        model[0][1] = oobb.orientation[0][1];
        model[0][2] = oobb.orientation[0][2];

        model[1][0] = oobb.orientation[1][0];
        model[1][1] = oobb.orientation[1][1];
        model[1][2] = oobb.orientation[1][2];

        model[2][0] = oobb.orientation[2][0];
        model[2][1] = oobb.orientation[2][1];
        model[2][2] = oobb.orientation[2][2];

        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), oobb.halfSize);
        model = model * scaleMat;

        model = glm::translate(glm::mat4(1.0f), oobb.center) * model;
        return model;
    }

    struct AABBNode {
        AABB aabb;
        Color color;

        bool operator==(const AABB& other) const {
            return aabb == other;
        }
    };

    ShaderStorageBuffer aabbs;
    int writeIndex = 0;

    std::vector<Entity> dynamics;
    std::vector<AABBNode> temporaries;

    std::vector<OOBB> oobbs;
public:
    AABBRenderer() {
        aabbs.allocate(sizeof(Instance) * 1680, BufferUsage::DYNAMIC);
    }

    void onRenderPrePass(Scene& scene, RenderInfo& info) override {
        if (!dynamics.empty() || !temporaries.empty()) {
            if (writeIndex + dynamics.size() + temporaries.size() + 1 >= aabbs.getAllocatedBytes() / sizeof(Instance)) {
                aabbs.reallocate(aabbs.getAllocatedBytes() * 2, BufferUsage::DYNAMIC);
            }
            const auto mapped = aabbs.map(BufferAccessBit::WRITE, writeIndex, dynamics.size() + temporaries.size(), sizeof(Instance));
            auto* writePointer = mapped.get<Instance>();

            for (const auto& entity : dynamics) {
                if (const auto* hitbox = info.entityManager.getComponent<Hitbox>(entity)) {
                    auto [center, halfSize] = hitbox->toWorld(info.entityManager.getComponent<Transform&>(entity));
                    new (writePointer) Instance(makeModel3D(center, halfSize), Color::RED);
                    ++writePointer;
                } else if (const auto* narrow = info.entityManager.getComponent<NarrowHitbox>(entity)) {
                    auto& transform = info.entityManager.getComponent<Transform&>(entity);
                    OOBB oobb = narrow->getOOBB(transform);
                    new (writePointer) Instance(makeModel3D(oobb), Color::BLUE);
                    ++writePointer;
                } else if (const auto* damage = info.entityManager.getComponent<DamageHitbox>(entity)) {
                    auto& transform = info.entityManager.getComponent<Transform&>(entity);
                    OOBB oobb = damage->getOOBB(transform);
                    new (writePointer) Instance(makeModel3D(oobb), Color::BLUE);
                    ++writePointer;
                }
            }

            for (const auto& [aabb, color] : temporaries) {
                new (writePointer) Instance(makeModel3D(aabb.center, aabb.halfSize), color);
                ++writePointer;
            }
        }      
    }

    void clear() {
        writeIndex = 0;
        dynamics.clear();
        temporaries.clear();
    }
    
    void onRender(RenderInfo & info) override {
        auto& renderer = info.renderer;
        renderer.setActiveShader("AABBShader");

        UniformData Uniforms(renderer.getActiveShader()->id());

        glBindVertexArray(GeometryCache::inst().query(GCache::Query::AABB, GCache::Index::VERTEX_ARRAY).ID);
        aabbs.setToBindingPoint(1);
        glLineWidth(3.0f);
        glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, nullptr, writeIndex + dynamics.size() + temporaries.size());
    }

    void addAABB(const AABB &aabb, const Color& color = Color::WHITE) {
        if (writeIndex >= aabbs.getAllocatedBytes() / sizeof(Instance)) {
            aabbs.reallocate(aabbs.getAllocatedBytes() * 2, BufferUsage::DYNAMIC);
        }
        const auto mapped = aabbs.map(BufferAccessBit::WRITE, writeIndex, 1, sizeof(Instance));
        new (mapped.get<Instance>()) Instance(makeModel3D(aabb.center, aabb.halfSize), color);
        writeIndex++;
    }

    void addOOBB(const OOBB& oobb, const Color& color = Color::WHITE) {
        if (writeIndex >= aabbs.getAllocatedBytes() / sizeof(Instance)) {
            aabbs.reallocate(aabbs.getAllocatedBytes() * 2, BufferUsage::DYNAMIC);
        }
        const auto mapped = aabbs.map(BufferAccessBit::WRITE, writeIndex, 1, sizeof(Instance));
        new (mapped.get<Instance>()) Instance(makeModel3D(oobb), color);
        writeIndex++;
    }

    void addDynamic(const Entity& entity, const Color& color = Color::WHITE) {
        dynamics.push_back(entity);
    }

    void addTemporary(const Scene& scene, AABB aabb, const float time, const Color& color = Color::WHITE) {
        temporaries.emplace_back(aabb, color);

        Scheduler::schedule(scene, [this, aabb] {
            temporaries.erase(std::find(temporaries.begin(), temporaries.end(), aabb));
        }, time);
    }

    void removeDynamic(const Entity& entity) {
        if (const auto it = std::ranges::find(dynamics, entity); it != dynamics.end()) {
            dynamics.erase(it);
        }
    }
};

struct Line {
    glm::vec3 from;
    glm::vec3 to;

    bool operator == (const Line& other) const {
        return from == other.from && to == other.to;
    }
};

inline glm::mat4 computeLineModelMatrix(const glm::vec3 from, const glm::vec3 to) {
    glm::vec3 direction = to - from;
    float length = glm::length(direction);

    glm::vec3 dirNorm = glm::normalize(direction);

    glm::vec3 defaultDir = glm::vec3(0, 0, 1);

    glm::vec3 rotationAxis = glm::cross(defaultDir, dirNorm);
    float angle = acos(glm::clamp(glm::dot(defaultDir, dirNorm), -1.0f, 1.0f));

    glm::mat4 rotation = glm::mat4(1.0f);
    if (glm::length(rotationAxis) > 0.0001f)
        rotation = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(rotationAxis));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, length));

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), from);

    return translation * rotation * scale;
}

class LineRenderer final : public RenderPass::MainPass {
    std::vector<Line> dynamics;

    VertexBufferObject VBO;
    VertexArrayObject VAO;

    ShaderStorageBuffer lines;
    int writeIndex = 0;

    DrawArraysInstanced draw;
public:
    LineRenderer() {
        constexpr glm::vec3 points[2] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        };

        VAO.allocate();

        VBO.allocate(sizeof(points), BufferUsage::STATIC);
        VBO.uploadData(points);

        VAO.addAttribute(3, 3, 0);

        lines.allocate(sizeof(Instance) * 64, BufferUsage::STATIC);

        draw.first_vertex = 0;
        draw.vertex_count = 2;
        draw.primitive = GLDrawPrimitive::LINES;
    }

    void onRender(RenderInfo & info) override {
        auto& renderer = info.renderer;

        renderer.setActiveShader("AABBShader");

        UniformData Uniforms(renderer.getActiveShader()->id());

        glBindVertexArray(VAO.id());

        if (!dynamics.empty()) {
            if (writeIndex + dynamics.size() + 1 >= lines.getAllocatedBytes() / sizeof(Instance)) {
                lines.reallocate(lines.getAllocatedBytes() * 2, BufferUsage::DYNAMIC);
            }
            const auto mapped = lines.map(BufferAccessBit::WRITE, writeIndex, dynamics.size(), sizeof(Instance));
            auto* writePointer = mapped.get<Instance>();

            for (const auto& [from, to] : dynamics) {
                new (writePointer) Instance(computeLineModelMatrix(from, to), Color::CYAN);
                ++writePointer;
            }
        }
        lines.setToBindingPoint(1);
        draw.instances = writeIndex + dynamics.size();
        draw.draw();
    }

    void addDynamicLine(const Line& line) {
        dynamics.push_back(line);
    }

    void removeDynamicLine(const Line& line) {
        dynamics.erase(std::ranges::find(dynamics, line));
    }

    void addLine(const Line& line) {
        if (writeIndex >= lines.getAllocatedBytes() / sizeof(Instance)) {
            lines.reallocate(lines.getAllocatedBytes() * 2, BufferUsage::DYNAMIC);
        }
        const auto mapped = lines.map(BufferAccessBit::WRITE, writeIndex, 1, sizeof(Instance));
        new (mapped.get<Instance>()) Instance(computeLineModelMatrix(line.from, line.to), Color::CYAN);
        writeIndex++;
    }
};