#include <chrono>

#include <gtest/gtest.h>

#include <ProjectLight/Core/Timestep.h>

namespace ProjectLight
{
    TEST(TimestepTests, DefaultTimestepIsZero)
    {
        const Timestep timestep;

        EXPECT_DOUBLE_EQ(timestep.GetSeconds(), 0.0);
        EXPECT_DOUBLE_EQ(timestep.GetMilliseconds(), 0.0);
    }

    TEST(TimestepTests, ConvertsOneSecondToMilliseconds)
    {
        const Timestep timestep{Timestep::Duration{1.0}};

        EXPECT_DOUBLE_EQ(timestep.GetSeconds(), 1.0);
        EXPECT_DOUBLE_EQ(timestep.GetMilliseconds(), 1000.0);
    }

    TEST(TimestepTests, ConvertsFractionalDurationBetweenUnits)
    {
        const auto duration = std::chrono::duration<double, std::milli>{16.0};
        const Timestep timestep{Timestep::Duration{duration}};

        EXPECT_DOUBLE_EQ(timestep.GetSeconds(), 0.016);
        EXPECT_DOUBLE_EQ(timestep.GetMilliseconds(), 16.0);
    }
}
