#include "UIImageRenderer.h"
#include <openGL/shaders/Shapes/GeometryCache.h>
#include <Systems/Components/UIComponents.h>
#include <Systems/ResourceSystem.h>
#include <Renderer/Renderer.h>
#include <Creator/EntityManager.h>
#include <openGL/shaders/Shader.h>
#include <openGL/shaders/UniformData.h>

UIImageRenderer::UIImageRenderer(ResourceSystem *resourceSystem) : resourceSystem(resourceSystem) {
    VAO = GCache::inst().query(Q::IMAGE_2D)[GCache::VERTEX_ARRAY];

    drawElements.indices = nullptr;
    drawElements.indices_count = 6;
    drawElements.primitive = GLDrawPrimitive::TRIANGLES;
    drawElements.type = GLType::UNSIGNED_INT;
}


void UIImageRenderer::onRender(RenderInfo& info) {
    auto& renderer = info.renderer;

    glBindVertexArray(VAO.ID);
    renderer.setActiveShader("Image2DShader");

    glActiveTexture(GL_TEXTURE0);

    info.entityManager.view<UITransform, UIImage>().forEach([&](const UITransform* element, const UIImage* image) {
        const std::string& texture = image->texture;

        UniformData uniforms(renderer.getActiveShader()->id());

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(glm::vec2(element->world.pos.x, element->world.pos.y), 0.0f));
        model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(glm::vec2(element->world.scale.x, element->world.scale.y) , 1.0f));
        model[3].z = element->layer.zIndex;

        uniforms.upload_nowrite("model", model);

        glBindTexture(GL_TEXTURE_2D, resourceSystem->getTexture2D(texture).textureID());

        drawElements.draw();
    });
}