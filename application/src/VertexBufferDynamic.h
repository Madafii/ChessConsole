#pragma once

#include <sys/types.h>

class VertexBufferDynamic {
  public:
    VertexBufferDynamic(const void *data, uint size);
    ~VertexBufferDynamic();

    void update(const void *data, uint size) const;

    void Bind() const;
    void Unbind() const;

  private:
    uint _rendererID;
};
