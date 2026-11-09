#include "ProjectLight/Editor/Panels/ViewportPanel.h"

#include <imgui.h>

namespace ProjectLight
{
    void ViewportPanel::OnImGuiRender() {
        ImGui::Begin("Viewport");

        ImGui::TextDisabled("Scene output will appear here.");

        ImGui::End();
    }
}
