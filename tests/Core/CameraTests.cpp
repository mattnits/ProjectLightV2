#include <ProjectLight/Camera/Camera.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <gtest/gtest.h>
#include <limits>

namespace ProjectLight
{
    namespace
    {
        constexpr float MatrixTolerance = 0.00001F;
        constexpr float ProjectionTolerance = 0.0001F;

        void ExpectMatrixNear(const glm::mat4& actual, const glm::mat4& expected,
                              float tolerance = MatrixTolerance) {
            for (glm::length_t column = 0; column < 4; ++column) {
                for (glm::length_t row = 0; row < 4; ++row) {
                    EXPECT_NEAR(actual[column][row], expected[column][row], tolerance);
                }
            }
        }

        float ProjectDepth(const glm::mat4& projection, float viewSpaceDepth) {
            const glm::vec4 clipPosition = projection * glm::vec4{0.0F, 0.0F, viewSpaceDepth, 1.0F};
            return clipPosition.z / clipPosition.w;
        }
    }

    TEST(CameraTests, DefaultViewMatrixIsIdentity) {
        const Camera camera;

        ExpectMatrixNear(camera.GetViewMatrix(), glm::mat4{1.0F});
    }

    TEST(CameraTests, PositionProducesInverseTranslation) {
        Camera camera;
        camera.SetPosition(glm::vec3{2.0F, -3.0F, 4.0F});

        const glm::mat4 expected = glm::translate(glm::mat4{1.0F}, glm::vec3{-2.0F, 3.0F, -4.0F});

        ExpectMatrixNear(camera.GetViewMatrix(), expected);
    }

    TEST(CameraTests, OrientationProducesInverseRotation) {
        Camera camera;
        const glm::quat orientation =
            glm::angleAxis(glm::radians(90.0F), glm::vec3{0.0F, 1.0F, 0.0F});

        camera.SetOrientation(orientation);

        const glm::mat4 expected = glm::mat4_cast(glm::conjugate(orientation));
        ExpectMatrixNear(camera.GetViewMatrix(), expected);
    }

    TEST(CameraTests, OrientationIsNormalized) {
        Camera camera;

        camera.SetOrientation(glm::quat{2.0F, 0.0F, 0.0F, 0.0F});

        EXPECT_FLOAT_EQ(camera.GetOrientation().w, 1.0F);
        EXPECT_FLOAT_EQ(camera.GetOrientation().x, 0.0F);
        EXPECT_FLOAT_EQ(camera.GetOrientation().y, 0.0F);
        EXPECT_FLOAT_EQ(camera.GetOrientation().z, 0.0F);
    }

    TEST(CameraTests, DefaultProjectionUsesRightHandedOpenGLDepthRange) {
        const Camera camera;
        const glm::mat4 projection = camera.GetProjectionMatrix();

        EXPECT_NEAR(ProjectDepth(projection, -camera.GetNearClip()), -1.0F, ProjectionTolerance);
        EXPECT_NEAR(ProjectDepth(projection, -camera.GetFarClip()), 1.0F, ProjectionTolerance);
    }

    TEST(CameraTests, ValidPerspectiveSettingsUpdateProjection) {
        Camera camera;

        ASSERT_TRUE(camera.SetPerspective(60.0F, 0.5F, 250.0F));

        EXPECT_FLOAT_EQ(camera.GetVerticalFieldOfViewDegrees(), 60.0F);
        EXPECT_FLOAT_EQ(camera.GetNearClip(), 0.5F);
        EXPECT_FLOAT_EQ(camera.GetFarClip(), 250.0F);

        const glm::mat4 expected =
            glm::perspectiveRH_NO(glm::radians(60.0F), camera.GetAspectRatio(), 0.5F, 250.0F);
        ExpectMatrixNear(camera.GetProjectionMatrix(), expected);
    }

    TEST(CameraTests, InvalidPerspectiveSettingsPreservePreviousProjection) {
        Camera camera;
        const glm::mat4 originalProjection = camera.GetProjectionMatrix();

        EXPECT_FALSE(camera.SetPerspective(0.0F, 0.1F, 100.0F));
        EXPECT_FALSE(camera.SetPerspective(180.0F, 0.1F, 100.0F));
        EXPECT_FALSE(camera.SetPerspective(45.0F, 0.0F, 100.0F));
        EXPECT_FALSE(camera.SetPerspective(45.0F, 10.0F, 10.0F));
        EXPECT_FALSE(camera.SetPerspective(std::numeric_limits<float>::quiet_NaN(), 0.1F, 100.0F));

        EXPECT_FLOAT_EQ(camera.GetVerticalFieldOfViewDegrees(), 45.0F);
        EXPECT_FLOAT_EQ(camera.GetNearClip(), 0.1F);
        EXPECT_FLOAT_EQ(camera.GetFarClip(), 1000.0F);
        ExpectMatrixNear(camera.GetProjectionMatrix(), originalProjection);
    }

    TEST(CameraTests, ValidViewportSizeUpdatesAspectRatioAndProjection) {
        Camera camera;

        ASSERT_TRUE(camera.SetViewportSize(1920, 1080));

        const float expectedAspectRatio = 1920.0F / 1080.0F;
        EXPECT_FLOAT_EQ(camera.GetAspectRatio(), expectedAspectRatio);

        const glm::mat4 expected =
            glm::perspectiveRH_NO(glm::radians(camera.GetVerticalFieldOfViewDegrees()),
                                  expectedAspectRatio, camera.GetNearClip(), camera.GetFarClip());
        ExpectMatrixNear(camera.GetProjectionMatrix(), expected);
    }

    TEST(CameraTests, InvalidViewportAndAspectRatioPreservePreviousProjection) {
        Camera camera;
        const float originalAspectRatio = camera.GetAspectRatio();
        const glm::mat4 originalProjection = camera.GetProjectionMatrix();

        EXPECT_FALSE(camera.SetViewportSize(0, 1080));
        EXPECT_FALSE(camera.SetViewportSize(1920, 0));
        EXPECT_FALSE(camera.SetViewportSize(-1, 1080));
        EXPECT_FALSE(camera.SetAspectRatio(0.0F));
        EXPECT_FALSE(camera.SetAspectRatio(-1.0F));
        EXPECT_FALSE(camera.SetAspectRatio(std::numeric_limits<float>::infinity()));

        EXPECT_FLOAT_EQ(camera.GetAspectRatio(), originalAspectRatio);
        ExpectMatrixNear(camera.GetProjectionMatrix(), originalProjection);
    }
}
