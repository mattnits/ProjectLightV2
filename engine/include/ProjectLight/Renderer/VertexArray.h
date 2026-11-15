#pragma once

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

    private:
        std::uint32_t m_RendererID{};
    };
}
