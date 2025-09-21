#include "RenderingSystem.h"

#include <chrono>
#include <random>
#include <openGL/shaders/Shader.h>

#include <ECS/ECS.h>
#include "Renderers/RenderInfo.h"
#include "Light.h"
#include <Core/World/FrustumCulling.h>

#include "App.h"
#include "RendererOps.h"
#include "Constants/ShaderConstants.h"

struct ConeVertex {
    glm::vec3 position;
};

void loadShaders(Viewable auto view) {
    view.loadShader("shaders/Cube/cube_vertex_depth.glsl", "shaders/Cube/cube_frag_depth.glsl", "CubeDepthShader");
    view.loadShader("shaders/widget/image_vertex.glsl", "shaders/widget/image_frag.glsl", "Image2DShader");
    view.loadShader("shaders/Mesh/mesh_skin_vertex.glsl", "shaders/Mesh/mesh_skin_frag.glsl", "MeshSkinShader");
    view.loadShader("shaders/text/3d_text_vertex.glsl", "shaders/text/3d_text_frag.glsl", "Text3DShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/screen/screen_frag.glsl", "ScreenShader");
    view.loadShader("shaders/Bloom/bloom_vertex.glsl", "shaders/Bloom/bloom_frag.glsl", "BloomShader");
    view.loadShader("shaders/Voxel/voxel_vertex_pool.glsl", "shaders/Voxel/voxel_frag.glsl", "VoxelShader");
    view.loadShader("shaders/Composite/composite_vertex.glsl", "shaders/Composite/composite_frag.glsl", "CompositeShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/Depth/depth_frag.glsl", "DepthShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/Depth/normal_frag.glsl", "NormalShader");
    view.loadShader("shaders/Lighting/light_volume_vertex.glsl", "shaders/Lighting/light_frag.glsl", "LightShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/Lighting/base_light_frag.glsl", "BaseLightShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/SSAO/ssao_frag.glsl", "SSAOShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/SSAO/ssao_blur_frag.glsl", "SSAOBlurShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/SSAO/ssao_debug.glsl", "SSAODebugShader");
    view.loadShader("shaders/Lighting/hdri_vertex_load.glsl", "shaders/Lighting/hdri_frag_load.glsl", "HDRILoadShader");
    view.loadShader("shaders/Lighting/hdri_vertex_load.glsl", "shaders/Lighting/hdri_irradiance_frag.glsl", "HDRIIrradianceMapShader");
    view.loadShader("shaders/Lighting/hdri_vertex_load.glsl", "shaders/Lighting/ibl_prefilter_frag.glsl", "IBLPrefilterMapShader");
    view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/Lighting/ibl_brdf_lut.glsl", "IBLBRDFLUTShader");
    view.loadShader("shaders/ParticlePipeline/particle_vertex.glsl", "shaders/ParticlePipeline/particle_frag.glsl", "ParticleShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_eval_compute.glsl", glm::vec3(32, 1, 1), "ParticleEvalShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_alive_buf_clear.glsl", glm::vec3(1, 1, 1), "ParticleClearShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_scale_decay.glsl", glm::vec3(32, 1, 1), "ParticleScaleShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_buffer_append.glsl", glm::vec3(32, 1, 1), "ParticleBufferAppendShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_buffer_append_counts.glsl", glm::vec3(1, 1, 1), "ParticleBufferAppendCountsShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_color_shader.glsl", glm::vec3(32, 1, 1), "ParticleColorShader");
    view.loadComputeShader("shaders/ParticlePipeline/particle_force_pass.glsl", glm::vec3(32, 1, 1), "ParticleForceShader");
}

static std::vector<ConeVertex> generateConeVertices(int segments = 32) {
    std::vector<ConeVertex> vertices;

    const float radius = 1.0f;
    const float height = 1.0f;
    const glm::vec3 tip(0.0f, 0.0f, 0.0f);
    const glm::vec3 baseCenter(0.0f, 0.0f, -height);

    // Base circle
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(segments);
        float x = cos(theta) * radius;
        float y = sin(theta) * radius;
        float z = -height;
        vertices.push_back({ glm::vec3(x, y, z) });
    }

    vertices.push_back({ tip });

    return vertices;
}

static std::vector<uint32_t> generateConeIndices(int segments = 32) {
    std::vector<uint32_t> indices;

    uint32_t tipIndex = segments + 1;
    for (uint32_t i = 0; i < segments; ++i) {
        indices.push_back(tipIndex);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    uint32_t baseCenterIndex = tipIndex + 1;
    for (uint32_t i = 0; i < segments; ++i) {
        indices.push_back(baseCenterIndex);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
    return indices;
}


void RenderingSystem::setGlobalTransformsBuffer(const CameraComponent &camera)
{
    globalTransforms.projection2D = glm::ortho(0.0f, camera.width, camera.height, 0.0f, -1.0f, 1.0f);
    globalTransforms.projection3D = camera.projection;
    globalTransforms.view = camera.view;

    glm::mat4 vectors;
    vectors[0] = glm::vec4(camera.front, 0);
    vectors[1] = glm::vec4(camera.right, 0);
    vectors[2] = glm::vec4(camera.up, 0);
    vectors[3] = glm::vec4(camera.position, 0);
    
    globalTransforms.cameraVectors = vectors;

    globalUniformsBuffer.bind();
    globalUniformsBuffer.uploadData(&globalTransforms, sizeof(GlobalTransformUniform));
    globalUniformsBuffer.setToBindingPoint(0);
}

void RenderingSystem::initializeLightVolume() {
    std::vector<float> sphereVertices;
    std::vector<unsigned int> indices;

    constexpr static unsigned int X_SEGMENTS = 32;
    constexpr static unsigned int Y_SEGMENTS = 16;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            constexpr static float PI = 3.14159265359f;
            const float xSegment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
            const float ySegment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
        }
    }
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
            unsigned int i0 = y       * (X_SEGMENTS + 1) + x;
            unsigned int i1 = (y + 1) * (X_SEGMENTS + 1) + x;
            unsigned int i2 = (y + 1) * (X_SEGMENTS + 1) + x + 1;
            unsigned int i3 = y       * (X_SEGMENTS + 1) + x + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }
    lightVolumeVAO.allocate();
    lightVolumeVBO.allocate(sizeof(float) * sphereVertices.size(), BufferUsage::STATIC, sphereVertices.data());
    lightVolumeEBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC, indices.data());
    lightVolumeVAO.addAttribute(3, 3, 0);

    std::vector<ConeVertex> coneVertices = generateConeVertices();
    std::vector<unsigned int> coneIndices = generateConeIndices();
    lightConeVAO.allocate();
    lightConeVBO.allocate(sizeof(ConeVertex) * coneVertices.size(), BufferUsage::STATIC, coneVertices.data());
    lightConeEBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC, indices.data());
    lightConeVAO.addAttribute(3, 3, 0);
}

void RenderingSystem::readShaderBatch(RenderPassContext& ctx) {}

void RenderingSystem::onRendererLoad(RendererLoadView<RenderingSystem> view) {
    particleSystem = ParticleSystem_(400);
    loadShaders(view);
    globalUniformsBuffer.allocate(sizeof(globalTransforms), BufferUsage::DYNAMIC);

    MeshMaterial::DEFAULT_NORMAL = MeshMaterial::createDefaultNormal();
    MeshMaterial::DEFAULT_EMISSIVE = MeshMaterial::createDefaultEmissive();
    MeshMaterial::DEFAULT_ROUGHNESS_METALLIC = MeshMaterial::createDefaultRoughnessMetallic();

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ColorBufferInternalFormat geometryInternalFormat[] = {
        ColorBufferInternalFormat::RGBA_8, ColorBufferInternalFormat::RGB_16F, ColorBufferInternalFormat::RGB_16F, ColorBufferInternalFormat::RED_32F, ColorBufferInternalFormat::RGBA_8
    };
    ColorBufferFormat geometryFormat[] = {
        ColorBufferFormat::RGBA, ColorBufferFormat::RGB, ColorBufferFormat::RGB, ColorBufferFormat::RED, ColorBufferFormat::RGBA
    };
    GLType geometryTypes[] = {
        GLType::UNSIGNED_BYTE, GLType::FLOAT, GLType::FLOAT, GLType::FLOAT, GLType::UNSIGNED_BYTE
    };

    geometryBuffer.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, geometryInternalFormat, geometryFormat, geometryTypes, 5);
    forwardBuffer.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, 2);
    compositeBuffer.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, 2);
    lightBuffer.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, 2, true);
    bloomBuffer0.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, 1, false);
    bloomBuffer1.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, 1, false);

    VertexArrayObject fullScreenVAO;
    VertexBufferObject fullScreenVBO;
    fullScreenVAO.allocate();

    constexpr float quadVertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    fullScreenVBO.allocate(sizeof(quadVertices), BufferUsage::STATIC);
    fullScreenVBO.uploadData(quadVertices);
    fullScreenVAO.addAttribute(2, 4, 0);
    fullScreenVAO.addAttribute(2, 4, 2);

    Geometry fullScreenGeometry;
    fullScreenGeometry.VAO = std::move(fullScreenVAO);
    fullScreenGeometry.VBO = std::move(fullScreenVBO);
    view.loadGeometry(fullScreenGeometry, GeometryConstants::FULL_SCREEN_QUAD);

    glm::vec3 ssaoKernel[64];
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::mt19937 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for (int i = 0; i < 64; ++i) {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = static_cast<float>(i) / 64.0f;
        scale = 0.1f + 0.9f * (scale * scale);
        sample *= scale;

        ssaoKernel[i] = sample;
    }

    SSAO.sampleKernel.allocate(sizeof(glm::vec3) * 64, BufferUsage::STATIC, ssaoKernel);

    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }
    SSAO.rotationKernel.allocate(TextureFormat::RGBA16F, TextureFormat::RGB, GLType::FLOAT, 4, 4, ssaoNoise.data());
    SSAO.rotationKernel.setFilter(TextureMinFilter::NEAREST, TextureMagFilter::NEAREST);
    SSAO.rotationKernel.setWrap(TextureWrap::REPEAT, TextureWrap::REPEAT);

    auto iFormat = ColorBufferInternalFormat::RED_32F;
    auto format = ColorBufferFormat::RED;
    auto type = GLType::FLOAT;
    SSAO.FBO.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, &iFormat, &format, &type, 1);
    SSAO.blur.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, &iFormat, &format, &type, 1);

    initializeLightVolume();
}

void RenderingSystem::onRender(ForwardRenderView<RenderingSystem> view) {
    view.query<CameraComponent>().forEach([&](const CameraComponent& cam) {
        this->currentCamera = &cam;
    });
    if (!currentCamera) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    setGlobalTransformsBuffer(*currentCamera);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    // render batches here

    // forwardBuffer.bind();
    // clear(ClearTarget::COLOR | ClearTarget::DEPTH);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryBuffer.id());
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, forwardBuffer.id());
    // glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);/
    particleSystem.render(view, view.getGeometry("Icosahedron"));
}