#pragma once

#include <entt/entity/entity.hpp>

namespace ProjectLight
{
    class Scene;

    class Entity
    {
    public:
        Entity() = default;

        [[nodiscard]] entt::entity GetHandle() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] explicit operator bool() const;

        friend bool operator==(const Entity&, const Entity&) = default;

    private:
        friend class Scene;

        Entity(entt::entity handle, Scene& scene);

        entt::entity m_Handle{entt::null};
        Scene* m_Scene{};
    };
}
