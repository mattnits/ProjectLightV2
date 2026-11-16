#include <cmath>
#include <cstddef>
#include <cstdint>
#include <glm/geometric.hpp>
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

#include "ProjectLight/Renderer/PrimitiveGeometry.h"

namespace ProjectLight
{
    namespace
    {
        constexpr float GeometryTolerance = 0.0001F;
        const glm::vec3 ExpectedPrimitiveColor{
            157.0F / 255.0F,
            168.0F / 255.0F,
            160.0F / 255.0F,
        };

        void ExpectIndicesReferToVertices(const MeshData& data) {
            for (const std::uint32_t index : data.Indices) {
                EXPECT_LT(index, data.Vertices.size());
            }
        }

        void ExpectOutwardWinding(const MeshData& data) {
            ASSERT_EQ(data.Indices.size() % 3, 0);

            for (std::size_t index = 0; index < data.Indices.size(); index += 3) {
                const glm::vec3& a = data.Vertices[data.Indices[index]].Position;
                const glm::vec3& b = data.Vertices[data.Indices[index + 1]].Position;
                const glm::vec3& c = data.Vertices[data.Indices[index + 2]].Position;

                const glm::vec3 normal = glm::cross(b - a, c - a);
                const glm::vec3 triangleCenter = (a + b + c) / 3.0F;

                EXPECT_GT(glm::dot(normal, triangleCenter), 0.0F);
            }
        }

        void ExpectDefaultPrimitiveColor(const MeshData& data) {
            for (const MeshVertex& vertex : data.Vertices) {
                EXPECT_FLOAT_EQ(vertex.Color.x, ExpectedPrimitiveColor.x);
                EXPECT_FLOAT_EQ(vertex.Color.y, ExpectedPrimitiveColor.y);
                EXPECT_FLOAT_EQ(vertex.Color.z, ExpectedPrimitiveColor.z);
            }
        }
    }

    TEST(PrimitiveGeometryTests, DefaultCubeHasExpectedVertexAndIndexCounts) {
        const MeshData cube = PrimitiveGeometry::CreateCube();

        EXPECT_EQ(cube.Vertices.size(), 8);
        EXPECT_EQ(cube.Indices.size(), 36);
    }

    TEST(PrimitiveGeometryTests, CubeSizeControlsFullEdgeLength) {
        constexpr float size = 4.0F;
        constexpr float expectedHalfSize = size * 0.5F;
        const MeshData cube = PrimitiveGeometry::CreateCube(size);

        for (const MeshVertex& vertex : cube.Vertices) {
            EXPECT_FLOAT_EQ(std::abs(vertex.Position.x), expectedHalfSize);
            EXPECT_FLOAT_EQ(std::abs(vertex.Position.y), expectedHalfSize);
            EXPECT_FLOAT_EQ(std::abs(vertex.Position.z), expectedHalfSize);
        }
    }

    TEST(PrimitiveGeometryTests, CubeIndicesReferToExistingVertices) {
        const MeshData cube = PrimitiveGeometry::CreateCube();

        ExpectIndicesReferToVertices(cube);
    }

    TEST(PrimitiveGeometryTests, CubeTrianglesUseCounterclockwiseOutwardWinding) {
        const MeshData cube = PrimitiveGeometry::CreateCube();

        ExpectOutwardWinding(cube);
    }

    TEST(PrimitiveGeometryTests, InvalidCubeSizesAreRejected) {
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCube(0.0F)), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCube(-1.0F)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(
                         PrimitiveGeometry::CreateCube(std::numeric_limits<float>::infinity())),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(
                         PrimitiveGeometry::CreateCube(std::numeric_limits<float>::quiet_NaN())),
                     std::invalid_argument);
    }

    TEST(PrimitiveGeometryTests, PlaneHasExpectedDimensionsAndUpwardWinding) {
        const MeshData plane = PrimitiveGeometry::CreatePlane(4.0F, 6.0F);

        ASSERT_EQ(plane.Vertices.size(), 4);
        ASSERT_EQ(plane.Indices.size(), 6);
        ExpectIndicesReferToVertices(plane);

        for (const MeshVertex& vertex : plane.Vertices) {
            EXPECT_FLOAT_EQ(vertex.Position.y, 0.0F);
            EXPECT_FLOAT_EQ(std::abs(vertex.Position.x), 2.0F);
            EXPECT_FLOAT_EQ(std::abs(vertex.Position.z), 3.0F);
        }

        for (std::size_t index = 0; index < plane.Indices.size(); index += 3) {
            const glm::vec3& a = plane.Vertices[plane.Indices[index]].Position;
            const glm::vec3& b = plane.Vertices[plane.Indices[index + 1]].Position;
            const glm::vec3& c = plane.Vertices[plane.Indices[index + 2]].Position;

            EXPECT_GT(glm::cross(b - a, c - a).y, 0.0F);
        }
    }

    TEST(PrimitiveGeometryTests, ConeCountsAndWindingMatchSegmentCount) {
        constexpr std::uint32_t segments = 8;
        const MeshData cone = PrimitiveGeometry::CreateCone(2.0F, 4.0F, segments);

        EXPECT_EQ(cone.Vertices.size(), segments + 2);
        EXPECT_EQ(cone.Indices.size(), segments * 6);
        ExpectIndicesReferToVertices(cone);
        ExpectOutwardWinding(cone);
    }

    TEST(PrimitiveGeometryTests, SphereCountsRadiusAndWindingAreCorrect) {
        constexpr float radius = 2.0F;
        constexpr std::uint32_t latitudeSegments = 8;
        constexpr std::uint32_t longitudeSegments = 12;
        const MeshData sphere =
            PrimitiveGeometry::CreateSphere(radius, latitudeSegments, longitudeSegments);

        EXPECT_EQ(sphere.Vertices.size(), (latitudeSegments + 1) * (longitudeSegments + 1));
        EXPECT_EQ(sphere.Indices.size(), (latitudeSegments - 1) * longitudeSegments * 6);
        ExpectIndicesReferToVertices(sphere);
        ExpectOutwardWinding(sphere);

        for (const MeshVertex& vertex : sphere.Vertices) {
            EXPECT_NEAR(glm::length(vertex.Position), radius, GeometryTolerance);
        }
    }

    TEST(PrimitiveGeometryTests, AllPrimitivesUseDefaultColor) {
        ExpectDefaultPrimitiveColor(PrimitiveGeometry::CreateCube());
        ExpectDefaultPrimitiveColor(PrimitiveGeometry::CreatePlane());
        ExpectDefaultPrimitiveColor(PrimitiveGeometry::CreateCone());
        ExpectDefaultPrimitiveColor(PrimitiveGeometry::CreateSphere());
    }

    TEST(PrimitiveGeometryTests, InvalidPlaneConeAndSphereParametersAreRejected) {
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreatePlane(0.0F, 1.0F)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreatePlane(1.0F, -1.0F)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreatePlane(
                         std::numeric_limits<float>::quiet_NaN(), 1.0F)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCone(0.0F, 1.0F, 8)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCone(1.0F, 0.0F, 8)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCone(1.0F, 1.0F, 2)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateCone(
                         1.0F, std::numeric_limits<float>::infinity(), 8)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateSphere(0.0F, 8, 12)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateSphere(1.0F, 1, 12)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateSphere(1.0F, 8, 2)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(PrimitiveGeometry::CreateSphere(
                         std::numeric_limits<float>::quiet_NaN(), 8, 12)),
                     std::invalid_argument);
    }
}
