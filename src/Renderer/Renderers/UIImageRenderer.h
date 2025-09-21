#pragma once
#include <openGL/BufferObjects/BufferGeneral.h>
#include <Core/Renderer/RenderingDetails.h>
#include "RenderInfo.h"

class ResourceSystem;
class Scene;
struct RenderInfo;

class UIImageRenderer final : public RenderPass::MainPass {
    ResourceSystem* resourceSystem;
    BufferMetadata VAO;
    DrawElements drawElements;
public:
    explicit UIImageRenderer(ResourceSystem* resourceSystem);
    void onRender(RenderInfo & info) override;
};