#pragma once

#include <array>
#include <cstddef>
#include <glm/vec2.hpp>

#include "ProjectLight/Input/InputCodes.h"

namespace ProjectLight
{
    class Input
    {
    public:
        void BeginFrame();
        void UpdateKeyState(KeyCode key, bool isDown);
        void UpdateMouseButtonState(MouseButton button, bool isDown);
        void UpdateMousePosition(double x, double y);
        void UpdateScrollDelta(double x, double y);

        bool IsKeyDown(KeyCode key) const;
        bool WasKeyPressed(KeyCode key) const;
        bool WasKeyReleased(KeyCode key) const;
        bool IsMouseButtonDown(MouseButton button) const;
        bool WasMouseButtonPressed(MouseButton button) const;
        bool WasMouseButtonReleased(MouseButton button) const;
        glm::dvec2 GetMousePosition() const;
        glm::dvec2 GetMouseDelta() const;
        glm::dvec2 GetScrollDelta() const;

        void Reset();

    private:
        struct ButtonState
        {
            bool down = false;
            bool pressed = false;
            bool released = false;
        };
        std::array<ButtonState, static_cast<std::size_t>(KeyCode::Count)> m_KeyStates{};
        std::array<ButtonState, static_cast<std::size_t>(MouseButton::Count)> m_MouseButtonStates{};
        glm::dvec2 m_MousePosition{0.0};
        glm::dvec2 m_MouseDelta{0.0};
        glm::dvec2 m_ScrollDelta{0.0};
        bool m_HasMousePosition = false;
    };
}
