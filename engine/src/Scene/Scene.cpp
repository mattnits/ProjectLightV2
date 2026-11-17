#include "ProjectLight/Scene/Scene.h"

#include <stdexcept>
#include <utility>

#include "ProjectLight/Scene/Components/RelationshipComponent.h"
#include "ProjectLight/Scene/Components/TagComponent.h"
#include "ProjectLight/Scene/Components/UUIDComponent.h"

namespace ProjectLight
{
    Entity Scene::CreateEntity(std::string name) {
        const entt::entity handle = m_Registry.create();
        try {
            m_Registry.emplace<UUIDComponent>(handle);
            m_Registry.emplace<TagComponent>(handle, TagComponent{
                                                         .Name = std::move(name),
                                                     });

            m_Registry.emplace<TransformComponent>(handle);
            m_Registry.emplace<RelationshipComponent>(handle);
        } catch (...) {
            m_Registry.destroy(handle);
            throw;
        }

        return Entity{handle, *this};
    }

    bool Scene::DestroyEntity(const Entity& entity) {
        if (!IsValid(entity)) {
            return false;
        }
        const entt::entity handle = entity.m_Handle;
        const entt::entity parent = m_Registry.get<RelationshipComponent>(handle).Parent;

        if (parent != entt::null) {
            RelationshipComponent& parentRelationship =
                m_Registry.get<RelationshipComponent>(parent);

            std::erase(parentRelationship.Children, handle);
        }

        DestroyEntityRecursive(handle);
        return true;
    }

    bool Scene::IsValid(const Entity& entity) const {
        return entity.m_Scene == this && entity.m_Handle != entt::null &&
               m_Registry.valid(entity.m_Handle);
    }

    const std::string& Scene::GetEntityName(const Entity& entity) const {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the name of an invalid entity");
        }

        return m_Registry.get<TagComponent>(entity.m_Handle).Name;
    }

    void Scene::SetEntityName(const Entity& entity, std::string name) {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot rename an invalid entity");
        }

        m_Registry.get<TagComponent>(entity.m_Handle).Name = std::move(name);
    }

    TransformComponent& Scene::GetEntityTransform(const Entity& entity) {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the transform of an invalid entity");
        }

        return m_Registry.get<TransformComponent>(entity.m_Handle);
    }

    const TransformComponent& Scene::GetEntityTransform(const Entity& entity) const {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the transform of an invalid entity");
        }

        return m_Registry.get<TransformComponent>(entity.m_Handle);
    }

    UUID Scene::GetEntityUUID(const Entity& entity) const {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the UUID of an invalid entity");
        }

        return m_Registry.get<UUIDComponent>(entity.m_Handle).ID;
    }

    Entity Scene::GetParent(const Entity& entity) {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the parent of an invalid entity");
        }
        const RelationshipComponent& relationship =
            m_Registry.get<RelationshipComponent>(entity.m_Handle);

        if (relationship.Parent == entt::null) {
            return {};
        }

        return Entity{relationship.Parent, *this};
    }

    std::size_t Scene::GetChildCount(const Entity& entity) const {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get the children of an invalid entity");
        }

        return m_Registry.get<RelationshipComponent>(entity.m_Handle).Children.size();
    }

    bool Scene::SetParent(const Entity& child, const Entity& parent) {
        if (!IsValid(child) || !IsValid(parent) || child == parent) {
            return false;
        }
        RelationshipComponent& childRelationship =
            m_Registry.get<RelationshipComponent>(child.m_Handle);

        if (childRelationship.Parent == parent.m_Handle) {
            return true;
        }
        if (WouldCreateCycle(child.m_Handle, parent.m_Handle)) {
            return false;
        }
        RelationshipComponent& newParentRelationship =
            m_Registry.get<RelationshipComponent>(parent.m_Handle);

        // Add first because push_back can throw. If it does, the old relationship remains unchanged
        newParentRelationship.Children.push_back(child.m_Handle);

        if (childRelationship.Parent != entt::null) {
            RelationshipComponent& previousParentRelationship =
                m_Registry.get<RelationshipComponent>(childRelationship.Parent);

            std::erase(previousParentRelationship.Children, child.m_Handle);
        }

        childRelationship.Parent = parent.m_Handle;
        return true;
    }

    bool Scene::ClearParent(const Entity& child) {
        if (!IsValid(child)) {
            return false;
        }
        RelationshipComponent& childRelationship =
            m_Registry.get<RelationshipComponent>(child.m_Handle);
        if (childRelationship.Parent == entt::null) {
            return true;
        }

        RelationshipComponent& parentRelationship =
            m_Registry.get<RelationshipComponent>(childRelationship.Parent);
        std::erase(parentRelationship.Children, child.m_Handle);

        childRelationship.Parent = entt::null;
        return true;
    }

    Entity Scene::GetChild(const Entity& entity, std::size_t index) {
        if (!IsValid(entity)) {
            throw std::invalid_argument("Cannot get a child of an invalid entity");
        }

        const std::vector<entt::entity>& children =
            m_Registry.get<RelationshipComponent>(entity.m_Handle).Children;
        if (index >= children.size()) {
            throw std::out_of_range("Child index is outside the entity hierarchy");
        }

        return Entity{children[index], *this};
    }

    bool Scene::WouldCreateCycle(entt::entity child, entt::entity proposedParent) const {
        entt::entity current = proposedParent;

        while (current != entt::null) {
            if (current == child) {
                return true;
            }

            current = m_Registry.get<RelationshipComponent>(current).Parent;
        }

        return false;
    }

    void Scene::DestroyEntityRecursive(entt::entity entity) {
        while (true) {
            const RelationshipComponent& relationship =
                m_Registry.get<RelationshipComponent>(entity);
            if (relationship.Children.empty()) {
                break;
            }

            const entt::entity child = relationship.Children.back();
            DestroyEntityRecursive(child);

            m_Registry.get<RelationshipComponent>(entity).Children.pop_back();
        }

        m_Registry.destroy(entity);
    }
}
