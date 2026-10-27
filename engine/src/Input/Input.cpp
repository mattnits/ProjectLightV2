#include "ProjectLight/Input/Input.h"

namespace ProjectLight
{
    void Input::BeginFrame() {
        for (auto& state : m_KeyStates) {
            state.pressed = false;
            state.released = false;
        }

        for (auto& state : m_MouseButtonStates) {
            state.pressed = false;
            state.released = false;
        }

        m_MouseDelta = glm::dvec2{0.0};
        m_ScrollDelta = glm::dvec2{0.0};
    }

    void Input::UpdateKeyState(KeyCode key, bool isDown) {
        auto& state = m_KeyStates[static_cast<std::size_t>(key)];

        if (isDown && !state.down) {
            state.pressed = true;
        }
        if (!isDown && state.down) {
            state.released = true;
        }
        state.down = isDown;
    }

    void Input::UpdateMousePosition(double x, double y) {
        const glm::dvec2 newPosition{x, y};
        if (!m_HasMousePosition) {
            m_HasMousePosition = true;
            m_MousePosition = newPosition;
            return;
        }
        m_MouseDelta += newPosition - m_MousePosition;
        m_MousePosition = newPosition;
    }

    void Input::UpdateMouseButtonState(MouseButton button, bool isDown) {
        auto& state = m_MouseButtonStates[static_cast<std::size_t>(button)];

        if (isDown && !state.down) {
            state.pressed = true;
        }
        if (!isDown && state.down) {
            state.released = true;
        }
        state.down = isDown;
    };

    void Input::UpdateScrollDelta(double x, double y) {
        m_ScrollDelta += glm::dvec2{x, y};
    }

    bool Input::IsKeyDown(KeyCode key) const {
        return m_KeyStates[static_cast<std::size_t>(key)].down;
    };

    bool Input::WasKeyPressed(KeyCode key) const {
        return m_KeyStates[static_cast<std::size_t>(key)].pressed;
    };

    bool Input::WasKeyReleased(KeyCode key) const {
        return m_KeyStates[static_cast<std::size_t>(key)].released;
    };

    bool Input::IsMouseButtonDown(MouseButton button) const {
        return m_MouseButtonStates[static_cast<std::size_t>(button)].down;
    };

    bool Input::WasMouseButtonPressed(MouseButton button) const {
        return m_MouseButtonStates[static_cast<std::size_t>(button)].pressed;
    };

    bool Input::WasMouseButtonReleased(MouseButton button) const {
        return m_MouseButtonStates[static_cast<std::size_t>(button)].released;
    };

    glm::dvec2 Input::GetMousePosition() const {
        return m_MousePosition;
    };

    glm::dvec2 Input::GetMouseDelta() const {
        return m_MouseDelta;
    };

    glm::dvec2 Input::GetScrollDelta() const {
        return m_ScrollDelta;
    };
}
