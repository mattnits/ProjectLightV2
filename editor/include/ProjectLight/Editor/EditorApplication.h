#pragma once

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Timestep.h"
#include "ProjectLight/Editor/Camera/EditorCamera.h"
#include "ProjectLight/Editor/Panels/InspectorPanel.h"
#include "ProjectLight/Editor/Panels/ViewportPanel.h"
#include "ProjectLight/Scene/Components/TransformComponent.h"

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

        Engine& m_Engine;
        TransformComponent m_SelectedTransform{};
        InspectorPanel m_Inspector;
        ViewportPanel m_Viewport;
        EditorCamera m_EditorCamera;
    };
}
