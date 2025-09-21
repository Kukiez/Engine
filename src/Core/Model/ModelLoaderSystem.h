#pragma once

#include <Math/Shapes/AABB.h>
#include "Model.h"
#include <memory/hash.h>

#include <ECS/ECS.h>
#include <expected>
#include <Renderer/Common.h>

class Texture2D;

struct aiNode;
struct aiScene;
struct aiMesh;

class ModelAnimation;
class TextureLoadInfo;

struct RenderModelCache {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

namespace Assimp { class Importer;}

enum class ModelLoadError {
	SUCCESS, FILE_NOT_FOUND
};

struct ModelLoadParams {
	std::string path;
	std::string name;
	glm::vec3 baseScale = glm::vec3(1);
	bool flipTextureUV = false;
	bool bakeTransforms = false;
};

class ModelLoaderSystem : ResourceSystem<> {
	FRIEND_DESCRIPTOR
	mem::unordered_stringmap<ModelDefinition*> modelAssets;
	std::vector<NewModelDefinition> newDefinitions;
public:
	ModelLoaderSystem() = default;

	ModelLoaderSystem(const ModelLoaderSystem&) = delete;
	ModelLoaderSystem(ModelLoaderSystem&&) = delete;
	ModelLoaderSystem& operator=(const ModelLoaderSystem&) = delete;
	ModelLoaderSystem& operator=(ModelLoaderSystem&&) = delete;

	static Geometry& loadModelGeometry(Geometry &geometry);

	const ModelDefinition* loadModel(const ModelLoadParams& params);

	static std::expected<Geometry, ModelLoadError> loadGeometry( std::string_view path);

	void onLevelOut(LevelOutView<ModelLoaderSystem> view) {
		for (auto& newDef : newDefinitions) {
			view.send<NewModelDefinition>(std::move(newDef)); // -> @class ModelSystem
		}
		newDefinitions.clear();
	}
};