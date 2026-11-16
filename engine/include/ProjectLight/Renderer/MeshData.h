#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>

namespace ProjectLight
{
    struct MeshVertex
    {
        glm::vec3 Position{0.0F};
        glm::vec3 Color{1.0F};
    };

    struct MeshData
    {
        std::vector<MeshVertex> Vertices;
        std::vector<std::uint32_t> Indices;
    };
}
