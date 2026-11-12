#include "ProjectLight/Editor/EditorApplication.h"

#include <GLFW/glfw3.h>
#include <array>
#include <cstddef>
#include <filesystem>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <span>
#include <system_error>

#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Editor/Camera/EditorCameraPersistence.h"
#include "ProjectLight/Input/Input.h"

namespace ProjectLight
{
    namespace
    {
        const std::filesystem::path EditorCameraStatePath{"editor_camera.ini"};

        struct TriangleVertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        const std::array<TriangleVertex, 3> TriangleVertices{
            TriangleVertex{
                .Position = {-1.0F, -1.0F, -5.0F},
                .Color = {1.0F, 0.0F, 0.0F},
            },
            TriangleVertex{
                .Position = {1.0F, -1.0F, -5.0F},
                .Color = {0.0F, 1.0F, 0.0F},
            },
            TriangleVertex{
                .Position = {0.0F, 1.0F, -5.0F},
                .Color = {0.0F, 0.0F, 1.0F},
            },
        };

        constexpr char TriangleVertexShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;

            layout(location = 0) out vec3 v_Color;

            uniform mat4 u_ViewProjection;

            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
            )";

        constexpr char TriangleFragmentShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 v_Color;
            layout(location = 0) out vec4 o_Color;

            void main() {
                o_Color = vec4(v_Color, 1.0);
            }
            )";
    }

    EditorApplication::EditorApplication(Engine& engine)
        : m_Engine(engine), m_TestShader(TriangleVertexShaderSource, TriangleFragmentShaderSource),
          m_TestVertexBuffer(std::as_bytes(std::span{TriangleVertices})) {
        const std::array<VertexAttribute, 2> attributes{
            VertexAttribute{
                .Location = 0,
                .ComponentCount = 3,
                .Offset = offsetof(TriangleVertex, Position),
            },
            VertexAttribute{
                .Location = 1,
                .ComponentCount = 3,
                .Offset = offsetof(TriangleVertex, Color),
            },
        };

        m_TestVertexArray.SetVertexBuffer(m_TestVertexBuffer, sizeof(TriangleVertex), attributes);
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

        m_Inspector.SetTransform(&m_SelectedTransform);
        LoadEditorCameraState();
    }

    void EditorApplication::OnUpdate(Timestep timestep) {
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

        if (window.GetCursorMode() != CursorMode::Captured && isViewportHovered) {
            m_EditorCamera.Zoom(input.GetScrollDelta().y);
        }

        if (window.GetCursorMode() != CursorMode::Captured && isViewportFocused &&
            input.WasKeyPressed(KeyCode::F)) {
            if (!m_EditorCamera.Focus(m_SelectedTransform.Translation)) {
                const glm::vec3& target = m_SelectedTransform.Translation;
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

        m_Inspector.OnImGuiRender();
        m_Viewport.OnImGuiRender();

        m_Viewport.BeginRender();

        const Camera& camera = m_EditorCamera.GetCamera();

        const glm::mat4 viewProjectionMatrix =
            camera.GetProjectionMatrix() * camera.GetViewMatrix();

        m_TestShader.SetMatrix4("u_ViewProjection", viewProjectionMatrix);

        m_Engine.GetRenderer().Draw(m_TestShader, m_TestVertexArray, PrimitiveTopology::Triangles,
                                    TriangleVertices.size());

        m_Viewport.EndRender();

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
