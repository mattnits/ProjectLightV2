#pragma once

namespace ProjectLight
{
    class Timestep;
    struct RendererStatistics;
    struct RendererPerformanceMeasurements;

    class PerformancePanel
    {
    public:
        void OnImGuiRender(const RendererStatistics& statistics,
                           const RendererPerformanceMeasurements& measurements,
                           const Timestep& timestep) const;
    };
}
