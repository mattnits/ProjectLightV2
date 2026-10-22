#pragma once

#include <string>
#include <utility>

struct GLFWwindow;

namespace ProjectLight
{
    class Window
    {
    public:
        Window(int width, int height, const std::string& title);

        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() const;

        void PollEvents();
        void SwapBuffers();

        void* GetNativeHandle() const;

        std::pair<int, int> GetLogicalSize() const;
        std::pair<int, int> GetFramebufferSize() const;

        void SetVSync(bool enabled);

    private:
        static void OnFramebufferResize(GLFWwindow*, int width, int height);

        GLFWwindow* m_Window = nullptr;
    };
}
