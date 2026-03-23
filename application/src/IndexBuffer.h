#pragma once

#include <sys/types.h>

class IndexBuffer {
  public:
    IndexBuffer(const void *data, uint count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline uint GetCount() const { return m_Count; }

  private:
    uint m_RendererID;
    uint m_Count;
};
