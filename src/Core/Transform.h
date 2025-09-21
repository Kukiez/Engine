#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "ECS/Component/Component.h"
#include "glm/fwd.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <util/glm_double.h>
#include <iomanip>

struct Transform : PrimaryComponent, TrackedComponent {
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::quat rotation = glm::quat(1, 0, 0, 0);

    Transform() = default;

    Transform(const float x, const float y, float z) : translation(x, y, z) {}

    Transform(const glm::vec3 translation, const glm::vec3 scale = glm::vec3(1), const glm::vec3 r = glm::vec3(0))
        : translation(translation), scale(scale) {
        setRotation(r.x, r.y, r.z);
    }

    Transform(const glm::vec3 translation, const glm::vec3 scale, const glm::quat rotation)
        : translation(translation), scale(scale), rotation(rotation) {}

    void setRotation(const float pitch, const float yaw, const float roll) {
        const glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1,0,0));
        const glm::quat qYaw   = glm::angleAxis(glm::radians(yaw),   glm::vec3(0,1,0));
        const glm::quat qRoll  = glm::angleAxis(glm::radians(roll),  glm::vec3(0,0,1));
        rotation = qYaw * qPitch * qRoll;
    }

    void setYaw(const float yaw) {
        const glm::vec3 euler = glm::eulerAngles(rotation);
        const float pitch = glm::degrees(euler.x);
        const float roll = glm::degrees(euler.z);

        const glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1,0,0));
        const glm::quat qYaw   = glm::angleAxis(glm::radians(yaw), glm::vec3(0,1,0));
        const glm::quat qRoll  = glm::angleAxis(glm::radians(roll), glm::vec3(0,0,1));

        rotation = qYaw * qPitch * qRoll;
    }

    void setRotation(const glm::quat rotation) {
        this->rotation = rotation;
    }

    void addYaw(const float yaw) {
        const glm::quat yawRotation = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = yawRotation * rotation;
    }

    void addPitch(const float pitch) {
        const glm::quat pitchRotation = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        rotation = pitchRotation * rotation;
    }

    void addRoll(const float roll) {
        const glm::quat rollRotation = glm::angleAxis(glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = rollRotation * rotation;
    }

    [[nodiscard]] glm::mat4 createModel3D() const {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, translation);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);
        return model;
    }

    [[nodiscard]] glm::mat4 createModel3DSkew(const glm::vec3 skew) const {
        const glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
        const glm::mat4 R = glm::mat4_cast(rotation);
        const glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 skewMat(1.0f);

        skewMat[1][0] = skew.x; // XY shear factor: second row, first column
        skewMat[2][0] = skew.y; // XZ shear factor: third row, first column
        skewMat[2][1] = skew.z; // YZ shear factor: third row, second column

        return T * R * skewMat * S;
    }

    friend std::ostream& operator<<(std::ostream& os, const Transform& t) {
        os << std::fixed << std::setprecision(3);
        os << "Transform {\n"
           << "  Translation: (" << t.translation.x << ", " << t.translation.y << ", " << t.translation.z << ")\n"
           << "  Scale:       (" << t.scale.x << ", " << t.scale.y << ", " << t.scale.z << ")\n"
           << "  Rotation: (" << t.rotation.x << ", " << t.rotation.y << ", "
                                    << t.rotation.z << ", " << t.rotation.w << ")\n"
           << "}";
        return os;
    }

};

inline Transform operator*(const Transform& parent, const Transform& child) {
    Transform out;
    out.scale = parent.scale * child.scale;
    out.rotation = parent.rotation * child.rotation;
    out.translation = parent.translation + (parent.rotation * (parent.scale * child.translation));
    return out;
}

inline Transform& operator*=(Transform& self, const Transform& rhs) {
    self = self * rhs;
    return self;
}

struct Velocity : PrimaryComponent, glm::vec3 {
    using glm::vec3::vec3;

    Velocity() : glm::vec3(0) {}
    Velocity(const glm::vec3& velocity) : glm::vec3(velocity) {}
};

struct CameraOps {
    void rotate(this auto&& camera, const float yaw, const float pitch) {
        camera.yaw = yaw;
        camera.pitch = pitch;

        if (camera.pitch > 89.0f) {
            camera.pitch = 89.0f;
        } else if (camera.pitch < -89.0f) {
            camera.pitch = -89.0f;
        }

        glm::vec3 temp;
        temp.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        temp.y = sin(glm::radians(camera.pitch));
        temp.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(temp);

        camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
        camera.up    = glm::normalize(glm::cross(camera.right, camera.front));
    }
};

struct CameraComponent : PrimaryComponent, TrackedComponent, CameraOps {
    float pitch = 0;
    float yaw = -90;
    float fov = 45;
    float nearClip = 0.1f;
    float farClip = 10000.0f;

    glm::vec3 position   = glm::vec3(0.0f, -0.5f,  3.0f);
    glm::vec3 front      = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up         = glm::vec3(0.0f, 1.0f,  0.0f);
    static inline glm::vec3 worldUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 right;

    glm::mat4 view;
    glm::mat4 projection;
    float sensitivity    = 0.001f;
    float viewDistance = 640;

    float width = 2560;
    float height = 1440;

    CameraComponent(glm::vec3 spawnPosition, const float yaw, const float pitch) : position(spawnPosition), yaw(yaw), pitch(pitch) {
        right = glm::normalize(glm::cross(front, up));
        view = glm::lookAt(position, position + front, up);
        projection = glm::perspective(glm::radians(fov), width / height, nearClip, farClip);
    }

    glm::mat4 getCameraWorldMatrix() const {
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        glm::vec3 camUp = glm::normalize(glm::cross(right, front));

        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0][0] = right.x;
        rotation[1][0] = right.y;
        rotation[2][0] = right.z;

        rotation[0][1] = camUp.x;
        rotation[1][1] = camUp.y;
        rotation[2][1] = camUp.z;

        rotation[0][2] = -front.x;
        rotation[1][2] = -front.y;
        rotation[2][2] = -front.z;

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 cameraWorldMatrix = translation * rotation;
        return cameraWorldMatrix;
    }

    bool isWithinFieldOfView(const CameraComponent& camera, const Transform& transform) {
        const glm::vec3 toObject = transform.translation - camera.position;

        if (const float distance = glm::length(toObject); distance > 130.0f) {
            return false;
        }
        const glm::vec3 toObjectDir = glm::normalize(toObject);
        const float dot = glm::dot(camera.front, toObjectDir);

        return true;
    }

    void move(this auto&& camera, const glm::vec3 position) {
        camera.position = position;
    }

    void updateViewMatrix(this auto&& camera) {
        camera.view  = glm::lookAt(camera.position, camera.position + camera.front , camera.up);
    }
};

struct CameraChangeEvent {
    Entity entity;
    CameraComponent camera;

    CameraChangeEvent(const Entity entity, const CameraComponent& camera) : entity(entity), camera(camera) {}
};

struct PlayerComponent : PrimaryComponent, CameraOps {
    glm::vec3 front      = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up         = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 right      = glm::normalize(glm::cross(front, up));
    glm::vec3 worldUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    float yaw = 0;
    float pitch = 0;
    glm::vec2 sensitivity = glm::vec2(.1f);

    PlayerComponent() = default;
    PlayerComponent(const float yaw, const float pitch) : yaw(yaw), pitch(pitch) {}
};