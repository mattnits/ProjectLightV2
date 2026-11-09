#pragma once

#include <cstdint>

namespace ProjectLight
{
    struct FramebufferSpecification
    {
        int Width{1};
        int Height{1};
    };

    // Owns OpenGL resources tied to the active context.
    // Instances must be destroyed before the window destroys that context.
    class Framebuffer
    {
    public:
        explicit Framebuffer(const FramebufferSpecification& specification);
        ~Framebuffer();

        // Delete default copy and move because Framebuffer owns OpenGL resources
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;

        void Bind() const;
        static void Unbind();
        void Resize(int width, int height);

        [[nodiscard]] std::uint32_t GetRendererID() const;
        [[nodiscard]] const FramebufferSpecification& GetSpecification() const;
        [[nodiscard]] std::uint32_t GetColorAttachmentRendererID() const;

    private:
        void CreateResources();
        void ReleaseResources();

        FramebufferSpecification m_Specification;
        std::uint32_t m_RendererID{};
        std::uint32_t m_ColorAttachmentRendererID{};
        std::uint32_t m_DepthAttachmentRendererID{};
    };
}
