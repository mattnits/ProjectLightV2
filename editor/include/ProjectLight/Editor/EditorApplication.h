#pragma once

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Timestep.h"
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
        void OnUpdate(Timestep timestep) override;
        void OnRender() override;
        void OnShutdown() override;

    private:
        Engine& m_Engine;
        TransformComponent m_SelectedTransform{};
        InspectorPanel m_Inspector;
    };
}
