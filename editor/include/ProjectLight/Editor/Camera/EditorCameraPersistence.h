#pragma once

#include <filesystem>
#include <optional>

#include "ProjectLight/Editor/Camera/EditorCamera.h"

namespace ProjectLight::EditorCameraPersistence
{
    [[nodiscard]] bool Save(const EditorCameraState& state, const std::filesystem::path& path);

    [[nodiscard]] std::optional<EditorCameraState> Load(const std::filesystem::path& path);
}
