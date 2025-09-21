#pragma once
#include "Model.h"

struct ModelGraphUpdateSystem : Writes<Model>, Reads<Transform> {
    FRIEND_DESCRIPTOR

    static void onLevelLoad(LevelLoadView<ModelGraphUpdateSystem> view) {
        view.enableEventEmission<Model>();
    }

    static void onLevelOut(LevelOutView<ModelGraphUpdateSystem> view) {
        view.query<Model>().forEachNewComponent([&](const Entity e, Model& model) {
            if (auto transform = view.get<Transform>(e)) {
                model.update(transform->createModel3D());
            }
        });
        view.query<Model, Transform>().forEachChanged<Transform>([&](const Entity e, Model& model, const Transform& transform) {
            model.update(transform.createModel3D());
            model.clearDirtyNodes();
        });
    }
};
