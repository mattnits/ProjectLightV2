#include <chrono>

#include <gtest/gtest.h>

#include <ProjectLight/Core/FrameTimer.h>

namespace ProjectLight
{
    TEST(FrameTimerTests, FirstTickReturnsZero)
    {
        FrameTimer timer;

        EXPECT_DOUBLE_EQ(timer.TickAt(FrameTimer::TimePoint{}).GetSeconds(), 0.0);
    }

    TEST(FrameTimerTests, ReturnsElapsedTimeBetweenTicks)
    {
        FrameTimer timer;
        const FrameTimer::TimePoint start{};

        timer.TickAt(start);
        const Timestep timestep = timer.TickAt(start + std::chrono::milliseconds{16});

        EXPECT_DOUBLE_EQ(timestep.GetSeconds(), 0.016);
        EXPECT_DOUBLE_EQ(timestep.GetMilliseconds(), 16.0);
    }

    TEST(FrameTimerTests, ClampsLargeDeltaAndUsesCurrentTimeAsNextBaseline)
    {
        FrameTimer timer;
        const FrameTimer::TimePoint start{};

        timer.TickAt(start);
        const Timestep clampedTimestep = timer.TickAt(start + std::chrono::seconds{2});
        const Timestep nextTimestep =
            timer.TickAt(start + std::chrono::seconds{2} + std::chrono::milliseconds{20});

        EXPECT_DOUBLE_EQ(clampedTimestep.GetSeconds(), 0.1);
        EXPECT_DOUBLE_EQ(nextTimestep.GetSeconds(), 0.02);
    }
}
