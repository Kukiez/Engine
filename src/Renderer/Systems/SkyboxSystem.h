#pragma once
#include <openGL/BufferObjects/FrameBufferObject.h>
#include <openGL/shaders/UniformData.h>
#include <openGL/Texture/Texture2D.h>
#include <openGL/Texture/cubemap/Cubemap.h>
#include <Renderer/RenderingDetails.h>
#include <Renderer/RenderingSystem.h>
#include <Renderer/Renderers/Renderer.h>
#include <Renderer/Systems/ChangeListener.h>
#include <tuple>
#include "../Components/Components.h"

struct SkyboxChangeEvent {
    Entity entity;
    Skybox skybox;
};

struct SkyboxRenderable : PrimaryComponent, TrackedComponent {
    static constexpr float CubemapVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    static inline std::array HDRIViews = std::array<glm::mat4, 6>{
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0), glm::vec3(0) + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))
    };

    HDRIObject envCubemap;
    HDRIObject irradianceMap;
    HDRIObject prefilterMap;
    Texture2D hdriTexture;

    std::string loadedFile;
    std::string hdriFile;

    SkyboxRenderable() = default;

    SkyboxRenderable(std::string file) : hdriFile(std::move(file)) {}

    void loadHDRFile(const Geometry& cubemapGeometry, ShaderBindableView auto renderer) {
        static auto captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        const HDRTextureData data = TextureLoader::loadEXR16F(hdriFile.data());
        data.flip();

        hdriTexture.discard();
        hdriTexture.allocate(ColorBufferInternalFormat::RGBA_16F,
            ColorBufferFormat::RGBA, GLType::HALF_FLOAT,
            data.width, data.height, data.pixels,
            TextureMinFilter::LINEAR, TextureMagFilter::LINEAR,
            TextureWrap::REPEAT, false
        );
        /* Convert Equirectangular into Cubemap */
        envCubemap.discard();
        envCubemap.allocate(ColorBufferInternalFormat::RGB_16F, 512, 512, true);

        Shader* shader = &renderer.setActiveShader("HDRILoadShader");
        UniformData uniforms(shader->id());
        uniforms.upload_nowrite("projection", captureProjection);
        uniforms.upload_nowrite("hdri", 0);

        DrawArrays drawArrays;
        drawArrays.first_vertex = 0;
        drawArrays.vertex_count = 36;
        drawArrays.primitive = GLDrawPrimitive::TRIANGLES;

        cubemapGeometry.VAO.bind();
        hdriTexture.bind(0);
        for (int i = 0; i < 6; ++i) {
            envCubemap.setActiveFace(CubemapFace{static_cast<int>(CubemapFace::POS_X) + i});
            glClear(static_cast<GLenum>(ClearTarget::COLOR | ClearTarget::DEPTH));
            uniforms.upload_nowrite("view", HDRIViews[i]);
            drawArrays.draw();
        }
        envCubemap.getCubemap().generateMipmaps();

        /* Render Irradiance Map */
        irradianceMap.discard();
        irradianceMap.allocate(ColorBufferInternalFormat::RGB_16F,  32, 32, false);

        shader = &renderer.setActiveShader("HDRIIrradianceMapShader");
        uniforms.setProgram(shader->id());
        uniforms.upload_nowrite("projection", captureProjection);
        uniforms.upload_nowrite("envCubemap", 0);

        envCubemap.getCubemap().bind(0);
        for (int i = 0; i < 6; ++i) {
            irradianceMap.setActiveFace(CubemapFace{static_cast<int>(CubemapFace::POS_X) + i});
            glClear(static_cast<GLenum>(ClearTarget::COLOR | ClearTarget::DEPTH));
            uniforms.upload_nowrite("view", HDRIViews[i]);
            drawArrays.draw();
        }
        /* Render PreFilter map */
        prefilterMap.discard();
        prefilterMap.allocate(ColorBufferInternalFormat::RGB_16F, 256, 256, true);

        shader = &renderer.setActiveShader("IBLPrefilterMapShader");
        uniforms.setProgram(shader->id());
        uniforms.upload_nowrite("projection", captureProjection);
        uniforms.upload_nowrite("envCubemap", 0);

        envCubemap.getCubemap().bind(0);
        const int mipmaps = prefilterMap.getCubemap().mipmaps();
        for (int level = 0; level < mipmaps; ++level) {
            const float roughness = static_cast<float>(level) / static_cast<float>(mipmaps - 1);
            uniforms.upload_nowrite("roughness", roughness);
            for (int i = 0; i < 6; ++i) {
                prefilterMap.setActiveFace(CubemapFace{static_cast<int>(CubemapFace::POS_X) + i}, level);
                glClear(static_cast<GLenum>(ClearTarget::COLOR | ClearTarget::DEPTH));
               
                uniforms.upload_nowrite("view", HDRIViews[i]);
                drawArrays.draw();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

struct SkyboxUpdateManager : LevelOutStage::Reads<Skybox>, Stages<LevelOutStage> {
    void onLevelLoad(LevelLoadView<SkyboxUpdateManager> view) {
        view.enableEventEmission<Skybox>();
    }

    void onLevelOut(LevelOutView<SkyboxUpdateManager> view) {
        view.query<Skybox>().forEachChangedOrNew<Skybox>([&](const Entity e, const Skybox& skybox) {
            view.send<SkyboxChangeEvent>(e, skybox);
        });
    }

    void onCleanup(LevelCleanupView<SkyboxUpdateManager> view) {
        view.query<Skybox>().forEachRemovedComponent([](const Entity e, const Skybox& skybox) {

        });
    }
};

struct SkyboxInitializer : ReadsResources<RenderingSystem, GeometrySystem> {
    FrameBufferObject brdfLUT;

    void initializeBRDFLutBuffer(const Geometry& fullScreenGeometry, ShaderBindableView auto view) {
        auto iformat = ColorBufferInternalFormat::RG_16F;
        auto format = ColorBufferFormat::RG;
        auto type = GLType::FLOAT;
        brdfLUT.allocate(256, 256, &iformat, &format, &type, 1, false);

        view.setActiveShader("IBLBRDFLUTShader");

        fullScreenGeometry.VAO.bind();

        brdfLUT.bind();
        GeometryConstants::FULL_SCREEN_DRAW_COMMAND.draw();
    }

    static Geometry initializeCubemapBuffers() {
        VertexBufferObject cubemapVBO;
        VertexArrayObject cubemapVAO;
        cubemapVAO.allocate();
        cubemapVBO.allocate(sizeof(SkyboxRenderable::CubemapVertices), BufferUsage::STATIC, SkyboxRenderable::CubemapVertices);
        cubemapVAO.addAttribute(3, 3, 0);

        Geometry cubemapGeometry;
        cubemapGeometry.VAO = std::move(cubemapVAO);
        cubemapGeometry.VBO = std::move(cubemapVBO);
        return cubemapGeometry;
    }

    void onRendererLoad(RendererLoadView<SkyboxInitializer> view) {
        view.loadShader("shaders/Lighting/hdri_vertex_load.glsl", "shaders/Lighting/hdri_irradiance_frag.glsl", "HDRIIrradianceMapShader");
        view.loadShader("shaders/Lighting/hdri_vertex_load.glsl", "shaders/Lighting/ibl_prefilter_frag.glsl", "IBLPrefilterMapShader");
        view.loadShader("shaders/screen/screen_vertex.glsl", "shaders/Lighting/ibl_brdf_lut.glsl", "IBLBRDFLUTShader");
        view.loadShader("shaders/Skybox/skybox_vertex.glsl", "shaders/Skybox/skybox_frag.glsl", "SkyboxShader");

        auto geom = initializeCubemapBuffers();

        view.loadGeometry(geom, GeometryConstants::CUBEMAP);

        auto& fullScreen = view.getGeometry(GeometryConstants::FULL_SCREEN_QUAD);

        initializeBRDFLutBuffer(fullScreen, view);
    }
};

struct SkyboxChangeListener : ReadsResources<EntityMapper>, Writes<SkyboxRenderable> {
    void onLevelLoad(LevelLoadView<SkyboxChangeListener> view) {
        view.enableEventEmission<SkyboxRenderable>();
    }

    void onRendererIn(RendererInView<SkyboxChangeListener> view) {
        auto& mapper = view.get<EntityMapper>();
        for (auto& change : view.read<SkyboxChangeEvent>()) {
            if (auto renderEntity = mapper.get(change.entity); renderEntity != NullEntity) {
                auto skybox = view.get<SkyboxRenderable>(renderEntity);
                skybox->hdriFile = change.skybox.file;
            } else {
                renderEntity = view.createEntity(SkyboxRenderable(std::move(change.skybox.file)));
                mapper.addEntity(change.entity, renderEntity);
            }
        }
    }
};

struct SkyboxRenderer : ReadsResources<RenderingSystem, GeometrySystem>, Writes<SkyboxRenderable>, Dependencies<RenderingSystem> {
    Entity activeSkybox = NullEntity;
    static constexpr DrawArrays drawArrays = {
        .function = GLDrawFunction::ARRAYS,
        .primitive = GLDrawPrimitive::TRIANGLES,
        .first_vertex = 0,
        .vertex_count = 36
    };

    void onRender(ForwardRenderView<SkyboxRenderer> view) {
        auto& cubemapGeometry = view.getGeometry(GeometryConstants::CUBEMAP);

        view.query<SkyboxRenderable>().forEachChangedOrNew<SkyboxRenderable>([&](const Entity e, SkyboxRenderable& skybox) {
            if (skybox.hdriFile == skybox.loadedFile) return;
            skybox.loadedFile = skybox.hdriFile;
            skybox.loadHDRFile(cubemapGeometry, view);
            activeSkybox = e;
        });

        if (activeSkybox == NullEntity) return;

        auto& skybox = *view.get<SkyboxRenderable>(activeSkybox);
        auto& shader = view.setActiveShader("SkyboxShader");
        UniformData uniforms(shader.id());
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        skybox.envCubemap.getCubemap().bind(0);
        cubemapGeometry.VAO.bind();

        uniforms.upload_nowrite("skybox", 0);
        drawArrays.draw();
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }
};

struct SkyboxGameEngineSystems : SystemPack {
    using Pack = std::tuple<SkyboxUpdateManager>;
};

struct SkyboxRenderingSystems : SystemPack {
    using Pack = std::tuple<SkyboxChangeListener, SkyboxInitializer, SkyboxRenderer>;
};