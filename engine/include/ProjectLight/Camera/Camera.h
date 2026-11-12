#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ProjectLight/Math/Ray.h"

namespace ProjectLight
{

    class Camera
    {
    public:
        Camera();
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        const glm::vec3& GetPosition() const;
        const glm::quat& GetOrientation() const;

        float GetAspectRatio() const;

        [[nodiscard]] bool SetAspectRatio(float aspectRatio);
        [[nodiscard]] bool SetViewportSize(int width, int height);
        void SetPosition(const glm::vec3& position);
        [[nodiscard]] bool SetOrientation(const glm::quat& orientation);

        float GetVerticalFieldOfViewDegrees() const;
        float GetNearClip() const;
        float GetFarClip() const;

        [[nodiscard]] bool SetPerspective(float verticalFieldOfViewDegrees, float nearClip,
                                          float farClip);
        [[nodiscard]] Ray CreateViewportRay(const glm::vec2& normalizedViewportPosition) const;

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

        float m_AspectRatio = 16.0F / 9.0F;
        glm::mat4 m_ViewMatrix{1.0F};
        glm::mat4 m_ProjectionMatrix{1.0F};
        glm::vec3 m_Position{0.0F};
        glm::quat m_Orientation{1.0F, 0.0F, 0.0F, 0.0F};
        float m_VerticalFieldOfViewDegrees = 45.0F;
        float m_NearClip = 0.1F;
        float m_FarClip = 1000.0F;
    };
}
