#pragma once

#include <cstddef>
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

    class Renderer
    {
    public:
        Renderer();

        [[nodiscard]] const RendererCapabilities& GetCapabilities() const;
        void Draw(const Shader& shader, const VertexArray& vertexArray, PrimitiveTopology topology,
                  std::size_t vertexCount) const;

    private:
        RendererCapabilities m_Capabilities;
    };
}
