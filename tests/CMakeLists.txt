add_executable(ProjectLightTests
    ${PROJECT_SOURCE_DIR}/editor/src/Camera/EditorCamera.cpp
    ${PROJECT_SOURCE_DIR}/editor/src/Camera/EditorCameraPersistence.cpp
    Core/CameraTests.cpp
    Core/EngineTests.cpp
    Core/SmokeTest.cpp
    Core/FrameTimerTests.cpp
    Core/InputTests.cpp
    Core/RayTests.cpp
    Core/TimestepTests.cpp
    Editor/EditorCameraTests.cpp
    Editor/EditorCameraPersistenceTests.cpp
)

target_include_directories(ProjectLightTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/editor/include
)

target_link_libraries(ProjectLightTests
    PRIVATE
        ProjectLightEngine
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(ProjectLightTests)
