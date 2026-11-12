#pragma once

#include <cstddef>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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

        void Render(const Renderer& renderer, const glm::mat4& viewProjectionMatrix,
                    const glm::vec3& gridCenterPosition, float cameraDistance);

    private:
        Shader m_Shader;
        VertexBuffer m_VertexBuffer;
        VertexArray m_VertexArray;
        std::size_t m_VertexCount{};

        glm::vec2 m_GridCenter{};
        float m_GridSpacing{};
        float m_HalfGridSize{};
        bool m_HasGridState = false;

        void UpdateGrid(const glm::vec3& gridCenterPosition, float cameraDistance);
    };
}
