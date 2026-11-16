#pragma once

#include "ProjectLight/Renderer/IndexBuffer.h"
#include "ProjectLight/Renderer/MeshData.h"
#include "ProjectLight/Renderer/VertexArray.h"
#include "ProjectLight/Renderer/VertexBuffer.h"

namespace ProjectLight
{
    class Mesh
    {
    public:
        explicit Mesh(const MeshData& data);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        [[nodiscard]] const VertexArray& GetVertexArray() const;

    private:
        VertexBuffer m_VertexBuffer;
        IndexBuffer m_IndexBuffer;
        VertexArray m_VertexArray;
    };
}
