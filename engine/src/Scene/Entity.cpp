#include "ProjectLight/Scene/Entity.h"

#include "ProjectLight/Scene/Scene.h"

namespace ProjectLight
{
    Entity::Entity(entt::entity handle, Scene& scene) : m_Handle(handle), m_Scene(&scene) {}

    entt::entity Entity::GetHandle() const {
        return m_Handle;
    }

    bool Entity::IsValid() const {
        return m_Scene != nullptr && m_Scene->IsValid(*this);
    }

    Entity::operator bool() const {
        return IsValid();
    }
}
