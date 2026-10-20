#include "ProjectLight/Editor/Panels/InspectorPanel.h"

#include <ProjectLight/Scene/Components/TransformComponent.h>
#include <imgui.h>

namespace ProjectLight
{

    void InspectorPanel::SetTransform(TransformComponent* transform) {
        m_Transform = transform;
    }

    void InspectorPanel::OnImGuiRender() {
        ImGui::Begin("Inspector");

        ImGui::Text("ProjectLight Editor");

        ImGui::Separator();

        static float position[3] = {0.0f, 0.0f, 0.0f};

        ImGui::DragFloat3("Position", position, 0.1f);

        ImGui::End();
    }
}
