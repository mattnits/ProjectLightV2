#include "ProjectLight/Renderer/Framebuffer.h"

#include <glad/glad.h>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    Framebuffer::Framebuffer(const FramebufferSpecification& specification)
        : m_Specification(specification) {
        if (m_Specification.Width <= 0 || m_Specification.Height <= 0) {
            Log::Error("Invalid framebuffer dimensions: {}x{}", m_Specification.Width,
                       m_Specification.Height);
            throw std::invalid_argument("Framebuffer dimensions must be positive");
        }

        glCreateFramebuffers(1, &m_RendererID);

        if (m_RendererID == 0) {
            Log::Critical("Failed to create OpenGL framebuffer");
            throw std::runtime_error("Failed to create OpenGL framebuffer");
        }

        Log::Trace("Created framebuffer {} ({}x{})", m_RendererID, m_Specification.Width,
                   m_Specification.Height);
    }

    Framebuffer::~Framebuffer() {
        if (m_RendererID == 0) {
            return;
        }

        Log::Trace("Destroying framebuffer {}", m_RendererID);
        glDeleteFramebuffers(1, &m_RendererID);
        m_RendererID = 0;
    }

    std::uint32_t Framebuffer::GetRendererID() const {
        return m_RendererID;
    }

    const FramebufferSpecification& Framebuffer::GetSpecification() const {
        return m_Specification;
    }
}
