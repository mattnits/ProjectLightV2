#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace ProjectLight
{
    class VertexArray;

    class IndexBuffer
    {
    public:
        explicit IndexBuffer(std::span<const std::uint32_t> indices);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;
        IndexBuffer& operator=(IndexBuffer&&) = delete;

        [[nodiscard]] std::size_t GetCount() const;

    private:
        std::uint32_t m_RendererID{};
        std::size_t m_Count{};

        friend class VertexArray;
    };
}
