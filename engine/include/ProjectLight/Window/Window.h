#pragma once

#include <string>

struct GLFWwindow;

namespace ProjectLight
{
    class Window
    {
    public:
        Window(
            int width,
            int height,
            const std::string& title
        );

        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() const;

        void PollEvents();
        void SwapBuffers();

    private:
        GLFWwindow* m_Window = nullptr;
    };
}