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

        void ExpectStateNear(const EditorCameraState& actual, const EditorCameraState& expected) {
            ExpectVectorNear(actual.Pivot, expected.Pivot);
            EXPECT_NEAR(actual.PivotDistance, expected.PivotDistance, VectorTolerance);
            EXPECT_NEAR(actual.YawDegrees, expected.YawDegrees, VectorTolerance);
            EXPECT_NEAR(actual.PitchDegrees, expected.PitchDegrees, VectorTolerance);
            EXPECT_NEAR(actual.MovementSpeed, expected.MovementSpeed, VectorTolerance);
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

    TEST(EditorCameraTests, MovementUsesUpdatedCameraOrientation) {
        EditorCamera editorCamera;

        editorCamera.Rotate(glm::dvec2{900.0, 0.0});

        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();
        const glm::vec3 expectedTranslation = GetForwardDirection(editorCamera) * 5.0F;

        editorCamera.Move(glm::vec3{0.0F, 0.0F, 1.0F}, MakeTimestep(1.0));

        ExpectVectorNear(editorCamera.GetCamera().GetPosition() - originalPosition,
                         expectedTranslation);
        ExpectPivotRelationship(editorCamera);
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

        EXPECT_LT(cameraTranslation.x, 0.0F);
        EXPECT_GT(cameraTranslation.y, 0.0F);
        EXPECT_NEAR(cameraTranslation.z, 0.0F, VectorTolerance);
        EXPECT_NEAR(cameraTranslation.x, -2.0F * cameraTranslation.y, VectorTolerance);
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

    TEST(EditorCameraTests, ViewportSizeUpdatesCameraAspectRatio) {
        EditorCamera editorCamera;

        ASSERT_TRUE(editorCamera.SetViewportSize(1600, 900));
        EXPECT_NEAR(editorCamera.GetCamera().GetAspectRatio(), 16.0F / 9.0F, VectorTolerance);

        const float validAspectRatio = editorCamera.GetCamera().GetAspectRatio();

        EXPECT_FALSE(editorCamera.SetViewportSize(0, 900));
        EXPECT_FLOAT_EQ(editorCamera.GetCamera().GetAspectRatio(), validAspectRatio);
    }

    TEST(EditorCameraTests, ValidStateRoundTripRestoresCamera) {
        EditorCamera originalCamera;

        originalCamera.Rotate(glm::dvec2{-350.0, 120.0});
        ASSERT_TRUE(originalCamera.Focus(glm::vec3{2.0F, 3.0F, 4.0F}, 7.0F));
        originalCamera.AdjustMovementSpeed(4.0);

        const EditorCameraState savedState = originalCamera.GetState();

        EditorCamera restoredCamera;
        ASSERT_TRUE(restoredCamera.SetState(savedState));

        ExpectStateNear(restoredCamera.GetState(), savedState);
        ExpectVectorNear(restoredCamera.GetCamera().GetPosition(),
                         originalCamera.GetCamera().GetPosition());
        ExpectVectorNear(GetForwardDirection(restoredCamera), GetForwardDirection(originalCamera));
        ExpectPivotRelationship(restoredCamera);
    }

    TEST(EditorCameraTests, InvalidStatePreservesCamera) {
        EditorCamera editorCamera;
        editorCamera.Rotate(glm::dvec2{200.0, -100.0});
        editorCamera.Move(glm::vec3{1.0F, 0.0F, 1.0F}, MakeTimestep(0.5));

        const EditorCameraState originalState = editorCamera.GetState();
        const glm::vec3 originalPosition = editorCamera.GetCamera().GetPosition();

        EditorCameraState invalidState = originalState;
        invalidState.Pivot.x = std::numeric_limits<float>::quiet_NaN();

        EXPECT_FALSE(editorCamera.SetState(invalidState));

        invalidState = originalState;
        invalidState.PitchDegrees = 90.0F;

        EXPECT_FALSE(editorCamera.SetState(invalidState));

        ExpectStateNear(editorCamera.GetState(), originalState);
        ExpectVectorNear(editorCamera.GetCamera().GetPosition(), originalPosition);
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
