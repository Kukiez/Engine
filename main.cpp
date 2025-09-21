#include <iostream>
#include <Math/Shapes/AABB.h>
#include <Core/Transform.h>
#include <ECS/ECS.h>

#include <Core/App.h>

#include "Input/InputSystem.h"
#include <Core/World/TerrainWorld.h>

#include "Model/AnimationRegistry.h"
#include "Model/ModelAnimationPlayer.h"
#include <Core/Model/ModelLoaderSystem.h>
#include <Minecraft/Voxel/VoxelWorld.h>

#include <Core/Player/PlayerController.h>
#include <Core/Collision/MovementSystem.h>
#include <Renderer/Systems/ChangeListener.h>
#include <Core/Collision/CollisionSystem.h>
#include <Renderer/AABBRenderer.h>
#include <Renderer/Systems/SkyboxSystem.h>
#include <Renderer/Resource/ShaderSystem.h>
#include <Renderer/ModelRenderer.h>
#include <Core/Model/ModelGraphUpdateSystem.h>
#include <Core/World/RenderWorld.h>

int main(int argc, char* argv[]) {
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    flags |= _CRTDBG_ALLOC_MEM_DF;
    flags |= _CRTDBG_CHECK_CRT_DF;
    flags |= _CRTDBG_LEAK_CHECK_DF;

    _CrtSetDbgFlag(flags);

    // Make CRT break on memory errors
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);  // send to debugger
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR); // optional
    _CrtSetBreakAlloc(-1); // break on a specific allocation number (set later if needed)
    assert(_CrtCheckMemory());
    using RendererStages = StageDetector<RendererLoadResources, LevelInStage, LevelOutStage, ForwardRenderStage, RendererInStage>;
    using OverworldStages = StageDetector<LevelInStage, LevelOutStage, PhysicsStage>;
    Level overworld("Overworld");
    Level renderer("Renderer");
    assert(_CrtCheckMemory());
    auto& ovIn = overworld.addStage<LevelInStage>();
    auto& renIn = renderer.addStage<RendererInStage>();

    overworld.addStage<PhysicsStage>();

    overworld.addStage<LevelOutStage>(&renIn.proxy, &renderer); // overworldOut writes to RendererIn
    renderer.addStage<ForwardRenderStage>();
    renderer.addStage<LevelOutStage>(&ovIn.proxy, &overworld); // rendererOut writes to OverworldIn

    renderer.addStage<RendererLoadResources>();
    assert(_CrtCheckMemory());
    auto owAssembler = overworld.createSystemAssembler<ECSStageDetector, OverworldStages>();
    owAssembler.addSystem<InputSystem::InputReceiver>();
    owAssembler.addSystem<PlayerController>();
    owAssembler.addSystem<MovementSystem>();
    owAssembler.addSystem<ChangeListener>();
    owAssembler.addSystem<SlowdownSystem>();
    owAssembler.addSystem<TerrainWorld>();
    owAssembler.addSystem<CollisionScanners>();
    owAssembler.addSystem<CollisionNarrowPhase>();
    owAssembler.addSystem<AABBRendererCollector>();
    owAssembler.addSystem<SkyboxGameEngineSystems>();
    owAssembler.addSystem<CameraMovementSystem>();
    owAssembler.addSystem<ModelLoaderSystem>();
    owAssembler.addSystem<ModelGraphUpdateSystem>();
    owAssembler.addSystem<ModelSendSystem>();
    assert(_CrtCheckMemory());
    overworld.enableEventEmission<StaticCollider>();
  //  overworld.createEntity(StaticCollider{}, Transform(glm::vec3(0, 50, 0)), AABBCollision(glm::vec3(0), glm::vec3(3)));
    overworld.initialize();
    assert(_CrtCheckMemory());
    auto assembler = renderer.createSystemAssembler<RendererStages, ECSStageDetector>();

    assembler.addSystem<GeometrySystem>();
    assembler.addSystem<ShaderSystem>();
    assembler.addSystem<ModelSystem>();
    assembler.addSystem<BufferSystem>();
    assembler.addSystem<TextureSystem>();

    assembler.addSystem<FrustumCullingSystem>();
    assembler.addSystem<EntityMapper>();
    assembler.addSystem<TerrainWorld>();
    assembler.addSystem<InputSystem::InputPoller>();
    assembler.addSystem<RenderingSystem>();
    assembler.addSystem<ModelLoaderSystem>();
    assembler.addSystem<ChangeReader>();
    assembler.addSystem<SkyboxRenderingSystems>();
    assembler.addSystem<AABBRenderer>();
    assembler.addSystem<ModelUploadSystem>();

    AppSystem app{};
    app.onLevelLoad();
    assert(_CrtCheckMemory());
    renderer.load<RendererLoadResources>();
    assert(_CrtCheckMemory());
    renderer.getStageView<RendererLoadResources>().loadGeometry("assets/Geometry/icosahedron.gltf", "Icosahedron");
    renderer.run<RendererLoadResources>();
    renderer.initialize();

    auto& s = overworld.getSystem<ModelLoaderSystem>();

    ModelLoadParams params;
    params.baseScale = glm::vec3(0.06);
    params.path = "assets/zombieFinal.gltf";
    params.name = "Zombie";
    params.flipTextureUV = true;
    params.bakeTransforms = true;
    auto zombie = s.loadModel(params);
    auto e = overworld.createEntity(Model(zombie), Transform(glm::vec3(90, 0, 0)));

    for (auto& node : zombie->nodes) {
        std::cout << node.name << std::endl;
        std::cout << "  : " << node.localTransform << std::endl;
    }
    size_t i = 0;
    while (true) {
        overworld.run();
        renderer.run();
        app.onUpdate();
    }
    app.onLevelUnload();
    assert(_CrtCheckMemory());

    // for (const auto& system : overworld.getRegisteredSystems()) {
    //     std::cout << overworld.getProfileReport(system.id) << std::endl;
    // }
    return 0;
}
