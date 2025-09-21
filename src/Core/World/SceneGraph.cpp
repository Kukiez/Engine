//
// Created by dkuki on 5/29/2025.
//

#include "SceneGraph.h"
#include <Systems/Text3DSystem.h>
#include <iomanip>
#include <Systems/Signals/ISignal.h>

void SceneGraph::printHierarchyRecursive(EntityManager &em, Entity entity, int detailLevel,
                                         const std::vector<bool> &hasNextSibling) {
    int depth = (int)hasNextSibling.size();

    for (int i = 0; i < depth; ++i) {
        if (i == depth - 1) {
            std::cout << "|--";
        } else {
            if (hasNextSibling[i]) {
                std::cout << "|  ";
            } else {
                std::cout << "   ";
            }
        }
    }

    const auto* node = em.getComponent<SceneNode>(entity);

    if (detailLevel > 0)
        std::cout << em.getName(entity) << " [" << entity.id() << "]";

    if (detailLevel > 1 && node)
        std::cout << " => (pos: " << std::setprecision(2) << node->local.local.translation << "|scale: " << node->local.local.scale << "|rot: " << node->local.local.rotation << ")";

    if (node) {
        std::cout << " (" << node->children.size() << ")\n";
        for (size_t i = 0; i < node->children.size(); ++i) {
            Entity child = node->children[i];
            bool nextSibling = (i + 1) < node->children.size();
            std::vector<bool> nextHasNextSibling = hasNextSibling;
            nextHasNextSibling.push_back(nextSibling);

            printHierarchyRecursive(em, child, detailLevel, nextHasNextSibling);
        }
    }
}

ModelPoseStack * SceneGraph::getClosestPoseStack(EntityManager &em, const SceneNode &node) {
    if (node.parent != nullentity) {
        if (auto* poses = em.getComponent<ModelPoseStack>(node.parent)) {
            return poses;
        }
        return getClosestPoseStack(em, em.getComponent<SceneNode&>(node.parent));
    }
    return nullptr;
}

void SceneGraph::updateNodeRecursive(const Scene& scene, const Entity entity, const Transform *world,
    ModelPoseStack *poseStack) {
    auto& em = scene.getEntityManager();

    auto& node = em.getComponent<SceneNode&>(entity);
    em.removeState<SceneGraphNodeInvalidated>(entity);

    Transform animationTransform;
    if (auto* mesh = em.getComponent<Mesh>(entity)) {
        auto& model = em.getComponent<Model&>(mesh->getParent());
        auto [transform, tint] = poseStack->getFinalPose(model.getMeshID(mesh->getName()), ModelPart::MESH);
        animationTransform = transform;
        mesh->setTint(tint);
    } else if (auto* pose = em.getComponent<ModelPoseStack>(entity)) {
        poseStack = pose;
        auto [transform, tint] = poseStack->getFinalPose(0, ModelPart::MESH);
        animationTransform = transform;

        if (auto* skeleton = em.getComponent<Skeleton>(entity)) {
            for (int i = 0; i < skeleton->getBoneCount(); ++i) {
                if (poseStack->hasPose(i, ModelPart::BONE))
                skeleton->setBoneTransform(i, poseStack->getFinalPose(i, ModelPart::BONE).transform);
            }
        }
    } else if (em.hasComponent<Text3D>(entity)) {
        em.addState<Text3DTransformUpdate>(entity);
    }

    Transform parentWorld;
    if (world) {
        parentWorld = *world * node.local.local * animationTransform;
        glm::vec3 localForward = parentWorld.rotation * glm::vec3(0, -1, 0);
        glm::vec3 localRight   = parentWorld.rotation * glm::vec3(1,  0, 0);
        glm::vec3 localUp      = parentWorld.rotation * glm::vec3(0,  0, 1);

        parentWorld.translation += localForward * node.local.forward + localRight * node.local.side + localUp * node.local.up;
        *node.world = parentWorld;
    } else {
        parentWorld = *node.world * animationTransform;
    }
    node.finalTransform = parentWorld.createModel3D();
    ++node.version;
    for (Entity child : node.children) {
        updateNodeRecursive(scene, child, &parentWorld, poseStack);
    }
}

void SceneGraph::markNodeChildrenAsDeleted(Scene &scene, const SceneNode &node) {
    for (Entity child : node.children) {
        if (!scene.getEntityManager().hasComponent<PendingDestruction>(child))
            scene.getEntityManager().addComponent<PendingDestruction>(child, PendingDestruction());
        markNodeChildrenAsDeleted(scene, scene.get<SceneNode&>(child));
    }
}

SceneGraph::SceneGraph(Scene & scene) {
    scene.getEventSystem().subscribe<NewComponentSignal, PendingDestruction>([&scene](const Entity& entity, void* s) {
        if (!scene.getEntityManager().hasComponent<SceneNode>(entity)) return;

        const auto& node = scene.get<SceneNode&>(entity);
        markNodeChildrenAsDeleted(scene, node);

        if (node.parent != nullentity) {
            auto& parentChildren = scene.get<SceneNode&>(node.parent).children;

            for (auto it = parentChildren.begin(); it != parentChildren.end(); ++it) {
                if (*it == entity) {
                    parentChildren.erase(it);
                    break;
                }
            }
        }
    });
}

void SceneGraph::updateEntityHierarchy(const Scene &scene, const Entity entity) {
    EntityManager& em = scene.getEntityManager();
    const SceneNode& node = scene.getEntityManager().getComponent<SceneNode&>(entity);

    ModelPoseStack* parentStack = nullptr;

    if (parentStack = em.getComponent<ModelPoseStack>(entity); parentStack == nullptr) {
        parentStack = getClosestPoseStack(em, node);
    }

    const Transform* parentTransform = nullptr;
    if (node.parent != nullentity) {
        parentTransform = scene.getEntityManager().getComponent<SceneNode&>(node.parent).world.get();
    }
    updateNodeRecursive(scene, entity, parentTransform, parentStack);
}

void SceneGraph::onUpdate(const Scene &scene) {
    auto& em = scene.getEntityManager();

    auto* entities = em.getEntitiesWithState<SceneGraphNodeInvalidated>();
    if (!entities) return;

    for (const auto copy = *entities; const auto& entity : copy) {
        if (!em.hasState<SceneGraphNodeInvalidated>(entity)) continue;
        updateEntityHierarchy(scene, entity);
    }
    entities->clear();
}

void SceneGraph::addSceneNode(const Scene &scene, const Entity parent, const Entity entity,
    const NodeTransform &local) {
    if (parent != nullentity) {
        SceneNode* node;
        if (node = scene.getEntityManager().getComponent<SceneNode>(parent); node == nullptr) {
            node = &scene.getEntityManager().addComponent(parent, SceneNode(nullentity, scene.getHandle<Transform>(parent), NodeTransform()));
        }
        node->children.push_back(entity);
    }
    if (auto* node = scene.get<SceneNode>(entity)) {
        node->parent = parent;
        node->local = local;
    } else {
        scene.getEntityManager().addComponent(entity, SceneNode(parent, scene.getHandle<Transform>(entity), local));
    }
}

void SceneGraph::setSceneNodeTransform(const Scene &scene, const Entity entity, const NodeTransform &local) {
    auto& current = scene.getEntityManager().getComponent<SceneNode&>(entity);
    current.local = local;
}

void SceneGraph::printHierarchy(const Scene &scene, const Entity rootEntity, const int detailLevel) {
    std::cout << std::endl;
    printHierarchyRecursive(scene.getEntityManager(), rootEntity, detailLevel);
    std::cout << std::endl;
}

void SceneGraph::invalidateNode(const Scene &scene, const Entity entity) {
    auto& node = scene.get<SceneNode&>(entity);

    unsigned parentEntity = node.parent.id();
}

