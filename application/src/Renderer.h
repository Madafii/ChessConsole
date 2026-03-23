#pragma once

#include <csignal>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

#define ASSERT(x)                                                                                                                          \
    if (!(x)) raise(SIGTRAP);
#define GLCall(x)                                                                                                                          \
    GLClearError();                                                                                                                        \
    x;                                                                                                                                     \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

class Renderer {
  public:
    Renderer();

    void Clear() const;
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;

  private:
};
