#include "ProjectLight/Renderer/IndexBuffer.h"

#include <glad/glad.h>
#include <limits>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    IndexBuffer::IndexBuffer(std::span<const std::uint32_t> indices) : m_Count(indices.size()) {
        if (indices.empty()) {
            throw std::invalid_argument("Index buffer data cannot be empty");
        }
        if (indices.size_bytes() >
            static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max())) {
            throw std::length_error("Index buffer data is too large");
        }
        if (m_Count > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
            throw std::length_error("Index count exceeds the OpenGL limit");
        }

        glCreateBuffers(1, &m_RendererID);

        if (m_RendererID == 0) {
            throw std::runtime_error("Failed to create OpenGL index buffer");
        }

        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(indices.size_bytes()),
                          indices.data(), GL_STATIC_DRAW);

        Log::Trace("Created index buffer {} ({} indices)", m_RendererID, m_Count);
    }

    IndexBuffer::~IndexBuffer() {
        if (m_RendererID != 0) {
            glDeleteBuffers(1, &m_RendererID);
        }
    }

    std::size_t IndexBuffer::GetCount() const {
        return m_Count;
    }
}
