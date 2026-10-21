#pragma once

namespace ProjectLight
{
    struct TransformComponent;

    class InspectorPanel
    {
    public:
        InspectorPanel() = default;
        void SetTransform(TransformComponent* transform);
        void OnImGuiRender();

    private:
        TransformComponent* m_Transform = nullptr;
    };

}
