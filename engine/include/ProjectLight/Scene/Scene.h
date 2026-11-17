#pragma once

#include <cstddef>
#include <entt/entity/registry.hpp>
#include <glm/mat4x4.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "ProjectLight/Core/UUID.h"
#include "ProjectLight/Scene/Components/MeshRendererComponent.h"
#include "ProjectLight/Scene/Components/TransformComponent.h"
#include "ProjectLight/Scene/Entity.h"

namespace ProjectLight
{
    struct RelationshipComponent;
    struct TagComponent;
    struct UUIDComponent;

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
        [[nodiscard]] Entity DuplicateEntity(const Entity& source);
        [[nodiscard]] bool IsValid(const Entity& entity) const;

        [[nodiscard]] const std::string& GetEntityName(const Entity& entity) const;
        [[nodiscard]] TransformComponent& GetEntityTransform(const Entity& entity);
        [[nodiscard]] const TransformComponent& GetEntityTransform(const Entity& entity) const;
        [[nodiscard]] UUID GetEntityUUID(const Entity& entity) const;
        [[nodiscard]] Entity GetParent(const Entity& entity);
        [[nodiscard]] std::size_t GetChildCount(const Entity& entity) const;
        [[nodiscard]] bool SetParent(const Entity& child, const Entity& parent);
        [[nodiscard]] bool ClearParent(const Entity& child);
        [[nodiscard]] Entity GetChild(const Entity& entity, std::size_t index);
        [[nodiscard]] glm::mat4 GetLocalTransformMatrix(const Entity& entity) const;
        [[nodiscard]] glm::mat4 GetWorldTransformMatrix(const Entity& entity) const;

        void SetEntityName(const Entity& entity, std::string name);

        template <typename Component> [[nodiscard]] bool HasComponent(const Entity& entity) const {
            if (!IsValid(entity)) {
                return false;
            }
            return m_Registry.all_of<Component>(entity.m_Handle);
        }

        template <typename Component, typename... Arguments>
        Component& AddComponent(const Entity& entity, Arguments&&... arguments) {
            if (!IsValid(entity)) {
                throw std::invalid_argument("Cannot add a component to an invalid entity");
            }
            if (HasComponent<Component>(entity)) {
                throw std::logic_error("Entity already contains the requested component");
            }
            return m_Registry.emplace<Component>(entity.m_Handle,
                                                 std::forward<Arguments>(arguments)...);
        }

        template <typename Component> [[nodiscard]] Component& GetComponent(const Entity& entity) {
            if (!IsValid(entity)) {
                throw std::invalid_argument("Cannot get a component from an invalid entity");
            }
            if (!HasComponent<Component>(entity)) {
                throw std::out_of_range("Entity does not contain the requested component");
            }

            return m_Registry.get<Component>(entity.m_Handle);
        }

        template <typename Component>
        [[nodiscard]] const Component& GetComponent(const Entity& entity) const {
            if (!IsValid(entity)) {
                throw std::invalid_argument("Cannot get a component from an invalid entity");
            }
            if (!HasComponent<Component>(entity)) {
                throw std::out_of_range("Entity does not contain the requested component");
            }

            return m_Registry.get<Component>(entity.m_Handle);
        }

        template <typename Component> [[nodiscard]] bool RemoveComponent(const Entity& entity) {
            static_assert(!IsRequiredComponent<Component>,
                          "Required scene components cannot be removed");
            if (!IsValid(entity)) {
                return false;
            }

            return m_Registry.remove<Component>(entity.m_Handle) != 0;
        }

        template <typename Function> void ForEachRenderable(Function&& function) {
            auto view = m_Registry.view<MeshRendererComponent>();
            for (const entt::entity handle : view) {
                const MeshRendererComponent& meshRenderer =
                    m_Registry.get<MeshRendererComponent>(handle);
                if (!meshRenderer.IsVisible || meshRenderer.MeshResource == nullptr) {
                    continue;
                }

                const Entity entity{handle, *this};
                const glm::mat4 worldMatrix = GetWorldTransformMatrix(entity);
                function(*meshRenderer.MeshResource, worldMatrix);
            }
        }

    private:
        entt::registry m_Registry;

        [[nodiscard]] bool WouldCreateCycle(entt::entity child, entt::entity proposedParent) const;
        void DestroyEntityRecursive(entt::entity entity);

        template <typename Component>
        static constexpr bool IsRequiredComponent =
            std::is_same_v<std::remove_cv_t<Component>, TagComponent> ||
            std::is_same_v<std::remove_cv_t<Component>, TransformComponent> ||
            std::is_same_v<std::remove_cv_t<Component>, UUIDComponent> ||
            std::is_same_v<std::remove_cv_t<Component>, RelationshipComponent>;
    };
}
