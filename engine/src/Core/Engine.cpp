#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Window/Window.h"

#include <glad/glad.h>

namespace ProjectLight
{
    void Engine::Run() {
        Window window(1280, 720, "ProjectLightV2");

        while (!window.ShouldClose()) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT);

            window.SwapBuffers();
            window.PollEvents();
        }
    }
}