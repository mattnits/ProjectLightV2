#pragma once

#include <memory>

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Timestep.h"
#include "ProjectLight/Editor/Camera/EditorCamera.h"
#include "ProjectLight/Editor/Panels/InspectorPanel.h"
#include "ProjectLight/Editor/Panels/PerformancePanel.h"
#include "ProjectLight/Editor/Panels/ViewportPanel.h"
#include "ProjectLight/Editor/Renderer/GridRenderer.h"
#include "ProjectLight/Renderer/DebugLineRenderer.h"
#include "ProjectLight/Renderer/Mesh.h"
#include "ProjectLight/Renderer/Shader.h"
#include "ProjectLight/Scene/Scene.h"

namespace ProjectLight
{
    class Engine;
    class Input;

    class EditorApplication : public Application
    {
    public:
        explicit EditorApplication(Engine& engine);

        void OnStart() override;
        void OnUpdate(Timestep timestep) override;
        void OnRender() override;
        void OnShutdown() override;

    private:
        void UpdateCapturedCameraControls(Timestep timestep, const Input& input);
        void LoadEditorCameraState();
        void SaveEditorCameraState() const;

        Engine& m_Engine;
        Shader m_MeshShader;
        Mesh m_CubeMesh;
        std::unique_ptr<Scene> m_ActiveScene;
        Entity m_SelectedEntity;

        InspectorPanel m_Inspector;
        ViewportPanel m_Viewport;
        PerformancePanel m_PerformancePanel;
        Timestep m_LastTimestep;
        EditorCamera m_EditorCamera;
        GridRenderer m_GridRenderer;
        DebugLineRenderer m_DebugLineRenderer;
    };
}
