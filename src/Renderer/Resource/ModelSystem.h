#pragma once
#include "Resource.h"
#include "TextureSystem.h"

class ModelSystem {
    std::vector<const ModelDefinition*> models;

    std::unordered_map<const ModelDefinition*, size_t> defToKey;
public:
    ModelSystem() = default;

    ModelSystem(const ModelSystem&) = delete;
    ModelSystem(ModelSystem&&) = delete;

    ModelKey loadModel(const NewModelDefinition& model, TextureSystem& textureSystem) {
        ModelLoaderSystem::loadModelGeometry(model.definition->geometry);

        for (const auto& path : model.texturePaths) {
            std::string fullPath = model.directory + "/" + path;
            std::cout << "Directory: " << model.directory << std::endl;
            std::cout << "Path: " << fullPath << std::endl;
            std::cout << "Texture: " << path << std::endl;
            Texture2D texture(fullPath.c_str(), model.flip);
            model.definition->textures.emplace_back(
                textureSystem.loadTexture(std::move(texture))
            );
        }
        models.emplace_back(model.definition);
        return ModelKey{models.size() - 1};
    }

    ModelKey getModelKey(const ModelDefinition* def) {
        const auto it = defToKey.find(def);
        return it != defToKey.end() ? ModelKey{it->second} : ModelKey{0};
    }

    const ModelDefinition& getModel(ModelKey key) const {
        cexpr::require(static_cast<size_t>(key) != 0);
        return *models[static_cast<size_t>(key)];
    }
};
