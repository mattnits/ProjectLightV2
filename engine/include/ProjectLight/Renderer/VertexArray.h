#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace ProjectLight
{
    class VertexBuffer;

    struct VertexAttribute
    {
        std::uint32_t Location{};
        int ComponentCount{};
        std::size_t Offset{};
    };

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray(VertexArray&&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;

        void Bind() const;
        static void Unbind();
        void SetVertexBuffer(const VertexBuffer& vertexBuffer, std::size_t stride,
                             std::span<const VertexAttribute> attributes);

    private:
        std::uint32_t m_RendererID{};
    };
}
