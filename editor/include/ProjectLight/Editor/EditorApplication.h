#pragma once

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Editor/Panels/InspectorPanel.h"
#include "ProjectLight/Scene/Components/TransformComponent.h"

namespace ProjectLight
{
    class Engine;

    class EditorApplication : public Application
    {
    public:
        explicit EditorApplication(Engine& engine);

        void OnStart() override;
        void OnUpdate() override;
        void OnShutdown() override;

    private:
        Engine& m_Engine;
        TransformComponent m_SelectedTransform{};
        InspectorPanel m_Inspector;
    };
}
