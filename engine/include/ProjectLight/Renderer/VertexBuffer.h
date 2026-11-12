#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace ProjectLight
{
    class VertexArray;
    class VertexBuffer
    {
    public:
        explicit VertexBuffer(std::span<const std::byte> data);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

        void Bind() const;
        static void Unbind();

        [[nodiscard]] std::size_t GetSize() const;

    private:
        std::uint32_t m_RendererID{};
        std::size_t m_Size{};
        friend class VertexArray;
    };
}
