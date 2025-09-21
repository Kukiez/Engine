#pragma once
#include <ECS/System/ISystem.h>

#include "Resource/ShaderSystem.h"
#include "Resource/ModelSystem.h"
#include "Resource/TextureSystem.h"
#include "Resource/BufferSystem.h"

struct ShaderBindingsAccess {
    void setActiveShader(this auto&& self, ShaderKey key) {
        self.template getSystem<ShaderSystem>().getShader(key).setActiveShader();
    }

    Shader& setActiveShader(this auto&& self, const std::string_view name) {
        auto& shaderSystem = self.template getSystem<ShaderSystem>();
        auto& shader = shaderSystem.getShader(shaderSystem.getShaderKey(name.data()));
        shader.setActiveShader();
        return shader;
    }

    ComputeShader& setActiveComputeShader(this auto&& self, const std::string_view name) {
        return static_cast<ComputeShader&>(self.setActiveShader(name));
    }
};

template <bool GeometryAccess, bool ShaderAccess, bool ModelAccess, bool TextureAccess, bool BufferAccess>
struct RenderResourceLoadAccess {
    GeometryKey loadGeometry(this auto&& self, std::string_view file, const std::string_view as) requires GeometryAccess {
        return self.template getSystem<GeometrySystem>().loadGeometry(file, std::string(as));
    }

    GeometryKey loadGeometry(this auto&& self, Geometry& geometry, const std::string_view as) requires GeometryAccess {
        return self.template getSystem<GeometrySystem>().loadGeometry(geometry, std::string(as));
    }

    ShaderKey loadShader(this auto&& self, const std::string_view vertex, const std::string_view frag, const std::string_view as) requires ShaderAccess {
        return self.template getSystem<ShaderSystem>().loadShader(vertex.data(), frag.data(), as.data());
    }

    ShaderKey loadComputeShader(this auto&& self, const std::string_view compute, glm::vec3 threads, const std::string_view as) requires ShaderAccess {
        return self.template getSystem<ShaderSystem>().loadComputeShader(compute.data(), threads, as.data());
    }

    ModelKey loadModel(this auto&& self, const NewModelDefinition& definition) requires ModelAccess {
        return self.template getSystem<ModelSystem>().loadModel(definition, self.template getSystem<TextureSystem>());
    }

    BufferKey createBuffer(this auto&& self, const size_t capacity) requires BufferAccess {
        return self.template getSystem<BufferSystem>().create(capacity);
    }
};

#define FRIEND_RESOURCE_LOAD_ACCESS \
    template <bool, bool, bool, bool, bool> \
    friend struct RenderResourceLoadAccess;


struct RenderResourceGetAccess {
    const Geometry& getGeometry(this auto&& self, GeometryKey key) {
        return self.template getSystem<GeometrySystem>().getGeometry(key);
    }

    const Geometry& getGeometry(this auto&& self, const std::string_view name) {
        return self.template getSystem<GeometrySystem>().getGeometry(name);
    }

    template <typename T>
    BufferView<T> getBuffer(this auto&& self, const BufferKey key) {
        return self.template getSystem<BufferSystem>().getBuffer(key);
    }

    const ModelDefinition& getModel(this auto&& self, const ModelKey key) {
        return self.template getSystem<ModelSystem>().getModel(key);
    }


    Texture2D& getTexture(this auto&& self, const Texture2DKey key) {
        return self.template getSystem<TextureSystem>().getTexture(key);
    }
};

template <typename S>
class RendererLoadView;

struct RendererResourcesStage : Stage<RendererResourcesStage> {
    using stage = RendererResourcesStage;

    static constexpr auto ExecutionModel = StageExecutionModel::SERIAL;
    static constexpr auto ScheduleModel = StageScheduleModel::MANUAL;

    template <typename System>
    using StageView = LevelLoadView<System>;
};

struct RendererLoadResources : Stage<RendererLoadResources> {
    using stage = RendererLoadResources;

    template <typename T>
    static constexpr auto Function = &T::onRendererLoad;

    template <typename T>
    constexpr static bool HasFunction = requires
    {
        &T::onRendererLoad;
    };

    static constexpr auto ExecutionModel = StageExecutionModel::SERIAL;
    static constexpr auto ScheduleModel = StageScheduleModel::MANUAL;

    template <typename System>
    using StageView = RendererLoadView<System>;
};


template <typename S>
class RendererLoadView :
    public LevelSerialView<RendererLoadResources, S>,
    public ShaderBindingsAccess,
    public RenderResourceGetAccess,
    public RenderResourceLoadAccess<true, true, true, true, true>
{
    friend struct RenderResourceGetAccess;
    friend struct ShaderBindingsAccess;
    FRIEND_RESOURCE_LOAD_ACCESS
public:
    using LevelSerialView<RendererLoadResources, S>::LevelSerialView;

    void setActiveShader(this auto&& self, ShaderKey key) {
        self.template getSystem<ShaderSystem>().getShader(key).setActiveShader();
    }

    Shader& setActiveShader(this auto&& self, const std::string_view name) {
        auto& shaderSystem = self.template getSystem<ShaderSystem>();
        auto& shader = shaderSystem.getShader(shaderSystem.getShaderKey(name.data()));
        shader.setActiveShader();
        return shader;
    }

    ComputeShader& setActiveComputeShader(this auto&& self, const std::string_view name) {
        return static_cast<ComputeShader&>(self.setActiveShader(name));
    }
};

template <typename S>
class RendererInView;

struct RendererInStage : Stage<RendererInStage> {
    using stage = RendererInStage;

    static constexpr auto ExecutionModel = StageExecutionModel::DETERMINISTIC;
    static constexpr auto ScheduleModel = StageScheduleModel::PER_FRAME;

    template <typename T>
    static constexpr auto Function = &T::onRendererIn;

    template <typename T>
    constexpr static bool HasFunction = requires
    {
        &T::onRendererIn;
    };

    template <typename System>
    using StageView = RendererInView<System>;

    template <typename S, typename Res>
    static constexpr bool ResourceAccessImpl = SystemStageDescriptor<RendererInStage, S>::template CanAccessResMut<Res>();

    template <typename S>
    static constexpr bool GeometryAccess = ResourceAccessImpl<S, GeometrySystem>;

    template <typename S>
    static constexpr bool ShaderAccess = ResourceAccessImpl<S, ShaderSystem>;

    template <typename S>
    static constexpr bool TextureAccess = ResourceAccessImpl<S, TextureSystem>;

    template <typename S>
    static constexpr bool BufferAccess = ResourceAccessImpl<S, BufferSystem>;

    template <typename S>
    static constexpr bool ModelAccess = ResourceAccessImpl<S, ModelSystem>;

    RendererProxy proxy;

    void onStageBegin(Level& level) {
        proxy.swap();
    }
};

template <typename S>
class RendererInView :
    public LevelDeterministicView<RendererInStage, S>,
    public ShaderBindingsAccess,
    public RenderResourceLoadAccess<
        RendererInStage::GeometryAccess<S>,
        RendererInStage::ShaderAccess<S>,
        RendererInStage::ModelAccess<S>,
        RendererInStage::TextureAccess<S>,
        RendererInStage::BufferAccess<S>
    >,
    public RenderResourceGetAccess,
    public LevelInTraits
{
    friend struct RenderResourceGetAccess;
    friend struct ShaderBindingsAccess;
    friend struct LevelInTraits;
    FRIEND_RESOURCE_LOAD_ACCESS
public:
    using LevelDeterministicView<RendererInStage, S>::LevelDeterministicView;
};

template <typename S> class ForwardRenderView;

struct ForwardRenderStage : Stage<ForwardRenderStage> {
    using stage = ForwardRenderStage;

    template <typename T>
    static constexpr auto Function = &T::onRender;

    template <typename T>
    constexpr static bool HasFunction = requires(T t, ForwardRenderView<T> c)
    {
        t.onRender(c);
    };

    static constexpr auto ExecutionModel = StageExecutionModel::SERIAL;
    static constexpr auto ScheduleModel = StageScheduleModel::PER_FRAME;

    template <typename System>
    using StageView = ForwardRenderView<System>;

    void onStageBegin(Level& level) {
        level.synchronize();
    }
};

template <typename S>
class ForwardRenderView : public LevelSerialView<ForwardRenderStage, S>,  public RenderResourceGetAccess {
    friend struct RenderResourceGetAccess;
public:
    using LevelSerialView<ForwardRenderStage, S>::LevelSerialView;

    const Geometry& getGeometry(GeometryKey key) {
        return this->template getSystem<GeometrySystem>().getGeometry(key);
    }

    const Geometry& getGeometry(const std::string_view name) {
        return this->template getSystem<GeometrySystem>().getGeometry(name);
    }

    void setActiveShader(this auto&& self, ShaderKey key) {
        self.template getSystem<ShaderSystem>().getShader(key).setActiveShader();
    }

    Shader& setActiveShader(this auto&& self, const std::string_view name) {
        auto& shaderSystem = self.template getSystem<ShaderSystem>();
        auto& shader = shaderSystem.getShader(shaderSystem.getShaderKey(name.data()));
        shader.setActiveShader();
        return shader;
    }

    ComputeShader& setActiveComputeShader(this auto&& self, const std::string_view name) {
        return static_cast<ComputeShader&>(self.setActiveShader(name));
    }

    unsigned getBufferID(const BufferKey key) {
        return this->template getSystem<BufferSystem>().getBufferID(key);
    }
};


template <typename View, typename... Params>
concept ShaderBindableView = Viewable<View, Params...> && requires(View v)
{
    v.setActiveShader("");
};