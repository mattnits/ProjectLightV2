#include "ProjectLight/Editor/Panels/PerformancePanel.h"

#include <imgui.h>

#include "ProjectLight/Core/Timestep.h"
#include "ProjectLight/Renderer/Renderer.h"

namespace ProjectLight
{
    void PerformancePanel::OnImGuiRender(const RendererStatistics& statistics,
                                         const RendererPerformanceMeasurements& measurements,
                                         const Timestep& timestep) const {
        const double frameTimeMilliseconds = timestep.GetMilliseconds();
        const double frameTimeSeconds = timestep.GetSeconds();
        const double framesPerSecond = frameTimeSeconds > 0.0 ? 1.0 / frameTimeSeconds : 0.0;

        const bool isVisible = ImGui::Begin("Performance");

        if (isVisible) {
            ImGui::Text("Frame time: %.2f ms", frameTimeMilliseconds);
            ImGui::Text("FPS: %.1f", framesPerSecond);

            if (measurements.HasCpuFrameTime) {
                ImGui::Text("CPU render time: %.3f ms", measurements.CpuFrameTimeMilliseconds);
            } else {
                ImGui::TextUnformatted("CPU render time: waiting...");
            }

            if (measurements.HasGpuFrameTime) {
                ImGui::Text("GPU frame time: %.3f ms", measurements.GpuFrameTimeMilliseconds);
            } else {
                ImGui::TextUnformatted("GPU frame time: waiting...");
            }

            ImGui::Separator();

            ImGui::Text("Draw calls: %zu", statistics.DrawCalls);
            ImGui::Text("Vertices submitted: %zu", statistics.VerticesSubmitted);
            ImGui::Text("Indices submitted: %zu", statistics.IndicesSubmitted);
        }

        ImGui::End();
    }
}
