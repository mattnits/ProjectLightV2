#include "ProjectLight/Editor/Widgets/CameraOrientationWidget.h"

#include <algorithm>
#include <array>

namespace ProjectLight
{
    namespace
    {
        struct Axis
        {
            glm::vec3 Direction;
            ImU32 Color;
            const char* Label;
        };

        constexpr float WidgetRadius = 32.0F;
        constexpr float WidgetPadding = 12.0F;
        constexpr glm::vec4 WidgetFontColour(0, 0, 0, 255);
    }

    void DrawCameraOrientationWidget(ImDrawList& drawList, ImVec2 imageMinimum, ImVec2 imageMaximum,
                                     const glm::quat& cameraOrientation) {
        const float imageWidth = imageMaximum.x - imageMinimum.x;
        const float imageHeight = imageMaximum.y - imageMinimum.y;

        // Skip drawing if it doesn't fit
        if (imageWidth >= 2.0F * (WidgetRadius + WidgetPadding) &&
            imageHeight >= 2.0F * (WidgetRadius + WidgetPadding)) {
            const ImVec2 center{
                imageMaximum.x - WidgetPadding - WidgetRadius,
                imageMinimum.y + WidgetPadding + WidgetRadius,
            };

            drawList.AddCircleFilled(center, WidgetRadius, IM_COL32(20, 23, 30, 200));

            const glm::quat worldToCamera = glm::conjugate(cameraOrientation);
            const float axisLength = WidgetRadius * 0.7F;

            std::array<Axis, 3> axes{
                Axis{worldToCamera * glm::vec3{1.0F, 0.0F, 0.0F}, IM_COL32(220, 60, 60, 255), "X"},
                Axis{worldToCamera * glm::vec3{0.0F, 1.0F, 0.0F}, IM_COL32(60, 210, 80, 255), "Y"},
                Axis{worldToCamera * glm::vec3{0.0F, 0.0F, 1.0F}, IM_COL32(60, 110, 230, 255), "Z"},
            };

            std::sort(axes.begin(), axes.end(),
                      [](const Axis& a, const Axis& b) { return a.Direction.z < b.Direction.z; });

            for (const Axis& axis : axes) {
                const ImVec2 end{
                    center.x + axis.Direction.x * axisLength,
                    center.y - axis.Direction.y * axisLength,
                };

                drawList.AddLine(center, end, axis.Color, 2.0F);
                drawList.AddCircleFilled(end, 7.0F, axis.Color);

                const ImVec2 textSize = ImGui::CalcTextSize(axis.Label);
                drawList.AddText({end.x - textSize.x * 0.5F, end.y - textSize.y * 0.5F},
                                 IM_COL32(WidgetFontColour.x, WidgetFontColour.y,
                                          WidgetFontColour.z, WidgetFontColour.w),
                                 axis.Label);
            }
        }
    }
}
