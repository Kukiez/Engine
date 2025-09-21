#pragma once

#include <utility>

#include "FCurve.h"
#include "ModelPose.h"
#include <Util/Color.h>

struct AnimationChannel {
    std::string target;
    FCurve3 position;
    FCurve3 scale;
    FCurve3 tint;
    QCurve rotation;
};


class ModelAnimation : public PrimaryComponent {
    AnimationChannel& findChannel(const std::string& mesh);

    std::string name;
    std::vector<AnimationChannel> channels;
    double duration = 0;
public:
    ModelAnimation() = default;
    explicit ModelAnimation(std::string animationName) : name(std::move(animationName)) {}

    void addFromPose(const std::string& mesh, double time, const MeshPose& pose, MeshAffected affected);
    void addRotation(const std::string& mesh, double time, float pitch, float yaw, float roll);
    void addRotation(const std::string& mesh, double time, glm::quat rot);
    void addTranslation(const std::string& mesh, double time, glm::vec3 position);
    void addScale(const std::string& mesh, double time, glm::vec3 scale);
    void addTint(const std::string& mesh, double time, Color color);

    void clear() {
        channels.clear();
    }

    const std::vector<AnimationChannel>& getChannels() const {
        return channels;
    }

    double getDuration() const {
        return duration;
    }

    const std::string& getName() const {
        return name;
    }
};