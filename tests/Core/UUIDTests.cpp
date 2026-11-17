#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <unordered_set>

#include "ProjectLight/Core/UUID.h"

namespace ProjectLight
{
    TEST(UUIDTests, GeneratedUUIDIsValid) {
        const UUID uuid;

        EXPECT_TRUE(uuid.IsValid());
        EXPECT_NE(uuid.GetValue(), 0);
    }

    TEST(UUIDTests, ExplicitValueIsPreserved) {
        constexpr std::uint64_t value = 42;
        constexpr UUID uuid{value};

        static_assert(uuid.IsValid());
        static_assert(uuid.GetValue() == value);
        EXPECT_EQ(uuid, UUID{value});
    }

    TEST(UUIDTests, ZeroIsReservedAsInvalid) {
        constexpr UUID uuid{0};

        static_assert(!uuid.IsValid());
        EXPECT_EQ(uuid.GetValue(), 0);
    }

    TEST(UUIDTests, GeneratedUUIDsAreUniqueWithinProcess) {
        constexpr std::size_t count = 10'000;
        std::unordered_set<std::uint64_t> values;
        values.reserve(count);

        for (std::size_t index = 0; index < count; ++index) {
            const UUID uuid;
            ASSERT_TRUE(values.insert(uuid.GetValue()).second);
        }

        EXPECT_EQ(values.size(), count);
    }
}
