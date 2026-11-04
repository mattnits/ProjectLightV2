#include "ProjectLight/Camera/Camera.h"

namespace ProjectLight {

    glm::mat4 Camera::GetViewMatrix() const {
        return m_ViewMatrix;
    }

    glm::mat4 Camera::GetProjectionMatrix() const  {
        return m_ProjectionMatrix;
    }

    void Camera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
    }
}