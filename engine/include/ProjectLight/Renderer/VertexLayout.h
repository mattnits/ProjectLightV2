#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ProjectLight
{
    struct VertexAttribute
    {
        std::uint32_t Location{};
        int ComponentCount{};
        std::size_t Offset{};
    };

    struct VertexLayout
    {
        std::size_t Stride{};
        std::vector<VertexAttribute> Attributes;
    };
}
