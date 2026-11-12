#pragma once

#include <glm/vec3.hpp>

namespace ProjectLight
{
    struct DebugLine
    {
        glm::vec3 Start{0.0F};
        glm::vec3 End{0.0F};
        glm::vec3 Color{1.0F};
    };
}
