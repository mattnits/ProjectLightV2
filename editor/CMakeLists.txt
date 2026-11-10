add_executable(ProjectLightEditor
    src/main.cpp
    src/EditorApplication.cpp
    src/Camera/EditorCamera.cpp
    src/Panels/InspectorPanel.cpp
    src/Panels/ViewportPanel.cpp
    src/Camera/EditorCameraPersistence.cpp
)

target_include_directories(ProjectLightEditor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ProjectLightEditor
    PRIVATE
        ProjectLightEngine
        imgui
)
