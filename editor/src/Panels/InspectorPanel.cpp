#include "ProjectLight/Editor/Panels/InspectorPanel.h"

#include <ProjectLight/Scene/Components/TransformComponent.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace ProjectLight
{

    void InspectorPanel::SetTransform(TransformComponent* transform) {
        m_Transform = transform;
    }

    void InspectorPanel::OnImGuiRender() {
        ImGui::Begin("Inspector");

        ImGui::Text("ProjectLight Editor");

        ImGui::Separator();


        ImGui::DragFloat3("Translation", glm::value_ptr(m_Transform->Translation), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(m_Transform->Rotation), 0.1f);
        ImGui::DragFloat3("Scale", glm::value_ptr(m_Transform->Scale), 0.1f);

        ImGui::End();
    }
}
