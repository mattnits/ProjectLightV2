#pragma once

#include <cstddef>
#include <cstdint>

#include "ProjectLight/Renderer/VertexLayout.h"

namespace ProjectLight
{
    class IndexBuffer;
    class VertexBuffer;
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
        void SetVertexBuffer(const VertexBuffer& vertexBuffer, const VertexLayout& layout);
        void SetIndexBuffer(const IndexBuffer& indexBuffer);

        [[nodiscard]] std::size_t GetIndexCount() const;

    private:
        std::uint32_t m_RendererID{};
        std::size_t m_IndexCount{};
    };
}
