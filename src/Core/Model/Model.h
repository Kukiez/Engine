#pragma once

#define MAX_BONE_INFLUENCE 4
#include <numeric>
#include <ranges>
#include <glm/fwd.hpp>
#include <Renderer/Resource/Resource.h>

#include "ModelPose.h"
#include "Skeleton.h"
#include "ECS/Component/Component.h"
#include "ECS/Entity/Entity.h"
#include "Math/Shapes/AABB.h"
#include "openGL/BufferObjects/ElementBufferObject.h"
#include "openGL/BufferObjects/VertexArrayObject.h"
#include "openGL/BufferObjects/VertexBufferObject.h"
#include "Renderer/Renderers/Material.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct aiScene;

struct Geometry {
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

struct MeshDrawParams {
    unsigned vertexOffset;
    unsigned indexOffset;
    unsigned vertexCount;
    unsigned indexCount;

    MeshDrawParams() = default;

    MeshDrawParams(const unsigned vOffset, const unsigned vCount, const unsigned iOffset, const unsigned iCount) :
    vertexOffset(vOffset), indexOffset(iOffset), vertexCount(vCount), indexCount(iCount) {}

    friend std::ostream& operator<<(std::ostream& os, const MeshDrawParams& params) {
        return os << "MeshDrawParams{" << params.vertexOffset << ", " << params.indexOffset << ", " << params.vertexCount << ", " << params.indexCount << "}";
    }
};

enum class MeshID : unsigned {
    INVALID = std::numeric_limits<unsigned>::max()
};

struct MeshNode {
    size_t parent;
    Transform localTransform;
    glm::mat4 localMatrix;
    glm::mat4 finalTransform;
    std::string_view name;
    size_t firstChild = 0;
    size_t childrenCount = 0;
    unsigned firstMeshIndex = 0;
    unsigned meshCount = 0;

    auto children() const {
        return std::ranges::iota_view(firstChild, firstChild + childrenCount);
    }

    auto meshes() const {
        return std::ranges::iota_view(firstMeshIndex, firstMeshIndex + meshCount);
    }
};

struct RMeshNode {
    MeshDrawParams drawParams;
    unsigned materialIndex;
};

struct ModelDefinitionBuilder {
    std::string directory;
    mem::vector<MeshNode> nodes;
    mem::vector<unsigned> meshIndicesArena;
    mem::vector<RMeshNode> renderNodes;
    mem::vector<std::string> texturePaths;
    mem::vector<MeshMaterial> materials;
    mem::vector<char> names;
    std::unordered_map<size_t, unsigned> nameToNode;
    std::unordered_map<size_t, unsigned> pathToTexture;

    std::unordered_map<const aiNode*, size_t> aiNodeToMeshNode;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    ModelDefinitionBuilder(const size_t forNodes, const size_t forModelxMeshes, const size_t chars) {
        nodes.reserve(forNodes);
        names.reserve(chars);
        meshIndicesArena.reserve(forModelxMeshes);
    }

    auto& getNodes() {
        return nodes;
    }

    auto& getNames() {
        return names;
    }

    auto linear() const {
        return mem::make_range(nodes.data(), nodes.size());
    }

    size_t size() const {
        return nodes.size();
    }

    template <typename Fn>
    void forEach(Fn&& fn) const {
        fn(nodes[0]);

        for (const auto& node : nodes) {
            for (const auto child : node.children()) {
                fn(nodes[child]);
            }
        }
    }
};

class ModelDefinition {
public:
    std::string name;
    mem::vector<MeshNode> nodes;
    mem::vector<char> names;
    std::unordered_map<size_t, unsigned> nameToNode;
    std::vector<MeshDrawParams> nodeDrawParams;

    std::vector<Texture2DKey> textures;
    mem::vector<RMeshNode> renderNodes;
    mem::vector<MeshMaterial> materials;
    Geometry geometry;

    explicit ModelDefinition(std::string name, ModelDefinitionBuilder& builder) : name(std::move(name)) {
        nodes = std::move(builder.nodes);
        names = std::move(builder.names);
        nameToNode = std::move(builder.nameToNode);

        geometry.vertices = std::move(builder.vertices);
        geometry.indices = std::move(builder.indices);

        renderNodes = std::move(builder.renderNodes);
        materials = std::move(builder.materials);
    }

    MeshID findMesh(const std::string_view name) const {
        const auto it = nameToNode.find(std::hash<std::string_view>{}(name));
        if (it == nameToNode.end()) {
            return MeshID::INVALID;
        }
        return static_cast<MeshID>(it->second);
    }

    auto linear() const {
        return mem::make_range(nodes.data(), nodes.size());
    }

    size_t size() const {
        return nodes.size();
    }
};

class Model : public PrimaryComponent, public TrackedComponent {
public:
    struct Node {
        Transform* localTransform = nullptr;
        glm::mat4* finalTransform = nullptr;
        mem::bit_reference dirtyTransform = nullptr;
    };
private:
    struct RuntimeModelHierarchy {
        using Arena = mem::byte_arena<mem::same_alloc_schema, 64>;
        mem::vector<Transform, mem::byte_arena_adaptor<Transform, Arena>> localTransforms;
        mem::vector<glm::mat4, mem::byte_arena_adaptor<glm::mat4, Arena>> finalTransforms;
        mem::bitset<mem::byte_arena_adaptor<size_t, Arena>> dirtyTransforms;
        Arena nodesArena;

        explicit RuntimeModelHierarchy(const ModelDefinition* definition) : localTransforms(&nodesArena), finalTransforms(&nodesArena), dirtyTransforms(&nodesArena) {
            mem::bytes_required bytes;
            bytes.include(mem::type_info_of<Transform>, definition->size());
            bytes.include(mem::type_info_of<glm::mat4>, definition->size());
            bytes.include(mem::type_info_of<size_t>, 1 + definition->size() / 64);

            nodesArena.initialize(bytes.get());

            localTransforms.reserve(definition->size());
            finalTransforms.reserve(definition->size());
            dirtyTransforms.reserve(definition->size());

            for (const auto& node : definition->linear()) {
                localTransforms.emplace_back(node.localTransform);
                finalTransforms.emplace_back(node.finalTransform);
            }
            dirtyTransforms.set_all();
        }

        void buildDirtyNode(const ModelDefinition* definition, const MeshNode& node) {
            for (size_t i : node.children()) {
                finalTransforms[i] = finalTransforms[node.parent] * localTransforms[i].createModel3D();
            }

            for (size_t i : node.children()) {
                for (size_t j : definition->nodes[i].children()) {
                    buildDirtyNode(definition, definition->nodes[j]);
                }
            }
        }

        void buildDirty(const ModelDefinition* definition) {
            for (const auto dirty : dirtyTransforms) {
                auto& node = definition->nodes[dirty];
                buildDirtyNode(definition, node);
            }
        }

        void buildDirty(const glm::mat4& modelTransform, const ModelDefinition* definition) {
            finalTransforms[0] = modelTransform * localTransforms[0].createModel3D();

            for (size_t i = 0; i < definition->size(); ++i) {
                for (const auto child : definition->nodes[i].children()) {
                 //   finalTransforms[child] = finalTransforms[i] * definition->nodes[child].localMatrix;
                    finalTransforms[child] = definition->nodes[child].finalTransform;
                }
            }
        }

        void clearDirtyNodes() {
            dirtyTransforms.clear();
        }
    };

    const ModelDefinition* definition;
    RuntimeModelHierarchy hierarchy;
public:
    explicit Model(const ModelDefinition* definition) : definition(definition), hierarchy(definition) {}

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&& other)  noexcept : definition(other.definition), hierarchy(std::move(other.hierarchy)) {
        other.definition = nullptr;
        std::cout << "Model moved" << std::endl;
    }

    Model& operator=(Model&&) = default;

    ~Model() {
        std::cout << "Model destroyed" << std::endl;
    }

    MeshID getMeshPart(const std::string_view name) const {
        return definition->findMesh(name);
    }

    auto& getMeshNode(this auto&& self, const MeshID id) {
        if (id == MeshID::INVALID) {
            cexpr::require(false);
        }
        return self.hierarchy.nodes[cexpr::enum_cast(id)];
    }

    const ModelDefinition* asset() const {
        return definition;
    }

    void clearDirtyNodes() {
        hierarchy.clearDirtyNodes();
    }

    void update() {
        hierarchy.buildDirty(definition);
    }

    void update(const glm::mat4& modelTransform) {
        hierarchy.buildDirty(modelTransform, definition);
    }

    auto getFinalTransforms() const {
        return mem::make_range(hierarchy.finalTransforms.data(), hierarchy.finalTransforms.size());
    }
};

struct RenderableModel {
    ModelKey modelKey;
    SubBufferKey instanceKey;

    RenderableModel(const ModelKey key, const SubBufferKey instanceIndex) : modelKey(key), instanceKey(instanceIndex) {}
};


struct NewModelDefinition {
    ModelDefinition* definition;
    std::string directory;
    mem::vector<std::string> texturePaths;
    bool flip = false;

    NewModelDefinition(ModelDefinitionBuilder& builder, ModelDefinition* def, std::string& directory, bool flip = false)
    : definition(def), texturePaths(std::move(builder.texturePaths)), directory(std::move(directory)), flip(flip)
    {}
};

struct NewModelEntityUpdate {
    Entity entity;
    const ModelDefinition* definition;
};

struct ModelTransformUpdate {
    Entity entity{};
    MeshID mesh{};
    glm::mat4 transform{};

    ModelTransformUpdate(const Entity entity, const MeshID mesh, const glm::mat4& transform) : entity(entity), mesh(mesh), transform(transform) {}
};