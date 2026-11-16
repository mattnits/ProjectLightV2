#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace ProjectLight
{
    [[nodiscard]] glm::mat4 CreateModelMatrix(const glm::vec3& translation,
                                              const glm::vec3& rotationDegrees,
                                              const glm::vec3& scale);
}
