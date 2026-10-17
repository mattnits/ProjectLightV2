#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Engine.h"

class SandboxApplication : public ProjectLight::Application
{
};

int main() {
    ProjectLight::Engine engine;
    SandboxApplication sandbox;

    engine.Run(sandbox);

    return 0;
}
