#include "ProjectLight/Window/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdexcept>

namespace ProjectLight
{
    Window::Window(int width, int height, const std::string& title) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_Window);
        SetVSync(true);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;

            glfwTerminate();

            throw std::runtime_error("Failed to initialize GLAD");
        }

        const auto [framebufferWidth, framebufferHeight] = GetFramebufferSize();
        OnFramebufferResize(m_Window, framebufferWidth, framebufferHeight);
        glfwSetFramebufferSizeCallback(m_Window, OnFramebufferResize);
    }

    Window::~Window() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(m_Window);
    }

    void* Window::GetNativeHandle() const {
        return m_Window;
    }

    std::pair<int, int> Window::GetLogicalSize() const {
        int width{};
        int height{};
        glfwGetWindowSize(m_Window, &width, &height);
        return {width, height};
    }

    std::pair<int, int> Window::GetFramebufferSize() const {
        int width{};
        int height{};
        glfwGetFramebufferSize(m_Window, &width, &height);
        return {width, height};
    }

    void Window::OnFramebufferResize(GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void Window::SetVSync(bool enabled) {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}
