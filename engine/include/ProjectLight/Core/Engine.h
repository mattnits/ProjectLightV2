#pragma once

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Window/Window.h"

namespace ProjectLight
{
    class Engine {
    public:
        Engine();

        void Run(Application& application);

        Window& GetWindow();

    private:
        Window m_Window;
    };
}