cmake_minimum_required(VERSION 3.25)

project(GameEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GLFW configuration
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Third-party dependencies
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/glad)

# Project targets
add_subdirectory(engine)
add_subdirectory(sandbox)