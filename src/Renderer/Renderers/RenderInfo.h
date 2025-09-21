#pragma once

class EntityManager;
class Renderer;
struct CameraComponent;
class FrustumCullingSystem;
class Scene;
class Rasterizer;
struct WorldChunk;
class Entity;
class RenderWorldPartitionSystem;
struct BoundingVolumeChunk;

struct RenderInfo {
    const FrustumCullingSystem& frustum;
    Renderer& renderer;
    Rasterizer& rasterizer;

    RenderInfo(const FrustumCullingSystem& frustum, Renderer& renderer, Rasterizer& rasterizer)
    : frustum(frustum),  renderer(renderer), rasterizer(rasterizer){}
};

struct RenderKey {
    unsigned key = 0;

    constexpr bool operator==(const RenderKey& other) const {
        return key == other.key;
    }

    friend std::ostream& operator<<(std::ostream& os, const RenderKey& key) {
        os << key.key;
        return os;
    }
};

template<> struct std::hash<RenderKey> {
    std::size_t operator()(const RenderKey& k) const noexcept {
        return std::hash<unsigned>()(k.key) ^ std::hash<unsigned>()(k.key) << 1;
    }
};

class RenderKeyRegistry {
    static unsigned next;
    static std::unordered_map<std::string, RenderKey> keys;
public:
    static RenderKey registerKey(const std::string& key) {
        keys[key] = RenderKey(next++);
        return keys[key];
    }

    static RenderKey getKey(const std::string& key) {
        return keys[key];
    }
};

struct RenderPass {
    struct PrePass {
        virtual void onRenderPrePass(Scene&, RenderInfo&) = 0;
    };

    struct MainPass {
        virtual ~MainPass() = default;

        virtual void onRender(RenderInfo&) = 0;
    };
};