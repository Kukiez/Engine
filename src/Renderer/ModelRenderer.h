#pragma once
#include "Resource/Resource.h"

struct ModelGraphUpdateSystem;

class ModelUploadSystem : RendererInStage::WritesResources<BufferSystem, ModelSystem> {
    FRIEND_DESCRIPTOR

    BufferKey buffer;
    BufferView<glm::mat4> matrices{};

    ShaderKey modelShader;

    const ModelDefinition* def = nullptr;
    const Model* model;
public:
    ModelUploadSystem() = default;

    void onRendererLoad(RendererLoadView<ModelUploadSystem> view) {
        buffer = view.createBuffer(sizeof(glm::mat4) * 128);
        matrices = view.getBuffer<glm::mat4>(buffer);

        modelShader = view.loadShader("shaders/Mesh/mesh_vertex.glsl", "shaders/Mesh/mesh_frag.glsl", "MeshShader");

        MeshMaterial::createDefaultEmissive();
        MeshMaterial::createDefaultRoughnessMetallic();
        MeshMaterial::createDefaultNormal();
    }

    static void onRendererIn(RendererInView<ModelUploadSystem> view) {
        for (auto& def : view.read<NewModelDefinition>()) {
            view.loadModel(def);
        }
    }

    void onUpload(const Model& model) {
        auto finalTransforms = model.getFinalTransforms();
        memcpy(matrices.data(), finalTransforms.data(), finalTransforms.size() * sizeof(glm::mat4));
        def = model.asset();
        this->model = &model;
    }

    void onRender(ForwardRenderView<ModelUploadSystem> view) {
        if (!def) return;
        const auto& shader = view.setActiveShader("MeshShader");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, view.getBufferID(buffer));

        def->geometry.VAO.bind();

        UniformData uniforms(shader.id());

        GLuint loc = glGetUniformLocation(shader.id(), "matrices");

        glUniformMatrix4fv(
            loc,
            6,
            GL_FALSE,
            (float*)model->getFinalTransforms().data()
        );


        int i = 0;
        for (auto& node : def->nodes) {
            for (auto meshIndex : node.meshes()) {
                auto& [drawParams, materialIndex] = def->renderNodes[meshIndex];
                auto& material = def->materials[materialIndex];

                Texture2D& texture = view.getTexture(def->textures[material.diffuse]);

                texture.bind(0);

                MeshMaterial::DEFAULT_NORMAL.bind(1);
                MeshMaterial::DEFAULT_EMISSIVE.bind(2);
                MeshMaterial::DEFAULT_ROUGHNESS_METALLIC.bind(3);

                uniforms.upload_nowrite("diffuse", 0);
                uniforms.upload_nowrite("normal", 1);
                uniforms.upload_nowrite("emissive", 2);
                uniforms.upload_nowrite("roughness", 3);

                uniforms.upload_nowrite("index", i);
                i++;

                glDrawElementsBaseVertex(
                    GL_TRIANGLES, drawParams.indexCount,
                    GL_UNSIGNED_INT, (void*)(drawParams.indexOffset * sizeof(unsigned int)),
                    drawParams.vertexOffset
                );
            }

        }
        def = nullptr;
    }
};


class ModelSendSystem : Reads<Model>, Dependencies<ModelGraphUpdateSystem> {
public:
    FRIEND_DESCRIPTOR

    static void onLevelOut(LevelOutView<ModelSendSystem> view) {
        view.query<Model>().forEach([&](const Model& model) {
            view.getRendererSystem<ModelUploadSystem>().onUpload(model);
        });
    }
};
