#include "ProjectLight/Core/Application.h"
#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Window/Window.h"

class SandboxApplication : public ProjectLight::Application
{
};

int main() {
    ProjectLight::WindowSpecification windowSpecification;
    windowSpecification.Title = "ProjectLight Sandbox";
    windowSpecification.Width = 1600;
    windowSpecification.Height = 900;
    windowSpecification.VSync = true;

    ProjectLight::Engine engine{windowSpecification};
    SandboxApplication sandbox;

    engine.Run(sandbox);

    return 0;
}
