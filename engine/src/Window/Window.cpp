#include "ProjectLight/Window/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <optional>
#include <stdexcept>

#include "ProjectLight/Input/Input.h"

namespace ProjectLight
{
    namespace
    {
        std::optional<KeyCode> TranslateKey(int glfwKey) {
            switch (glfwKey) {
            case GLFW_KEY_W:
                return KeyCode::W;
            case GLFW_KEY_A:
                return KeyCode::A;
            case GLFW_KEY_S:
                return KeyCode::S;
            case GLFW_KEY_D:
                return KeyCode::D;
            case GLFW_KEY_Q:
                return KeyCode::Q;
            case GLFW_KEY_E:
                return KeyCode::E;
            case GLFW_KEY_F:
                return KeyCode::F;
            case GLFW_KEY_LEFT_SHIFT:
                return KeyCode::LeftShift;
            case GLFW_KEY_RIGHT_SHIFT:
                return KeyCode::RightShift;
            case GLFW_KEY_LEFT_ALT:
                return KeyCode::LeftAlt;
            case GLFW_KEY_RIGHT_ALT:
                return KeyCode::RightAlt;
            default:
                return std::nullopt;
            }
        }

        std::optional<MouseButton> TranslateMouseButton(int glfwButton) {
            switch (glfwButton) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return MouseButton::Right;
            default:
                return std::nullopt;
            }
        }

        Window* GetWindowFromHandle(GLFWwindow* handle) {
            return static_cast<Window*>(glfwGetWindowUserPointer(handle));
        }
    }

    Window::Window(Input& input, const WindowSpecification& specification) : m_Input(input) {
        if (specification.Width <= 0 || specification.Height <= 0) {
            throw std::invalid_argument("Window dimensions must be positive");
        }

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(specification.Width, specification.Height,
                                    specification.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        // GLFW callbacks
        glfwSetWindowUserPointer(m_Window, this);

        glfwSetKeyCallback(m_Window, OnKeyEvent);
        glfwSetMouseButtonCallback(m_Window, OnMouseButtonEvent);
        glfwSetCursorPosCallback(m_Window, OnCursorPosition);
        glfwSetScrollCallback(m_Window, OnScroll);

        glfwSetWindowFocusCallback(m_Window, OnWindowFocus);
        glfwSetWindowCloseCallback(m_Window, OnWindowClose);
        glfwSetWindowSizeCallback(m_Window, OnWindowResize);
        glfwSetWindowIconifyCallback(m_Window, OnWindowMinimized);

        glfwMakeContextCurrent(m_Window);
        SetVSync(specification.VSync);

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

    void Window::SetVSync(bool enabled) {
        glfwSwapInterval(enabled ? 1 : 0);
    }

    const std::vector<Event>& Window::GetPendingEvents() const {
        return m_PendingEvents;
    }

    void Window::ClearPendingEvents() {
        m_PendingEvents.clear();
    }

    void Window::OnKeyEvent(GLFWwindow* glfwWindow, int key, int, int action, int) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        const std::optional<KeyCode> keyCode = TranslateKey(key);
        // Ignore keys that aren't supported
        if (!keyCode) {
            return;
        }

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            window->m_Input.UpdateKeyState(*keyCode, true);
        } else if (action == GLFW_RELEASE) {
            window->m_Input.UpdateKeyState(*keyCode, false);
        }
    }

    void Window::OnMouseButtonEvent(GLFWwindow* glfwWindow, int button, int action, int) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        const std::optional<MouseButton> mouseButton = TranslateMouseButton(button);
        // Ignore mouse buttons that aren't supported
        if (!mouseButton) {
            return;
        }

        if (action == GLFW_PRESS) {
            window->m_Input.UpdateMouseButtonState(*mouseButton, true);
        } else if (action == GLFW_RELEASE) {
            window->m_Input.UpdateMouseButtonState(*mouseButton, false);
        }
    }

    void Window::OnCursorPosition(GLFWwindow* glfwWindow, double x, double y) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }
        window->m_Input.UpdateMousePosition(x, y);
    }

    void Window::OnScroll(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }
        window->m_Input.UpdateScrollDelta(xOffset, yOffset);
    }

    void Window::OnWindowFocus(GLFWwindow* glfwWindow, int focused) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        const bool isFocused = focused == GLFW_TRUE;

        if (!isFocused) {
            window->m_Input.Reset();
        }

        window->m_PendingEvents.emplace_back(WindowFocusEvent{isFocused});
    }

    void Window::OnWindowClose(GLFWwindow* glfwWindow) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        window->m_PendingEvents.emplace_back(WindowCloseEvent{});
    }

    void Window::OnFramebufferResize(GLFWwindow* glfwWindow, int width, int height) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        window->m_PendingEvents.emplace_back(FramebufferResizeEvent{width, height});
    }

    void Window::OnWindowMinimized(GLFWwindow* glfwWindow, int minimized) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        const bool isMinimized = minimized == GLFW_TRUE;
        window->m_PendingEvents.emplace_back(WindowMinimizedEvent{isMinimized});
    }

    void Window::OnWindowResize(GLFWwindow* glfwWindow, int width, int height) {
        auto* window = GetWindowFromHandle(glfwWindow);
        if (window == nullptr) {
            return;
        }

        window->m_PendingEvents.emplace_back(WindowResizeEvent{width, height});
    }

}
