#pragma once

#include <openGL/BufferObjects/IndirectDrawBuffer.h>
#include <openGL/BufferObjects/ShaderStorageBuffer.h>

#include "RenderInfo.h"
#include <Systems/Mesh/Model.h>
#include <Systems/World/RenderWorld.h>
#include <openGL/shaders/Shader.h>
#include <Systems/ResourceSystem.h>
#include <Systems/World/FrustumCulling.h>
#include <Systems/World/SceneGraph.h>
/*
class MeshRenderer final : public RenderPass::MainPass, public RenderPass::PrePass {
    IndirectDrawBuffer meshIDB;
    ShaderStorageBuffer meshSSBO;

    ShaderStorageBuffer skeletonSSBO;
    ShaderStorageBuffer skeletonMetaSSBO;
    IndirectDrawBuffer skeletonIDB;

    std::vector<Entity> single;
    struct IndirectDrawOffset {
        unsigned offset = 0;
        unsigned count = 0;
    };

    struct ModelDrawCommand {
        std::vector<unsigned> models;
        unsigned ssboOffset = 0;
        std::vector<IndirectDrawOffset> commands;

        size_t size() const {
            return models.size();
        }

        unsigned operator[](const unsigned idx) const {
            return models[idx];
        }
    };
    struct SkeletonDrawCommand {
        unsigned firstMeshIdx = 0;
        unsigned ssboOffset = 0;
        unsigned textureIdxOffset = 0;
        std::vector<IndirectDrawOffset> commands;

        SkeletonDrawCommand(const unsigned first, const unsigned texIdx) : firstMeshIdx(first), textureIdxOffset(texIdx) {}
    };

    struct SkeletonDrawMetadata {
        int boneCount;
        int o;
        int m, n;
    };
    std::unordered_map<const ModelAsset*, ModelDrawCommand> models;
    std::vector<std::pair<const ModelAsset*, SkeletonDrawCommand>> skeletonModels;
    std::vector<DrawElementsIndirectCommand> tempDraws;

    struct ModelAllocation {
        unsigned index;
        unsigned length;
        unsigned version = 999999;

        ModelAllocation(const unsigned index, const unsigned l) : index(index), length(l) {}
    };

    struct DrawCommand {
        unsigned textureIndex;
        unsigned drawOffset;
        unsigned drawCount;
    };

    struct SequentialDrawBuffer {
        unsigned entity;
        unsigned offset;
        unsigned count;
        const ModelAsset* asset;
        std::vector<DrawCommand> commands;
    };
    std::unordered_map<unsigned, ModelAllocation> modelAllocations;
    std::vector<SequentialDrawBuffer> drawBuffer;
    unsigned nextFree = 0;
    std::unordered_map<const ModelAsset*, std::vector<ModelAllocation*>> skeletalModels;
public:
    MeshRenderer() {
        const DrawElementsIndirectCommand* draw = new DrawElementsIndirectCommand[6000]{};
        const auto mat = new glm::mat4[5000]{};

        meshIDB.allocate(6000, sizeof(DrawElementsIndirectCommand), draw);
        meshSSBO.allocate(sizeof(glm::mat4) * 5000, BufferUsage::DYNAMIC, mat);

        skeletonIDB.allocate(600, sizeof(DrawElementsIndirectCommand), draw);
        skeletonSSBO.allocate(sizeof(glm::mat4) * 5000, BufferUsage::DYNAMIC, mat);
        skeletonMetaSSBO.allocate(sizeof(SkeletonDrawMetadata) * 500, BufferUsage::DYNAMIC);
    }

    void onRenderPrePass(Scene& scene, RenderInfo& info) override {
        models.clear();
        drawBuffer.clear();
        skeletalModels.clear();
        for (const auto& [entity, model] : info.entityManager.view<Model>()) {
            models[&model.getAsset()].models.emplace_back(entity.id());
        }
        {
            meshIDB.bind();
            auto* idbPtr = static_cast<DrawElementsIndirectCommand *>(glMapBufferRange(
                GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand) * 6000,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            meshSSBO.bindBuffer();
            auto* ssboPtr = static_cast<glm::mat4 *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * 5000,
                                                                      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            if (!ssboPtr) {
                std::cout << "SSBO IS NULLPTR" << std::endl;
                std::exit(1);
            }
            unsigned idbOffset = 0;
            unsigned ssboOffset = 0;

            for (auto& [modelType, modelEntities] : models) {
                auto& drawInfo = modelType->meshDrawCommands;
                auto& idbCommands = modelEntities.commands;
                idbCommands.assign(modelType->texture.size(), {});

                std::vector<DrawElementsIndirectCommand> indices;

                int fIdx = 0;
                int previousTextureGroup = 0;

                for (int meshIdx = 0; meshIdx < drawInfo.size(); ++meshIdx) {
                    if (modelType->meshDrawInfos[meshIdx].second.textureIndex != previousTextureGroup) {
                        idbCommands[previousTextureGroup].count = indices.size();
                        idbCommands[previousTextureGroup].offset = idbOffset;
                        memcpy(idbPtr + idbOffset, indices.data(), sizeof(DrawElementsIndirectCommand) * indices.size());
                        idbOffset += indices.size();
                        indices.clear();
                        previousTextureGroup = modelType->meshDrawInfos[meshIdx].second.textureIndex;
                    }
                    const unsigned baseInstance = ssboOffset + fIdx;

                    for (int entityIdx = 0; entityIdx < modelEntities.size(); ++entityIdx) {
                        const auto entity = modelEntities[entityIdx];
                        const auto& meshEntity = info.entityManager.getComponent<Model&>(entity).getMeshByMeshID(meshIdx + 1); // model is [0]
                        ssboPtr[ssboOffset + fIdx++] = info.entityManager.getComponent<SceneNode&>(meshEntity).finalTransform;
                    }

                    auto& cmd = indices.emplace_back(drawInfo[meshIdx]);
                    cmd.baseInstance = baseInstance;
                    cmd.instanceCount = modelEntities.size();
                }
                if (indices.size() > 0) {
                    idbCommands[previousTextureGroup].count = indices.size();
                    idbCommands[previousTextureGroup].offset = idbOffset;
                    memcpy(idbPtr + idbOffset, indices.data(), sizeof(DrawElementsIndirectCommand) * indices.size());
                    idbOffset += indices.size();
                    indices.clear();
                }
                modelEntities.ssboOffset = ssboOffset;
                ssboOffset += fIdx;
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            meshIDB.bind();
            glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
        }
        {
            unsigned idbOffset = 0;

            skeletonSSBO.bindBuffer();
            auto* ssboPtr = static_cast<glm::mat4 *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * 5000,
                GL_MAP_WRITE_BIT));
            skeletonIDB.bind();
            auto* idbPtr = static_cast<DrawElementsIndirectCommand*>(
                glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0,
                    sizeof(DrawElementsIndirectCommand) * 500,
                    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            skeletonMetaSSBO.bindBuffer();
            auto* metaPtr = static_cast<SkeletonDrawMetadata *>(glMapBufferRange(
                GL_SHADER_STORAGE_BUFFER, 0, sizeof(SkeletonDrawMetadata) * 500,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            auto now = std::chrono::high_resolution_clock::now();
            for (const auto& [entity, model, armature, sceneNode] : scene.getEntityManager().view<Model, Skeleton, SceneNode>()) {
                auto it = modelAllocations.find(entity.id());

                ModelAllocation* block;
                if (it == modelAllocations.end()) {
                    block = &modelAllocations.emplace(entity.id(), ModelAllocation(nextFree, armature.getBoneCount() + 1)).first->second;
                    nextFree += armature.getBoneCount() + 1;
                } else {
                    block = &it->second;
                }
                if (block->version != sceneNode.version) {
                    ssboPtr[block->index] = sceneNode.finalTransform;
                    armature.writeHierarchy(ssboPtr + block->index + 1);
                    block->version = sceneNode.version;
                }
                skeletalModels[&model.getAsset()].emplace_back(block);
            }

            for (auto& [modelType, blocks] : skeletalModels) {
                auto& skeletalDrawCommands = modelType->skeletalMeshDrawCommands;

                unsigned idbOffsetSnapshot = idbOffset;
                unsigned previousTextureIndex = modelType->meshDrawInfos[modelType->skeletalMeshDrawInfoOffset].second.textureIndex;

                SequentialDrawBuffer drawCommand{
                    .entity = 0,
                    .offset = idbOffsetSnapshot,
                    .count = static_cast<unsigned>(blocks.size() * skeletalDrawCommands.size()),
                    .asset = modelType,
                    .commands = {}
                };
                unsigned lastDrawCommandIndex = 0;

                for (int i = 0; i < skeletalDrawCommands.size(); ++i) {
                    for (const auto block : blocks) {
                        idbPtr[idbOffset] = skeletalDrawCommands[i];
                        idbPtr[idbOffset].baseInstance = block->index;
                        idbPtr[idbOffset].instanceCount = 1;

                        new (metaPtr + idbOffset) SkeletonDrawMetadata{
                            .boneCount = static_cast<int>(block->length - 1)
                        };
                        ++idbOffset;
                    }
                    const unsigned textureIndex = modelType->meshDrawInfos[modelType->skeletalMeshDrawInfoOffset + i].second.textureIndex;

                    bool isLastMesh = i + 1 == skeletalDrawCommands.size();
                    if (textureIndex != previousTextureIndex || isLastMesh) {
                        if (isLastMesh) previousTextureIndex = textureIndex;
                        const unsigned drawCount = static_cast<unsigned>(blocks.size() * (i + 1 - lastDrawCommandIndex));
                        const unsigned drawOffset = drawCommand.offset + lastDrawCommandIndex * blocks.size();
                        drawCommand.commands.emplace_back(DrawCommand{
                            .textureIndex = previousTextureIndex,
                            .drawOffset   = drawOffset,
                            .drawCount    = drawCount
                        });
                        lastDrawCommandIndex = i + 1;
                        previousTextureIndex = textureIndex;
                    }
                }

                drawBuffer.emplace_back(drawCommand);
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            skeletonIDB.bind();
            glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
            skeletonSSBO.bindBuffer();
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
    }

    void onRender(RenderInfo& info) override {
        glEnable(GL_BLEND);
        info.renderer.setActiveShader("MeshShader");
        UniformData uniforms(info.renderer.getActiveShader()->id());
        meshSSBO.setToBindingPoint(1);
        meshIDB.bind();

        uniforms.upload_nowrite("emissive", 0);
        uniforms.upload_nowrite("diffuse", 1);
        uniforms.upload_nowrite("roughness", 2);
        uniforms.upload_nowrite("normal", 3);

        for (const auto& [modelType, modelEntities] : models) {
            auto& drawInfo = modelType->meshDrawCommands;
            if (modelEntities.commands.empty()) continue;

            modelType->VAO.bind();

            for (int i = 0; i < modelEntities.commands.size(); ++i) {
                const auto& [offset, count] = modelEntities.commands[i];
                const ModelTexture& tex = modelType->texture[i];
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex.getEmissive());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, tex.getAlbedo());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, tex.getRoughness());
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, tex.getNormal());
                glMultiDrawElementsIndirect(
                    GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(DrawElementsIndirectCommand)), count, 0
                );
            }
        }
    }
}; */

struct MeshTransformUpdate : IBridgeData<MeshTransformUpdate> {
    unsigned entityID;
    glm::mat4 transform;
    const ModelAsset* modelAsset;
    std::string mesh;

    MeshTransformUpdate(const unsigned modelEntityID, const glm::mat4& transform, const ModelAsset* asset, std::string mesh) : entityID(modelEntityID), transform(transform), modelAsset(asset), mesh(std::move(mesh)) {}
};

struct MeshDeleteUpdate : IBridgeData<MeshDeleteUpdate> {};

class MeshRenderer_ : public IEntitySystem {
    ShaderStorageBuffer meshModels;

    struct Index {
        unsigned index;

        operator unsigned() {
            return index;
        }
    };

    struct FreeListIndex {
        unsigned index;
        unsigned length;
    };

    struct MeshItem {
        Index index;
        MeshDrawParams drawInfo;
    };

    struct ModelItem {
        const ModelAsset* asset;
        std::unordered_map<unsigned, MeshItem> meshes;

        ModelItem(const ModelAsset* asset) : asset(asset) {}
    };;
    std::vector<FreeListIndex> freeList;
    std::unordered_map<unsigned /* Entity */, ModelItem> models;
    unsigned next = 0;
public:
    MeshRenderer_() {
        meshModels.allocate(sizeof(glm::mat4) * 500, BufferUsage::DYNAMIC);
        freeList.emplace_back(0, 500);
    }

    unsigned allocate1() {
        return next++;
    }

    void onEnable(Scene &) override {}
    void onDisable(EntityManager &) override {}

    void onUpdate(Scene & scene) override {
        auto& bridge = scene.getRenderingSystem().getBridge();
        for (const auto& [entity, model, _] : scene.getEntityManager().view<Model, PendingDestruction>()) {
            bridge.push(entity.id(), MeshDeleteUpdate{}, Operation::DELETE);
        }
        for (const auto& [entity, model] : scene.getEntityManager().eview<include<Model>, exclude<PendingDestruction>>()) {
            for (int i = 1; i < model.getMeshes().size(); ++i) {
                auto meshEntity = model.getMeshes()[i];
                const auto& pos = scene.get<SceneNode&>(meshEntity).finalTransform;
                bridge.push(meshEntity, MeshTransformUpdate(entity.id(), pos, &model.getAsset(), scene.get<Mesh&>(meshEntity).getName()), Operation::CREATE);
            }
        }
    }

    MeshDrawParams findDrawInfo(const ModelAsset* asset, const std::string& name) {
        return asset->meshDrawParams[asset->meshToIndex.at(name)];
    }

    void onRenderSync(RendererDataBridge &bridge) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshModels.id());
        auto* mapped = static_cast<glm::mat4 *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * 500, GL_MAP_WRITE_BIT));

        for (const auto& update : bridge.take<MeshTransformUpdate>()) {
            auto& [modelID, transform, asset, name] = update.data;

            if (update.op == Operation::CREATE) {
                const auto mdl = models.find(modelID);
                ModelItem* item;
                if (mdl == models.end()) {
                    item = &models.emplace(modelID, asset).first->second;
                } else {
                    item = &mdl->second;
                }
                const auto it = item->meshes.find(update.entity);

                if (it == item->meshes.end()) {
                    const unsigned idx = allocate1();

                    MeshItem meshItem;
                    meshItem.index = Index(idx);
                    meshItem.drawInfo = findDrawInfo(asset, name);
                    std::cout << "Mesh GPU: " << std::endl;
                    meshItem.drawInfo.print(name);
                    mapped[meshItem.index] = transform;

                    item->meshes.emplace(update.entity, meshItem);
                } else {
                    auto& [index, drawInfo] = it->second;
                    mapped[index] = transform;
                }
            }
        }
        for (const auto& update : bridge.take<MeshDeleteUpdate>()) {
            const auto mdl = models.find(update.entity);

            ModelItem& item = mdl->second;

            for (const auto &[index, drawInfo]: item.meshes | std::views::values) {
                freeList.emplace_back(index.index);
            }
            models.erase(mdl);
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    RenderPassContext onRenderNew(const std::unordered_set<glm::ivec3, IVec3Hash> & visibleChunks) {
        RenderPassContext ctx;
        ctx.buffers.emplace_back(BufferHandleTarget::SSBO, meshModels.id(), 1);
        ctx.shader = "MeshShader";

        auto& instances = ctx.instances;

        for (const auto & modelItem: models | std::views::values) {
            for (const auto & [idx, drawParams] : modelItem.meshes | std::views::values) {
                Renderable renderable;
                renderable.VAO = modelItem.asset->VAO.id();

                DrawElementsIndirectCommand cmd;
                cmd.instanceCount = 1;
                cmd.firstIndex = drawParams.indexOffset;
                cmd.baseVertex = drawParams.vertexOffset;
                cmd.count = drawParams.indexCount;
                cmd.baseInstance = idx.index;
                renderable.cmd = cmd;

                auto& textures = modelItem.asset->textures;
                int diffuse = drawParams.material.diffuse;
                int emissive = drawParams.material.emissive;
                int normal = drawParams.material.normal;
                int metallicRoughness = drawParams.material.metallicRoughness;

                diffuse = diffuse != -1 ? textures[diffuse].id() : MeshMaterial::DEFAULT_EMISSIVE.id();
                emissive = emissive != -1 ? textures[emissive].id() : MeshMaterial::DEFAULT_EMISSIVE.id();
                normal = normal != -1 ? textures[normal].id() : MeshMaterial::DEFAULT_NORMAL.id();
                metallicRoughness = metallicRoughness != -1 ? textures[metallicRoughness].id() : MeshMaterial::DEFAULT_ROUGHNESS_METALLIC.id();

                renderable.materials.materials.emplace_back("diffuse",
                    diffuse);
                renderable.materials.materials.emplace_back("emissive",
                    emissive);
                renderable.materials.materials.emplace_back("normal",
                    normal);
                renderable.materials.materials.emplace_back("roughness",
                    metallicRoughness);
                instances.emplace_back(renderable);
            }
        }
        return ctx;
    }
};