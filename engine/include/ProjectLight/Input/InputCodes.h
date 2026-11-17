#pragma once

#include <cstdint>

namespace ProjectLight
{
    enum class KeyCode : std::uint8_t
    {
        W,
        A,
        S,
        D,
        Q,
        E,
        F,
        LeftShift,
        RightShift,
        LeftAlt,
        RightAlt,
        Count
    };

    enum class MouseButton : std::uint8_t
    {
        Left,
        Middle,
        Right,
        Count
    };
}
