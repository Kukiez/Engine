#pragma once
#include <Transform.h>

struct UpdateConnection {};

struct ChangeListener : LevelOutStage::Reads<CameraComponent>, Stages<LevelOutStage> {
    void onLevelLoad(LevelLoadView<ChangeListener> level) {
        level.enableEventEmission<CameraComponent>();
    }

    void onLevelOut(LevelOutView<ChangeListener> level) {
        level.query<CameraComponent>().forEachChanged<CameraComponent>([&](Entity e, const CameraComponent& camera) {
            level.send<CameraChangeEvent>(e, camera);
        });
    }
};


struct EntityMapper : ResourceSystem<ConflictMode::SHARED>, Stages<LevelOutStage> {
    std::unordered_map<Entity, Entity> entities;
    tbb::concurrent_unordered_map<Entity, Entity> concurrentEntities;

    void addEntity(const Entity& entity, const Entity& mappedEntity) {
        concurrentEntities.emplace(entity, mappedEntity);
    }

    auto get(const Entity& entity) const {
        return mem::find_or(entities, entity, [&] {
            const auto it = concurrentEntities.find(entity);

            if (it != concurrentEntities.end()) {
                return it->second;
            }
            return NullEntity;
        });
    }

    void onLevelOut(LevelOutView<EntityMapper> level) {
        for (auto& [entity, mapped] : concurrentEntities) {
            entities.emplace(entity, mapped);
        }
        concurrentEntities.clear();
    }
};

struct ChangeReader : ReadsResources<EntityMapper>, Writes<CameraComponent> {
    void onRendererIn(RendererInView<ChangeReader> level) const {
        auto camChanges = level.read<CameraChangeEvent>();
        for (auto& change : camChanges) {
            auto entity = level.get<EntityMapper>().get(change.entity);

            if (!level.has<CameraComponent>(entity)) {
                entity = level.createEntity(change.camera);
                level.get<EntityMapper>().addEntity(change.entity, entity);
            } else {
                auto& camera = *level.get<CameraComponent>(entity);
                camera = change.camera;
            }
        }
    }
};