#pragma once

#include <chrono>
#include <optional>

#include "ProjectLight/Core/Timestep.h"

namespace ProjectLight
{
    class FrameTimer
    {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = Timestep::Duration;

        FrameTimer() = default;

        Timestep Tick();
        Timestep TickAt(TimePoint t);

    private:
        std::optional<TimePoint> m_PreviousTime;
        Duration m_MaxDelta{0.1}; // Clamped at 100ms so we don't jump when something stalls
    };
}
