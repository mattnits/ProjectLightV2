#include "ProjectLight/Renderer/PrimitiveGeometry.h"

#include <cmath>
#include <cstdint>
#include <glm/vec3.hpp>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace ProjectLight::PrimitiveGeometry
{

    namespace
    {
        const glm::vec3 DefaultPrimitiveColor{
            157.0F / 255.0F,
            168.0F / 255.0F,
            160.0F / 255.0F,
        };
    }
    MeshData CreateCube(float size) {
        if (!std::isfinite(size) || size <= 0.0F) {
            throw std::invalid_argument("Invalid cube size");
        }

        const float halfSize = size * 0.5F;
        MeshData data;

        data.Vertices = {
            {{-halfSize, -halfSize, -halfSize}, DefaultPrimitiveColor},
            {{halfSize, -halfSize, -halfSize}, DefaultPrimitiveColor},
            {{halfSize, halfSize, -halfSize}, DefaultPrimitiveColor},
            {{-halfSize, halfSize, -halfSize}, DefaultPrimitiveColor},
            {{-halfSize, -halfSize, halfSize}, DefaultPrimitiveColor},
            {{halfSize, -halfSize, halfSize}, DefaultPrimitiveColor},
            {{halfSize, halfSize, halfSize}, DefaultPrimitiveColor},
            {{-halfSize, halfSize, halfSize}, DefaultPrimitiveColor},
        };

        data.Indices = {
            4, 5, 6, 6, 7, 4, // Front
            1, 0, 3, 3, 2, 1, // Back
            0, 4, 7, 7, 3, 0, // Left
            5, 1, 2, 2, 6, 5, // Right
            3, 7, 6, 6, 2, 3, // Top
            0, 1, 5, 5, 4, 0, // Bottom
        };

        return data;
    }

    MeshData CreatePlane(float width, float depth) {
        if (!std::isfinite(width) || width <= 0.0F) {
            throw std::invalid_argument("Invalid width specified");
        }
        if (!std::isfinite(depth) || depth <= 0.0F) {
            throw std::invalid_argument("Invalid depth specified");
        }

        const float halfWidth = width * 0.5F;
        const float halfDepth = depth * 0.5F;

        MeshData data;

        data.Vertices = {
            {{-halfWidth, 0.0F, -halfDepth}, DefaultPrimitiveColor},
            {{halfWidth, 0.0F, -halfDepth}, DefaultPrimitiveColor},
            {{halfWidth, 0.0F, halfDepth}, DefaultPrimitiveColor},
            {{-halfWidth, 0.0F, halfDepth}, DefaultPrimitiveColor},
        };

        data.Indices = {
            0, 2, 1, 0, 3, 2,
        };

        return data;
    }

    MeshData CreateCone(float radius, float height, std::uint32_t segments) {
        if (!std::isfinite(radius) || radius <= 0.0F) {
            throw std::invalid_argument("Invalid cone radius");
        }
        if (!std::isfinite(height) || height <= 0.0F) {
            throw std::invalid_argument("Invalid cone height");
        }
        if (segments < 3) {
            throw std::invalid_argument("Cone must have at least three segments");
        }

        constexpr std::uint32_t ApexIndex = 0;
        constexpr std::uint32_t BaseCenterIndex = 1;
        constexpr std::uint32_t FirstRingIndex = 2;

        const float halfHeight = height * 0.5F;

        MeshData data;
        data.Vertices.reserve(static_cast<std::size_t>(segments) + FirstRingIndex);
        data.Indices.reserve(static_cast<std::size_t>(segments) * 6);

        data.Vertices.push_back({
            .Position = {0.0F, halfHeight, 0.0F},
            .Color = DefaultPrimitiveColor,
        });

        data.Vertices.push_back({
            .Position = {0.0F, -halfHeight, 0.0F},
            .Color = DefaultPrimitiveColor,
        });

        for (std::uint32_t segment = 0; segment < segments; ++segment) {
            const float fraction = static_cast<float>(segment) / static_cast<float>(segments);

            const float angle = fraction * 2.0F * std::numbers::pi_v<float>;

            const float cosine = std::cos(angle);
            const float sine = std::sin(angle);

            data.Vertices.push_back({
                .Position =
                    {
                        radius * cosine,
                        -halfHeight,
                        radius * sine,
                    },
                .Color = DefaultPrimitiveColor,
            });
        }

        for (std::uint32_t segment = 0; segment < segments; ++segment) {
            const std::uint32_t current = FirstRingIndex + segment;
            const std::uint32_t next = FirstRingIndex + (segment + 1) % segments;

            data.Indices.push_back(ApexIndex);
            data.Indices.push_back(next);
            data.Indices.push_back(current);

            data.Indices.push_back(BaseCenterIndex);
            data.Indices.push_back(current);
            data.Indices.push_back(next);
        }

        return data;
    }

    MeshData CreateSphere(float radius, std::uint32_t latitudeSegments,
                          std::uint32_t longitudeSegments) {
        if (!std::isfinite(radius) || radius <= 0.0F) {
            throw std::invalid_argument("Invalid sphere radius");
        }
        if (latitudeSegments < 2) {
            throw std::invalid_argument("Sphere latitude segments must be greater than 2");
        }
        if (longitudeSegments < 3) {
            throw std::invalid_argument("Sphere longitude segments must be greater than 3");
        }

        const std::uint64_t rowSize = static_cast<std::uint64_t>(longitudeSegments) + 1;

        const std::uint64_t vertexCount =
            (static_cast<std::uint64_t>(latitudeSegments) + 1) * rowSize;

        if (vertexCount > std::numeric_limits<std::uint32_t>::max()) {
            throw std::length_error("Sphere has too many vertices for 32-bit indices");
        }

        MeshData data;
        data.Vertices.reserve(static_cast<std::size_t>(vertexCount));

        const std::size_t indexCount = static_cast<std::size_t>(latitudeSegments - 1) *
                                       static_cast<std::size_t>(longitudeSegments) * 6;

        data.Indices.reserve(indexCount);

        for (std::uint32_t latitude = 0; latitude <= latitudeSegments; ++latitude) {
            const float latitudeFraction =
                static_cast<float>(latitude) / static_cast<float>(latitudeSegments);

            const float theta = latitudeFraction * std::numbers::pi_v<float>;

            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            for (std::uint32_t longitude = 0; longitude <= longitudeSegments; ++longitude) {
                const float longitudeFraction =
                    static_cast<float>(longitude) / static_cast<float>(longitudeSegments);

                const float phi = longitudeFraction * 2.0F * std::numbers::pi_v<float>;

                data.Vertices.push_back({
                    .Position =
                        {
                            radius * sinTheta * std::cos(phi),
                            radius * cosTheta,
                            radius * sinTheta * std::sin(phi),
                        },
                    .Color = DefaultPrimitiveColor,
                });
            }
        }

        const std::uint32_t verticesPerRow = longitudeSegments + 1;

        for (std::uint32_t latitude = 0; latitude < latitudeSegments; ++latitude) {
            for (std::uint32_t longitude = 0; longitude < longitudeSegments; ++longitude) {
                const std::uint32_t topLeft = latitude * verticesPerRow + longitude;

                const std::uint32_t topRight = topLeft + 1;
                const std::uint32_t bottomLeft = topLeft + verticesPerRow;
                const std::uint32_t bottomRight = bottomLeft + 1;

                if (latitude != 0) {
                    data.Indices.push_back(topLeft);
                    data.Indices.push_back(topRight);
                    data.Indices.push_back(bottomLeft);
                }

                if (latitude != latitudeSegments - 1) {
                    data.Indices.push_back(topRight);
                    data.Indices.push_back(bottomRight);
                    data.Indices.push_back(bottomLeft);
                }
            }
        }

        return data;
    }
}
