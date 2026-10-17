#pragma once

namespace ProjectLight
{
    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void OnStart() {}
        virtual void OnUpdate() {}
        virtual void OnShutdown() {}
    };
}
