#pragma once

#include "VertexBuffer.h"
#include "VertexBufferDynamic.h"
#include "VertexBufferLayout.h"

class VertexBufferLayout;

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);
    void AddBuffer(const VertexBufferDynamic &vb, const VertexBufferLayout &layout);

    void Bind() const;
    void Unbind() const;

  private:
    uint _rendererID;
    uint _indexOffset = 0;
};
