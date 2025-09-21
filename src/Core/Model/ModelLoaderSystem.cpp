#include <assimp/postprocess.h>
#include "ModelLoaderSystem.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "ModelAnimation.h"


constexpr auto AI_PROCESS_FLAGS = aiProcess_Triangulate |
                       aiProcess_GenSmoothNormals |
                       aiProcess_CalcTangentSpace |
                       aiProcess_JoinIdenticalVertices |
                       aiProcess_ImproveCacheLocality |
                       aiProcess_RemoveRedundantMaterials |
                       aiProcess_SortByPType;

template <typename S>
void loadAnimations(S &model, const aiScene *scene) {
    for (int a = 0; a < scene->mNumAnimations; a++) {
        const auto& anim = *scene->mAnimations[a];
        auto& animation = model.animations.emplace_back(scene->mAnimations[a]->mName.C_Str(), ModelAnimation()).second;

        const double speed = anim.mTicksPerSecond != 0 ? anim.mTicksPerSecond : 25;
        for (int c = 0; c < anim.mNumChannels; c++) {
            const auto& chan = anim.mChannels[c];

            for (int i = 0; i < chan->mNumPositionKeys; ++i) {
                const auto& posKey = chan->mPositionKeys[i];
                animation.addTranslation(chan->mNodeName.C_Str(), posKey.mTime / speed, glm::vec3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z));
            }
            for (int i = 0; i < chan->mNumScalingKeys; ++i) {
                const auto& posKey = chan->mScalingKeys[i];
                animation.addScale(chan->mNodeName.C_Str(), posKey.mTime / speed, glm::vec3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z));
            }
            for (int i = 0; i < chan->mNumRotationKeys; ++i) {
                const auto& posKey = chan->mRotationKeys[i];
                animation.addRotation(chan->mNodeName.C_Str(), posKey.mTime / speed, glm::quat(posKey.mValue.w, posKey.mValue.x, posKey.mValue.y, posKey.mValue.z));
            }
        }
        std::cout << "Animation Loaded: " << scene->mAnimations[a]->mName.C_Str() << ": " << scene->mAnimations[a]->mDuration << std::endl;
    }
}

static aiMatrix4x4 getNodeWorldTransform(const aiNode* node) {
    aiMatrix4x4 transform = node->mTransformation;
    const aiNode* current = node->mParent;
    while (current) {
        transform = current->mTransformation * transform;
        current = current->mParent;
    }
    return transform;
}

template <typename Fn>
void forEachNode(const aiNode* node, Fn&& fn) {
    fn(node);

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        forEachNode(node->mChildren[i], fn);
    }
}

auto& loadTransformNode(ModelDefinitionBuilder& def, const MeshNode* parentNode, const aiNode* node) {
    auto& defNode = def.nodes.emplace_back();
    const unsigned meshID = def.nodes.size() - 1;

    const aiMatrix4x4& aiTransform = node->mTransformation;
    const auto glmTransform = glm::transpose(glm::make_mat4(&aiTransform.a1));

    Transform transform;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(glmTransform, transform.scale, transform.rotation, transform.translation, skew, perspective);

    defNode.localTransform = transform;
    defNode.finalTransform = (parentNode ? parentNode->finalTransform : glm::mat4(1.f)) * glmTransform;
    defNode.localMatrix = glmTransform;

    std::cout << "Mesh: " << node->mName.C_Str() << ": " << std::endl;
    std::cout << "  Transform: " << transform << std::endl;
    const char* meshName = node->mName.C_Str();
    const char* firstPtr = def.getNames().data() + def.getNames().size();

    def.names.insert(def.names.end(), meshName, meshName + node->mName.length);
    def.names.emplace_back('\0');

    defNode.name = {firstPtr, node->mName.length};

    def.nameToNode.emplace(std::hash<std::string_view>{}(defNode.name), meshID);
    return defNode;
}

void loadTransformHierarchyImpl(ModelDefinitionBuilder& def, MeshNode& current, const MeshNode* parentNode, const aiNode* node) {
    if (node->mNumChildren > 0) {
        current.parent = parentNode ? parentNode - def.nodes.data() : -1;
        current.firstChild = def.nodes.size();
        current.childrenCount = node->mNumChildren;
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        loadTransformNode(def, parentNode, node->mChildren[i]);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        def.aiNodeToMeshNode.emplace(node->mChildren[i], current.firstChild + i);
        loadTransformHierarchyImpl(def, def.nodes[current.firstChild + i], &current, node->mChildren[i]);
    }
}

ModelDefinitionBuilder createDefinitionBuilder(const aiScene* scene) {
    auto* node = scene->mRootNode;

    size_t length = 0;
    size_t charLength = 0;
    size_t totalMeshIndices = 0;

    forEachNode(node, [&](const aiNode* node) {
        ++length;
        charLength += node->mName.length + 1;

        totalMeshIndices += node->mNumMeshes;
    });

    return ModelDefinitionBuilder(length, totalMeshIndices, charLength);
}

void loadTransformHierarchy(ModelDefinitionBuilder& def, const aiScene* scene) {
    auto* node = scene->mRootNode;

    auto& root = loadTransformNode(def, nullptr, node);
    loadTransformHierarchyImpl(def, root, nullptr, node);

    def.aiNodeToMeshNode.emplace(node, 0);
}

MeshDrawParams loadMeshVertices(ModelDefinitionBuilder& model, const aiMesh* mesh) {
    const unsigned indicesOffset = model.indices.size();
    const unsigned vertexOffset = model.vertices.size();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        for (int b = 0; b < MAX_BONE_INFLUENCE; b++) {
            vertex.m_BoneIDs[b] = -1;
            vertex.m_Weights[b] = 0.0f;
        }
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z); // TODO need to adjust bones to compensate for scaling
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);                        // TODO also add Origin/Pivot Offset while am at it
        vertex.Normal = glm::normalize(vertex.Normal);

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        vertex.Tangent = glm::normalize(vertex.Tangent);
        vertex.Bitangent = glm::normalize(vertex.Bitangent);

        model.vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            model.indices.push_back(face.mIndices[j]);
        }
    }
    return MeshDrawParams(vertexOffset, mesh->mNumVertices, indicesOffset, mesh->mNumFaces * 3);
}

enum class ModelRenderLoad {
    VERTICES = 1,
    TEXTURES = 2
};

#include <util/enum_bit.h>

template <typename Enum>
class EnumFlags {
    Enum e;
public:
    EnumFlags(Enum e) : e(e) {}

    bool has(Enum f) const {
        return (cexpr::enum_cast(e) & cexpr::enum_cast(f)) == cexpr::enum_cast(f);
    }
};

using ModelRenderLoadFlags = EnumFlags<ModelRenderLoad>;

aiString loadMaterial(const aiMaterial* material, const aiTextureType texType) {
    aiString path;
    if (material->GetTexture(texType, 0, &path) != AI_SUCCESS) return aiString("");
    return path;
}

void loadModelRenderData(ModelDefinitionBuilder& model, const aiScene& scene, const ModelLoadParams& params) {
    const aiNode* root = scene.mRootNode;

    mem::bitset<> loadedMeshes;
    loadedMeshes.reserve(scene.mNumMeshes);
    model.renderNodes.assign(scene.mNumMeshes, {});

    forEachNode(root, [&](const aiNode* node) {
        const unsigned first = model.meshIndicesArena.size();

        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            const unsigned meshIndex = node->mMeshes[i];
            const aiMesh* mesh = scene.mMeshes[meshIndex];

            if (loadedMeshes.test(meshIndex)) continue;

            const auto param = loadMeshVertices(model, mesh);

            model.renderNodes[meshIndex].drawParams = param;
            model.renderNodes[meshIndex].materialIndex = mesh->mMaterialIndex;
            loadedMeshes.set(meshIndex);

            model.meshIndicesArena.emplace_back(meshIndex);
        }

        auto& meshNode = model.nodes[model.aiNodeToMeshNode[node]];
        meshNode.firstMeshIndex = first;
        meshNode.meshCount = node->mNumMeshes;
    });

    model.materials.reserve(scene.mNumMaterials);

    for (unsigned i = 0; i < scene.mNumMaterials; ++i) {
        const aiMaterial* mat = scene.mMaterials[i];

        auto diffuse = loadMaterial(mat, aiTextureType_DIFFUSE);
        auto normal = loadMaterial(mat, aiTextureType_NORMALS);
        auto emissive = loadMaterial(mat, aiTextureType_EMISSIVE);
        auto metallicRoughness = loadMaterial(mat, aiTextureType_GLTF_METALLIC_ROUGHNESS);

        auto addMaterialPath = [&](const aiString& path) -> unsigned {
            if (path.length > 0) {
                const size_t hash = std::hash<std::string_view>{}(path.C_Str());

                if (!model.pathToTexture.contains(hash)) {
                    model.texturePaths.emplace_back(path.C_Str());
                    model.pathToTexture.emplace(hash, 0);
                    return model.texturePaths.size() - 1;
                }
                return model.pathToTexture[hash];
            }
            return -1;
        };
        const unsigned diffIndex = addMaterialPath(diffuse);
        const unsigned nIdx = addMaterialPath(normal);
        const unsigned eIdx = addMaterialPath(emissive);
        const unsigned mrIdx = addMaterialPath(metallicRoughness);

        model.materials.emplace_back();
        model.materials[i].diffuse = diffIndex;
        model.materials[i].normal = nIdx;
        model.materials[i].emissive = eIdx;
        model.materials[i].metallicRoughness = mrIdx;
    }
}


static auto convertAssimpToGLM(auto& to, auto& from) {
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
}
template <typename S>
bool extractBoneWeightForVertices(S &model, const aiMesh *mesh, std::vector<Vertex> &vertices, const unsigned vertexOffset) {
    auto& boneInfoMap = model.boneInfo;
    int& boneCount = model.boneCounter;

    for (unsigned boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID;
        if (std::string boneName = mesh->mBones[boneIndex]->mName.C_Str(); !boneInfoMap.contains(boneName)) {
            BoneInfo newBoneInfo{};
            newBoneInfo.boneName = boneName;
            newBoneInfo.id = boneCount;

            auto& to = newBoneInfo.offset;
            const auto& from = mesh->mBones[boneIndex]->mOffsetMatrix;
            convertAssimpToGLM(to, from);

            auto worldMat = getNodeWorldTransform(model.aiScene->mRootNode->FindNode(boneName.c_str()));
            convertAssimpToGLM(newBoneInfo.local, worldMat);

            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            ++boneCount;
        } else {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        const auto weights = mesh->mBones[boneIndex]->mWeights;
        const unsigned numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (unsigned weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            const unsigned vertexId = weights[weightIndex].mVertexId + vertexOffset;
            const float weight = weights[weightIndex].mWeight;
            assert(vertexId < vertices.size());

            auto& vertex = vertices[vertexId];
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                if (vertex.m_BoneIDs[i] <= 0) {
                    vertex.m_Weights[i] = weight;
                    vertex.m_BoneIDs[i] = boneID;
                    break;
                }
            }
        }
    }
    for (unsigned i = vertexOffset; i < vertices.size(); ++i) {
        auto& vertex = vertices[i];
        float total = 0.0f;

        for (const float m_Weight : vertex.m_Weights)
            total += m_Weight;

        if (total > 0.0f) {
            for (float & m_Weight : vertex.m_Weights)
                m_Weight /= total;
        } else {
            vertex.m_BoneIDs[0] = 0;
            vertex.m_Weights[0] = 1.0f;
            for (int w = 0; w < MAX_BONE_INFLUENCE; ++w) {
                vertex.m_BoneIDs[w] = 0;
                vertex.m_Weights[w] = 0.25f;
            }
        }
    }
    std::cout << "[BONES] Mesh Bones: " << boneCount << std::endl;
    return boneCount != 0;
}

Geometry createGeometry(ModelDefinitionBuilder& model) {
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;

    VAO.allocate();
    VBO.allocate(model.vertices.size() * sizeof(Vertex), BufferUsage::STATIC);
    VBO.uploadData(model.vertices.data());

    EBO.allocate(model.indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(model.indices.data());

    constexpr uint8_t data_per_vertex = sizeof(Vertex) / sizeof(float);

    VAO.addAttribute(3, data_per_vertex, 0);
    VAO.addAttribute(3, data_per_vertex, 3);
    VAO.addAttribute(2, data_per_vertex, 6);
    VAO.addAttribute(3, data_per_vertex, 8);
    VAO.addAttribute(3, data_per_vertex, 11);
    VAO.addAttributeInteger(4, data_per_vertex, 14);
    VAO.addAttribute(4, data_per_vertex, 18);

    return Geometry{
        .VAO = std::move(VAO),
        .VBO = std::move(VBO),
        .EBO = std::move(EBO),
        .vertices = std::move(model.vertices),
        .indices = std::move(model.indices)
    };
}

Geometry& ModelLoaderSystem::loadModelGeometry(Geometry& geometry) {
    auto& VAO = geometry.VAO;
    auto& VBO = geometry.VBO;
    auto& EBO = geometry.EBO;

    VAO.allocate();
    VBO.allocate(geometry.vertices.size() * sizeof(Vertex), BufferUsage::STATIC);
    VBO.uploadData(geometry.vertices.data());

    EBO.allocate(geometry.indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(geometry.indices.data());

    constexpr static uint8_t data_per_vertex = sizeof(Vertex) / sizeof(float);

    VAO.addAttribute(3, data_per_vertex, 0);
    VAO.addAttribute(3, data_per_vertex, 3);
    VAO.addAttribute(2, data_per_vertex, 6);
    VAO.addAttribute(3, data_per_vertex, 8);
    VAO.addAttribute(3, data_per_vertex, 11);
    VAO.addAttributeInteger(4, data_per_vertex, 14);
    VAO.addAttribute(4, data_per_vertex, 18);
    return geometry;
}

const ModelDefinition* ModelLoaderSystem::loadModel(const ModelLoadParams& params) {
    assert(_CrtCheckMemory());
    Assimp::Importer importer;
    auto scene = importer.ReadFile(params.path, AI_PROCESS_FLAGS);

    auto def = createDefinitionBuilder(scene);
    loadTransformHierarchy(def, scene);
    loadModelRenderData(def, *scene, params);

    std::string directory = params.path.substr(0, params.path.find_last_of('/'));
    for (auto& node : def.linear()) {
        std::cout << "Node: " << node.name << std::endl;
    }
    ModelDefinition* mdef = modelAssets.emplace(params.name, new ModelDefinition(params.name, def)).first->second;
    newDefinitions.emplace_back(def, mdef, directory, params.flipTextureUV);
    return mdef;
}

std::expected<Geometry, ModelLoadError> ModelLoaderSystem::loadGeometry(std::string_view path) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path.data(), AI_PROCESS_FLAGS);

    auto def = createDefinitionBuilder(scene);
    loadModelRenderData(def, *scene, {});
    Geometry geometry;
    geometry.vertices = std::move(def.vertices);
    geometry.indices = std::move(def.indices);
    return std::move(loadModelGeometry(geometry));
}