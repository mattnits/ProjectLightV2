#include "ProjectLight/Editor/EditorApplication.h"

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Input/Input.h"

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

    void EditorApplication::OnUpdate(Timestep timestep) {
        const Input& input = m_Engine.GetInput();
        Window& window = m_Engine.GetWindow();

        const bool isAltDown =
            input.IsKeyDown(KeyCode::LeftAlt) || input.IsKeyDown(KeyCode::RightAlt);
        const bool isOrbiting = isAltDown && input.IsMouseButtonDown(MouseButton::Left);

        if (input.WasMouseButtonPressed(MouseButton::Right)) {
            window.SetCursorMode(CursorMode::Captured);
        }

        if (input.WasMouseButtonReleased(MouseButton::Right)) {
            window.SetCursorMode(CursorMode::Normal);
        }

        if (window.GetCursorMode() == CursorMode::Captured) {
            UpdateCapturedCameraControls(timestep, input);
        } else if (isOrbiting) {
            m_EditorCamera.Orbit(input.GetMouseDelta());
        } else if (input.IsMouseButtonDown(MouseButton::Middle)) {
            m_EditorCamera.Pan(input.GetMouseDelta());
        }

        if (window.GetCursorMode() != CursorMode::Captured) {
            m_EditorCamera.Zoom(input.GetScrollDelta().y);
        }

        if (window.GetCursorMode() != CursorMode::Captured && input.WasKeyPressed(KeyCode::F)) {
            static_cast<void>(m_EditorCamera.Focus(m_SelectedTransform.Translation));
        }
    }

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

    void EditorApplication::UpdateCapturedCameraControls(Timestep timestep, const Input& input) {
        m_EditorCamera.Rotate(input.GetMouseDelta());

        glm::vec3 movementInput{0.0F};

        if (input.IsKeyDown(KeyCode::W)) {
            movementInput.z += 1.0F;
        }
        if (input.IsKeyDown(KeyCode::S)) {
            movementInput.z -= 1.0F;
        }
        if (input.IsKeyDown(KeyCode::E)) {
            movementInput.y += 1.0F;
        }
        if (input.IsKeyDown(KeyCode::Q)) {
            movementInput.y -= 1.0F;
        }
        if (input.IsKeyDown(KeyCode::D)) {
            movementInput.x += 1.0F;
        }
        if (input.IsKeyDown(KeyCode::A)) {
            movementInput.x -= 1.0F;
        }

        m_EditorCamera.AdjustMovementSpeed(input.GetScrollDelta().y);
        m_EditorCamera.Move(movementInput, timestep);
    }

}
