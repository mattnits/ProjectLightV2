#include "ProjectLight/Renderer/RendererDebug.h"

#include <glad/glad.h>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight::RendererDebug
{
#ifndef NDEBUG
    namespace
    {
        const char* GetSourceName(GLenum source) {
            switch (source) {
            case GL_DEBUG_SOURCE_API:
                return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION:
                return "Application";
            default:
                return "Other";
            }
        }

        const char* GetTypeName(GLenum type) {
            switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "Deprecated Behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "Undefined Behavior";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "Performance";
            case GL_DEBUG_TYPE_MARKER:
                return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP:
                return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP:
                return "Pop Group";
            default:
                return "Other";
            }
        }

        void GLAPIENTRY OnMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                                  GLsizei length, const GLchar* message,
                                  const void* userParameter) {
            static_cast<void>(length);
            static_cast<void>(userParameter);

            const char* sourceName = GetSourceName(source);
            const char* typeName = GetTypeName(type);

            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                Log::Error("[OpenGL] ID {} | {} | {}: {}", id, sourceName, typeName, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                Log::Warn("[OpenGL] ID {} | {} | {}: {}", id, sourceName, typeName, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                Log::Info("[OpenGL] ID {} | {} | {}: {}", id, sourceName, typeName, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                Log::Trace("[OpenGL] ID {} | {} | {}: {}", id, sourceName, typeName, message);
                break;
            default:
                Log::Warn("[OpenGL] ID {} | {} | Unknown Severity: {}", id, sourceName, message);
                break;
            }
        }
    }
#endif

    void Initialize() {
#ifndef NDEBUG
        GLint contextFlags{};
        glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);

        if ((contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0) {
            Log::Warn("OpenGL debug context was requested but not created");
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(OnMessage, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0,
                              nullptr, GL_FALSE);

        Log::Info("OpenGL debug output enabled");
#endif
    }
}
