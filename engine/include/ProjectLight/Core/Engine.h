#pragma once

#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Input/Input.h"
#include "ProjectLight/Window/Window.h"

namespace ProjectLight
{
    class Engine
    {
    public:
        explicit Engine(const WindowSpecification& windowSpecification = {});

        void Run(Application& application);

        Window& GetWindow();
        const Input& GetInput() const;

    private:
        Log m_Log;
        Input m_Input;
        Window m_Window;

        void HandleEvent(const Event& event);
    };
}
