#pragma once
#include <openGL/shaders/Shapes/GeometryCache.h>
#include <Systems/Components/UIComponents.h>
#include <util/SortedArray.h>
#include <Creator/Scene.h>
#include <Creator/EntityManager.h>
#include <openGL/BufferObjects/ShaderStorageBuffer.h>
#include <Renderer/RenderingDetails.h>
#include "RenderInfo.h"

class UIRenderingSystem final : public RenderPass::MainPass {
    struct Renderable {
        Entity entity;
        int zOrder;

        bool operator > (const Renderable &other) const {
            return zOrder < other.zOrder;
        }
    };

    struct alignas(16) UIUniform {
        glm::mat4 model;
        UIElement::Color color;

        bool operator > (const UIUniform& other) const {
            return model[3].z < other.model[3].z;
        }

        bool operator == (const UIUniform& other) const {
            return model[3].z == other.model[3].z;
        }
        };

    using RenderCache = std::vector<Renderable>;

    ShaderStorageBuffer uniformsStorage;
    SortedBuffer<UIUniform> uniforms;
    SortedBuffer<Renderable> renderables;

    struct RenderIndex {
        DrawArraysInstancedBase command;
        glm::vec4 scissors;

        void draw(const RenderInfo& info) const {
            command.draw();
        }
    };
    RenderIndex order;
    Scene* scene;
public:
    UIRenderingSystem(Scene& scene) : scene(&scene) {
        uniformsStorage.allocate(sizeof(UIUniform) * 6400, BufferUsage::STATIC);
        uniforms.reallocate(6400);
        renderables.reallocate(6400);

        DrawArraysInstancedBase command;

        command.primitive = GLDrawPrimitive::TRIANGLES_FAN;
        command.first_vertex = 0;
        command.vertex_count = 4;
        command.first_instance = 0;
        command.instances = 0;

        order = {command, glm::vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)};
    }

    void uploadToBuffer(int index) {
        if (uniforms.size() >= uniformsStorage.getAllocatedBytes() / sizeof(UIUniform)) {
            const int newSize = uniforms.capacity() * 2;
            uniformsStorage.reallocate(newSize * sizeof(UIUniform), BufferUsage::DYNAMIC);

            uniforms.reallocate(newSize);
            renderables.reallocate(newSize);
        }
        const Buffer::MappedData mapped = uniformsStorage.map(BufferAccessMode::WRITE_ONLY);
        auto* data = mapped.get<UIUniform>();
        memcpy(data, uniforms.data(), uniforms.size() * sizeof(UIUniform));
    }

    void onRender(RenderInfo& info) override {
        static unsigned id = GCache::inst().query(Q::WIDGET,  GeometryCache::VERTEX_ARRAY).ID;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        info.renderer.setActiveShader("WidgetShader");
        glBindVertexArray(id);
        uniformsStorage.setToBindingPoint(1);

        order.draw(info);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    static bool isUIElementVisible(const UIParent& parent, const UITransform& elementTransform) {
        const glm::vec2 pos(elementTransform.world.pos.x, elementTransform.world.pos.y);
        const glm::vec2 size(elementTransform.world.scale.x, elementTransform.world.scale.y);

        const auto& t = parent.transform;
        const auto region = glm::vec4(t.pos.x, t.pos.y, t.pos.x + t.scale.x,
                                     t.pos.y + t.scale.y);

        return !(pos.x + size.x < region.x ||
                 pos.x > region.z ||
                 pos.y + size.y < region.y ||
                 pos.y > region.w);
    }

    [[nodiscard]] static glm::mat4 createZOrderModel2D(const UITransform::Transform& world, int z) {
        auto model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(glm::vec2(world.pos.x, world.pos.y), 0.0f));
        model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(glm::vec2(world.scale.x, world.scale.y) , 1.0f));

        model[3].z = z;
        return model;
    }

    void addUIElementRenderComponent(const Entity& entity, const UIElement& element) {
        auto& transform = scene->getEntityManager().getComponent<UITransform&>(entity);
        if (!isUIElementVisible(
            scene->getEntityManager().getComponent<UIParent&>(element.parent), transform)
        ) return;

        const int zIndex = transform.layer.zIndex;

        uniforms.insert(UIUniform{
            .model = createZOrderModel2D(transform.world, zIndex),
            .color = element.color
        });

        const int i = renderables.insert(Renderable{
            .entity = entity,
            .zOrder = zIndex
        });

        uploadToBuffer(i);
        order.command.instances++;
    }

    void removeUIUniform(const int index) {
        uniforms.erase(index);
        renderables.erase(index);

        uploadToBuffer(index);
        order.command.instances--;
    }

    void updateRenderComponent(const Entity& target) {
        const int i = renderables.find([&target](const Renderable& renderable) {
            return target == renderable.entity;
        });

        const auto* element   = scene->getEntityManager().getComponent<UIElement>(target);
        const auto* parent    = scene->getEntityManager().getComponent<UIParent>(element->parent);
        const auto* transform = scene->getEntityManager().getComponent<UITransform>(target);

        if (i != -1) {
            if (!isUIElementVisible(*parent, *transform)) {
                removeUIUniform(i);
                return;
            }

            const int zIndex = transform->layer.zIndex;

            uniforms.erase(i);
            renderables.erase(i);

            uniforms.insert(UIUniform{
                .model = createZOrderModel2D(transform->world, zIndex),
                .color = element->color
            });

            renderables.insert(Renderable{
                .entity = target,
                .zOrder = zIndex
            });
            uploadToBuffer(i);
        } else {
            addUIElementRenderComponent(target, *element);
        }
    }

    void setUIElementVisibility(const Entity& entity, const UIElement& element, const bool visibility) {
        const int i = renderables.find([&entity](const Renderable& renderable) {
            return renderable.entity == entity;
        });

        if (visibility) {
            if (i == -1) {
                addUIElementRenderComponent(entity, element);
            }
        } else {
            if (i != -1) {
                removeUIUniform(i);
            }
        }
    }

    void removeUIElement(const Entity& entity) {
        const int i = renderables.find([&entity](const Renderable& renderable) {
            return entity == renderable.entity;
        });

        if (i != -1) {
            removeUIUniform(i);
        }
    }
};
