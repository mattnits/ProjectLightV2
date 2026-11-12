#include "ProjectLight/Math/Ray.h"

#include <cmath>

namespace ProjectLight
{
    std::optional<glm::vec3> IntersectHorizontalPlane(const Ray& ray, float planeHeight) {
        constexpr float ParallelTolerance = 0.000001F;

        if (std::abs(ray.Direction.y) <= ParallelTolerance) {
            return std::nullopt;
        }
        const float distance = (planeHeight - ray.Origin.y) / ray.Direction.y;

        if (distance < 0.0F) {
            return std::nullopt;
        }

        return ray.Origin + ray.Direction * distance;
    }
}
