#include "ProjectLight/Core/Engine.h"
#include "ProjectLight/Editor/EditorApplication.h"

int main() {
    ProjectLight::Engine engine;
    ProjectLight::EditorApplication editor(engine);

    engine.Run(editor);

    return 0;
}
