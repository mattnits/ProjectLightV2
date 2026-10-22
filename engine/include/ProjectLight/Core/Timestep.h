#pragma once

#include <chrono>

namespace ProjectLight
{
    class Timestep
    {
    public:
        using Duration = std::chrono::duration<double>;

        constexpr Timestep() noexcept = default;

        explicit constexpr Timestep(Duration duration) noexcept : m_Duration(duration) {}

        [[nodiscard]] constexpr double GetSeconds() const noexcept {
            return m_Duration.count();
        }

        [[nodiscard]] constexpr double GetMilliseconds() const noexcept {
            return std::chrono::duration<double, std::milli>{m_Duration}.count();
        }

    private:
        Duration m_Duration{Duration::zero()};
    };
}
