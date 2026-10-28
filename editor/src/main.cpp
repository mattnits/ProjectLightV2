#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Editor/EditorApplication.h"
#include "ProjectLight/Window/Window.h"

int main() {
    ProjectLight::WindowSpecification windowSpecification;
    windowSpecification.Title = "ProjectLight Editor";
    windowSpecification.Width = 1600;
    windowSpecification.Height = 900;
    windowSpecification.VSync = true;

    ProjectLight::Engine engine{windowSpecification};
    ProjectLight::EditorApplication editor{engine};

    engine.Run(editor);

    return 0;
}
