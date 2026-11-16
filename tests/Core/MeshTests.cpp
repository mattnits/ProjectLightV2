#include <gtest/gtest.h>
#include <stdexcept>

#include "ProjectLight/Renderer/Mesh.h"

namespace ProjectLight
{
    TEST(MeshTests, EmptyVertexDataIsRejectedBeforeUpload) {
        const MeshData data{
            .Indices = {0, 1, 2},
        };

        EXPECT_THROW(static_cast<void>(Mesh{data}), std::invalid_argument);
    }

    TEST(MeshTests, EmptyIndexDataIsRejectedBeforeUpload) {
        MeshData data;
        data.Vertices.resize(3);

        EXPECT_THROW(static_cast<void>(Mesh{data}), std::invalid_argument);
    }

    TEST(MeshTests, IncompleteTriangleIsRejectedBeforeUpload) {
        MeshData data;
        data.Vertices.resize(3);
        data.Indices = {0, 1};

        EXPECT_THROW(static_cast<void>(Mesh{data}), std::invalid_argument);
    }

    TEST(MeshTests, IndexOutsideVertexDataIsRejectedBeforeUpload) {
        MeshData data;
        data.Vertices.resize(3);
        data.Indices = {0, 1, 3};

        EXPECT_THROW(static_cast<void>(Mesh{data}), std::out_of_range);
    }
}
