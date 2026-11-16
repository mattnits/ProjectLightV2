#include "ProjectLight/Renderer/VertexArray.h"

#include <cstddef>
#include <glad/glad.h>
#include <stdexcept>

#include "ProjectLight/Core/Log.h"
#include "ProjectLight/Renderer/IndexBuffer.h"
#include "ProjectLight/Renderer/VertexBuffer.h"

namespace ProjectLight
{
    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &m_RendererID);

        if (m_RendererID == 0) {
            throw std::runtime_error("Failed to create OpenGL vertex array");
        }

        Log::Trace("Created vertex array {}", m_RendererID);
    }

    VertexArray::~VertexArray() {
        if (m_RendererID != 0) {
            glDeleteVertexArrays(1, &m_RendererID);
        }
    }

    void VertexArray::Bind() const {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() {
        glBindVertexArray(0);
    }

    void VertexArray::SetVertexBuffer(const VertexBuffer& vertexBuffer,
                                      const VertexLayout& layout) {
        const std::size_t stride = layout.Stride;
        const std::vector<VertexAttribute>& attributes = layout.Attributes;
        if (stride == 0) {
            throw std::invalid_argument("Vertex stride must be positive");
        }
        if (attributes.empty()) {
            throw std::invalid_argument("Vertex array requires at least one attribute");
        }

        GLint maximumStride{};
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &maximumStride);

        if (stride > static_cast<std::size_t>(maximumStride)) {
            throw std::length_error("Vertex stride exceeds the OpenGL limit");
        }
        if (vertexBuffer.GetSize() % stride != 0) {
            throw std::invalid_argument("Vertex buffer size must contain complete vertices");
        }

        GLint maximumAttributeCount{};
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximumAttributeCount);

        for (std::size_t index = 0; index < attributes.size(); ++index) {
            const VertexAttribute& attribute = attributes[index];
            if (attribute.Location >= static_cast<std::uint32_t>(maximumAttributeCount)) {
                throw std::out_of_range("Vertex attribute location exceeds the OpenGL limit");
            }
            if (attribute.ComponentCount < 1 || attribute.ComponentCount > 4) {
                throw std::invalid_argument(
                    "Vertex attribute must contain between 1 and 4 components");
            }

            const std::size_t attributeSize =
                static_cast<std::size_t>(attribute.ComponentCount) * sizeof(GLfloat);
            if (attributeSize > stride || attribute.Offset > stride - attributeSize) {
                throw std::invalid_argument("Vertex attribute extends beyond the vertex stride");
            }
            for (std::size_t previousIndex = 0; previousIndex < index; ++previousIndex) {
                if (attributes[previousIndex].Location == attribute.Location) {
                    throw std::invalid_argument("Vertex attribute locations must be unique");
                }
            }
        }

        constexpr GLuint bindingIndex = 0;
        glVertexArrayVertexBuffer(m_RendererID, bindingIndex, vertexBuffer.m_RendererID, 0,
                                  static_cast<GLsizei>(stride));

        for (const VertexAttribute& attribute : attributes) {
            glEnableVertexArrayAttrib(m_RendererID, attribute.Location);
            glVertexArrayAttribFormat(m_RendererID, attribute.Location, attribute.ComponentCount,
                                      GL_FLOAT, GL_FALSE, static_cast<GLuint>(attribute.Offset));
            glVertexArrayAttribBinding(m_RendererID, attribute.Location, bindingIndex);
        }
    }

    void VertexArray::SetIndexBuffer(const IndexBuffer& indexBuffer) {
        glVertexArrayElementBuffer(m_RendererID, indexBuffer.m_RendererID);
        m_IndexCount = indexBuffer.GetCount();
    }

    std::size_t VertexArray::GetIndexCount() const {
        return m_IndexCount;
    }
}
