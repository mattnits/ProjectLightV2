#include "ProjectLight/Editor/Renderer/GridRenderer.h"

#include <algorithm>
#include <array>
#include <cmath>
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

        constexpr int HalfGridLineCount = 30;
        constexpr float InitialGridSpacing = 1.0F;
        constexpr int MajorLineInterval = 5;
        constexpr float VerticalAxisHalfLength = 5.0F;

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

        bool IsWorldAxis(float coordinate, float spacing) {
            const float tolerance = spacing * 0.001F;
            return std::abs(coordinate) <= tolerance;
        }

        const glm::vec3& GetGridLineColor(float coordinate, float spacing) {
            // Correct floating point errors
            const long long worldLineIndex = std::llround(coordinate / spacing);
            const bool isMajorLine = worldLineIndex % MajorLineInterval == 0;
            return isMajorLine ? MajorLineColor : MinorLineColor;
        }

        std::vector<GridVertex> CreateGridVertices(const glm::vec2& center, float spacing) {
            const int linesPerDirection = HalfGridLineCount * 2 + 1;
            const float halfGridSize = static_cast<float>(HalfGridLineCount) * spacing;

            const float minimumX = center.x - halfGridSize;
            const float maximumX = center.x + halfGridSize;
            const float minimumZ = center.y - halfGridSize;
            const float maximumZ = center.y + halfGridSize;

            std::vector<GridVertex> vertices;
            vertices.reserve(static_cast<std::size_t>(linesPerDirection) * 4 + 6);

            for (int offset = -HalfGridLineCount; offset <= HalfGridLineCount; ++offset) {
                const float offsetDistance = static_cast<float>(offset) * spacing;
                const float x = center.x + offsetDistance;
                const float z = center.y + offsetDistance;

                if (!IsWorldAxis(x, spacing)) {
                    AddLine(vertices, {x, 0.0F, minimumZ}, {x, 0.0F, maximumZ},
                            GetGridLineColor(x, spacing));
                }
                if (!IsWorldAxis(z, spacing)) {
                    AddLine(vertices, {minimumX, 0.0F, z}, {maximumX, 0.0F, z},
                            GetGridLineColor(z, spacing));
                }
            }

            // Draw world axes only when they intersect the local grid area
            // X axis
            if (minimumZ <= 0.0F && maximumZ >= 0.0F) {
                AddLine(vertices, {minimumX, 0.0F, 0.0F}, {maximumX, 0.0F, 0.0F}, XAxisColor);
            }
            // Z Axis
            if (minimumX <= 0.0F && maximumX >= 0.0F) {
                AddLine(vertices, {0.0F, 0.0F, minimumZ}, {0.0F, 0.0F, maximumZ}, ZAxisColor);
            }
            // Y Axis
            if (minimumX <= 0.0F && maximumX >= 0.0F && minimumZ <= 0.0F && maximumZ >= 0.0F) {
                AddLine(vertices, {0.0F, -VerticalAxisHalfLength, 0.0F},
                        {0.0F, VerticalAxisHalfLength, 0.0F}, YAxisColor);
            }

            return vertices;
        }

        float CalculateGridSpacing(float cameraDistance) {
            constexpr float ReferenceDistance = 10.0F;
            constexpr float MinimumSpacing = 0.01F;

            const float targetSpacing =
                std::max(cameraDistance / ReferenceDistance, MinimumSpacing);
            const float magnitude = std::pow(10.0F, std::floor(std::log10(targetSpacing)));
            const float normalizedSpacing = targetSpacing / magnitude;

            if (normalizedSpacing < 2.0F) {
                return magnitude;
            }
            if (normalizedSpacing < 5.0F) {
                return 2.0F * magnitude;
            }

            return 5.0F * magnitude;
        }

        glm::vec2 CalculateGridCenter(const glm::vec3& gridCenterPosition, float spacing) {
            return {
                std::round(gridCenterPosition.x / spacing) * spacing,
                std::round(gridCenterPosition.z / spacing) * spacing,
            };
        }

        const std::vector<GridVertex> InitialGridVertices =
            CreateGridVertices(glm::vec2{0.0F}, InitialGridSpacing);

        constexpr char VertexShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;

            layout(location = 0) out vec3 v_Color;
            layout(location = 1) out vec3 v_WorldPosition;

            uniform mat4 u_ViewProjection;

            void main() {
                v_Color = a_Color;
                v_WorldPosition = a_Position;

                gl_Position =
                    u_ViewProjection * vec4(a_Position, 1.0);
            }
            )";

        constexpr char FragmentShaderSource[] = R"(#version 460 core

            layout(location = 0) in vec3 v_Color;
            layout(location = 1) in vec3 v_WorldPosition;

            layout(location = 0) out vec4 o_Color;

            uniform vec2 u_GridCenter;
            uniform float u_GridHalfSize;

            void main() {
                vec2 distanceFromCenter =
                    abs(v_WorldPosition.xz - u_GridCenter);

                float furthestDistance =
                    max(distanceFromCenter.x, distanceFromCenter.y);

                float fadeStart = u_GridHalfSize * 0.75;

                float alpha = 1.0 - smoothstep(
                    fadeStart,
                    u_GridHalfSize,
                    furthestDistance);

                if (alpha <= 0.001) {
                    discard;
                }

                o_Color = vec4(v_Color, alpha);
            }
            )";
    }

    GridRenderer::GridRenderer()
        : m_Shader(VertexShaderSource, FragmentShaderSource),
          m_VertexBuffer(std::as_bytes(std::span{InitialGridVertices}), BufferUsage::Dynamic),
          m_VertexCount(InitialGridVertices.size()) {
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

    void GridRenderer::Render(const Renderer& renderer, const glm::mat4& viewProjectionMatrix,
                              const glm::vec3& gridCenterPosition, float cameraDistance) {
        UpdateGrid(gridCenterPosition, cameraDistance);

        m_Shader.SetMatrix4("u_ViewProjection", viewProjectionMatrix);
        m_Shader.SetVector2("u_GridCenter", m_GridCenter);
        m_Shader.SetFloat("u_GridHalfSize", m_HalfGridSize);

        renderer.Draw(m_Shader, m_VertexArray, PrimitiveTopology::Lines, m_VertexCount);
    }

    void GridRenderer::UpdateGrid(const glm::vec3& gridCenterPosition, float cameraDistance) {
        const float spacing = CalculateGridSpacing(cameraDistance);
        const glm::vec2 center = CalculateGridCenter(gridCenterPosition, spacing);
        const bool gridIsUnchanged = m_HasGridState && center.x == m_GridCenter.x &&
                                     center.y == m_GridCenter.y && spacing == m_GridSpacing;

        if (gridIsUnchanged) {
            return;
        }

        const std::vector<GridVertex> vertices = CreateGridVertices(center, spacing);
        m_VertexBuffer.SetData(std::as_bytes(std::span{vertices}));

        m_VertexCount = vertices.size();
        m_GridCenter = center;
        m_GridSpacing = spacing;
        m_HalfGridSize = static_cast<float>(HalfGridLineCount) * spacing;
        m_HasGridState = true;
    }

}
