#include "VertexArray.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray() { GLCall(glGenVertexArrays(1, &_rendererID)); }

VertexArray::~VertexArray() { GLCall(glDeleteVertexArrays(1, &_rendererID)); }

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) {
    Bind();
    vb.Bind();
    const auto &elements = layout.getElements();
    uint offset = 0;
    for (uint i = 0; i < elements.size(); ++i) {
        const auto &element = elements[i];
        uint attribIndex = i + _indexOffset;
        GLCall(glEnableVertexAttribArray(attribIndex));
        GLCall(glVertexAttribPointer(attribIndex, element.count, element.type, element.normalized, layout.GetStride(), (const void *)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    _indexOffset+= elements.size();
}

void VertexArray::AddBuffer(const VertexBufferDynamic &vb, const VertexBufferLayout &layout) {
    Bind();
    vb.Bind();
    const auto &elements = layout.getElements();
    uint offset = 0;
    for (uint i = 0; i < elements.size(); ++i) {
        const auto &element = elements[i];
        uint attribIndex = i + _indexOffset;
        GLCall(glEnableVertexAttribArray(attribIndex));
        GLCall(glVertexAttribPointer(attribIndex, element.count, element.type, element.normalized, layout.GetStride(), (const void *)offset));
        GLCall(glVertexAttribDivisor(attribIndex, 1));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    _indexOffset+= elements.size();
}

void VertexArray::Bind() const { GLCall(glBindVertexArray(_rendererID)); }

void VertexArray::Unbind() const { GLCall(glBindVertexArray(0)); }
