#pragma once

#include <string>

namespace ProjectLight
{
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

    private:
        RendererCapabilities m_Capabilities;
    };
}
