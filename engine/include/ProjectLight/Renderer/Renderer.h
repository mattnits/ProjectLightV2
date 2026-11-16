#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ProjectLight
{
    class Shader;
    class VertexArray;

    enum class PrimitiveTopology
    {
        Lines,
        Triangles
    };

    struct RendererCapabilities
    {
        std::string Vendor;
        std::string Device;
        std::string OpenGLVersion;
        std::string ShadingLanguageVersion;

        int MaximumTextureSize{};
        int MaximumCombinedTextureUnits{};
        int MaximumSamples{};
    };

    struct RendererStatistics
    {
        std::size_t DrawCalls{};
        std::size_t VerticesSubmitted{};
        std::size_t IndicesSubmitted{};
    };

    struct RendererPerformanceMeasurements
    {
        double CpuFrameTimeMilliseconds{};
        double GpuFrameTimeMilliseconds{};
        bool HasCpuFrameTime{};
        bool HasGpuFrameTime{};
    };

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void BeginFrame();
        void EndFrame();

        void Draw(const Shader& shader, const VertexArray& vertexArray, PrimitiveTopology topology,
                  std::size_t vertexCount) const;
        void DrawIndexed(const Shader& shader, const VertexArray& vertexArray,
                         PrimitiveTopology topology) const;

        void ResetStatistics();

        [[nodiscard]] const RendererCapabilities& GetCapabilities() const;
        [[nodiscard]] const RendererPerformanceMeasurements& GetPerformanceMeasurements() const;
        [[nodiscard]] const RendererStatistics& GetStatistics() const;

    private:
        RendererCapabilities m_Capabilities;
        mutable RendererStatistics m_Statistics;
        static constexpr std::size_t GpuTimeQueryCount = 2;

        RendererPerformanceMeasurements m_PerformanceMeasurements;
        std::array<std::uint32_t, GpuTimeQueryCount> m_GpuTimeQueries{};
        std::size_t m_CurrentGpuTimeQuery{};
        bool m_HasSubmittedGpuTimeQuery{};
        std::chrono::steady_clock::time_point m_CpuFrameStart;
    };
}
