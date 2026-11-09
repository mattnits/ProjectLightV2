#include "ProjectLight/Renderer/Framebuffer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    namespace
    {
        const char* GetFramebufferStatusName(GLenum status) {
            switch (status) {
            case GL_FRAMEBUFFER_COMPLETE:
                return "GL_FRAMEBUFFER_COMPLETE";
            case GL_FRAMEBUFFER_UNDEFINED:
                return "GL_FRAMEBUFFER_UNDEFINED";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            case GL_FRAMEBUFFER_UNSUPPORTED:
                return "GL_FRAMEBUFFER_UNSUPPORTED";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            default:
                return "Unknown framebuffer status";
            }
        }
    }
    Framebuffer::Framebuffer(const FramebufferSpecification& specification)
        : m_Specification(specification) {
        if (m_Specification.Width <= 0 || m_Specification.Height <= 0) {
            Log::Error("Invalid framebuffer dimensions: {}x{}", m_Specification.Width,
                       m_Specification.Height);
            throw std::invalid_argument("Framebuffer dimensions must be positive");
        }

        CreateResources();
    }

    Framebuffer::~Framebuffer() {
        ReleaseResources();
    }

    std::uint32_t Framebuffer::GetRendererID() const {
        return m_RendererID;
    }

    const FramebufferSpecification& Framebuffer::GetSpecification() const {
        return m_Specification;
    }

    std::uint32_t Framebuffer::GetColorAttachmentRendererID() const {
        return m_ColorAttachmentRendererID;
    }

    void Framebuffer::Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(int width, int height) {
        if (width <= 0 || height <= 0) {
            return;
        }

        if (width == m_Specification.Width && height == m_Specification.Height) {
            return;
        }

        ReleaseResources();

        m_Specification.Width = width;
        m_Specification.Height = height;

        CreateResources();
    }

    void Framebuffer::Clear(const glm::vec4& color) const {
        glClearNamedFramebufferfv(m_RendererID, GL_COLOR, 0, glm::value_ptr(color));

        constexpr float clearDepth = 1.0F;
        glClearNamedFramebufferfv(m_RendererID, GL_DEPTH, 0, &clearDepth);
    }

    void Framebuffer::CreateResources() {
        glCreateFramebuffers(1, &m_RendererID);

        if (m_RendererID == 0) {
            Log::Critical("Failed to create OpenGL framebuffer");
            throw std::runtime_error("Failed to create OpenGL framebuffer");
        }

        // Colour
        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentRendererID);

        if (m_ColorAttachmentRendererID == 0) {
            Log::Critical("Failed to create framebuffer color attachment");
            ReleaseResources();
            throw std::runtime_error("Failed to create framebuffer color attachment");
        }

        glTextureStorage2D(m_ColorAttachmentRendererID, 1, GL_RGBA8, m_Specification.Width,
                           m_Specification.Height);

        glTextureParameteri(m_ColorAttachmentRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ColorAttachmentRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_ColorAttachmentRendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_ColorAttachmentRendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0, m_ColorAttachmentRendererID,
                                  0);

        // Depth
        glCreateRenderbuffers(1, &m_DepthAttachmentRendererID);

        if (m_DepthAttachmentRendererID == 0) {
            Log::Critical("Failed to create framebuffer depth attachment");
            ReleaseResources();
            throw std::runtime_error("Failed to create framebuffer depth attachment");
        }

        glNamedRenderbufferStorage(m_DepthAttachmentRendererID, GL_DEPTH_COMPONENT24,
                                   m_Specification.Width, m_Specification.Height);
        glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                       m_DepthAttachmentRendererID);

        const GLenum status = glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER);

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Log::Critical("Framebuffer {} is incomplete: {} (OpenGL status: 0x{:X})", m_RendererID,
                          GetFramebufferStatusName(status), status);
            ReleaseResources();
            throw std::runtime_error("OpenGL framebuffer is incomplete");
        }
        Log::Trace("Created framebuffer {} ({}x{})", m_RendererID, m_Specification.Width,
                   m_Specification.Height);
    }

    void Framebuffer::ReleaseResources() {
        if (m_DepthAttachmentRendererID != 0) {
            glDeleteRenderbuffers(1, &m_DepthAttachmentRendererID);
            m_DepthAttachmentRendererID = 0;
        }

        if (m_ColorAttachmentRendererID != 0) {
            glDeleteTextures(1, &m_ColorAttachmentRendererID);
            m_ColorAttachmentRendererID = 0;
        }

        if (m_RendererID != 0) {
            Log::Trace("Destroying framebuffer {}", m_RendererID);
            glDeleteFramebuffers(1, &m_RendererID);
            m_RendererID = 0;
        }
    }
}
