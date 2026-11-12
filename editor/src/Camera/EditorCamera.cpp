#include "ProjectLight/Editor/Camera/EditorCamera.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

namespace ProjectLight
{
    namespace
    {
        constexpr float MinimumPivotDistance = 0.5F;
        constexpr float MaximumPivotDistance = 1000.0F;
        constexpr float MinimumMovementSpeed = 0.5F;
        constexpr float MaximumMovementSpeed = 50.0F;
        constexpr float MinimumPitchDegrees = -89.0F;
        constexpr float MaximumPitchDegrees = 89.0F;

        bool IsFinite(const glm::vec3& value) {
            return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
        }

        glm::quat MakeOrientation(float yawDegrees, float pitchDegrees) {
            const glm::quat yawRotation =
                glm::angleAxis(glm::radians(yawDegrees), glm::vec3{0.0F, 1.0F, 0.0F});

            const glm::quat pitchRotation =
                glm::angleAxis(glm::radians(pitchDegrees), glm::vec3{1.0F, 0.0F, 0.0F});

            return yawRotation * pitchRotation;
        }
    }

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
        Zoom(scrollDelta, m_Pivot);
    }

    void EditorCamera::Zoom(double scrollDelta, const glm::vec3& target) {
        if (scrollDelta == 0.0 || !std::isfinite(scrollDelta) || !IsFinite(target)) {
            return;
        }

        const float requestedScale =
            std::pow(1.0F - m_ZoomSensitivity, static_cast<float>(scrollDelta));
        const float newDistance = std::clamp(m_PivotDistance * requestedScale, MinimumPivotDistance,
                                             MaximumPivotDistance);
        const float appliedScale = newDistance / m_PivotDistance;

        m_Pivot = target + (m_Pivot - target) * appliedScale;
        m_PivotDistance = newDistance;

        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);
    }

    bool EditorCamera::Focus(const glm::vec3& target, float distance) {
        const bool targetIsFinite = IsFinite(target);

        if (!targetIsFinite || !std::isfinite(distance) || distance <= 0.0F) {
            return false;
        }

        m_Pivot = target;
        m_PivotDistance = std::clamp(distance, MinimumPivotDistance, MaximumPivotDistance);

        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);

        return true;
    }

    float EditorCamera::GetMovementSpeed() const {
        return m_MovementSpeed;
    }

    void EditorCamera::AdjustMovementSpeed(double scrollDelta) {
        m_MovementSpeed += static_cast<float>(scrollDelta);
        m_MovementSpeed = std::clamp(m_MovementSpeed, MinimumMovementSpeed, MaximumMovementSpeed);
    }

    const glm::vec3& EditorCamera::GetPivot() const {
        return m_Pivot;
    }

    float EditorCamera::GetPivotDistance() const {
        return m_PivotDistance;
    }

    bool EditorCamera::SetViewportSize(int width, int height) {
        return m_Camera.SetViewportSize(width, height);
    }

    EditorCameraState EditorCamera::GetState() const {
        EditorCameraState state;
        state.Pivot = m_Pivot;
        state.PivotDistance = m_PivotDistance;
        state.YawDegrees = m_YawDegrees;
        state.PitchDegrees = m_PitchDegrees;
        state.MovementSpeed = m_MovementSpeed;
        return state;
    }

    bool EditorCamera::SetState(const EditorCameraState& state) {
        const bool valuesAreFinite = IsFinite(state.Pivot) && std::isfinite(state.PivotDistance) &&
                                     std::isfinite(state.YawDegrees) &&
                                     std::isfinite(state.PitchDegrees) &&
                                     std::isfinite(state.MovementSpeed);
        const bool valuesAreInRange = state.PivotDistance >= MinimumPivotDistance &&
                                      state.PivotDistance <= MaximumPivotDistance &&
                                      state.PitchDegrees >= MinimumPitchDegrees &&
                                      state.PitchDegrees <= MaximumPitchDegrees &&
                                      state.MovementSpeed >= MinimumMovementSpeed &&
                                      state.MovementSpeed <= MaximumMovementSpeed;
        if (!valuesAreFinite || !valuesAreInRange) {
            return false;
        }

        const glm::quat orientation = MakeOrientation(state.YawDegrees, state.PitchDegrees);

        if (!m_Camera.SetOrientation(orientation)) {
            return false;
        }

        m_Pivot = state.Pivot;
        m_PivotDistance = state.PivotDistance;
        m_YawDegrees = state.YawDegrees;
        m_PitchDegrees = state.PitchDegrees;
        m_MovementSpeed = state.MovementSpeed;

        m_Camera.SetPosition(m_Pivot - GetForwardDirection() * m_PivotDistance);

        return true;
    }

    bool EditorCamera::UpdateOrientation(const glm::dvec2& mouseDelta) {
        m_YawDegrees -= static_cast<float>(mouseDelta.x) * m_MouseSensitivity;
        m_PitchDegrees -= static_cast<float>(mouseDelta.y) * m_MouseSensitivity;
        m_PitchDegrees = std::clamp(m_PitchDegrees, MinimumPitchDegrees, MaximumPitchDegrees);

        return m_Camera.SetOrientation(MakeOrientation(m_YawDegrees, m_PitchDegrees));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const {
        return m_Camera.GetOrientation() * glm::vec3{0.0F, 0.0F, -1.0F};
    }
}
