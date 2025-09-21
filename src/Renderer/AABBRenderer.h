#pragma once
#include "Common.h"
#include <Renderer/RendererOps.h>

struct AABBRendererCollector : Reads<AABBCollision, Transform, StaticCollider>, Stages<LevelOutStage> {
    static void onLevelOut(LevelOutView<AABBRendererCollector> level) {
        level.query<Transform, AABBCollision, StaticCollider>().forEach([&](Entity e, const Transform& transform, const AABBCollision& aabb, const StaticCollider&) {
            auto worldAABB = aabb.world(transform);
            level.send<AABB>(worldAABB);
        });
    }
};

struct AABBRenderer :
    Reads<AABBCollision>,
    ReadsResources<RenderingSystem, GeometrySystem>
{
    struct AABBInstance {
        glm::mat4 model;
        glm::vec4 color;
    };

    ShaderStorageBuffer aabbs;
    size_t aabbsCount = 0;

    void onRendererLoad(RendererLoadView<AABBRenderer> level) {
        level.loadGeometry("assets/geometry/Cube.gltf", "Cube");
        level.loadShader("shaders/Debug/AABB/aabb_vertex.glsl", "shaders/Debug/AABB/aabb_frag.glsl", "AABBShader");
    }

    void onRendererIn(RendererInView<AABBRenderer> level) {
        auto in = level.read<AABB>();

        if (in.empty()) {
            aabbsCount = 0;
            return;
        }
        aabbsCount = in.size();

        std::vector<AABBInstance> out;
        out.resize(in.size());

        for (size_t i = 0; i < in.size(); ++i) {
            out[i].model = glm::translate(glm::mat4(1.0f), in[i].center);
            out[i].model = glm::scale(out[i].model, glm::vec3(3));

            out[i].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        aabbs.allocate(out.size() * sizeof(AABBInstance), BufferUsage::STREAM, out.data());
    }

    void onRender(ForwardRenderView<AABBRenderer> view) const {
        if (aabbsCount == 0) {
            return;
        }

        Shader& shader = view.setActiveShader("AABBShader");

        UniformData Uniforms(shader.id());

        auto& geometry = view.getGeometry("Cube");
        geometry.VAO.bind();
        aabbs.setToBindingPoint(1);
        glLineWidth(3.0f);
        glDrawElementsInstanced(GL_TRIANGLES, geometry.EBO.indices(), GL_UNSIGNED_INT, nullptr, aabbsCount);
    }
};