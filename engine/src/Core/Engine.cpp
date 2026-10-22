#include "ProjectLight/Core/Engine.h"

#include <glad/glad.h>

#include "ProjectLight/Core/FrameTimer.h"

namespace ProjectLight
{
    Engine::Engine() : m_Window(1280, 720, "ProjectLightV2") {}

    void Engine::Run(Application& application) {
        application.OnStart();
        FrameTimer frameTimer;

        while (!m_Window.ShouldClose()) {
            m_Window.PollEvents();
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
}
