#pragma once

#include "ProjectLight/Core/Event.h"
#include "ProjectLight/Core/Timestep.h"

namespace ProjectLight
{
    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void OnStart() {}
        virtual void OnUpdate(Timestep timestep) {}
        virtual void OnRender() {}
        virtual void OnShutdown() {}

        virtual void OnEvent(const Event& event) {}
    };
}
