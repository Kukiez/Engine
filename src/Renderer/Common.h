#pragma once
#include <Collision/CollisionComponents.h>
#include <ECS/ECS.h>

class LevelConnectionStream {
    template <typename T>
    using vector = mem::vector<T>;

    mutable std::unordered_map<const mem::type_info*,
        vector<char>
    > updates;

    template <typename T>
    auto& getTarget() {
        auto& vec = mem::find_or_emplace(updates, mem::type_info_of<T>, [&] {
            return vector<char>();
        });
        return reinterpret_cast<vector<T>&>(vec);
    }
    mem::vector<AABBCollision> aabb;
public:
    LevelConnectionStream() = default;

    void clear() const {
        for (auto & [type, vec] : updates) {
            char* data = vec.data();
            const size_t size = vec.size();

            mem::destroy_at(type, data, size);
            vec.clear();
            vec.reserve(size);
        }
    }

    template <typename T>
    void sendOne(T&& val) {
        auto& typedVector = getTarget<T>();
        typedVector.emplace_back(std::forward<T>(val));
    }

    template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
    void send(Args&&... args) {
        auto& typedVector = getTarget<T>();
        typedVector.emplace_back(std::forward<Args>(args)...);
    }

    template <typename T>
    requires cexpr::is_iteratable_v<T>
    void send(T&& data) {
        using value_type = std::decay_t<cexpr::iterator_yield_t<T>>;

        auto& typedVector = getTarget<value_type>();
        typedVector.insert(typedVector.end(), data);
    }

    template <typename T>
    auto read() {
        const auto it = updates.find(mem::type_info_of<T>);

        if (it == updates.end()) {
            return mem::make_null_range<T>();
        }
        auto& typedVector = reinterpret_cast<vector<T>&>(it->second);
        return mem::make_range(typedVector.data(), typedVector.size());
    }
};

struct LevelInStage;
struct LevelOutStage;

template <typename> class LevelInView;
template <typename> class LevelOutView;

struct RendererProxy {
    LevelConnectionStream inUpdates[2];
    std::atomic<uint8_t> writeIndex;

    auto& getReadStream() {
        return inUpdates[writeIndex];
    }

    auto& getWriteStream() {
        return inUpdates[!writeIndex];
    }

    void swap() {
        const uint8_t idx = writeIndex;
        inUpdates[idx].clear();
        writeIndex = !idx;
    }
};

struct LevelInStage : Stage<LevelInStage> {
    using stage = LevelInStage;

    template <typename T>
    static constexpr auto Function = &T::onLevelIn;

    template <typename T>
    static constexpr auto HasFunction = requires
    {
        &T::onLevelIn;
    };

    static constexpr auto ExecutionModel = StageExecutionModel::DETERMINISTIC;
    static constexpr auto ScheduleModel = StageScheduleModel::PER_FRAME;

    template <typename System>
    using StageView = LevelInView<System>;
    /* Deterministic: LevelUpdateView,
     * Serial: LevelSerialView,
     * Parallel: LevelParallelView,
     */

    struct OnInherit {
        template <typename S>
        using UpdateView = StageView<S>;

        template <typename S>
        using In = ::In<LevelInStage, S>;
    };
    // OnStageEnd, onStageBegin, ShouldRun, GetHz

    RendererProxy proxy;

    void onStageBegin(Level& level) {
        proxy.swap();
    }
};



struct LevelOutStage : Stage<LevelOutStage> {
    using stage = LevelOutStage;

    template <typename T>
    static constexpr auto Function = &T::onLevelOut;

    template <typename T>
    static constexpr auto HasFunction = requires
    {
        &T::onLevelOut;
    };

    static constexpr auto ExecutionModel = StageExecutionModel::DETERMINISTIC;
    static constexpr auto ScheduleModel = StageScheduleModel::PER_FRAME;

    template <typename System>
    using StageView = LevelOutView<System>;

    struct OnInherit {
        template <typename S>
        using UpdateView = StageView<S>;

        template <typename S>
        using In = ::In<LevelOutStage, S>;
    };

    RendererProxy* proxy;
    Level* level;

    LevelOutStage(RendererProxy* proxy, Level* level) : proxy(proxy), level(level) {}

    void onStageBegin(Level& level) {
        level.synchronize();
    }
};

class LevelInTraits {
    auto& getStream(this auto&& self) {
        return self.stage->proxy.getReadStream();
    }
public:
    template <typename T>
    auto read(this auto&& self) {
        return self.getStream().template read<T>();
    }
};

template <typename T>
class LevelInView : public LevelDeterministicView<LevelInStage, T>, public LevelInTraits {
    friend class LevelInTraits;
public:
    using LevelDeterministicView<LevelInStage, T>::LevelDeterministicView;
};

template <typename S>
using LevelInIn = In<LevelInStage, S>;

template <typename T>
class LevelOutView : public LevelDeterministicView<LevelOutStage, T> {
    auto& getStream() {
        return this->stage->proxy->getWriteStream();
    }
public:
    using LevelDeterministicView<LevelOutStage, T>::LevelDeterministicView;

    template <typename T>
    void sendOne(T&& val) {
        getStream().sendOne(std::forward<T>(val));
    }

    template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
    void send(Args&&... args) {
        getStream().send<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    requires cexpr::is_iteratable_v<T>
    void send(T&& data) {
        getStream().send(std::forward<T>(data));
    }

    template <typename S>
    S& getRendererSystem() {
        return this->stage->level->template getSystem<S>();
    }
};