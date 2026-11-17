add_library(ProjectLightEngine STATIC
    src/Camera/Camera.cpp
    src/Core/Engine.cpp
    src/Core/FrameTimer.cpp
    src/Core/Log.cpp
    src/Core/UUID.cpp
    src/Input/Input.cpp
    src/Math/ModelMatrix.cpp
    src/Math/Ray.cpp
    src/Renderer/DebugLineRenderer.cpp
    src/Renderer/Framebuffer.cpp
    src/Renderer/IndexBuffer.cpp
    src/Renderer/Mesh.cpp
    src/Renderer/PrimitiveGeometry.cpp
    src/Renderer/RendererDebug.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Shader.cpp
    src/Renderer/VertexBuffer.cpp
    src/Renderer/VertexArray.cpp
    src/Scene/Scene.cpp
    src/Scene/Entity.cpp
    src/Window/Window.cpp
)

target_include_directories(ProjectLightEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_compile_definitions(ProjectLightEngine PRIVATE
    PROJECTLIGHT_ENGINE_SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders\"
)

target_compile_features(ProjectLightEngine
    PUBLIC
        cxx_std_20
)

target_link_libraries(ProjectLightEngine
    PUBLIC
        EnTT::EnTT
        glm::glm
        spdlog::spdlog
    PRIVATE
        glfw
        glad
)