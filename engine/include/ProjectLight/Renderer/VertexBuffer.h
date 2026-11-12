#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace ProjectLight
{
    class VertexArray;

    enum class BufferUsage
    {
        Static,
        Dynamic
    };

    class VertexBuffer
    {
    public:
        explicit VertexBuffer(std::span<const std::byte> data,
                              BufferUsage usage = BufferUsage::Static);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

        void Bind() const;
        static void Unbind();

        [[nodiscard]] std::size_t GetSize() const;

        void SetData(std::span<const std::byte> data);

    private:
        std::uint32_t m_RendererID{};
        std::size_t m_Size{};
        BufferUsage m_Usage;

        friend class VertexArray;
    };
}
