#pragma once

namespace ProjectLight
{
    class Mesh;

    struct MeshRendererComponent
    {
        const Mesh* MeshResource{};
        bool IsVisible{true};
    };
}
