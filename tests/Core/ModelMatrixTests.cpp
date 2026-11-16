#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "ProjectLight/Math/ModelMatrix.h"

namespace ProjectLight
{
    namespace
    {
        constexpr float VectorTolerance = 0.0001F;

        void ExpectPointNear(const glm::mat4& matrix, const glm::vec3& point,
                             const glm::vec3& expected) {
            const glm::vec4 transformed = matrix * glm::vec4{point, 1.0F};

            EXPECT_NEAR(transformed.x, expected.x, VectorTolerance);
            EXPECT_NEAR(transformed.y, expected.y, VectorTolerance);
            EXPECT_NEAR(transformed.z, expected.z, VectorTolerance);
            EXPECT_NEAR(transformed.w, 1.0F, VectorTolerance);
        }
    }

    TEST(ModelMatrixTests, DefaultTransformLeavesPointUnchanged) {
        const glm::mat4 model =
            CreateModelMatrix(glm::vec3{0.0F}, glm::vec3{0.0F}, glm::vec3{1.0F});

        ExpectPointNear(model, glm::vec3{1.0F, 2.0F, 3.0F}, glm::vec3{1.0F, 2.0F, 3.0F});
    }

    TEST(ModelMatrixTests, TranslationMovesPointIntoWorldSpace) {
        const glm::mat4 model =
            CreateModelMatrix(glm::vec3{4.0F, -2.0F, 7.0F}, glm::vec3{0.0F}, glm::vec3{1.0F});

        ExpectPointNear(model, glm::vec3{1.0F, 2.0F, 3.0F}, glm::vec3{5.0F, 0.0F, 10.0F});
    }

    TEST(ModelMatrixTests, ScaleChangesDistanceFromLocalOrigin) {
        const glm::mat4 model =
            CreateModelMatrix(glm::vec3{0.0F}, glm::vec3{0.0F}, glm::vec3{2.0F, 3.0F, 4.0F});

        ExpectPointNear(model, glm::vec3{1.0F, 2.0F, 3.0F}, glm::vec3{2.0F, 6.0F, 12.0F});
    }

    TEST(ModelMatrixTests, RotationUsesDegrees) {
        const glm::mat4 model =
            CreateModelMatrix(glm::vec3{0.0F}, glm::vec3{0.0F, 0.0F, 90.0F}, glm::vec3{1.0F});

        ExpectPointNear(model, glm::vec3{1.0F, 0.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F});
    }

    TEST(ModelMatrixTests, AppliesScaleThenRotationThenTranslation) {
        const glm::mat4 model =
            CreateModelMatrix(glm::vec3{10.0F, 0.0F, 0.0F}, glm::vec3{0.0F, 0.0F, 90.0F},
                              glm::vec3{2.0F, 1.0F, 1.0F});

        ExpectPointNear(model, glm::vec3{1.0F, 0.0F, 0.0F}, glm::vec3{10.0F, 2.0F, 0.0F});
    }
}
