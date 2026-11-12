#pragma once

#include <glm/vec3.hpp>
#include <optional>

namespace ProjectLight
{
    struct Ray
    {
        glm::vec3 Origin;
        glm::vec3 Direction;
    };

    [[nodiscard]] std::optional<glm::vec3> IntersectHorizontalPlane(const Ray& ray,
                                                                    float planeHeight);
}
