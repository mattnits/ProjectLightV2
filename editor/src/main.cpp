#include "ProjectLight/Editor/EditorApplication.h"

#include "ProjectLight/Core/Engine.h"

int main() {
    ProjectLight::Engine engine;
    ProjectLight::EditorApplication editor(engine);

    engine.Run(editor);

    return 0;
}
