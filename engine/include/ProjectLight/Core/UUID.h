#pragma once

#include <cstdint>

namespace ProjectLight
{
    class UUID
    {
    public:
        UUID();

        explicit constexpr UUID(std::uint64_t value) noexcept : m_Value(value) {}

        [[nodiscard]] constexpr std::uint64_t GetValue() const noexcept {
            return m_Value;
        }

        [[nodiscard]] constexpr bool IsValid() const noexcept {
            return m_Value != 0;
        }

        friend bool operator==(const UUID&, const UUID&) = default;

    private:
        std::uint64_t m_Value{};
    };
}
