#pragma once

#include <cstddef>
#include <glm/mat4x4.hpp>

#include "ProjectLight/Renderer/Shader.h"
#include "ProjectLight/Renderer/VertexArray.h"
#include "ProjectLight/Renderer/VertexBuffer.h"

namespace ProjectLight
{
    class Renderer;

    class GridRenderer
    {
    public:
        GridRenderer();

        void Render(const Renderer& renderer, const glm::mat4& viewProjectionMatrix) const;

    private:
        Shader m_Shader;
        VertexBuffer m_VertexBuffer;
        VertexArray m_VertexArray;
        std::size_t m_VertexCount{};
    };
}
