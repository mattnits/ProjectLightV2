#pragma once

#include <entt/entity/entity.hpp>
#include <vector>

namespace ProjectLight
{
    struct RelationshipComponent
    {
        entt::entity Parent{entt::null};
        std::vector<entt::entity> Children;
    };
}
