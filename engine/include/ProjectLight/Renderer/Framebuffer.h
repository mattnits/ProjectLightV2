#pragma once

#include <cstdint>

namespace ProjectLight
{
    struct FramebufferSpecification
    {
        int Width{1};
        int Height{1};
    };

    class Framebuffer
    {
    public:
        explicit Framebuffer(const FramebufferSpecification& specification);
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;

        [[nodiscard]] std::uint32_t GetRendererID() const;
        [[nodiscard]] const FramebufferSpecification& GetSpecification() const;

    private:
        FramebufferSpecification m_Specification;
        std::uint32_t m_RendererID{};
    };
}
