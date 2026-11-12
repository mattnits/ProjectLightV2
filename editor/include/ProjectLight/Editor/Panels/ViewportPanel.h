#pragma once

#include <glm/vec2.hpp>

#include "ProjectLight/Renderer/Framebuffer.h"

namespace ProjectLight
{
    class ViewportPanel
    {
    public:
        ViewportPanel();

        void BeginRender();
        void EndRender();
        void OnImGuiRender();

        [[nodiscard]] glm::ivec2 GetContentSize() const;
        [[nodiscard]] bool IsFocused() const;
        [[nodiscard]] bool IsHovered() const;

    private:
        Framebuffer m_Framebuffer;
        glm::ivec2 m_ContentSize;
        bool m_IsFocused = false;
        bool m_IsHovered = false;
    };
}
