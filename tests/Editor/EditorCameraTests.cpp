#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gtest/gtest.h>
#include <limits>

#include "ProjectLight/Editor/Camera/EditorCamera.h"

namespace ProjectLight
{
    namespace
    {
        constexpr float VectorTolerance = 0.0001F;

        Timestep MakeTimestep(double seconds) {
            return Timestep{Timestep::Duration{seconds}};
        }

        void ExpectVectorNear(const glm::vec3& actual, const glm::vec3& expected) {
            EXPECT_NEAR(actual.x, expected.x, VectorTolerance);
            EXPECT_NEAR(actual.y, expected.y, VectorTolerance);
            EXPECT_NEAR(actual.z, expected.z, VectorTolerance);
        }

        glm::vec3 GetForwardDirection(const EditorCamera& editorCamera) {
            return editorCamera.GetCamera().GetOrientation() * glm::vec3{0.0F, 0.0F, -1.0F};
        }

        void ExpectPivotRelationship(const EditorCamera& editorCamera) {
            const glm::vec3 expectedPivot =
                editorCamera.GetCamera().GetPosition() +
                GetForwardDirection(editorCamera) * editorCamera.GetPivotDistance();

            ExpectVectorNear(editorCamera.GetPivot(), expectedPivot);
        }
    }

    TEST(EditorCameraTests, DefaultsEstablishValidPivotInFrontOfCamera) {
        const EditorCamera editorCamera;

        ExpectVectorNear(editorCamera.GetCamera().GetPosition(), glm::vec3{0.0F});
        ExpectVectorNear(editorCamera.GetPivot(), glm::vec3{0.0F, 0.0F, -10.0F});
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), 10.0F);
        EXPECT_FLOAT_EQ(editorCamera.GetMovementSpeed(), 5.0F);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, RotateKeepsCameraFixedAndMovesPivotInFront) {
        EditorCamera editorCamera;
        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();

        editorCamera.Rotate(glm::dvec2{100.0, -50.0});

        ExpectVectorNear(editorCamera.GetCamera().GetPosition(), originalPosition);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, MovementIsFrameRateIndependentAndPreservesPivot) {
        EditorCamera oneStepCamera;
        EditorCamera twoStepCamera;
        const glm::vec3 forwardInput{0.0F, 0.0F, 1.0F};

        oneStepCamera.Move(forwardInput, MakeTimestep(1.0));
        twoStepCamera.Move(forwardInput, MakeTimestep(0.5));
        twoStepCamera.Move(forwardInput, MakeTimestep(0.5));

        ExpectVectorNear(oneStepCamera.GetCamera().GetPosition(),
                         twoStepCamera.GetCamera().GetPosition());
        ExpectVectorNear(oneStepCamera.GetCamera().GetPosition(), glm::vec3{0.0F, 0.0F, -5.0F});
        ExpectPivotRelationship(oneStepCamera);
        ExpectPivotRelationship(twoStepCamera);
    }

    TEST(EditorCameraTests, DiagonalMovementDoesNotIncreaseSpeed) {
        EditorCamera editorCamera;

        editorCamera.Move(glm::vec3{1.0F, 0.0F, 1.0F}, MakeTimestep(1.0));

        EXPECT_NEAR(glm::length(editorCamera.GetCamera().GetPosition()), 5.0F, VectorTolerance);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, VerticalMovementUsesWorldUpAfterRotation) {
        EditorCamera editorCamera;
        editorCamera.Rotate(glm::dvec2{100.0, 100.0});
        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();
        const glm::vec3 originalPivot = editorCamera.GetPivot();

        editorCamera.Move(glm::vec3{0.0F, 1.0F, 0.0F}, MakeTimestep(1.0));

        ExpectVectorNear(editorCamera.GetCamera().GetPosition() - originalPosition,
                         glm::vec3{0.0F, 5.0F, 0.0F});
        ExpectVectorNear(editorCamera.GetPivot() - originalPivot, glm::vec3{0.0F, 5.0F, 0.0F});
    }

    TEST(EditorCameraTests, PanMovesCameraAndPivotBySameTranslation) {
        EditorCamera editorCamera;
        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();
        const glm::vec3 originalPivot = editorCamera.GetPivot();

        editorCamera.Pan(glm::dvec2{100.0, 50.0});

        const glm::vec3 cameraTranslation =
            editorCamera.GetCamera().GetPosition() - originalPosition;
        const glm::vec3 pivotTranslation = editorCamera.GetPivot() - originalPivot;

        ExpectVectorNear(cameraTranslation, glm::vec3{-1.0F, 0.5F, 0.0F});
        ExpectVectorNear(pivotTranslation, cameraTranslation);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, OrbitKeepsPivotAndDistanceFixed) {
        EditorCamera editorCamera;
        const glm::vec3 originalPivot = editorCamera.GetPivot();
        const float originalDistance = editorCamera.GetPivotDistance();

        editorCamera.Orbit(glm::dvec2{100.0, -50.0});

        ExpectVectorNear(editorCamera.GetPivot(), originalPivot);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), originalDistance);
        EXPECT_NEAR(glm::length(editorCamera.GetPivot() - editorCamera.GetCamera().GetPosition()),
                    originalDistance, VectorTolerance);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, ZoomChangesDistanceAndRespectsBounds) {
        EditorCamera editorCamera;

        editorCamera.Zoom(1.0);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), 9.0F);
        ExpectPivotRelationship(editorCamera);

        editorCamera.Zoom(1000000.0);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), 0.1F);
        ExpectPivotRelationship(editorCamera);

        editorCamera.Zoom(-1000000.0);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), 1000.0F);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, MovementSpeedAdjustmentRespectsBounds) {
        EditorCamera editorCamera;

        editorCamera.AdjustMovementSpeed(2.0);
        EXPECT_FLOAT_EQ(editorCamera.GetMovementSpeed(), 7.0F);

        editorCamera.AdjustMovementSpeed(-1000.0);
        EXPECT_FLOAT_EQ(editorCamera.GetMovementSpeed(), 0.5F);

        editorCamera.AdjustMovementSpeed(1000.0);
        EXPECT_FLOAT_EQ(editorCamera.GetMovementSpeed(), 50.0F);
    }

    TEST(EditorCameraTests, FocusCentersValidTargetAtRequestedDistance) {
        EditorCamera editorCamera;
        const glm::vec3 target{2.0F, 3.0F, 4.0F};

        ASSERT_TRUE(editorCamera.Focus(target, 5.0F));

        ExpectVectorNear(editorCamera.GetPivot(), target);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), 5.0F);
        EXPECT_NEAR(glm::length(target - editorCamera.GetCamera().GetPosition()), 5.0F,
                    VectorTolerance);
        ExpectPivotRelationship(editorCamera);
    }

    TEST(EditorCameraTests, InvalidFocusPreservesPreviousState) {
        EditorCamera editorCamera;
        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();
        const glm::vec3 originalPivot = editorCamera.GetPivot();
        const float originalDistance = editorCamera.GetPivotDistance();
        const float nan = std::numeric_limits<float>::quiet_NaN();

        EXPECT_FALSE(editorCamera.Focus(glm::vec3{nan, 0.0F, 0.0F}));
        EXPECT_FALSE(editorCamera.Focus(glm::vec3{1.0F}, 0.0F));

        ExpectVectorNear(editorCamera.GetCamera().GetPosition(), originalPosition);
        ExpectVectorNear(editorCamera.GetPivot(), originalPivot);
        EXPECT_FLOAT_EQ(editorCamera.GetPivotDistance(), originalDistance);
    }
}
