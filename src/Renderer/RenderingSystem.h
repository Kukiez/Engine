#pragma once

#include <App.h>
#include <openGL/Texture/cubemap/Cubemap.h>
#include <openGL/BufferObjects/UniformBufferObject.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <openGL/BufferObjects/ElementBufferObject.h>
#include <openGL/BufferObjects/FrameBufferObject.h>
#include <openGL/BufferObjects/IndirectDrawBuffer.h>
#include <openGL/BufferObjects/VertexArrayObject.h>
#include <openGL/shaders/UniformData.h>
#include <openGL/Texture/Texture2D.h>
#include <memory/Arena.h>
#include "Renderers/MaterialSystem.h"
#include "RenderingDetails.h"
#include "Particle/ParticleSystem.h"
#include "Renderers/Renderer.h"
#include "Renderers/Rasterizer/Rasterizer.h"
#include <ECS/Level/LevelView.h>
#include "GeometryConstants.h"
#include "Light.h"
#include <Core/World/FrustumCulling.h>

#include "RendererOps.h"
#include "Renderers/RenderInfo.h"

class IRenderingSubsystem;
struct CameraComponent;

template <typename T>
class Octree;
struct OctreeNode;

struct MaterialHandle {
    struct Material {
        enum class Type {
            TEXTURE_2D, TEXTURE_ARRAY_2D
        };
        std::string material;
        unsigned texture;
        Type type;
    };
    std::vector<Material> materials;
    bool operator==(const MaterialHandle& other) const {
        if (materials.size() != other.materials.size()) return false;
        for (size_t i = 0; i < materials.size(); ++i) {
            if (materials[i].texture != other.materials[i].texture)
                return false;
        }
        return true;
    }

    size_t hash() const {
        size_t hashValue = 0;
        for (const auto& mat : materials) {
            hashValue ^= std::hash<unsigned>()(mat.texture) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
        }
        return hashValue;
    }

    bool empty() const {
        return materials.empty();
    }
};

enum class BufferHandleTarget {
    SSBO, UBO
};

struct BufferHandle {
    BufferHandleTarget target;
    unsigned handle;
    unsigned location;
};

struct Renderable {
    unsigned VAO;
    MaterialHandle materials;
    DrawElementsIndirectCommand cmd;
};

struct RenderPassContext {
    std::string shader;
    UniformData uniforms;
    MaterialHandle materials;
    std::vector<BufferHandle> buffers;
    std::vector<Renderable> instances;
    bool isSorted = false;
};

struct RenderScene {
    MaterialSystem& materials;
};

inline bool operator<(const Renderable& a, const Renderable& b) {
    if (a.VAO != b.VAO)
        return a.VAO < b.VAO;
    return a.materials.hash() < b.materials.hash();
}

enum class RenderTarget {
    GEOMETRY_BUFFER, POST_PROCESSING
};

struct RenderBatch {
    unsigned offset = -1;
    unsigned count = 0;
    std::vector<MaterialHandle::Material> materials;
};

struct GeometryBatch {
    /* material hash */
    unsigned VAO;
    MemorySlice<RenderBatch> batches;
};

struct ShaderBatch {
    MemorySlice<GeometryBatch> geometries; /* by VAO */
    std::vector<BufferHandle> buffers;
    std::vector<MaterialHandle::Material> materials; /* shared material per shader */
    IndirectDrawBuffer drawBuffer; /* Final Sorted Traversable command */
    GLDrawPrimitive drawPrimitive;
};

class AppSystem;

struct RenderingSystem :
    Reads<SpotLight, DirectionalLight, PointLight, CameraComponent>,
    ReadsResources<FrustumCullingSystem, GeometrySystem>,
    ResourceSystem<ConflictMode::SHARED>
{
private:
    std::unordered_map<std::string, ShaderBatch> batches;

    void setGlobalTransformsBuffer(const CameraComponent &camera);

    void initializeLightVolume();
    void readShaderBatch(RenderPassContext &ctx);

    UniformBufferObject globalUniformsBuffer;

    struct GlobalTransformUniform {
        glm::mat4 projection3D;
        glm::mat4 projection2D;
        glm::mat4 view;
        glm::mat4 cameraVectors;
    } globalTransforms{};

    Rasterizer rasterizer;
    HDRIObject envCubemap;
    HDRIObject irradianceMap;
    HDRIObject prefilterMap;
    FrameBufferObject brdfLUT;
    Texture2D hdriTexture;

    std::vector<glm::ivec3> frustumIntersections;

    FrameBufferObject forwardBuffer;
    FrameBufferObject bloomBuffer0;
    FrameBufferObject bloomBuffer1;
    FrameBufferObject geometryBuffer;
    FrameBufferObject lightBuffer;
    FrameBufferObject compositeBuffer;

    struct {
        Texture2D rotationKernel;
        UniformBufferObject sampleKernel;
        FrameBufferObject FBO;
        FrameBufferObject blur;
    } SSAO;

    VertexBufferObject lightVolumeVBO;
    VertexArrayObject lightVolumeVAO;
    ElementBufferObject lightVolumeEBO;

    VertexArrayObject lightConeVAO;
    VertexBufferObject lightConeVBO;
    ElementBufferObject lightConeEBO;

    VertexBufferObject cubemapVBO;
    VertexArrayObject cubemapVAO;

    const CameraComponent* currentCamera;

    MemoryArena<DrawElementsIndirectCommand> commandBufferArena;
    MemoryArena<GeometryBatch> geometryBatchArena;
    MemoryArena<RenderBatch> renderBatchArena;
    MemoryArena<Renderable> renderableArena;

    ParticleSystem_ particleSystem;

    friend struct ForwardPassPrepare;
    friend struct NewFrameBegin;

    Entity skyboxEntity;
public:
    RenderingSystem() = default;

    void onRendererLoad(RendererLoadView<RenderingSystem> view);

    void onRender(ForwardRenderView<RenderingSystem> view);
};
