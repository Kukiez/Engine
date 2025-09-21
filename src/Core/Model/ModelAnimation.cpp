#pragma once
#include "ModelAnimation.h"

#include <math/Rotation.h>

AnimationChannel & ModelAnimation::findChannel(const std::string &mesh) {
    const auto it = std::ranges::find_if(channels,
                               [&](const AnimationChannel& c) { return c.target == mesh; });
    if (it == channels.end()) {
        channels.emplace_back(mesh);
        return channels.back();
    }
    return *it;
}

void ModelAnimation::addFromPose(const std::string &mesh, const double time, const MeshPose &pose, const MeshAffected affected) {
    if (affected & MeshAffected::POSITION) {
        addTranslation(mesh, time, pose.transform.translation);
    }
    if (affected & MeshAffected::ROTATION) {
        addRotation(mesh, time, pose.transform.rotation);
    }
    if (affected & MeshAffected::SCALE) {
        addScale(mesh, time, pose.transform.scale);
    }
    if (affected & MeshAffected::COLOR) {
        addTint(mesh, time, Color(pose.tint.x, pose.tint.y, pose.tint.z, 1));
    }
}

void ModelAnimation::addRotation(const std::string &mesh, double time, const float pitch, const float yaw,
                                 const float roll) {
    auto& kfs = findChannel(mesh);
    kfs.rotation.add(Rotation(pitch, yaw, roll).quat(), time);
    if (time > duration) {
        duration = time;
    }
}

void ModelAnimation::addRotation(const std::string &mesh, double time, const glm::quat rot) {
    auto& kfs = findChannel(mesh);
    kfs.rotation.add(rot, time);

    if (time > duration) {
        duration = time;
    }
}

void ModelAnimation::addTranslation(const std::string &mesh, double time, glm::vec3 position) {
    auto& kfs = findChannel(mesh);
    kfs.position.add(position, time);
    if (time > duration) {
        duration = time;
    }
}

void ModelAnimation::addScale(const std::string &mesh, double time, glm::vec3 scale) {
    auto& kfs = findChannel(mesh);
    kfs.scale.add(scale, time);

    if (time > duration) {
        duration = time;
    }
}

void ModelAnimation::addTint(const std::string &mesh, double time, const Color color) {
    auto& kfs = findChannel(mesh);
    kfs.tint.add(Color::xyz(color), time);

    if (time > duration) {
        duration = time;
    }
}
