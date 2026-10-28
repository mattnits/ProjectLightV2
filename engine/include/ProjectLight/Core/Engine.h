#pragma once

#include "ProjectLight/Core/Application.h"
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
        Input m_Input;
        Window m_Window;
    };
}
