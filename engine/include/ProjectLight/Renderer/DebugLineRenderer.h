#pragma once

#include <glm/mat4x4.hpp>
#include <vector>

#include "ProjectLight/Renderer/DebugLine.h"
#include "ProjectLight/Renderer/Shader.h"
#include "ProjectLight/Renderer/VertexArray.h"
#include "ProjectLight/Renderer/VertexBuffer.h"

namespace ProjectLight
{
    class Renderer;

    class DebugLineRenderer
    {
    public:
        DebugLineRenderer();
        void AddLine(const DebugLine& line);
        void Flush(const Renderer& renderer, const glm::mat4& viewProjectionMatrix);

    private:
        Shader m_Shader;
        VertexBuffer m_VertexBuffer;
        VertexArray m_VertexArray;
        std::vector<DebugLine> m_PendingLines;
    };
}
