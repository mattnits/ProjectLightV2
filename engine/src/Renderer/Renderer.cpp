#include "ProjectLight/Renderer/Renderer.h"

#include <glad/glad.h>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Renderer/RendererDebug.h"

namespace ProjectLight
{
    namespace
    {
        std::string QueryString(GLenum property) {
            const GLubyte* value = glGetString(property);

            if (value == nullptr) {
                throw std::runtime_error("Failed to query OpenGL capability string");
            }

            return reinterpret_cast<const char*>(value);
        }

        int QueryInteger(GLenum property) {
            GLint value{};
            glGetIntegerv(property, &value);
            return static_cast<int>(value);
        }
    }

    Renderer::Renderer() {
        RendererDebug::Initialize();

        m_Capabilities.Vendor = QueryString(GL_VENDOR);
        m_Capabilities.Device = QueryString(GL_RENDERER);
        m_Capabilities.OpenGLVersion = QueryString(GL_VERSION);
        m_Capabilities.ShadingLanguageVersion = QueryString(GL_SHADING_LANGUAGE_VERSION);

        m_Capabilities.MaximumTextureSize = QueryInteger(GL_MAX_TEXTURE_SIZE);

        m_Capabilities.MaximumCombinedTextureUnits =
            QueryInteger(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

        m_Capabilities.MaximumSamples = QueryInteger(GL_MAX_SAMPLES);

        Log::Info("Graphics device: {} ({})", m_Capabilities.Device, m_Capabilities.Vendor);

        Log::Info("OpenGL {} | GLSL {}", m_Capabilities.OpenGLVersion,
                  m_Capabilities.ShadingLanguageVersion);

        Log::Info("Renderer limits: texture size {}, texture units {}, samples {}",
                  m_Capabilities.MaximumTextureSize, m_Capabilities.MaximumCombinedTextureUnits,
                  m_Capabilities.MaximumSamples);
    }

    const RendererCapabilities& Renderer::GetCapabilities() const {
        return m_Capabilities;
    }
}
