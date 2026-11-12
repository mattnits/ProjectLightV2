#include "ProjectLight/Renderer/DebugLineRenderer.h"

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <span>
#include <vector>

#include "ProjectLight/Renderer/Renderer.h"

namespace ProjectLight
{
    namespace
    {
        struct DebugLineVertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        constexpr char VertexShaderSource[] = R"(#version 460 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;

            out vec3 v_Color;
            uniform mat4 u_ViewProjection;

            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        constexpr char FragmentShaderSource[] = R"(#version 460 core
            in vec3 v_Color;
            layout(location = 0) out vec4 o_Color;

            void main() {
                o_Color = vec4(v_Color, 1.0);
            }
        )";

        const std::array<DebugLineVertex, 2> InitialVertices{
            DebugLineVertex{.Position = glm::vec3{0.0F}, .Color = glm::vec3{1.0F}},
            DebugLineVertex{.Position = glm::vec3{0.0F}, .Color = glm::vec3{1.0F}},
        };

    }

    DebugLineRenderer::DebugLineRenderer()
        : m_Shader(VertexShaderSource, FragmentShaderSource),
          m_VertexBuffer(std::as_bytes(std::span{InitialVertices}), BufferUsage::Dynamic) {
        const std::array<VertexAttribute, 2> attributes{
            VertexAttribute{
                .Location = 0, .ComponentCount = 3, .Offset = offsetof(DebugLineVertex, Position)},
            VertexAttribute{
                .Location = 1, .ComponentCount = 3, .Offset = offsetof(DebugLineVertex, Color)},
        };

        m_VertexArray.SetVertexBuffer(m_VertexBuffer, sizeof(DebugLineVertex), attributes);
    }

    void DebugLineRenderer::AddLine(const DebugLine& line) {
        m_PendingLines.push_back(line);
    }

    void DebugLineRenderer::Flush(const Renderer& renderer, const glm::mat4& viewProjectionMatrix) {
        if (m_PendingLines.empty()) {
            return;
        }

        std::vector<DebugLineVertex> vertices;
        vertices.reserve(m_PendingLines.size() * 2);

        for (const DebugLine& line : m_PendingLines) {
            vertices.push_back({.Position = line.Start, .Color = line.Color});
            vertices.push_back({.Position = line.End, .Color = line.Color});
        }

        m_VertexBuffer.SetData(std::as_bytes(std::span{vertices}));
        m_Shader.SetMatrix4("u_ViewProjection", viewProjectionMatrix);
        renderer.Draw(m_Shader, m_VertexArray, PrimitiveTopology::Lines, vertices.size());

        m_PendingLines.clear();
    }

}
