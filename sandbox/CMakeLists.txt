add_executable(ProjectLightSandbox
    src/main.cpp
)

target_link_libraries(ProjectLightSandbox
    PRIVATE
        ProjectLightEngine
)