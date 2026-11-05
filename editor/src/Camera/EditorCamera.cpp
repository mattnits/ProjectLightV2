#include "ProjectLight/Editor/Camera/EditorCamera.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

namespace ProjectLight
{
    const Camera& EditorCamera::GetCamera() const {
        return m_Camera;
    }

    void EditorCamera::Rotate(const glm::dvec2& mouseDelta) {
        if (!UpdateOrientation(mouseDelta)) {
            return;
        }

        m_Pivot = m_Camera.GetPosition() + GetForwardDirection() * m_PivotDistance;
    }

    void EditorCamera::Move(const glm::vec3& movementInput, Timestep timestep) {
        const glm::quat& orientation = m_Camera.GetOrientation();

        const glm::vec3 right = orientation * glm::vec3{1.0F, 0.0F, 0.0F};
        const glm::vec3 forward = orientation * glm::vec3{0.0F, 0.0F, -1.0F};
        const glm::vec3 worldUp{0.0F, 1.0F, 0.0F};

        const glm::vec3 direction =
            right * movementInput.x + worldUp * movementInput.y + forward * movementInput.z;

        if (glm::dot(direction, direction) <= 0.0F) {
            return;
        }

        const glm::vec3 normalizedDirection = glm::normalize(direction);
        const float distance = m_MovementSpeed * static_cast<float>(timestep.GetSeconds());
        const glm::vec3 translation = normalizedDirection * distance;

        m_Camera.SetPosition(m_Camera.GetPosition() + translation);
        m_Pivot += translation;
    }

    // Moves both camera and pivot
    void EditorCamera::Pan(const glm::dvec2& mouseDelta) {
        const glm::quat& orientation = m_Camera.GetOrientation();

        const glm::vec3 right = orientation * glm::vec3{1.0F, 0.0F, 0.0F};

        const glm::vec3 up = orientation * glm::vec3{0.0F, 1.0F, 0.0F};

        const glm::vec3 translation =
            (-right * static_cast<float>(mouseDelta.x) + up * static_cast<float>(mouseDelta.y)) *
            m_PanSensitivity;

        m_Camera.SetPosition(m_Camera.GetPosition() + translation);

        m_Pivot += translation;
    }

    // Keeps pivot point fixed
    void EditorCamera::Orbit(const glm::dvec2& mouseDelta) {
        if (!UpdateOrientation(mouseDelta)) {
            return;
        }

        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);
    }

    void EditorCamera::Zoom(double scrollDelta) {
        if (scrollDelta == 0.0) {
            return;
        }

        constexpr float MinimumDistance = 0.1F;
        constexpr float MaximumDistance = 1000.0F;

        const float zoomAmount =
            static_cast<float>(scrollDelta) * m_ZoomSensitivity * m_PivotDistance;

        m_PivotDistance =
            std::clamp(m_PivotDistance - zoomAmount, MinimumDistance, MaximumDistance);
        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);
    }

    bool EditorCamera::Focus(const glm::vec3& target, float distance) {
        const bool targetIsFinite =
            std::isfinite(target.x) && std::isfinite(target.y) && std::isfinite(target.z);

        if (!targetIsFinite || !std::isfinite(distance) || distance <= 0.0F) {
            return false;
        }

        constexpr float MinimumDistance = 0.1F;
        constexpr float MaximumDistance = 1000.0F;
        m_Pivot = target;
        m_PivotDistance = std::clamp(distance, MinimumDistance, MaximumDistance);

        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);

        return true;
    }

    float EditorCamera::GetMovementSpeed() const {
        return m_MovementSpeed;
    }

    void EditorCamera::AdjustMovementSpeed(double scrollDelta) {
        constexpr float MinimumSpeed = 0.5F;
        constexpr float MaximumSpeed = 50.0F;

        m_MovementSpeed += static_cast<float>(scrollDelta);
        m_MovementSpeed = std::clamp(m_MovementSpeed, MinimumSpeed, MaximumSpeed);
    }

    bool EditorCamera::UpdateOrientation(const glm::dvec2& mouseDelta) {
        m_YawDegrees -= static_cast<float>(mouseDelta.x) * m_MouseSensitivity;
        m_PitchDegrees -= static_cast<float>(mouseDelta.y) * m_MouseSensitivity;
        m_PitchDegrees = std::clamp(m_PitchDegrees, -89.0F, 89.0F);

        const glm::quat yawRotation =
            glm::angleAxis(glm::radians(m_YawDegrees), glm::vec3{0.0F, 1.0F, 0.0F});

        const glm::quat pitchRotation =
            glm::angleAxis(glm::radians(m_PitchDegrees), glm::vec3{1.0F, 0.0F, 0.0F});

        return m_Camera.SetOrientation(yawRotation * pitchRotation);
    }

    glm::vec3 EditorCamera::GetForwardDirection() const {
        return m_Camera.GetOrientation() * glm::vec3{0.0F, 0.0F, -1.0F};
    }

    const glm::vec3& EditorCamera::GetPivot() const {
        return m_Pivot;
    }

    float EditorCamera::GetPivotDistance() const {
        return m_PivotDistance;
    }
}
