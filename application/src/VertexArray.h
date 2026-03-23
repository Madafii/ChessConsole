#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexBufferLayout;

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) const;

    void Bind() const;
    void Unbind() const;

  private:
    uint m_RendererID;
};
