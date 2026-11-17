#pragma once

#include <cstddef>
#include <entt/entity/registry.hpp>
#include <string>

#include "ProjectLight/Core/UUID.h"
#include "ProjectLight/Scene/Components/TransformComponent.h"
#include "ProjectLight/Scene/Entity.h"

namespace ProjectLight
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        [[nodiscard]] Entity CreateEntity(std::string name = "Entity");
        [[nodiscard]] bool DestroyEntity(const Entity& entity);
        [[nodiscard]] bool IsValid(const Entity& entity) const;

        [[nodiscard]] const std::string& GetEntityName(const Entity& entity) const;
        void SetEntityName(const Entity& entity, std::string name);
        [[nodiscard]] TransformComponent& GetEntityTransform(const Entity& entity);
        [[nodiscard]] const TransformComponent& GetEntityTransform(const Entity& entity) const;
        [[nodiscard]] UUID GetEntityUUID(const Entity& entity) const;
        [[nodiscard]] Entity GetParent(const Entity& entity);
        [[nodiscard]] std::size_t GetChildCount(const Entity& entity) const;
        [[nodiscard]] bool SetParent(const Entity& child, const Entity& parent);
        [[nodiscard]] bool ClearParent(const Entity& child);
        [[nodiscard]] Entity GetChild(const Entity& entity, std::size_t index);

    private:
        entt::registry m_Registry;

        [[nodiscard]] bool WouldCreateCycle(entt::entity child, entt::entity proposedParent) const;
        void DestroyEntityRecursive(entt::entity entity);
    };
}
