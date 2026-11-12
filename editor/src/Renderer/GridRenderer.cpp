#include "ProjectLight/Editor/Renderer/GridRenderer.h"

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
        struct GridVertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        const glm::vec3 MinorLineColor{0.25F, 0.25F, 0.25F};
        const glm::vec3 MajorLineColor{0.45F, 0.45F, 0.45F};

        const glm::vec3 XAxisColor{0.85F, 0.15F, 0.15F};
        const glm::vec3 YAxisColor{0.15F, 0.80F, 0.20F};
        const glm::vec3 ZAxisColor{0.15F, 0.35F, 0.90F};

        void AddLine(std::vector<GridVertex>& vertices, const glm::vec3& start,
                     const glm::vec3& end, const glm::vec3& color) {
            vertices.push_back({
                .Position = start,
                .Color = color,
            });

            vertices.push_back({
                .Position = end,
                .Color = color,
            });
        }

        constexpr int HalfGridLength = 10;
        constexpr int MajorLineInterval = 5;

        std::vector<GridVertex> CreateGridVertices() {
            const int linesPerDirection = HalfGridLength * 2 + 1;
            std::vector<GridVertex> vertices;
            vertices.reserve(static_cast<std::size_t>(linesPerDirection - 1) * 4 + 6);
            const float gridLength = static_cast<float>(HalfGridLength);

            for (int coordinate = -HalfGridLength; coordinate <= HalfGridLength; ++coordinate) {
                if (coordinate == 0) {
                    continue;
                }
                const float position = static_cast<float>(coordinate);
                const bool isMajorLine = coordinate % MajorLineInterval == 0;
                const glm::vec3& color = isMajorLine ? MajorLineColor : MinorLineColor;
                // Z axis
                AddLine(vertices, {position, 0.0F, -gridLength}, {position, 0.0F, gridLength},
                        color);
                // X axis
                AddLine(vertices, {-gridLength, 0.0F, position}, {gridLength, 0.0F, position},
                        color);
            }

            AddLine(vertices, {-gridLength, 0.0F, 0.0F}, {gridLength, 0.0F, 0.0F}, XAxisColor);
            AddLine(vertices, {0.0F, -gridLength, 0.0F}, {0.0F, gridLength, 0.0F}, YAxisColor);
            AddLine(vertices, {0.0F, 0.0F, -gridLength}, {0.0F, 0.0F, gridLength}, ZAxisColor);

            return vertices;
        }

        const std::vector<GridVertex> GridVertices = CreateGridVertices();

        constexpr char VertexShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;

            layout(location = 0) out vec3 v_Color;

            uniform mat4 u_ViewProjection;

            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
            )";

        constexpr char FragmentShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 v_Color;
            layout(location = 0) out vec4 o_Color;

            void main() {
                o_Color = vec4(v_Color, 1.0);
            }
            )";
    }

    GridRenderer::GridRenderer()
        : m_Shader(VertexShaderSource, FragmentShaderSource),
          m_VertexBuffer(std::as_bytes(std::span{GridVertices})),
          m_VertexCount(GridVertices.size()) {
        const std::array<VertexAttribute, 2> attributes{
            VertexAttribute{
                .Location = 0,
                .ComponentCount = 3,
                .Offset = offsetof(GridVertex, Position),
            },
            VertexAttribute{
                .Location = 1,
                .ComponentCount = 3,
                .Offset = offsetof(GridVertex, Color),
            },
        };

        m_VertexArray.SetVertexBuffer(m_VertexBuffer, sizeof(GridVertex), attributes);
    }

    void GridRenderer::Render(const Renderer& renderer,
                              const glm::mat4& viewProjectionMatrix) const {
        m_Shader.SetMatrix4("u_ViewProjection", viewProjectionMatrix);

        renderer.Draw(m_Shader, m_VertexArray, PrimitiveTopology::Lines, m_VertexCount);
    }
}
