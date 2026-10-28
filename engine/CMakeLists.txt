add_library(ProjectLightEngine STATIC
    src/Core/Engine.cpp
    src/Core/FrameTimer.cpp
    src/Core/Log.cpp
    src/Input/Input.cpp
    src/Window/Window.cpp
)

target_include_directories(ProjectLightEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ProjectLightEngine
    PUBLIC
        cxx_std_20
)

target_link_libraries(ProjectLightEngine
    PUBLIC
        glm::glm
        spdlog::spdlog
    PRIVATE
        glfw
        glad
)