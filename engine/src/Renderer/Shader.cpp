#include "ProjectLight/Renderer/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    namespace
    {
        GLuint CompileShader(GLenum shader, std::string_view source, std::string_view ShaderName) {
            const GLuint shaderID = glCreateShader(shader);

            if (shaderID == 0) {
                throw std::runtime_error("Failed to create OpenGL shader");
            }

            const char* sourceData = source.data();
            const GLint sourceLength = static_cast<GLint>(source.size());

            glShaderSource(shaderID, 1, &sourceData, &sourceLength);
            glCompileShader(shaderID);

            GLint compilationSucceeded{};
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationSucceeded);

            if (compilationSucceeded == GL_TRUE) {
                return shaderID;
            }

            GLint logLength{};
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

            std::string errorMessage;

            if (logLength > 0) {
                errorMessage.resize(static_cast<std::size_t>(logLength));

                GLsizei writtenLength{};
                glGetShaderInfoLog(shaderID, logLength, &writtenLength, errorMessage.data());

                errorMessage.resize(static_cast<std::size_t>(writtenLength));
            }

            glDeleteShader(shaderID);

            Log::Error("{} shader compilation failed:\n{}", ShaderName, errorMessage);

            throw std::runtime_error("Shader compilation failed");
        }

        GLuint LinkProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
            const GLuint programID = glCreateProgram();

            if (programID == 0) {
                throw std::runtime_error("Failed to create OpenGL shader program");
            }

            glAttachShader(programID, vertexShaderID);
            glAttachShader(programID, fragmentShaderID);
            glLinkProgram(programID);

            GLint linkingSucceeded{};
            glGetProgramiv(programID, GL_LINK_STATUS, &linkingSucceeded);

            if (linkingSucceeded == GL_TRUE) {
                return programID;
            }

            GLint logLength{};
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

            std::string errorMessage;

            if (logLength > 0) {
                errorMessage.resize(static_cast<std::size_t>(logLength));

                GLsizei writtenLength{};
                glGetProgramInfoLog(programID, logLength, &writtenLength, errorMessage.data());

                errorMessage.resize(static_cast<std::size_t>(writtenLength));
            }

            glDeleteProgram(programID);

            Log::Error("Shader program linking failed:\n{}", errorMessage);
            throw std::runtime_error("Shader program linking failed");
        }
    }

    Shader::Shader(std::string_view vertexSource, std::string_view fragmentSource) {
        GLuint vertexShaderID{};
        GLuint fragmentShaderID{};

        try {
            vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexSource, "Vertex");
            fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, "Fragment");
            m_ProgramID = LinkProgram(vertexShaderID, fragmentShaderID);
        } catch (...) {
            if (vertexShaderID != 0) {
                glDeleteShader(vertexShaderID);
            }

            if (fragmentShaderID != 0) {
                glDeleteShader(fragmentShaderID);
            }

            throw;
        }

        glDetachShader(m_ProgramID, vertexShaderID);
        glDetachShader(m_ProgramID, fragmentShaderID);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    Shader::~Shader() {
        if (m_ProgramID != 0) {
            glDeleteProgram(m_ProgramID);
        }
    }

    void Shader::Bind() const {
        glUseProgram(m_ProgramID);
    }

    void Shader::Unbind() {
        glUseProgram(0);
    }

    int Shader::GetUniformLocation(std::string_view name) const {
        const std::string nullTerminatedName{name};

        const GLint location = glGetUniformLocation(m_ProgramID, nullTerminatedName.c_str());

        if (location == -1) {
            Log::Warn("Shader uniform '{}' was not found", nullTerminatedName);
        }

        return static_cast<int>(location);
    }

    void Shader::SetMatrix4(std::string_view name, const glm::mat4& value) const {
        const int location = GetUniformLocation(name);

        if (location == -1) {
            return;
        }

        glProgramUniformMatrix4fv(m_ProgramID, location, 1, GL_FALSE, glm::value_ptr(value));
    }
}
