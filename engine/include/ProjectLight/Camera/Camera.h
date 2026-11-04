#pragma once

#include <glm/mat4x4.hpp>

namespace ProjectLight {

    class Camera {
        public:
            
            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;

            void SetAspectRatio(float aspectRatio);
        private:
            float m_AspectRatio = 16/9;
            glm::mat4 m_ViewMatrix;
            glm::mat4 m_ProjectionMatrix;
    };
}