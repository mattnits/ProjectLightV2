#include "ProjectLight/Camera/Camera.h"

#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <limits>

namespace ProjectLight
{
    Camera::Camera() {
        RecalculateProjectionMatrix();
    }

    void Camera::RecalculateViewMatrix() {
        const glm::mat4 inverseRotation = glm::mat4_cast(glm::conjugate(m_Orientation));

        const glm::mat4 inverseTranslation = glm::translate(glm::mat4{1.0F}, -m_Position);

        m_ViewMatrix = inverseRotation * inverseTranslation;
    }

    void Camera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspectiveRH_NO(glm::radians(m_VerticalFieldOfViewDegrees),
                                                   m_AspectRatio, m_NearClip, m_FarClip);
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return m_ViewMatrix;
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        return m_ProjectionMatrix;
    }

    float Camera::GetAspectRatio() const {
        return m_AspectRatio;
    }

    bool Camera::SetAspectRatio(float aspectRatio) {
        if (!std::isfinite(aspectRatio) || aspectRatio <= 0.0F) {
            return false;
        }

        m_AspectRatio = aspectRatio;
        RecalculateProjectionMatrix();
        return true;
    }

    bool Camera::SetViewportSize(int width, int height) {
        if (width <= 0 || height <= 0) {
            return false;
        }

        const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        return SetAspectRatio(aspectRatio);
    }

    const glm::vec3& Camera::GetPosition() const {
        return m_Position;
    }

    const glm::quat& Camera::GetOrientation() const {
        return m_Orientation;
    }

    void Camera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        RecalculateViewMatrix();
    }

    bool Camera::SetOrientation(const glm::quat& orientation) {
        const float lengthSquared = glm::dot(orientation, orientation);

        const bool isFinite = std::isfinite(orientation.w) && std::isfinite(orientation.x) &&
                              std::isfinite(orientation.y) && std::isfinite(orientation.z) &&
                              std::isfinite(lengthSquared);

        if (!isFinite || lengthSquared <= std::numeric_limits<float>::epsilon()) {
            return false;
        }

        m_Orientation = glm::normalize(orientation);
        RecalculateViewMatrix();
        return true;
    }

    float Camera::GetVerticalFieldOfViewDegrees() const {
        return m_VerticalFieldOfViewDegrees;
    }

    float Camera::GetNearClip() const {
        return m_NearClip;
    }

    float Camera::GetFarClip() const {
        return m_FarClip;
    }

    bool Camera::SetPerspective(float verticalFieldOfViewDegrees, float nearClip, float farClip) {
        const bool isFinite = std::isfinite(verticalFieldOfViewDegrees) &&
                              std::isfinite(nearClip) && std::isfinite(farClip);

        const bool isCameraValid = verticalFieldOfViewDegrees > 0.0F &&
                                   verticalFieldOfViewDegrees < 180.0F && nearClip > 0.0F &&
                                   farClip > nearClip;

        if (!isFinite || !isCameraValid) {
            return false;
        }

        m_VerticalFieldOfViewDegrees = verticalFieldOfViewDegrees;
        m_NearClip = nearClip;
        m_FarClip = farClip;

        RecalculateProjectionMatrix();
        return true;
    }
}
