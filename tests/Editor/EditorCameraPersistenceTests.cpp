#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <system_error>

#include "ProjectLight/Editor/Camera/EditorCameraPersistence.h"

namespace ProjectLight
{
    namespace
    {
        class EditorCameraPersistenceTests : public testing::Test
        {
        protected:
            void SetUp() override {
                const testing::TestInfo* testInfo =
                    testing::UnitTest::GetInstance()->current_test_info();

                m_StatePath = std::filesystem::path{testing::TempDir()} /
                              (std::string{testInfo->name()} + ".ini");
                RemoveStateFile();
            }

            void TearDown() override {
                RemoveStateFile();
            }

            void WriteStateFile(std::string_view contents) const {
                std::ofstream output{m_StatePath, std::ios::trunc};
                ASSERT_TRUE(output);

                output << contents;
                output.flush();
                ASSERT_TRUE(output.good());
            }

            std::filesystem::path m_StatePath;

        private:
            void RemoveStateFile() const {
                std::error_code error;
                std::filesystem::remove(m_StatePath, error);
            }
        };

        void ExpectStateEqual(const EditorCameraState& actual, const EditorCameraState& expected) {
            EXPECT_FLOAT_EQ(actual.Pivot.x, expected.Pivot.x);
            EXPECT_FLOAT_EQ(actual.Pivot.y, expected.Pivot.y);
            EXPECT_FLOAT_EQ(actual.Pivot.z, expected.Pivot.z);
            EXPECT_FLOAT_EQ(actual.PivotDistance, expected.PivotDistance);
            EXPECT_FLOAT_EQ(actual.YawDegrees, expected.YawDegrees);
            EXPECT_FLOAT_EQ(actual.PitchDegrees, expected.PitchDegrees);
            EXPECT_FLOAT_EQ(actual.MovementSpeed, expected.MovementSpeed);
        }
    }

    TEST_F(EditorCameraPersistenceTests, SaveAndLoadRoundTripPreservesState) {
        EditorCameraState expectedState;
        expectedState.Pivot = {1.234567F, -8.765432F, 3.141592F};
        expectedState.PivotDistance = 23.4567F;
        expectedState.YawDegrees = -137.25F;
        expectedState.PitchDegrees = 42.125F;
        expectedState.MovementSpeed = 17.75F;

        ASSERT_TRUE(EditorCameraPersistence::Save(expectedState, m_StatePath));

        const std::optional<EditorCameraState> loadedState =
            EditorCameraPersistence::Load(m_StatePath);

        ASSERT_TRUE(loadedState.has_value());
        ExpectStateEqual(*loadedState, expectedState);
    }

    TEST_F(EditorCameraPersistenceTests, MissingFileReturnsNoState) {
        EXPECT_FALSE(EditorCameraPersistence::Load(m_StatePath).has_value());
    }

    TEST_F(EditorCameraPersistenceTests, MalformedFileReturnsNoState) {
        WriteStateFile("This is not a camera state file.\n");

        EXPECT_FALSE(EditorCameraPersistence::Load(m_StatePath).has_value());
    }

    TEST_F(EditorCameraPersistenceTests, UnsupportedVersionReturnsNoState) {
        WriteStateFile("ProjectLightEditorCamera 999\n");

        EXPECT_FALSE(EditorCameraPersistence::Load(m_StatePath).has_value());
    }
}
