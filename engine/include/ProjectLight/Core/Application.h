#pragma once

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
    };
}
