#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <string_view>

namespace ProjectLight
{
    class Shader
    {
    public:
        Shader(std::string_view vertexSource, std::string_view fragmentSource);
        ~Shader();

        void Bind() const;
        static void Unbind();

        void SetMatrix4(std::string_view name, const glm::mat4& value) const;
        void SetFloat(std::string_view name, float value) const;
        void SetVector2(std::string_view name, const glm::vec2& value) const;

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) = delete;
        Shader& operator=(Shader&&) = delete;

    private:
        std::uint32_t m_ProgramID{};

        [[nodiscard]] int GetUniformLocation(std::string_view name) const;
    };
}
