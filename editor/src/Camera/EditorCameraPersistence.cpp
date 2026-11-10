#include "ProjectLight/Editor/Camera/EditorCameraPersistence.h"

#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <string_view>

namespace ProjectLight::EditorCameraPersistence
{
    namespace
    {
        constexpr std::string_view FileIdentifier = "ProjectLightEditorCamera";

        constexpr int FileVersion = 1;

        bool ReadKey(std::istream& input, std::string_view expectedKey) {
            std::string key;
            return static_cast<bool>(input >> key) && key == expectedKey;
        }
    }

    bool Save(const EditorCameraState& state, const std::filesystem::path& path) {
        std::ofstream output{path, std::ios::trunc};

        if (!output) {
            return false;
        }

        output << std::setprecision(std::numeric_limits<float>::max_digits10);

        output << FileIdentifier << ' ' << FileVersion << '\n';
        output << "Pivot " << state.Pivot.x << ' ' << state.Pivot.y << ' ' << state.Pivot.z << '\n';
        output << "PivotDistance " << state.PivotDistance << '\n';
        output << "YawDegrees " << state.YawDegrees << '\n';
        output << "PitchDegrees " << state.PitchDegrees << '\n';
        output << "MovementSpeed " << state.MovementSpeed << '\n';

        output.flush();
        return output.good();
    }

    std::optional<EditorCameraState> Load(const std::filesystem::path& path) {
        std::ifstream input{path};

        if (!input) {
            return std::nullopt;
        }

        std::string identifier;
        int version{};

        if (!(input >> identifier >> version) || identifier != FileIdentifier ||
            version != FileVersion) {
            return std::nullopt;
        }

        EditorCameraState state;

        if (!ReadKey(input, "Pivot") ||
            !(input >> state.Pivot.x >> state.Pivot.y >> state.Pivot.z)) {
            return std::nullopt;
        }

        if (!ReadKey(input, "PivotDistance") || !(input >> state.PivotDistance)) {
            return std::nullopt;
        }

        if (!ReadKey(input, "YawDegrees") || !(input >> state.YawDegrees)) {
            return std::nullopt;
        }

        if (!ReadKey(input, "PitchDegrees") || !(input >> state.PitchDegrees)) {
            return std::nullopt;
        }

        if (!ReadKey(input, "MovementSpeed") || !(input >> state.MovementSpeed)) {
            return std::nullopt;
        }

        std::string unexpectedValue;

        if (input >> unexpectedValue) {
            return std::nullopt;
        }

        if (!input.eof()) {
            return std::nullopt;
        }

        return state;
    }
}
