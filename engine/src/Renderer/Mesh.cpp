#include "ProjectLight/Renderer/Mesh.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

#include "ProjectLight/Renderer/VertexLayout.h"

namespace ProjectLight
{
    namespace
    {
        std::span<const std::byte> GetValidatedVertexBytes(const MeshData& data) {
            if (data.Vertices.empty()) {
                throw std::invalid_argument("Mesh requires at least one vertex");
            }
            if (data.Indices.empty()) {
                throw std::invalid_argument("Mesh requires at least one index");
            }
            if (data.Indices.size() % 3 != 0) {
                throw std::invalid_argument("Mesh index count must form complete triangles");
            }
            for (const std::uint32_t index : data.Indices) {
                if (index >= data.Vertices.size()) {
                    throw std::out_of_range("Mesh index refers to a missing vertex");
                }
            }

            return std::as_bytes(std::span{data.Vertices});
        }
    }

    Mesh::Mesh(const MeshData& data)
        : m_VertexBuffer(GetValidatedVertexBytes(data)), m_IndexBuffer(data.Indices) {
        const VertexLayout layout{
            .Stride = sizeof(MeshVertex),
            .Attributes =
                {
                    VertexAttribute{
                        .Location = 0,
                        .ComponentCount = 3,
                        .Offset = offsetof(MeshVertex, Position),
                    },
                    VertexAttribute{
                        .Location = 1,
                        .ComponentCount = 3,
                        .Offset = offsetof(MeshVertex, Color),
                    },
                },
        };

        m_VertexArray.SetVertexBuffer(m_VertexBuffer, layout);
        m_VertexArray.SetIndexBuffer(m_IndexBuffer);
    }

    const VertexArray& Mesh::GetVertexArray() const {
        return m_VertexArray;
    }

    const IndexBuffer& Mesh::GetIndexBuffer() const {
        return m_IndexBuffer;
    }
}
