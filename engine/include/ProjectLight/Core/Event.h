#pragma once

#include <variant>

namespace ProjectLight
{
    struct WindowCloseEvent
    {
    };

    struct WindowResizeEvent
    {
        int Width;
        int Height;
    };

    struct FramebufferResizeEvent
    {
        int Width;
        int Height;
    };

    struct WindowFocusEvent
    {
        bool Focused;
    };

    struct WindowMinimizedEvent
    {
        bool Minimized;
    };

    using Event = std::variant<WindowCloseEvent, WindowResizeEvent, FramebufferResizeEvent,
                               WindowFocusEvent, WindowMinimizedEvent>;
}
