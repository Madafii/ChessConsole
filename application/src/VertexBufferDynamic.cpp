#include "VertexBufferDynamic.h"
#include "Renderer.h"
#include <GL/glew.h>

VertexBufferDynamic::VertexBufferDynamic(const void *data, uint size) {
    GLCall(glGenBuffers(1, &_rendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _rendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

VertexBufferDynamic::~VertexBufferDynamic() { GLCall(glDeleteBuffers(1, &_rendererID)); }

void VertexBufferDynamic::update(const void *data, uint size) const {
    Bind();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

void VertexBufferDynamic::Bind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, _rendererID)); }

void VertexBufferDynamic::Unbind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }
