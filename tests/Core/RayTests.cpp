#include <gtest/gtest.h>

#include "ProjectLight/Math/Ray.h"

namespace ProjectLight
{
    namespace
    {
        constexpr float VectorTolerance = 0.0001F;

        void ExpectVectorNear(const glm::vec3& actual, const glm::vec3& expected) {
            EXPECT_NEAR(actual.x, expected.x, VectorTolerance);
            EXPECT_NEAR(actual.y, expected.y, VectorTolerance);
            EXPECT_NEAR(actual.z, expected.z, VectorTolerance);
        }
    }

    TEST(RayTests, IntersectsHorizontalPlaneInFrontOfRay) {
        const Ray ray{
            .Origin = {2.0F, 10.0F, -1.0F},
            .Direction = {1.0F, -2.0F, 3.0F},
        };

        const std::optional<glm::vec3> intersection = IntersectHorizontalPlane(ray, 0.0F);

        ASSERT_TRUE(intersection.has_value());
        ExpectVectorNear(*intersection, glm::vec3{7.0F, 0.0F, 14.0F});
    }

    TEST(RayTests, ParallelRayDoesNotIntersectHorizontalPlane) {
        const Ray ray{
            .Origin = {0.0F, 10.0F, 0.0F},
            .Direction = {1.0F, 0.0F, 0.0F},
        };

        EXPECT_FALSE(IntersectHorizontalPlane(ray, 0.0F).has_value());
    }

    TEST(RayTests, IntersectionBehindRayIsRejected) {
        const Ray ray{
            .Origin = {0.0F, 10.0F, 0.0F},
            .Direction = {0.0F, 1.0F, 0.0F},
        };

        EXPECT_FALSE(IntersectHorizontalPlane(ray, 0.0F).has_value());
    }
}
