add_executable(ProjectLightTests
    Core/EngineTests.cpp
    Core/SmokeTest.cpp
    Core/FrameTimerTests.cpp
    Core/InputTests.cpp
    Core/TimestepTests.cpp
)

target_link_libraries(ProjectLightTests
    PRIVATE
        ProjectLightEngine
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(ProjectLightTests)
