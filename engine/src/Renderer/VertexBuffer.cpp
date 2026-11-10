#include "ProjectLight/Renderer/VertexBuffer.h"

#include <glad/glad.h>
#include <limits>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    VertexBuffer::VertexBuffer(std::span<const std::byte> data) : m_Size(data.size()) {
        if (data.empty()) {
            throw std::invalid_argument("Vertex buffer data cannot be empty");
        }

        if (m_Size > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max())) {
            throw std::length_error("Vertex buffer data is too large");
        }

        glCreateBuffers(1, &m_RendererID);

        if (m_RendererID == 0) {
            throw std::runtime_error("Failed to create OpenGL vertex buffer");
        }

        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(m_Size), data.data(),
                          GL_STATIC_DRAW);

        Log::Trace("Created vertex buffer {} ({} bytes)", m_RendererID, m_Size);
    }

    VertexBuffer::~VertexBuffer() {
        if (m_RendererID != 0) {
            glDeleteBuffers(1, &m_RendererID);
        }
    }

    void VertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    std::size_t VertexBuffer::GetSize() const {
        return m_Size;
    }
}
