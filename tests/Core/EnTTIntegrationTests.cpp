#include <entt/entity/registry.hpp>
#include <gtest/gtest.h>

namespace ProjectLight
{
    namespace
    {
        struct TestComponent
        {
            int Value{};
        };
    }

    TEST(EnTTIntegrationTests, RegistryCreatesEntityAndStoresComponent) {
        entt::registry registry;
        const entt::entity entity = registry.create();

        registry.emplace<TestComponent>(entity, 42);

        ASSERT_TRUE(registry.valid(entity));
        ASSERT_TRUE(registry.all_of<TestComponent>(entity));
        EXPECT_EQ(registry.get<TestComponent>(entity).Value, 42);

        registry.destroy(entity);
        EXPECT_FALSE(registry.valid(entity));
    }
}
