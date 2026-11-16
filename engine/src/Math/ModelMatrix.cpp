#include "ProjectLight/Math/ModelMatrix.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

namespace ProjectLight
{
    glm::mat4 CreateModelMatrix(const glm::vec3& translation, const glm::vec3& rotationDegrees,
                                const glm::vec3& scale) {
        const glm::mat4 identity{1.0F};

        const glm::mat4 translationMatrix = glm::translate(identity, translation);

        const glm::quat rotation = glm::quat{glm::radians(rotationDegrees)};
        const glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        const glm::mat4 scaleMatrix = glm::scale(identity, scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }
}
