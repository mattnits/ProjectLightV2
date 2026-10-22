add_library(ProjectLightEngine STATIC
    src/Core/Engine.cpp
    src/Window/Window.cpp
    src/Core/FrameTimer.cpp
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
    PRIVATE
        glfw
        glad
)