#pragma once

#include <GL/glew.h>
#include <sys/types.h>
#include <vector>

struct VertexBufferElement {
    uint type;
    uint count;
    unsigned char normalized;

    static uint GetSizeOfType(uint type) {
        switch (type) {
            case GL_FLOAT:
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        // ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout {
  public:
    VertexBufferLayout() : m_Stride(0) {}

    template <typename T>
    void Push([[maybe_unused]] uint count) {
        static_assert(false);
    }

    const std::vector<VertexBufferElement> &getElements() const { return m_Elements; }
    uint GetStride() const { return m_Stride; }

  private:
    std::vector<VertexBufferElement> m_Elements;
    uint m_Stride;
};

template <>
inline void VertexBufferLayout::Push<float>(uint count) {
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
}

template <>
inline void VertexBufferLayout::Push<uint>(uint count) {
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(uint count) {
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_BYTE) * count;
}
