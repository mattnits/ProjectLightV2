#include "ProjectLight/Renderer/VertexBuffer.h"

#include <glad/glad.h>
#include <limits>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"

namespace ProjectLight
{
    namespace
    {
        GLenum ToOpenGLUsage(BufferUsage usage) {
            switch (usage) {
            case BufferUsage::Static:
                return GL_STATIC_DRAW;

            case BufferUsage::Dynamic:
                return GL_DYNAMIC_DRAW;
            }
            throw std::invalid_argument("Unsupported buffer usage");
        }

        void ValidateBufferData(std::span<const std::byte> data) {
            if (data.empty()) {
                throw std::invalid_argument("Vertex buffer data cannot be empty");
            }
            if (data.size() > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max())) {
                throw std::length_error("Vertex buffer data is too large");
            }
        }
    }

    VertexBuffer::VertexBuffer(std::span<const std::byte> data, BufferUsage usage)
        : m_Size(data.size()), m_Usage(usage) {
        ValidateBufferData(data);
        glCreateBuffers(1, &m_RendererID);

        if (m_RendererID == 0) {
            throw std::runtime_error("Failed to create OpenGL vertex buffer");
        }
        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(m_Size), data.data(),
                          ToOpenGLUsage(m_Usage));

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

    void VertexBuffer::SetData(std::span<const std::byte> data) {
        if (m_Usage != BufferUsage::Dynamic) {
            throw std::logic_error("Cannot update a static vertex buffer");
        }
        ValidateBufferData(data);

        if (data.size() == m_Size) {
            glNamedBufferSubData(m_RendererID, 0, static_cast<GLsizeiptr>(data.size()),
                                 data.data());
            return;
        }
        m_Size = data.size();
        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(m_Size), data.data(),
                          ToOpenGLUsage(m_Usage));
    }
}
