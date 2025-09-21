#pragma once

#include <openGL/BufferObjects/IndirectDrawBuffer.h>
#include <openGL/BufferObjects/VertexArrayObject.h>
#include <openGL/BufferObjects/VertexBufferObject.h>
#include <Core/Model/Model.h>
#include <Math/Shapes/geom.h>
#include "Voxel.h"
#include <openGL/BufferObjects/PersistentBuffer.h>
#include <opengl/BufferObjects/ShaderStorageBuffer.h>
#include "Collision/CollisionComponents.h"
#include <Renderer/RenderingSystem.h>
#include <util/glm_double.h>

#include <ECS/ECS.h>

enum class Face {
    X_NEG = 0,
    X_POS = 1,
    Y_NEG = 2,
    Y_POS = 3,
    Z_NEG = 4,
    Z_POS = 5
};

struct QuadVertex {
    glm::vec3 localOffset;
    glm::vec2 uvSize;
};

struct alignas(16) QuadDrawInstance {
    glm::mat4 model;
    int textureID;
    int aFace;
    glm::vec2 uv;

    QuadDrawInstance() = default;

    QuadDrawInstance(const glm::mat4& model, int textureID, Face face, glm::vec2 uv)
        : model(model), textureID(textureID), aFace(int(face)), uv(uv) {}
};

struct Quad {
    AABB aabb;
    Face face;
    unsigned texture;

    Quad(Face axis, const AABB& aabb, unsigned texture)
        : face(axis), aabb(aabb), texture(texture) {}
};

class VoxelWorldSystem : Reads<VoxelVolume> {
    FRIEND_DESCRIPTOR

    static glm::ivec3 worldPosToVoxelChunk(const glm::vec3& pos) {
        return {
            std::floor(pos.x / 16),
            std::floor(pos.y / 16),
            std::floor(pos.z / 16)
        };
    }

    struct alignas(64) Chunk {
        glm::ivec3 coords = glm::ivec3(5, 0, 0);
        Entity voxelVolumeEntity{};
        std::vector<BVHCollisionNode<AABB>> quads;
        std::vector<QuadDrawInstance> drawCommands;
        int startingIndex = 0;
        int commands = 0;
        bool dirty = false;

        Chunk() = default;
        Chunk(glm::ivec3 coords) : coords(coords) {}
    };
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> chunks;

    struct alignas(16) FaceTBNUV {
        glm::vec3 tangent;
        float _;
        glm::vec3 bitangent;
        float __;
        glm::vec3 normal;
        float ___;
        glm::vec2 uv[4];
    };

    static std::array<FaceTBNUV, 6> loadTBNUVFaces() {
        std::array<FaceTBNUV, 6> faces;

        faces[int(Face::X_POS)].normal    = glm::vec3(1, 0, 0);
        faces[int(Face::X_POS)].tangent   = glm::vec3(0, 1, 0);
        faces[int(Face::X_POS)].bitangent = glm::vec3(0, 0, 1);

        faces[int(Face::X_NEG)].normal    = glm::vec3(-1, 0, 0);
        faces[int(Face::X_NEG)].tangent   = glm::vec3(0, 1, 0);
        faces[int(Face::X_NEG)].bitangent = glm::vec3(0, 0, -1);

        faces[int(Face::Y_POS)].normal    = glm::vec3(0, 1, 0);
        faces[int(Face::Y_POS)].tangent   = glm::vec3(1, 0, 0);
        faces[int(Face::Y_POS)].bitangent = glm::vec3(0, 0, 1);

        faces[int(Face::Y_NEG)].normal    = glm::vec3(0, -1, 0);
        faces[int(Face::Y_NEG)].tangent   = glm::vec3(1, 0, 0);
        faces[int(Face::Y_NEG)].bitangent = glm::vec3(0, 0, 1);

        faces[int(Face::Z_POS)].normal    = glm::vec3(0, 0, 1);
        faces[int(Face::Z_POS)].tangent   = glm::vec3(1, 0, 0);
        faces[int(Face::Z_POS)].bitangent = glm::vec3(0, 1, 0);

        faces[int(Face::Z_NEG)].normal    = glm::vec3(0, 0, -1);
        faces[int(Face::Z_NEG)].tangent   = glm::vec3(1, 0, 0);
        faces[int(Face::Z_NEG)].bitangent = glm::vec3(0, 1, 0);

        faces[int(Face::X_POS)].uv[0] = glm::vec2(0, 0);
        faces[int(Face::X_POS)].uv[1] = glm::vec2(0, 1);
        faces[int(Face::X_POS)].uv[2] = glm::vec2(1, 1);
        faces[int(Face::X_POS)].uv[3] = glm::vec2(1, 0);

        faces[int(Face::X_NEG)].uv[0] = glm::vec2(1, 0);
        faces[int(Face::X_NEG)].uv[1] = glm::vec2(1, 1);
        faces[int(Face::X_NEG)].uv[2] = glm::vec2(0, 1);
        faces[int(Face::X_NEG)].uv[3] = glm::vec2(0, 0);

        faces[int(Face::Y_POS)].uv[0] = glm::vec2(1, 0);
        faces[int(Face::Y_POS)].uv[1] = glm::vec2(0, 0);
        faces[int(Face::Y_POS)].uv[2] = glm::vec2(0, 1);
        faces[int(Face::Y_POS)].uv[3] = glm::vec2(1, 1);

        faces[int(Face::Y_NEG)].uv[0] = glm::vec2(0, 0);
        faces[int(Face::Y_NEG)].uv[1] = glm::vec2(0, 1);
        faces[int(Face::Y_NEG)].uv[2] = glm::vec2(1, 1);
        faces[int(Face::Y_NEG)].uv[3] = glm::vec2(1, 0);

        faces[int(Face::Z_POS)].uv[0] = glm::vec2(1, 0);
        faces[int(Face::Z_POS)].uv[1] = glm::vec2(0, 0);
        faces[int(Face::Z_POS)].uv[2] = glm::vec2(0, 1);
        faces[int(Face::Z_POS)].uv[3] = glm::vec2(1, 1);
// -Z
        faces[int(Face::Z_NEG)].uv[0] = glm::vec2(1, 0);
        faces[int(Face::Z_NEG)].uv[1] = glm::vec2(0, 0);
        faces[int(Face::Z_NEG)].uv[2] = glm::vec2(0, 1);
        faces[int(Face::Z_NEG)].uv[3] = glm::vec2(1, 1);

        return faces;
    }
    FaceTBNUV faces[6];
    ShaderStorageBuffer FacesUBO;
    std::vector<glm::ivec3> dirtyChunks;
    struct VertexPool {
        PersistentBuffer<GL_SHADER_STORAGE_BUFFER> instancePool;
        VertexBufferObject vertexPool;
        ElementBufferObject indexPool;
        VertexArrayObject poolVAO;
        int next = 0;
        QuadDrawInstance* vInstancePtr = nullptr;
        std::map<unsigned, unsigned> freeBlocks;

        VertexPool() {}

        void initialize() {
            instancePool = PersistentBuffer<GL_SHADER_STORAGE_BUFFER>(sizeof(QuadDrawInstance) * 4096);

            constexpr uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
            constexpr glm::vec3 vertices[] = {
                {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}
            };
            poolVAO.allocate();
            vertexPool.allocate(sizeof(vertices), BufferUsage::STATIC, vertices);
            indexPool.allocate(sizeof(indices) / sizeof(unsigned), sizeof(unsigned), BufferUsage::STATIC, indices);
            poolVAO.addAttribute(3, 3, 0);
            vInstancePtr = instancePool.get<QuadDrawInstance>();

            freeBlocks.emplace(0, 4096);
        }

        unsigned allocate(unsigned count) {
            for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
                unsigned start = it->first;
                unsigned size = it->second;

                if (size >= count) {
                    if (size == count) {
                        freeBlocks.erase(it);
                    } else {
                        unsigned newStart = start + count;
                        unsigned newSize = size - count;
                        freeBlocks.erase(it);
                        freeBlocks[newStart] = newSize;
                    }
                    return start;
                }
            }
            assert(false);
        }

        void free(unsigned offset, unsigned count) {
            unsigned end = offset + count;

            auto next = freeBlocks.lower_bound(offset);
            auto prev = (next == freeBlocks.begin()) ? freeBlocks.end() : std::prev(next);

            if (prev != freeBlocks.end() && prev->first + prev->second == offset) {
                offset = prev->first;
                count += prev->second;
                freeBlocks.erase(prev);
            }

            if (next != freeBlocks.end() && offset + count == next->first) {
                count += next->second;
                freeBlocks.erase(next);
            }

            freeBlocks[offset] = count;
        }
    } vertexPool;

    ShaderStorageBuffer instanceSSBO;
public:
    void onLevelLoad(LevelLoadView<VoxelWorldSystem>& view) {
        auto d = loadTBNUVFaces();
        memcpy(faces, d.data(), sizeof(FaceTBNUV) * 6);
        FacesUBO.allocate(sizeof(FaceTBNUV) * 6, BufferUsage::STATIC, faces);
        vertexPool.initialize();
    }

    BVH<BVHCollisionNode<AABB>> buildCollisionMesh(Chunk& chunk);

    void addVolume(const glm::ivec3 localCoords, const Entity entity) {
        const auto it = chunks.find(localCoords);
        Chunk& chunk = it != chunks.end() ? it->second : chunks.emplace(localCoords, localCoords).first->second;
        chunk.voxelVolumeEntity = entity;
    }

    void onUpdate(LevelUpdateView<VoxelWorldSystem>& view);

    std::vector<Quad> greedyMesh(const VoxelVolume& volume, glm::ivec3 dims);

    static void ensureCorrectWinding(std::array<glm::ivec3,4>& corners, const int d, const bool facePositive) {
        const auto edge1 = glm::vec3(corners[1] - corners[0]);
        const auto edge2 = glm::vec3(corners[3] - corners[0]);
        const glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        glm::vec3 expectedNormal(0.f);
        expectedNormal[d] = facePositive ? 1.f : -1.f;

        if (glm::dot(normal, expectedNormal) < 0.f) {
            std::swap(corners[1], corners[3]);
        }
    }

    void buildChunkMesh(const VoxelVolume& volume, Chunk& mesh);

    void buildVolume(const VoxelVolume &volume, Chunk &chunk);

    RenderPassContext onRenderNew(RenderInfo& info, RenderScene& rScene, const std::unordered_set<glm::ivec3, IVec3Hash> & visibleChunks);
};
