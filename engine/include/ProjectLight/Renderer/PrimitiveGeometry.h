#pragma once

#include <cstdint>

#include "ProjectLight/Renderer/MeshData.h"

namespace ProjectLight::PrimitiveGeometry
{
    inline constexpr std::uint32_t DefaultSegmentCount = 32;
    [[nodiscard]] MeshData CreateCube(float size = 1.0F);
    [[nodiscard]] MeshData CreatePlane(float width = 1.0F, float depth = 1.0F);
    [[nodiscard]] MeshData CreateCone(float radius = 0.5F, float height = 1.0F,
                                      std::uint32_t segments = DefaultSegmentCount);

    [[nodiscard]] MeshData CreateSphere(float radius = 0.5F, std::uint32_t latitudeSegments = 16,
                                        std::uint32_t longitudeSegments = DefaultSegmentCount);
}
