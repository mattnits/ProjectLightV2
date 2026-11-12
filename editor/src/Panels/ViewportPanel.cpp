#include "ProjectLight/Editor/Panels/ViewportPanel.h"

#include <glm/vec4.hpp>
#include <imgui.h>

namespace ProjectLight
{
    namespace
    {
        constexpr FramebufferSpecification InitialFramebufferSpecification{1, 1};
    }
    ViewportPanel::ViewportPanel()
        : m_Framebuffer(InitialFramebufferSpecification),
          m_ContentSize{InitialFramebufferSpecification.Width,
                        InitialFramebufferSpecification.Height} {}

    void ViewportPanel::OnImGuiRender() {
        const bool isVisible = ImGui::Begin("Viewport");
        m_IsFocused = isVisible && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        m_IsHovered = false;

        if (isVisible) {
            const ImVec2 availableSize = ImGui::GetContentRegionAvail();
            const ImVec2 framebufferScale = ImGui::GetWindowViewport()->FramebufferScale;

            // 0.5F makes sure the framebuffer is the correct size by having the case round up or
            // down
            const int width = static_cast<int>(availableSize.x * framebufferScale.x + 0.5F);
            const int height = static_cast<int>(availableSize.y * framebufferScale.y + 0.5F);

            if (width > 0 && height > 0) {
                if (width != m_ContentSize.x || height != m_ContentSize.y) {
                    m_ContentSize = {width, height};
                    m_Framebuffer.Resize(width, height);
                }

                const ImTextureID textureID =
                    static_cast<ImTextureID>(m_Framebuffer.GetColorAttachmentRendererID());

                ImGui::Image(ImTextureRef{textureID}, availableSize, ImVec2{0.0F, 1.0F},
                             ImVec2{1.0F, 0.0F});
                m_IsHovered = ImGui::IsItemHovered();
            }
        }

        ImGui::End();
    }

    void ViewportPanel::BeginRender() {
        m_Framebuffer.Bind();
        m_Framebuffer.Clear(glm::vec4{0.12F, 0.14F, 0.18F, 1.0F});
    }

    void ViewportPanel::EndRender() {
        Framebuffer::Unbind();
    }

    glm::ivec2 ViewportPanel::GetContentSize() const {
        return m_ContentSize;
    }

    bool ViewportPanel::IsFocused() const {
        return m_IsFocused;
    }

    bool ViewportPanel::IsHovered() const {
        return m_IsHovered;
    }
}
