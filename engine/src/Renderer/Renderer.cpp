#include "ProjectLight/Renderer/Renderer.h"

#include <glad/glad.h>
#include <limits>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Renderer/RendererDebug.h"
#include "ProjectLight/Renderer/Shader.h"
#include "ProjectLight/Renderer/VertexArray.h"

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

        GLenum ToOpenGLTopology(PrimitiveTopology topology) {
            switch (topology) {
            case PrimitiveTopology::Lines:
                return GL_LINES;

            case PrimitiveTopology::Triangles:
                return GL_TRIANGLES;
            }

            throw std::invalid_argument("Unsupported primitive topology");
        }

        std::size_t GetVerticesPerPrimitive(PrimitiveTopology topology) {
            switch (topology) {
            case PrimitiveTopology::Lines:
                return 2;

            case PrimitiveTopology::Triangles:
                return 3;
            }

            throw std::invalid_argument("Unsupported primitive topology");
        }
    }

    Renderer::Renderer() {
        RendererDebug::Initialize();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    void Renderer::Draw(const Shader& shader, const VertexArray& vertexArray,
                        PrimitiveTopology topology, std::size_t vertexCount) const {
        if (vertexCount == 0) {
            throw std::invalid_argument("Draw requires at least one vertex");
        }
        if (vertexCount > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
            throw std::length_error("Vertex count exceeds the OpenGL limit");
        }

        const std::size_t verticesPerPrimitive = GetVerticesPerPrimitive(topology);
        if (vertexCount % verticesPerPrimitive != 0) {
            throw std::invalid_argument("Vertex count does not form complete primitives");
        }

        const GLenum openGLTopology = ToOpenGLTopology(topology);

        shader.Bind();
        vertexArray.Bind();

        glDrawArrays(openGLTopology, 0, static_cast<GLsizei>(vertexCount));
    }

}
