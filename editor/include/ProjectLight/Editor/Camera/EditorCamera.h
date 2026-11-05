#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ProjectLight/Camera/Camera.h"
#include "ProjectLight/Core/Timestep.h"

namespace ProjectLight
{
    class EditorCamera
    {
    public:
        const Camera& GetCamera() const;

        void Rotate(const glm::dvec2& mouseDelta);
        void Move(const glm::vec3& movementInput, Timestep timestep);
        void Pan(const glm::dvec2& mouseDelta);
        void Orbit(const glm::dvec2& mouseDelta);
        void Zoom(double scrollDelta);
        [[nodiscard]] bool Focus(const glm::vec3& target, float distance = 10.0F);

        const glm::vec3& GetPivot() const;
        float GetPivotDistance() const;
        float GetMovementSpeed() const;
        void AdjustMovementSpeed(double scrollDelta);

    private:
        Camera m_Camera;
        float m_YawDegrees = 0.0F;
        float m_PitchDegrees = 0.0F;
        float m_MouseSensitivity = 0.1F;
        float m_MovementSpeed = 5.0F;
        glm::vec3 m_Pivot{0.0F, 0.0F, -10.0F};
        float m_PivotDistance = 10.0F;
        float m_PanSensitivity = 0.01F;
        float m_ZoomSensitivity = 0.1F;

        [[nodiscard]] bool UpdateOrientation(const glm::dvec2& mouseDelta);

        glm::vec3 GetForwardDirection() const;
    };
}
