#include "ProjectLight/Editor/EditorApplication.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <optional>
#include <system_error>

#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Editor/Camera/EditorCameraPersistence.h"
#include "ProjectLight/Input/Input.h"
#include "ProjectLight/Math/Ray.h"
#include "ProjectLight/Renderer/PrimitiveGeometry.h"
#include "ProjectLight/Renderer/Renderer.h"
#include "ProjectLight/Scene/Components/MeshRendererComponent.h"

namespace ProjectLight
{
    namespace
    {
        const std::filesystem::path EditorCameraStatePath{"editor_camera.ini"};

        const std::filesystem::path MeshVertexShaderPath =
            std::filesystem::path{PROJECTLIGHT_EDITOR_SHADER_DIR} / "Mesh.vert";

        const std::filesystem::path MeshFragmentShaderPath =
            std::filesystem::path{PROJECTLIGHT_EDITOR_SHADER_DIR} / "Mesh.frag";
    }

    EditorApplication::EditorApplication(Engine& engine)
        : m_Engine(engine),
          m_MeshShader(Shader::FromFiles(MeshVertexShaderPath, MeshFragmentShaderPath)),
          m_CubeMesh(PrimitiveGeometry::CreateCube(3)), m_ActiveScene(std::make_unique<Scene>()) {
        m_SelectedEntity = m_ActiveScene->CreateEntity("Cube");

        static_cast<void>(m_ActiveScene->AddComponent<MeshRendererComponent>(
            m_SelectedEntity, MeshRendererComponent{
                                  .MeshResource = &m_CubeMesh,
                              }));
    }

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

        LoadEditorCameraState();
    }

    void EditorApplication::OnUpdate(Timestep timestep) {
        m_LastTimestep = timestep;
        const glm::ivec2 viewportSize = m_Viewport.GetContentSize();

        if (!m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y)) {
            Log::Warn("Editor camera rejected viewport size {}x{}", viewportSize.x, viewportSize.y);
        }

        const Input& input = m_Engine.GetInput();
        Window& window = m_Engine.GetWindow();

        const bool isViewportHovered = m_Viewport.IsHovered();
        const bool isViewportFocused = m_Viewport.IsFocused();
        const bool isAltDown =
            input.IsKeyDown(KeyCode::LeftAlt) || input.IsKeyDown(KeyCode::RightAlt);

        const bool isOrbiting =
            isViewportHovered && isAltDown && input.IsMouseButtonDown(MouseButton::Left);

        if (isViewportHovered && input.WasMouseButtonPressed(MouseButton::Right)) {
            window.SetCursorMode(CursorMode::Captured);
        }

        if (window.GetCursorMode() == CursorMode::Captured &&
            input.WasMouseButtonReleased(MouseButton::Right)) {
            window.SetCursorMode(CursorMode::Normal);
        }

        // Navigation priority: captured fly mode, then orbit, then pan
        // Mouse wheel may combine with orbit or pan
        if (window.GetCursorMode() == CursorMode::Captured) {
            UpdateCapturedCameraControls(timestep, input);
        } else if (isOrbiting) {
            m_EditorCamera.Orbit(input.GetMouseDelta());
        } else if (isViewportHovered && input.IsMouseButtonDown(MouseButton::Middle)) {
            m_EditorCamera.Pan(input.GetMouseDelta());
        }

        const double scrollDelta = input.GetScrollDelta().y;
        if (window.GetCursorMode() != CursorMode::Captured && isViewportHovered &&
            scrollDelta != 0.0) {
            const glm::vec2 mousePosition =
                m_Viewport.GetNormalizedMousePosition().value_or(glm::vec2{0.5F, 0.5F});
            const Ray mouseRay = m_EditorCamera.GetCamera().CreateViewportRay(mousePosition);
            m_EditorCamera.Dolly(scrollDelta, mouseRay.Direction);
        }

        if (window.GetCursorMode() != CursorMode::Captured && isViewportFocused &&
            input.WasKeyPressed(KeyCode::F)) {
            const TransformComponent& selectedTransform =
                m_ActiveScene->GetEntityTransform(m_SelectedEntity);

            if (!m_EditorCamera.Focus(selectedTransform.Translation)) {
                const glm::vec3& target = selectedTransform.Translation;
                Log::Warn("Editor camera could not focus target ({}, {}, {})", target.x, target.y,
                          target.z);
            }
        }
    }

    void EditorApplication::OnRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#ifdef IMGUI_HAS_DOCK
        ImGui::DockSpaceOverViewport();
#endif
        m_Inspector.SetTransform(&m_ActiveScene->GetEntityTransform(m_SelectedEntity));
        m_Inspector.OnImGuiRender();
        m_Viewport.OnImGuiRender(m_EditorCamera.GetCamera().GetOrientation());

        m_Viewport.BeginRender();

        const Camera& camera = m_EditorCamera.GetCamera();

        const glm::mat4 viewProjectionMatrix =
            camera.GetProjectionMatrix() * camera.GetViewMatrix();
        const Renderer& renderer = m_Engine.GetRenderer();
        const float gridScaleDistance =
            std::max(std::abs(camera.GetPosition().y), m_EditorCamera.GetPivotDistance());

        m_GridRenderer.Render(m_Engine.GetRenderer(), viewProjectionMatrix,
                              m_EditorCamera.GetPivot(), gridScaleDistance);

        m_MeshShader.SetMatrix4("u_ViewProjection", viewProjectionMatrix);
        m_ActiveScene->ForEachRenderable([&](const Mesh& mesh, const glm::mat4& worldMatrix) {
            m_MeshShader.SetMatrix4("u_Model", worldMatrix);
            renderer.DrawIndexed(m_MeshShader, mesh.GetVertexArray(), PrimitiveTopology::Triangles);
        });

        m_DebugLineRenderer.Flush(m_Engine.GetRenderer(), viewProjectionMatrix);

        m_Viewport.EndRender();

        m_PerformancePanel.OnImGuiRender(renderer.GetStatistics(),
                                         renderer.GetPerformanceMeasurements(), m_LastTimestep);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorApplication::OnShutdown() {
        SaveEditorCameraState();
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

    void EditorApplication::LoadEditorCameraState() {
        std::error_code fileError;

        const bool stateFileExists = std::filesystem::exists(EditorCameraStatePath, fileError);

        if (fileError) {
            Log::Warn("Could not inspect editor camera state file '{}'",
                      EditorCameraStatePath.string());
            return;
        }

        if (!stateFileExists) {
            return;
        }

        const std::optional<EditorCameraState> state =
            EditorCameraPersistence::Load(EditorCameraStatePath);

        if (!state.has_value()) {
            Log::Warn("Could not parse editor camera state file '{}'",
                      EditorCameraStatePath.string());
            return;
        }

        if (!m_EditorCamera.SetState(*state)) {
            Log::Warn("Editor camera state file '{}' contains invalid values",
                      EditorCameraStatePath.string());
            return;
        }

        Log::Debug("Restored editor camera state");
    }

    void EditorApplication::SaveEditorCameraState() const {
        if (!EditorCameraPersistence::Save(m_EditorCamera.GetState(), EditorCameraStatePath)) {
            Log::Warn("Could not save editor camera state file '{}'",
                      EditorCameraStatePath.string());
            return;
        }

        Log::Debug("Saved editor camera state");
    }

}
