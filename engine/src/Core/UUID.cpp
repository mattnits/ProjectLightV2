#include "ProjectLight/Core/UUID.h"

#include <atomic>
#include <cstdint>
#include <limits>
#include <random>

namespace ProjectLight
{
    namespace
    {
        std::uint64_t CreateInitialUUIDValue() {
            std::mt19937_64 generator{std::random_device{}()};

            std::uniform_int_distribution<std::uint64_t> distribution{
                1,
                std::numeric_limits<std::uint64_t>::max() - 1,
            };

            return distribution(generator);
        }

        std::atomic<std::uint64_t> NextUUID{
            CreateInitialUUIDValue(),
        };

        std::uint64_t GenerateUUID() {
            std::uint64_t value = NextUUID.fetch_add(1, std::memory_order_relaxed);

            if (value == 0) {
                value = NextUUID.fetch_add(1, std::memory_order_relaxed);
            }

            return value;
        }
    }

    UUID::UUID() : m_Value(GenerateUUID()) {}
}
