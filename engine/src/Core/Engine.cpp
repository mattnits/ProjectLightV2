#include "ProjectLight/Core/Engine.h"

#include <glad/glad.h>

#include "ProjectLight/Core/FrameTimer.h"
#include "ProjectLight/Input/Input.h"

namespace ProjectLight
{
    Engine::Engine(const WindowSpecification& windowSpecification)
        : m_Window(m_Input, windowSpecification) {}

    void Engine::HandleEvent(const Event& event) {
        if (const auto* resizeEvent = std::get_if<FramebufferResizeEvent>(&event)) {
            if (resizeEvent->Width > 0 && resizeEvent->Height > 0) {
                glViewport(0, 0, resizeEvent->Width, resizeEvent->Height);
            }
        }
    }

    void Engine::Run(Application& application) {
        application.OnStart();
        FrameTimer frameTimer;

        while (!m_Window.ShouldClose()) {
            m_Input.BeginFrame();
            m_Window.PollEvents();

            for (const Event& event : m_Window.GetPendingEvents()) {
                HandleEvent(event);
                application.OnEvent(event);
            }

            m_Window.ClearPendingEvents();

            const Timestep timestep = frameTimer.Tick();
            application.OnUpdate(timestep);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            application.OnRender();

            m_Window.SwapBuffers();
        }

        application.OnShutdown();
    }

    Window& Engine::GetWindow() {
        return m_Window;
    }

    const Input& Engine::GetInput() const {
        return m_Input;
    }
}
