#pragma once

#include <string>
#include <utility>

struct GLFWwindow;

namespace ProjectLight
{

    struct WindowSpecification
    {
        std::string Title = "ProjectLightV2";
        int Width = 1280;
        int Height = 720;
        bool VSync = true;
    };

    class Input;
    class Window
    {
    public:
        Window(Input& input, const WindowSpecification& specification);
        ~Window();
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        bool ShouldClose() const;

        void PollEvents();
        void SwapBuffers();

        void* GetNativeHandle() const;

        std::pair<int, int> GetLogicalSize() const;
        std::pair<int, int> GetFramebufferSize() const;

        void SetVSync(bool enabled);

    private:
        static void OnFramebufferResize(GLFWwindow*, int width, int height);
        static void OnKeyEvent(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
        static void OnMouseButtonEvent(GLFWwindow* glfwWindow, int button, int action, int mods);
        static void OnCursorPosition(GLFWwindow* glfwWindow, double x, double y);
        static void OnScroll(GLFWwindow* glfwWindow, double xOffset, double yOffset);
        static void OnWindowFocus(GLFWwindow* glfwWindow, int focused);

        GLFWwindow* m_Window = nullptr;
        Input& m_Input;
    };
}
