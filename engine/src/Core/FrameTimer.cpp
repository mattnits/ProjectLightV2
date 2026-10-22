#include "ProjectLight/Core/FrameTimer.h"

#include <algorithm>

namespace ProjectLight
{
    Timestep FrameTimer::Tick() {
        return TickAt(Clock::now());
    }

    Timestep FrameTimer::TickAt(TimePoint currentTime) {
        if (!m_PreviousTime) {
            m_PreviousTime = currentTime;
            return Timestep{};
        }

        const Duration elapsed = currentTime - *m_PreviousTime;
        const Duration clampedElapsed = std::min(elapsed, m_MaxDelta);

        m_PreviousTime = currentTime;

        return Timestep{clampedElapsed};
    }
}
