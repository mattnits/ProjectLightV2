#pragma once

#include <glm/gtc/quaternion.hpp>
#include <imgui.h>

namespace ProjectLight
{

    void DrawCameraOrientationWidget(ImDrawList& drawList, ImVec2 imageMinimum, ImVec2 imageMaximum,
                                     const glm::quat& cameraOrientation);
}
