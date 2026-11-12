#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <optional>

#include "ProjectLight/Renderer/Framebuffer.h"

namespace ProjectLight
{
    class ViewportPanel
    {
    public:
        ViewportPanel();

        void BeginRender();
        void EndRender();
        void OnImGuiRender(const glm::quat& cameraOrientation);

        [[nodiscard]] glm::ivec2 GetContentSize() const;
        [[nodiscard]] bool IsFocused() const;
        [[nodiscard]] bool IsHovered() const;
        [[nodiscard]] std::optional<glm::vec2> GetNormalizedMousePosition() const;

    private:
        Framebuffer m_Framebuffer;
        glm::ivec2 m_ContentSize;
        bool m_IsFocused = false;
        bool m_IsHovered = false;
        std::optional<glm::vec2> m_NormalizedMousePosition;
    };
}
