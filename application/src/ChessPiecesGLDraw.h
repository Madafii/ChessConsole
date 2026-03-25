#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureSet.h"
#include "VertexArray.h"
#include "VertexBufferDynamic.h"
#include <memory>

class ChessPiecesGLDraw {
  public:
    struct InstanceData {
        glm::vec2 position;
        float layer;
    };

    explicit ChessPiecesGLDraw();

    void OnRender();
    void OnUpdate();

  private:
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<Shader> _shader;
    std::unique_ptr<TextureSet> _textureSet;
    glm::vec3 _translation;
    glm::mat4 _proj;
    glm::mat4 _view;
    std::vector<InstanceData> _instances;
};
