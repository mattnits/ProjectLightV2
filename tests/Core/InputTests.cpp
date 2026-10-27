#include <ProjectLight/Input/Input.h>
#include <gtest/gtest.h>

namespace ProjectLight
{
    TEST(InputTests, KeysStartReleased) {
        const Input input;

        EXPECT_FALSE(input.IsKeyDown(KeyCode::W));
        EXPECT_FALSE(input.WasKeyPressed(KeyCode::W));
        EXPECT_FALSE(input.WasKeyReleased(KeyCode::W));
    }

    TEST(InputTests, PressingKeySetsDownAndPressed) {
        Input input;

        input.UpdateKeyState(KeyCode::W, true);

        EXPECT_TRUE(input.IsKeyDown(KeyCode::W));
        EXPECT_TRUE(input.WasKeyPressed(KeyCode::W));
        EXPECT_FALSE(input.WasKeyReleased(KeyCode::W));
    }

    TEST(InputTests, BeginFrameClearsTransitionsButPreservesHeldState) {
        Input input;
        input.UpdateKeyState(KeyCode::W, true);

        input.BeginFrame();

        EXPECT_TRUE(input.IsKeyDown(KeyCode::W));
        EXPECT_FALSE(input.WasKeyPressed(KeyCode::W));
        EXPECT_FALSE(input.WasKeyReleased(KeyCode::W));
    }

    TEST(InputTests, ReleasingKeySetsReleasedTransition) {
        Input input;
        input.UpdateKeyState(KeyCode::W, true);
        input.BeginFrame();

        input.UpdateKeyState(KeyCode::W, false);

        EXPECT_FALSE(input.IsKeyDown(KeyCode::W));
        EXPECT_FALSE(input.WasKeyPressed(KeyCode::W));
        EXPECT_TRUE(input.WasKeyReleased(KeyCode::W));
    }

    TEST(InputTests, RepeatedPressDoesNotCreateAnotherPressedTransition) {
        Input input;
        input.UpdateKeyState(KeyCode::W, true);
        input.BeginFrame();

        input.UpdateKeyState(KeyCode::W, true);

        EXPECT_TRUE(input.IsKeyDown(KeyCode::W));
        EXPECT_FALSE(input.WasKeyPressed(KeyCode::W));
    }

    TEST(InputTests, TracksMultipleKeysIndependently) {
        Input input;

        input.UpdateKeyState(KeyCode::W, true);
        input.UpdateKeyState(KeyCode::LeftShift, true);

        EXPECT_TRUE(input.IsKeyDown(KeyCode::W));
        EXPECT_TRUE(input.IsKeyDown(KeyCode::LeftShift));
        EXPECT_TRUE(input.WasKeyPressed(KeyCode::W));
        EXPECT_TRUE(input.WasKeyPressed(KeyCode::LeftShift));
    }

    TEST(InputTests, MouseButtonsStartReleased) {
        const Input input;

        EXPECT_FALSE(input.IsMouseButtonDown(MouseButton::Left));
        EXPECT_FALSE(input.WasMouseButtonPressed(MouseButton::Left));
        EXPECT_FALSE(input.WasMouseButtonReleased(MouseButton::Left));
    }

    TEST(InputTests, PressingMouseButtonSetsDownAndPressed) {
        Input input;

        input.UpdateMouseButtonState(MouseButton::Right, true);

        EXPECT_TRUE(input.IsMouseButtonDown(MouseButton::Right));
        EXPECT_TRUE(input.WasMouseButtonPressed(MouseButton::Right));
        EXPECT_FALSE(input.WasMouseButtonReleased(MouseButton::Right));
    }

    TEST(InputTests, BeginFrameClearsMouseTransitionsButPreservesHeldState) {
        Input input;
        input.UpdateMouseButtonState(MouseButton::Middle, true);

        input.BeginFrame();

        EXPECT_TRUE(input.IsMouseButtonDown(MouseButton::Middle));
        EXPECT_FALSE(input.WasMouseButtonPressed(MouseButton::Middle));
        EXPECT_FALSE(input.WasMouseButtonReleased(MouseButton::Middle));
    }

    TEST(InputTests, ReleasingMouseButtonSetsReleasedTransition) {
        Input input;
        input.UpdateMouseButtonState(MouseButton::Left, true);
        input.BeginFrame();

        input.UpdateMouseButtonState(MouseButton::Left, false);

        EXPECT_FALSE(input.IsMouseButtonDown(MouseButton::Left));
        EXPECT_FALSE(input.WasMouseButtonPressed(MouseButton::Left));
        EXPECT_TRUE(input.WasMouseButtonReleased(MouseButton::Left));
    }

    TEST(InputTests, FirstMousePositionEstablishesBaselineWithoutDelta) {
        Input input;

        input.UpdateMousePosition(400.0, 250.0);

        const glm::dvec2 position = input.GetMousePosition();
        const glm::dvec2 delta = input.GetMouseDelta();
        EXPECT_DOUBLE_EQ(position.x, 400.0);
        EXPECT_DOUBLE_EQ(position.y, 250.0);
        EXPECT_DOUBLE_EQ(delta.x, 0.0);
        EXPECT_DOUBLE_EQ(delta.y, 0.0);
    }

    TEST(InputTests, MouseDeltaIsDifferenceFromPreviousPosition) {
        Input input;
        input.UpdateMousePosition(400.0, 250.0);

        input.UpdateMousePosition(404.0, 247.0);

        const glm::dvec2 delta = input.GetMouseDelta();
        EXPECT_DOUBLE_EQ(delta.x, 4.0);
        EXPECT_DOUBLE_EQ(delta.y, -3.0);
    }

    TEST(InputTests, MouseMovementAccumulatesWithinFrame) {
        Input input;
        input.UpdateMousePosition(400.0, 250.0);

        input.UpdateMousePosition(404.0, 247.0);
        input.UpdateMousePosition(409.0, 250.0);

        const glm::dvec2 delta = input.GetMouseDelta();
        EXPECT_DOUBLE_EQ(delta.x, 9.0);
        EXPECT_DOUBLE_EQ(delta.y, 0.0);
    }

    TEST(InputTests, BeginFrameResetsMouseDeltaButPreservesPosition) {
        Input input;
        input.UpdateMousePosition(400.0, 250.0);
        input.UpdateMousePosition(404.0, 247.0);

        input.BeginFrame();

        const glm::dvec2 position = input.GetMousePosition();
        const glm::dvec2 delta = input.GetMouseDelta();
        EXPECT_DOUBLE_EQ(position.x, 404.0);
        EXPECT_DOUBLE_EQ(position.y, 247.0);
        EXPECT_DOUBLE_EQ(delta.x, 0.0);
        EXPECT_DOUBLE_EQ(delta.y, 0.0);
    }

    TEST(InputTests, ScrollEventsAccumulateWithinFrame) {
        Input input;

        input.UpdateScrollDelta(0.5, 1.25);
        input.UpdateScrollDelta(-0.25, 2.0);

        const glm::dvec2 delta = input.GetScrollDelta();
        EXPECT_DOUBLE_EQ(delta.x, 0.25);
        EXPECT_DOUBLE_EQ(delta.y, 3.25);
    }

    TEST(InputTests, BeginFrameResetsScrollDelta) {
        Input input;
        input.UpdateScrollDelta(0.5, 1.25);

        input.BeginFrame();

        const glm::dvec2 delta = input.GetScrollDelta();
        EXPECT_DOUBLE_EQ(delta.x, 0.0);
        EXPECT_DOUBLE_EQ(delta.y, 0.0);
    }

    TEST(InputTests, ResetClearsHeldButtonsAndTransientInput) {
        Input input;
        input.UpdateKeyState(KeyCode::W, true);
        input.UpdateMouseButtonState(MouseButton::Left, true);
        input.UpdateMousePosition(10.0, 20.0);
        input.UpdateMousePosition(15.0, 25.0);
        input.UpdateScrollDelta(1.0, -2.0);

        input.Reset();

        EXPECT_FALSE(input.IsKeyDown(KeyCode::W));
        EXPECT_FALSE(input.WasKeyPressed(KeyCode::W));
        EXPECT_FALSE(input.WasKeyReleased(KeyCode::W));
        EXPECT_FALSE(input.IsMouseButtonDown(MouseButton::Left));
        EXPECT_FALSE(input.WasMouseButtonPressed(MouseButton::Left));
        EXPECT_FALSE(input.WasMouseButtonReleased(MouseButton::Left));
        EXPECT_DOUBLE_EQ(input.GetMouseDelta().x, 0.0);
        EXPECT_DOUBLE_EQ(input.GetMouseDelta().y, 0.0);
        EXPECT_DOUBLE_EQ(input.GetScrollDelta().x, 0.0);
        EXPECT_DOUBLE_EQ(input.GetScrollDelta().y, 0.0);
    }

    TEST(InputTests, ResetMakesNextMousePositionABaseline) {
        Input input;
        input.UpdateMousePosition(10.0, 20.0);
        input.UpdateMousePosition(15.0, 25.0);

        input.Reset();
        input.UpdateMousePosition(500.0, 600.0);

        EXPECT_DOUBLE_EQ(input.GetMousePosition().x, 500.0);
        EXPECT_DOUBLE_EQ(input.GetMousePosition().y, 600.0);
        EXPECT_DOUBLE_EQ(input.GetMouseDelta().x, 0.0);
        EXPECT_DOUBLE_EQ(input.GetMouseDelta().y, 0.0);
    }
}
