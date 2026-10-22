#include "ProjectLight/Editor/EditorApplication.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ProjectLight/Core/Engine.h"

namespace ProjectLight
{
    EditorApplication::EditorApplication(Engine& engine) : m_Engine(engine) {}

    void EditorApplication::OnStart() {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

#ifdef IMGUI_HAS_DOCK
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

        ImGui::StyleColorsDark();

        auto* window = static_cast<GLFWwindow*>(m_Engine.GetWindow().GetNativeHandle());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        m_Inspector.SetTransform(&m_SelectedTransform);
    }

    void EditorApplication::OnUpdate(Timestep) {}

    void EditorApplication::OnRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#ifdef IMGUI_HAS_DOCK
        ImGui::DockSpaceOverViewport();
#endif

        m_Inspector.OnImGuiRender();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorApplication::OnShutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

}
